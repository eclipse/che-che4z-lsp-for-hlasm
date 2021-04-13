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

#include "lookahead_processor.h"

#include "ebcdic_encoding.h"
#include "expressions/mach_expr_term.h"
#include "ordinary_processor.h"
#include "processing/instruction_sets/asm_processor.h"

namespace hlasm_plugin::parser_library::processing {

processing_status lookahead_processor::get_processing_status(const semantics::instruction_si& instruction) const
{
    if (instruction.type == semantics::instruction_si_type::ORD)
    {
        auto status = ordinary_processor::get_instruction_processing_status(
            std::get<context::id_index>(instruction.value), hlasm_ctx);

        if (status)
        {
            status->first.kind = processing_kind::LOOKAHEAD;

            if (status->second.type == context::instruction_type::CA
                || status->second.type == context::instruction_type::MAC)
                status->first.form = processing_form::IGNORED;

            return *status;
        }
    }

    return std::make_pair(processing_format(processing_kind::LOOKAHEAD, processing_form::IGNORED), op_code());
}

void lookahead_processor::process_statement(context::shared_stmt_ptr statement)
{
    if (macro_nest_ == 0)
    {
        find_seq(static_cast<const resolved_statement&>(*statement));
        find_ord(static_cast<const resolved_statement&>(*statement));
    }

    auto resolved = statement->access_resolved();

    if (resolved->opcode_ref().value == macro_id)
    {
        process_MACRO();
    }
    else if (resolved->opcode_ref().value == mend_id)
    {
        process_MEND();
    }
    else if (macro_nest_ == 0 && resolved->opcode_ref().value == copy_id)
    {
        process_COPY(*resolved);
    }
}

void lookahead_processor::end_processing()
{
    hlasm_ctx.pop_statement_processing();

    for (auto&& symbol_name : to_find_)
        register_attr_ref(symbol_name, context::symbol_attributes(context::symbol_origin::UNKNOWN));

    listener_.finish_lookahead(std::move(result_));

    finished_flag_ = true;
}

bool lookahead_processor::terminal_condition(const statement_provider_kind prov_kind) const
{
    return prov_kind == statement_provider_kind::MACRO || prov_kind == statement_provider_kind::OPEN;
}

bool lookahead_processor::finished() { return finished_flag_; }

void lookahead_processor::collect_diags() const {}

lookahead_processor::lookahead_processor(analyzing_context ctx,
    branching_provider& branch_provider,
    processing_state_listener& listener,
    workspaces::parse_lib_provider& lib_provider,
    lookahead_start_data start)
    : statement_processor(processing_kind::LOOKAHEAD, ctx)
    , finished_flag_(start.action == lookahead_action::ORD && start.targets.empty())
    , result_(std::move(start))
    , macro_nest_(0)
    , branch_provider_(branch_provider)
    , listener_(listener)
    , lib_provider_(lib_provider)
    , to_find_(std::move(start.targets))
    , target_(start.target)
    , action(start.action)
    , asm_proc_table_(create_table(*ctx.hlasm_ctx))
{}

void lookahead_processor::process_MACRO() { ++macro_nest_; }
void lookahead_processor::process_MEND() { macro_nest_ -= macro_nest_ == 0 ? 0 : 1; }
void lookahead_processor::process_COPY(const resolved_statement& statement)
{
    if (statement.operands_ref().value.size() == 1 && statement.operands_ref().value.front()->access_asm())
    {
        asm_processor::process_copy(statement, ctx, lib_provider_, nullptr);
    }
}

lookahead_processor::process_table_t lookahead_processor::create_table(context::hlasm_context& h_ctx)
{
    process_table_t table;
    table.emplace(h_ctx.ids().add("CSECT"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("DSECT"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("RSECT"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("COM"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("DXD"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("LOCTR"),
        std::bind(&lookahead_processor::assign_section_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("EQU"),
        std::bind(&lookahead_processor::assign_EQU_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("DC"),
        std::bind(
            &lookahead_processor::assign_data_def_attributes, this, std::placeholders::_1, std::placeholders::_2));
    table.emplace(h_ctx.ids().add("DS"),
        std::bind(
            &lookahead_processor::assign_data_def_attributes, this, std::placeholders::_1, std::placeholders::_2));

    return table;
}

void lookahead_processor::assign_EQU_attributes(context::id_index symbol_name, const resolved_statement& statement)
{
    // type attribute operand
    context::symbol_attributes::type_attr t_attr = context::symbol_attributes::undef_type;
    if (statement.operands_ref().value.size() >= 3
        && statement.operands_ref().value[2]->type == semantics::operand_type::ASM)
    {
        auto asm_op = statement.operands_ref().value[2]->access_asm();
        auto expr_op = asm_op->access_expr();

        if (expr_op && !expr_op->has_error(hlasm_ctx.ord_ctx) && !expr_op->has_dependencies(hlasm_ctx.ord_ctx))
        {
            auto t_value = expr_op->expression->resolve(hlasm_ctx.ord_ctx);
            if (t_value.value_kind() == context::symbol_value_kind::ABS && t_value.get_abs() >= 0
                && t_value.get_abs() <= 255)
                t_attr = (context::symbol_attributes::type_attr)t_value.get_abs();
        }
    }

    // length attribute operand
    context::symbol_attributes::len_attr length_attr = context::symbol_attributes::undef_length;
    if (statement.operands_ref().value.size() >= 2
        && statement.operands_ref().value[1]->type == semantics::operand_type::ASM)
    {
        auto asm_op = statement.operands_ref().value[1]->access_asm();
        auto expr_op = asm_op->access_expr();

        if (expr_op && !expr_op->has_error(hlasm_ctx.ord_ctx) && !expr_op->has_dependencies(hlasm_ctx.ord_ctx))
        {
            auto length_value = expr_op->expression->resolve(hlasm_ctx.ord_ctx);
            if (length_value.value_kind() == context::symbol_value_kind::ABS && length_value.get_abs() >= 0
                && length_value.get_abs() <= 65535)
                length_attr = (context::symbol_attributes::len_attr)length_value.get_abs();
        }
    }

    if (length_attr == context::symbol_attributes::undef_length)
    {
        if (statement.operands_ref().value.size() >= 1
            && statement.operands_ref().value[0]->type == semantics::operand_type::ASM)
        {
            auto asm_op = statement.operands_ref().value[0]->access_asm();
            auto expr_op = asm_op->access_expr();
            if (!expr_op)
                return;

            auto l_term = expr_op->expression->leftmost_term();
            if (auto symbol_term = dynamic_cast<const expressions::mach_expr_symbol*>(l_term))
            {
                auto len_symbol = hlasm_ctx.ord_ctx.get_symbol(symbol_term->value);

                if (len_symbol != nullptr && len_symbol->kind() != context::symbol_value_kind::UNDEF)
                    length_attr = len_symbol->attributes().length();
            }
            else
                length_attr = 1;
        }
    }

    register_attr_ref(symbol_name, context::symbol_attributes(context::symbol_origin::DAT, t_attr, length_attr));
}

void lookahead_processor::assign_data_def_attributes(context::id_index symbol_name, const resolved_statement& statement)
{
    if (statement.operands_ref().value.empty())
        return;

    auto data_op = statement.operands_ref().value.front()->access_data_def();

    if (!data_op)
        return;

    context::symbol_attributes::type_attr type =
        ebcdic_encoding::a2e[(unsigned char)data_op->value->get_type_attribute()];
    ;
    context::symbol_attributes::len_attr len = context::symbol_attributes::undef_length;
    context::symbol_attributes::scale_attr scale = context::symbol_attributes::undef_scale;

    auto tmp = data_op->get_operand_value(hlasm_ctx.ord_ctx);
    auto value = dynamic_cast<checking::data_definition_operand*>(tmp.get());

    if (!data_op->value->length || !data_op->value->length->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
    {
        len = value->get_length_attribute();
    }
    if (data_op->value->scale && !data_op->value->scale->get_dependencies(hlasm_ctx.ord_ctx).contains_dependencies())
    {
        scale = value->get_scale_attribute();
    }

    register_attr_ref(symbol_name, context::symbol_attributes(context::symbol_origin::DAT, type, len, scale));
}

void lookahead_processor::assign_section_attributes(context::id_index symbol_name, const resolved_statement&)
{
    register_attr_ref(symbol_name, context::symbol_attributes::make_section_attrs());
}

void lookahead_processor::assign_machine_attributes(context::id_index symbol_name, const resolved_statement& statement)
{
    auto mnem_tmp = context::instruction::mnemonic_codes.find(*statement.opcode_ref().value);

    auto tmp_instr = mnem_tmp != context::instruction::mnemonic_codes.end()
        ? context::instruction::machine_instructions.find(mnem_tmp->second.instruction)
        : context::instruction::machine_instructions.find(*statement.opcode_ref().value);

    register_attr_ref(symbol_name,
        context::symbol_attributes::make_machine_attrs(
            (context::symbol_attributes::len_attr)tmp_instr->second->size_for_alloc / 8));
}

void lookahead_processor::assign_assembler_attributes(
    context::id_index symbol_name, const resolved_statement& statement)
{
    auto it = asm_proc_table_.find(statement.opcode_ref().value);
    if (it != asm_proc_table_.end())
    {
        auto& [key, func] = *it;
        func(symbol_name, statement);
    }
    else
        register_attr_ref(symbol_name, context::symbol_attributes(context::symbol_origin::MACH, 'M'_ebcdic));
}

void lookahead_processor::find_seq(const semantics::core_statement& statement)
{
    if (statement.label_ref().type == semantics::label_si_type::SEQ)
    {
        const auto& symbol = std::get<semantics::seq_sym>(statement.label_ref().value);

        branch_provider_.register_sequence_symbol(symbol.name, symbol.symbol_range);

        if (symbol.name == target_)
        {
            finished_flag_ = true;
            result_ = lookahead_processing_result(symbol.name, symbol.symbol_range);
        }
    }
}

void lookahead_processor::find_ord(const resolved_statement& statement)
{
    // checks
    if (statement.label_ref().type != semantics::label_si_type::ORD)
        return;

    auto name = std::get<std::string>(statement.label_ref().value);
    auto [valid, id] = context_manager(hlasm_ctx).try_get_symbol_name(std::move(name));
    if (!valid)
        return;

    to_find_.erase(id);

    // find attributes
    // if found ord symbol on CA, macro or undefined instruction, only type attribute is assigned
    // 'U' for CA and 'M' for undefined and macro
    switch (statement.opcode_ref().type)
    {
        case context::instruction_type::CA:
        case context::instruction_type::UNDEF:
        case context::instruction_type::MAC:
            register_attr_ref(id,
                context::symbol_attributes(context::symbol_origin::MACH,
                    statement.opcode_ref().type == context::instruction_type::CA ? 'U'_ebcdic : 'M'_ebcdic));
            break;
        case context::instruction_type::MACH:
            assign_machine_attributes(id, statement);
            break;
        case context::instruction_type::ASM:
            assign_assembler_attributes(id, statement);
            break;
        default:
            assert(false);
            break;
    }

    finished_flag_ = action == lookahead_action::ORD && to_find_.empty();
}

void lookahead_processor::register_attr_ref(context::id_index name, context::symbol_attributes attributes)
{
    hlasm_ctx.ord_ctx.add_symbol_reference(context::symbol(name, context::symbol_value(), attributes, location(), {}));
}

} // namespace hlasm_plugin::parser_library::processing
