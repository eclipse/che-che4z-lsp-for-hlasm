/*
 * Copyright (c) 2019 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Broadcom, Inc. - initial API and implementation
 */

#include "asm_processor.h"

#include "checking/instr_operand.h"
#include "data_def_postponed_statement.h"
#include "ebcdic_encoding.h"
#include "expressions/mach_expr_term.h"
#include "expressions/mach_expr_visitor.h"
#include "postponed_statement_impl.h"
#include "processing/context_manager.h"

namespace hlasm_plugin::parser_library::processing {

void asm_processor::process_sect(const context::section_kind kind, rebuilt_statement stmt)
{
    auto sect_name = find_label_symbol(stmt);

    if (hlasm_ctx.ord_ctx.symbol_defined(sect_name) && !hlasm_ctx.ord_ctx.section_defined(sect_name, kind))
    {
        add_diagnostic(diagnostic_op::error_E031("symbol", stmt.label_ref().field_range));
    }
    else
    {
        auto sym_loc = hlasm_ctx.processing_stack().back().proc_location;
        sym_loc.pos.column = 0;
        hlasm_ctx.ord_ctx.set_section(sect_name, kind, std::move(sym_loc));
    }
    check(stmt, hlasm_ctx, checker_, *this);
}

void asm_processor::process_LOCTR(rebuilt_statement stmt)
{
    auto loctr_name = find_label_symbol(stmt);

    if (loctr_name == context::id_storage::empty_id)
        add_diagnostic(diagnostic_op::error_E053(stmt.label_ref().field_range));

    if (hlasm_ctx.ord_ctx.symbol_defined(loctr_name) && !hlasm_ctx.ord_ctx.counter_defined(loctr_name))
    {
        add_diagnostic(diagnostic_op::error_E031("symbol", stmt.label_ref().field_range));
    }
    else
    {
        auto sym_loc = hlasm_ctx.processing_stack().back().proc_location;
        sym_loc.pos.column = 0;
        hlasm_ctx.ord_ctx.set_location_counter(loctr_name, std::move(sym_loc));
    }
    check(stmt, hlasm_ctx, checker_, *this);
}

void asm_processor::process_EQU(rebuilt_statement stmt)
{
    auto symbol_name = find_label_symbol(stmt);

    if (symbol_name == context::id_storage::empty_id)
    {
        if (stmt.label_ref().type == semantics::label_si_type::EMPTY)
            add_diagnostic(diagnostic_op::error_E053(stmt.label_ref().field_range));
        return;
    }

    if (hlasm_ctx.ord_ctx.symbol_defined(symbol_name))
    {
        add_diagnostic(diagnostic_op::error_E031("symbol", stmt.label_ref().field_range));
        return;
    }
    // type attribute operand
    context::symbol_attributes::type_attr t_attr = context::symbol_attributes::undef_type;
    if (stmt.operands_ref().value.size() >= 3 && stmt.operands_ref().value[2]->type != semantics::operand_type::EMPTY)
    {
        auto asm_op = stmt.operands_ref().value[2]->access_asm();
        auto expr_op = asm_op->access_expr();

        if (expr_op && !expr_op->has_dependencies(hlasm_ctx.ord_ctx))
        {
            auto t_value = expr_op->expression->resolve(hlasm_ctx.ord_ctx);
            if (t_value.value_kind() == context::symbol_value_kind::ABS && t_value.get_abs() >= 0
                && t_value.get_abs() <= 255)
                t_attr = (context::symbol_attributes::type_attr)t_value.get_abs();
            else
                add_diagnostic(diagnostic_op::error_A134_EQU_type_att_format(asm_op->operand_range));
        }
        else
            add_diagnostic(diagnostic_op::error_A134_EQU_type_att_format(asm_op->operand_range));
    }

    // length attribute operand
    context::symbol_attributes::len_attr length_attr = context::symbol_attributes::undef_length;
    if (stmt.operands_ref().value.size() >= 2 && stmt.operands_ref().value[1]->type != semantics::operand_type::EMPTY)
    {
        auto asm_op = stmt.operands_ref().value[1]->access_asm();
        auto expr_op = asm_op->access_expr();

        if (expr_op && !expr_op->has_dependencies(hlasm_ctx.ord_ctx))
        {
            auto length_value = expr_op->expression->resolve(hlasm_ctx.ord_ctx);
            if (length_value.value_kind() == context::symbol_value_kind::ABS && length_value.get_abs() >= 0
                && length_value.get_abs() <= 65535)
                length_attr = (context::symbol_attributes::len_attr)length_value.get_abs();
            else
                add_diagnostic(diagnostic_op::error_A133_EQU_len_att_format(asm_op->operand_range));
        }
        else
            add_diagnostic(diagnostic_op::error_A133_EQU_len_att_format(asm_op->operand_range));
    }

    // value operand
    if (stmt.operands_ref().value.size() != 0 && stmt.operands_ref().value[0]->type != semantics::operand_type::EMPTY)
    {
        auto asm_op = stmt.operands_ref().value[0]->access_asm();
        auto expr_op = asm_op->access_expr();

        if (expr_op)
        {
            auto holder(expr_op->expression->get_dependencies(hlasm_ctx.ord_ctx));

            if (length_attr == context::symbol_attributes::undef_length)
            {
                auto l_term = expr_op->expression->leftmost_term();
                if (auto symbol_term = dynamic_cast<const expressions::mach_expr_symbol*>(l_term))
                {
                    auto len_symbol = hlasm_ctx.ord_ctx.get_symbol(symbol_term->value);

                    if (len_symbol != nullptr && len_symbol->kind() != context::symbol_value_kind::UNDEF)
                        length_attr = len_symbol->attributes().length();
                    else
                        length_attr = 1;
                }
                else
                    length_attr = 1;
            }

            context::symbol_attributes attrs(context::symbol_origin::EQU, t_attr, length_attr);

            if (!holder.contains_dependencies())
            {
                create_symbol(
                    stmt.stmt_range_ref(), symbol_name, expr_op->expression->resolve(hlasm_ctx.ord_ctx), attrs);
            }
            else
            {
                if (!holder.is_address())
                {
                    bool cycle_ok = create_symbol(stmt.stmt_range_ref(), symbol_name, context::symbol_value(), attrs);

                    if (cycle_ok)
                    {
                        const auto& stmt_range = stmt.stmt_range_ref();
                        add_dependency(stmt_range,
                            symbol_name,
                            &*expr_op->expression,
                            std::make_unique<postponed_statement_impl>(std::move(stmt), hlasm_ctx.processing_stack()));
                    }
                }
                else
                    create_symbol(stmt.stmt_range_ref(), symbol_name, *holder.unresolved_address, attrs);
            }
        }
    }
}

template<checking::data_instr_type instr_type>
void asm_processor::process_data_instruction(rebuilt_statement stmt)
{
    // enforce alignment of the first operand
    context::alignment al = context::no_align;
    if (!stmt.operands_ref().value.empty() && stmt.operands_ref().value[0]->type != semantics::operand_type::EMPTY)
        al = stmt.operands_ref().value[0]->access_data_def()->value->get_alignment();

    context::address adr = hlasm_ctx.ord_ctx.align(al);

    // dependency sources is list of all expressions in data def operand, that have some unresolved dependencies.
    bool has_dependencies = false;
    // has_length_dependencies specifies whether the length of the data instruction can be resolved right now or must be
    // postponed
    bool has_length_dependencies = false;
    for (const auto& op : stmt.operands_ref().value)
    {
        if (op->type == semantics::operand_type::EMPTY)
            continue;
        auto data_op = op->access_data_def();

        data_op->value->assign_location_counter(adr);

        has_dependencies |= data_op->value->type != 'V' && data_op->has_dependencies(hlasm_ctx.ord_ctx);

        has_length_dependencies |= data_op->get_length_dependencies(hlasm_ctx.ord_ctx).contains_dependencies();

        // some types require operands that consist only of one symbol
        data_op->value->check_single_symbol_ok(diagnostic_collector(this));
    }

    // process label
    auto label = find_label_symbol(stmt);

    if (label != context::id_storage::empty_id && stmt.operands_ref().value.size()
        && stmt.operands_ref().value.front()->type != semantics::operand_type::EMPTY)
    {
        if (!hlasm_ctx.ord_ctx.symbol_defined(label))
        {
            auto data_op = stmt.operands_ref().value.front()->access_data_def();

            context::symbol_attributes::type_attr type =
                ebcdic_encoding::a2e[(unsigned char)data_op->value->get_type_attribute()];

            context::symbol_attributes::len_attr len = context::symbol_attributes::undef_length;
            context::symbol_attributes::scale_attr scale = context::symbol_attributes::undef_scale;

            auto tmp = data_op->get_operand_value(hlasm_ctx.ord_ctx);
            auto& value = dynamic_cast<checking::data_definition_operand&>(*tmp);

            if (!data_op->value->length
                || !data_op->value->length->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
            {
                len = value.get_length_attribute();
            }
            if (data_op->value->scale
                && !data_op->value->scale->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
            {
                scale = value.get_scale_attribute();
            }
            create_symbol(stmt.stmt_range_ref(),
                label,
                std::move(adr),
                context::symbol_attributes(
                    context::symbol_origin::DAT, type, len, scale, value.get_integer_attribute()));
        }
        else
            add_diagnostic(diagnostic_op::error_E031("symbol", stmt.label_ref().field_range));
    }

    // TODO issue warning when alignment is bigger than section's alignment
    // hlasm_ctx.ord_ctx.current_section()->current_location_counter().


    if (has_dependencies)
    {
        auto adder = hlasm_ctx.ord_ctx.symbol_dependencies.add_dependencies(
            std::make_unique<data_def_postponed_statement<instr_type>>(std::move(stmt), hlasm_ctx.processing_stack()));
        if (has_length_dependencies)
        {
            auto sp = hlasm_ctx.ord_ctx.register_ordinary_space(al);
            adder.add_dependency(
                sp, dynamic_cast<const data_def_postponed_statement<instr_type>*>(&*adder.source_stmt));
        }
        else
            hlasm_ctx.ord_ctx.reserve_storage_area(
                data_def_postponed_statement<instr_type>::get_operands_length(
                    adder.source_stmt->impl()->operands_ref().value, hlasm_ctx.ord_ctx),
                context::no_align);

        bool cycle_ok = true;

        if (label != context::id_storage::empty_id)
        {
            if (adder.source_stmt->impl()->operands_ref().value.empty())
                return;

            auto data_op = adder.source_stmt->impl()->operands_ref().value.front()->access_data_def();

            if (data_op->value->length
                && data_op->value->length->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
                cycle_ok = adder.add_dependency(label, context::data_attr_kind::L, data_op->value->length.get());

            if (data_op->value->scale
                && data_op->value->scale->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
                cycle_ok &= adder.add_dependency(label, context::data_attr_kind::S, data_op->value->scale.get());
        }

        adder.add_dependency();

        if (!cycle_ok)
            add_diagnostic(
                diagnostic_op::error_E033(adder.source_stmt->impl()->operands_ref().value.front()->operand_range));

        adder.finish();
    }
    else
    {
        hlasm_ctx.ord_ctx.reserve_storage_area(
            data_def_postponed_statement<instr_type>::get_operands_length(stmt.operands_ref().value, hlasm_ctx.ord_ctx),
            context::no_align);
        check(stmt, hlasm_ctx, checker_, *this);
    }
}

void asm_processor::process_DC(rebuilt_statement stmt)
{
    process_data_instruction<checking::data_instr_type::DC>(std::move(stmt));
}

void asm_processor::process_DS(rebuilt_statement stmt)
{
    process_data_instruction<checking::data_instr_type::DS>(std::move(stmt));
}

void asm_processor::process_COPY(rebuilt_statement stmt)
{
    find_sequence_symbol(stmt);

    if (stmt.operands_ref().value.size() == 1 && stmt.operands_ref().value.front()->access_asm()->access_expr())
    {
        process_copy(stmt, ctx, lib_provider, this);
    }
    else
    {
        check(stmt, hlasm_ctx, checker_, *this);
    }
}

void asm_processor::process_EXTRN(rebuilt_statement stmt)
{
    for (const auto& op : stmt.operands_ref().value)
    {
        if (auto op_asm = op->access_asm())
        {
            if (auto expr = op_asm->access_expr())
            {
                auto sym = dynamic_cast<const expressions::mach_expr_symbol*>(expr->expression.get());
                if (sym)
                    create_symbol(sym->get_range(),
                        sym->value,
                        hlasm_ctx.ord_ctx.align(context::no_align),
                        context::symbol_attributes::make_extrn_attrs());
            }
        }
    }
    check(stmt, hlasm_ctx, checker_, *this);
}

void asm_processor::process_WXTRN(rebuilt_statement stmt)
{
    for (const auto& op : stmt.operands_ref().value)
    {
        if (auto op_asm = op->access_asm())
        {
            if (auto expr = op_asm->access_expr())
            {
                if (auto sym = dynamic_cast<const expressions::mach_expr_symbol*>(expr->expression.get()))
                    create_symbol(sym->get_range(),
                        sym->value,
                        hlasm_ctx.ord_ctx.align(context::no_align),
                        context::symbol_attributes::make_extrn_attrs());
            }
        }
    }
    check(stmt, hlasm_ctx, checker_, *this);
}

std::optional<context::A_t> asm_processor::try_get_abs_value(const semantics::simple_expr_operand* op) const
{
    if (op->has_dependencies(hlasm_ctx.ord_ctx))
        return std::nullopt;

    auto val = op->expression->evaluate(hlasm_ctx.ord_ctx);

    if (val.value_kind() != context::symbol_value_kind::ABS)
        return std::nullopt;
    return val.get_abs();
}

void asm_processor::process_ORG(rebuilt_statement stmt)
{
    find_sequence_symbol(stmt);

    auto label = find_label_symbol(stmt);

    if (label != context::id_storage::empty_id)
    {
        if (hlasm_ctx.ord_ctx.symbol_defined(label))
            add_diagnostic(diagnostic_op::error_E031("symbol", stmt.label_ref().field_range));
        else
            create_symbol(stmt.stmt_range_ref(),
                label,
                hlasm_ctx.ord_ctx.align(context::no_align),
                context::symbol_attributes::make_org_attrs());
    }

    const semantics::expr_assembler_operand* reloc_expr = nullptr;
    bool undefined_absolute_part = false;
    size_t boundary = 0;
    int offset = 0;

    for (size_t i = 0; i < stmt.operands_ref().value.size(); ++i)
    {
        if (stmt.operands_ref().value[i]->type == semantics::operand_type::EMPTY)
            continue;

        auto asm_op = stmt.operands_ref().value[i]->access_asm();
        assert(asm_op);
        auto expr = asm_op->access_expr();
        if (!expr)
        {
            if (i == 0)
                add_diagnostic(diagnostic_op::error_A245_ORG_expression(stmt.stmt_range_ref()));
            else
                add_diagnostic(diagnostic_op::error_A115_ORG_op_format(stmt.stmt_range_ref()));
            break;
        }

        if (i == 0)
        {
            auto deps = expr->expression->get_dependencies(hlasm_ctx.ord_ctx);
            undefined_absolute_part =
                deps.undefined_attr_refs.size() || deps.undefined_symbols.size() || deps.unresolved_spaces.size();
            if (!deps.unresolved_address)
            {
                add_diagnostic(diagnostic_op::error_A245_ORG_expression(stmt.stmt_range_ref()));
                return;
            }
            reloc_expr = expr;
        }

        if (i == 1)
        {
            auto val = try_get_abs_value(expr);
            if (!val || *val < 2 || *val > 4096 || ((*val & (*val - 1)) != 0)) // check range and test for power of 2
            {
                add_diagnostic(diagnostic_op::error_A116_ORG_boundary_operand(stmt.stmt_range_ref()));
                return;
            }
            boundary = (size_t)*val;
        }
        if (i == 2)
        {
            auto val = try_get_abs_value(expr);
            if (!val)
            {
                add_diagnostic(diagnostic_op::error_A115_ORG_op_format(stmt.stmt_range_ref()));
                return;
            }
            offset = *val;
        }
    }

    if (reloc_expr)
    {
        auto reloc_val = !undefined_absolute_part
            ? reloc_expr->expression->evaluate(hlasm_ctx.ord_ctx).get_reloc()
            : *reloc_expr->expression->get_dependencies(hlasm_ctx.ord_ctx).unresolved_address;

        if (!check_address_for_ORG(
                stmt.stmt_range_ref(), reloc_val, hlasm_ctx.ord_ctx.align(context::no_align), boundary, offset))
            return;

        if (undefined_absolute_part)
            hlasm_ctx.ord_ctx.set_location_counter_value(std::move(reloc_val),
                boundary,
                offset,
                reloc_expr->expression.get(),
                std::make_unique<postponed_statement_impl>(std::move(stmt), hlasm_ctx.processing_stack()));
        else
            hlasm_ctx.ord_ctx.set_location_counter_value(std::move(reloc_val), boundary, offset, nullptr, nullptr);
    }
    else
        hlasm_ctx.ord_ctx.set_available_location_counter_value(boundary, offset);
}

void asm_processor::process_OPSYN(rebuilt_statement stmt)
{
    const auto& operands = stmt.operands_ref().value;
    if (!check(stmt, hlasm_ctx, checker_, *this))
        return;

    auto label = find_label_symbol(stmt);
    if (label == context::id_storage::empty_id)
    {
        if (stmt.label_ref().type == semantics::label_si_type::EMPTY)
            add_diagnostic(diagnostic_op::error_E053(stmt.label_ref().field_range));
        return;
    }

    context::id_index operand = context::id_storage::empty_id;
    if (operands.size() == 1) // covers also the " , " case
    {
        auto expr_op = operands.front()->access_asm()->access_expr();
        if (expr_op)
        {
            if (auto expr = dynamic_cast<const expressions::mach_expr_symbol*>(expr_op->expression.get()))
                operand = expr->value;
        }
    }

    if (operand == context::id_storage::empty_id)
    {
        if (hlasm_ctx.get_operation_code(label))
            hlasm_ctx.remove_mnemonic(label);
        else
            add_diagnostic(diagnostic_op::error_E049(*label, stmt.label_ref().field_range));
    }
    else
    {
        if (hlasm_ctx.get_operation_code(operand))
            hlasm_ctx.add_mnemonic(label, operand);
        else
            add_diagnostic(diagnostic_op::error_A246_OPSYN(operands.front()->operand_range));
    }
}

asm_processor::asm_processor(analyzing_context ctx,
    branching_provider& branch_provider,
    workspaces::parse_lib_provider& lib_provider,
    statement_fields_parser& parser,
    opencode_provider& open_code)
    : low_language_processor(ctx, branch_provider, lib_provider, parser)
    , table_(create_table(*ctx.hlasm_ctx))
    , open_code_(&open_code)
{}

void asm_processor::process(context::shared_stmt_ptr stmt)
{
    auto rebuilt_stmt = preprocess(stmt);

    auto it = table_.find(rebuilt_stmt.opcode_ref().value);
    if (it != table_.end())
    {
        auto& [key, func] = *it;
        func(std::move(rebuilt_stmt));
    }
    else
    {
        // until implementation of all instructions, if has deps, ignore
        for (auto& op : rebuilt_stmt.operands_ref().value)
        {
            auto tmp = context::instruction::assembler_instructions.find(*rebuilt_stmt.opcode_ref().value);
            bool can_have_ord_syms =
                tmp != context::instruction::assembler_instructions.end() ? tmp->second.has_ord_symbols : true;

            if (op->type != semantics::operand_type::EMPTY && can_have_ord_syms
                && op->access_asm()->has_dependencies(hlasm_ctx.ord_ctx))
                return;
        }
        check(rebuilt_stmt, hlasm_ctx, checker_, *this);
    }
}

bool asm_processor::process_copy(const semantics::complete_statement& stmt,
    analyzing_context ctx,
    workspaces::parse_lib_provider& lib_provider,
    diagnosable_ctx* diagnoser)
{
    auto& expr = stmt.operands_ref().value.front()->access_asm()->access_expr()->expression;
    auto sym_expr = dynamic_cast<expressions::mach_expr_symbol*>(expr.get());

    if (!sym_expr)
    {
        if (diagnoser)
            diagnoser->add_diagnostic(diagnostic_op::error_E058(stmt.operands_ref().value.front()->operand_range));
        return false;
    }

    auto tmp = ctx.hlasm_ctx->copy_members().find(sym_expr->value);

    if (tmp == ctx.hlasm_ctx->copy_members().end())
    {
        bool result = lib_provider.parse_library(
            *sym_expr->value, ctx, workspaces::library_data { processing_kind::COPY, sym_expr->value });

        if (!result)
        {
            if (diagnoser)
                diagnoser->add_diagnostic(diagnostic_op::error_E058(stmt.operands_ref().value.front()->operand_range));
            return false;
        }
    }
    auto whole_copy_stack = ctx.hlasm_ctx->whole_copy_stack();

    auto cycle_tmp = std::find(whole_copy_stack.begin(), whole_copy_stack.end(), sym_expr->value);

    if (cycle_tmp != whole_copy_stack.end())
    {
        if (diagnoser)
            diagnoser->add_diagnostic(diagnostic_op::error_E062(stmt.stmt_range_ref()));
        return false;
    }

    ctx.hlasm_ctx->enter_copy_member(sym_expr->value);

    return true;
}

asm_processor::process_table_t asm_processor::create_table(context::hlasm_context& h_ctx)
{
    process_table_t table;
    table.emplace(h_ctx.ids().add("CSECT"),
        std::bind(&asm_processor::process_sect, this, context::section_kind::EXECUTABLE, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("DSECT"),
        std::bind(&asm_processor::process_sect, this, context::section_kind::DUMMY, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("RSECT"),
        std::bind(&asm_processor::process_sect, this, context::section_kind::READONLY, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("COM"),
        std::bind(&asm_processor::process_sect, this, context::section_kind::COMMON, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("LOCTR"), std::bind(&asm_processor::process_LOCTR, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("EQU"), std::bind(&asm_processor::process_EQU, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("DC"), std::bind(&asm_processor::process_DC, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("DS"), std::bind(&asm_processor::process_DS, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().well_known.COPY, std::bind(&asm_processor::process_COPY, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("EXTRN"), std::bind(&asm_processor::process_EXTRN, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("WXTRN"), [this](rebuilt_statement stmt) { process_WXTRN(std::move(stmt)); });
    table.emplace(h_ctx.ids().add("ORG"), std::bind(&asm_processor::process_ORG, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("OPSYN"), std::bind(&asm_processor::process_OPSYN, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("AINSERT"), std::bind(&asm_processor::process_AINSERT, this, std::placeholders::_1));

    return table;
}

context::id_index asm_processor::find_sequence_symbol(const rebuilt_statement& stmt)
{
    semantics::seq_sym symbol;
    switch (stmt.label_ref().type)
    {
        case semantics::label_si_type::SEQ:
            symbol = std::get<semantics::seq_sym>(stmt.label_ref().value);
            branch_provider.register_sequence_symbol(symbol.name, symbol.symbol_range);
            return symbol.name;
        default:
            return context::id_storage::empty_id;
    }
}

namespace {
class AINSERT_operand_visitor final : public expressions::mach_expr_visitor
{
public:
    // Inherited via mach_expr_visitor
    void visit(const expressions::mach_expr_constant&) override {}
    void visit(const expressions::mach_expr_data_attr&) override {}
    void visit(const expressions::mach_expr_symbol& expr) override { value = expr.value; }
    void visit(const expressions::mach_expr_location_counter&) override {}
    void visit(const expressions::mach_expr_self_def&) override {}
    void visit(const expressions::mach_expr_default&) override {}

    context::id_index value = nullptr;
};
} // namespace

void asm_processor::process_AINSERT(rebuilt_statement stmt)
{
    const auto& ops = stmt.operands_ref();

    if (!check(stmt, hlasm_ctx, checker_, *this))
        return;

    const auto& record = dynamic_cast<const semantics::string_assembler_operand&>(*ops.value[0]).value;
    AINSERT_operand_visitor visitor;
    dynamic_cast<const semantics::expr_assembler_operand&>(*ops.value[1]).expression->apply(visitor);
    auto [value] = visitor;

    if (!value)
        return;
    auto dest = *value == "FRONT" ? processing::ainsert_destination::front : processing::ainsert_destination::back;

    open_code_->ainsert(record, dest);
}

} // namespace hlasm_plugin::parser_library::processing
