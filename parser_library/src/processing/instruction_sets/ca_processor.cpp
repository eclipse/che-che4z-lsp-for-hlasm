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

#include "ca_processor.h"

#include "expressions/conditional_assembly/terms/ca_symbol.h"
#include "semantics/operand_impls.h"
#include "semantics/range_provider.h"

using namespace hlasm_plugin::parser_library;
using namespace processing;
using namespace workspaces;

ca_processor::ca_processor(analyzing_context ctx,
    branching_provider& branch_provider,
    parse_lib_provider& lib_provider,
    processing_state_listener& listener,
    opencode_provider& open_code)
    : instruction_processor(ctx, branch_provider, lib_provider)
    , table_(create_table(*ctx.hlasm_ctx))
    , listener_(listener)
    , open_code_(&open_code)
{}

void ca_processor::process(context::shared_stmt_ptr stmt)
{
    auto res = stmt->access_resolved();
    auto& func = table_.at(res->opcode_ref().value);
    func(*res);
}

ca_processor::process_table_t ca_processor::create_table(context::hlasm_context& h_ctx)
{
    process_table_t table;
    table.emplace(
        h_ctx.ids().add("SETA"), std::bind(&ca_processor::process_SET<context::A_t>, this, std::placeholders::_1));
    table.emplace(
        h_ctx.ids().add("SETB"), std::bind(&ca_processor::process_SET<context::B_t>, this, std::placeholders::_1));
    table.emplace(
        h_ctx.ids().add("SETC"), std::bind(&ca_processor::process_SET<context::C_t>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("LCLA"),
        std::bind(&ca_processor::process_GBL_LCL<context::A_t, false>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("LCLB"),
        std::bind(&ca_processor::process_GBL_LCL<context::B_t, false>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("LCLC"),
        std::bind(&ca_processor::process_GBL_LCL<context::C_t, false>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("GBLA"),
        std::bind(&ca_processor::process_GBL_LCL<context::A_t, true>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("GBLB"),
        std::bind(&ca_processor::process_GBL_LCL<context::B_t, true>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("GBLC"),
        std::bind(&ca_processor::process_GBL_LCL<context::C_t, true>, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("ANOP"), std::bind(&ca_processor::process_ANOP, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("ACTR"), std::bind(&ca_processor::process_ACTR, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("AGO"), std::bind(&ca_processor::process_AGO, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("AIF"), std::bind(&ca_processor::process_AIF, this, std::placeholders::_1));
    table.emplace(context::id_storage::empty_id, std::bind(&ca_processor::process_empty, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("MACRO"), std::bind(&ca_processor::process_MACRO, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("MEND"), std::bind(&ca_processor::process_MEND, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("MEXIT"), std::bind(&ca_processor::process_MEXIT, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("AREAD"), std::bind(&ca_processor::process_AREAD, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("ASPACE"), std::bind(&ca_processor::process_ASPACE, this, std::placeholders::_1));
    table.emplace(h_ctx.ids().add("AEJECT"), std::bind(&ca_processor::process_AEJECT, this, std::placeholders::_1));

    return table;
}

void ca_processor::register_seq_sym(const semantics::complete_statement& stmt)
{
    if (stmt.label_ref().type == semantics::label_si_type::SEQ)
    {
        auto symbol = std::get<semantics::seq_sym>(stmt.label_ref().value);
        branch_provider.register_sequence_symbol(symbol.name, symbol.symbol_range);
    }
    else if (stmt.label_ref().type != semantics::label_si_type::EMPTY)
    {
        add_diagnostic(diagnostic_op::warning_W010("Name field", stmt.label_ref().field_range));
    }
}

template<typename T>
ca_processor::SET_info ca_processor::get_SET_symbol(const semantics::complete_statement& stmt)
{
    if (stmt.label_ref().type != semantics::label_si_type::VAR)
    {
        add_diagnostic(diagnostic_op::error_E010("label", stmt.label_ref().field_range));
        return {};
    }

    auto symbol = std::get<semantics::vs_ptr>(stmt.label_ref().value).get();
    bool is_scalar_expression = symbol->subscript.empty();

    int index = -1;

    auto name = symbol->evaluate_name(eval_ctx);

    context::set_symbol_base* set_sym = nullptr;

    if (auto var_symbol = hlasm_ctx.get_var_sym(name); var_symbol)
    {
        if (var_symbol->access_macro_param_base())
        {
            add_diagnostic(diagnostic_op::error_E030("symbolic parameter", symbol->symbol_range));
            return {};
        }

        set_sym = var_symbol->access_set_symbol_base();
        assert(set_sym);
        if (set_sym->type != context::object_traits<T>::type_enum)
        {
            add_diagnostic(diagnostic_op::error_E013("wrong type of variable symbol", symbol->symbol_range));
            return {};
        }
    }
    else
        set_sym = hlasm_ctx.template create_local_variable<T>(name, is_scalar_expression)->access_set_symbol_base();

    if (symbol->subscript.size() > 1)
    {
        add_diagnostic(diagnostic_op::error_E020("variable symbol subscript", symbol->symbol_range));
        return {};
    }

    if (!is_scalar_expression)
    {
        index = symbol->subscript.front()->evaluate<context::A_t>(eval_ctx);

        if (index < 1)
        {
            add_diagnostic(diagnostic_op::error_E012("subscript value has to be 1 or more", symbol->symbol_range));
            return {};
        }
    }

    if (set_sym->is_scalar ^ is_scalar_expression)
    {
        add_diagnostic(diagnostic_op::error_E013("subscript error", symbol->symbol_range));
        return {};
    }

    return SET_info { set_sym, name, index };
}

template ca_processor::SET_info ca_processor::get_SET_symbol<context::A_t>(const semantics::complete_statement& stmt);
template ca_processor::SET_info ca_processor::get_SET_symbol<context::B_t>(const semantics::complete_statement& stmt);
template ca_processor::SET_info ca_processor::get_SET_symbol<context::C_t>(const semantics::complete_statement& stmt);

bool ca_processor::prepare_SET_operands(
    const semantics::complete_statement& stmt, std::vector<expressions::ca_expression*>& expr_values)
{
    bool has_operand = false;
    for (auto& op : stmt.operands_ref().value)
    {
        if (op->type == semantics::operand_type::EMPTY)
            continue;

        has_operand = true;

        auto ca_op = op->access_ca();
        assert(ca_op);

        if (ca_op->kind != semantics::ca_kind::VAR && ca_op->kind != semantics::ca_kind::EXPR)
        {
            add_diagnostic(diagnostic_op::error_E012("SET instruction", ca_op->operand_range));
            return false;
        }

        expr_values.push_back(ca_op->access_expr()->expression.get());
    }

    if (!has_operand)
    {
        add_diagnostic(diagnostic_op::error_E022("SET instruction", stmt.instruction_ref().field_range));
        return false;
    }
    return true;
}

bool ca_processor::prepare_GBL_LCL(
    const semantics::complete_statement& stmt, std::vector<context::id_index>& ids, std::vector<bool>& scalar_info)
{
    bool has_operand = false;
    for (auto& op : stmt.operands_ref().value)
    {
        if (op->type == semantics::operand_type::EMPTY)
            continue;

        has_operand = true;

        auto ca_op = op->access_ca();
        assert(ca_op);

        if (ca_op->kind == semantics::ca_kind::VAR)
        {
            auto [id, subscript] = ca_op->access_var()->variable_symbol->evaluate_symbol(eval_ctx);

            if (id == context::id_storage::empty_id)
                continue;

            if (auto var_sym = hlasm_ctx.get_var_sym(id))
            {
                if (var_sym->access_set_symbol_base())
                    add_diagnostic(diagnostic_op::error_E051(*id, ca_op->operand_range));
                else if (var_sym->access_macro_param_base())
                    add_diagnostic(diagnostic_op::error_E052(*id, ca_op->operand_range));
                else
                    assert(false);
                continue;
            }
            if (std::find(ids.begin(), ids.end(), id) != ids.end())
            {
                add_diagnostic(diagnostic_op::error_E051(*id, ca_op->operand_range));
            }
            else
            {
                ids.push_back(id);
                scalar_info.push_back(subscript.empty());
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_E010("operand", ca_op->operand_range));
            return false;
        }
    }

    if (!has_operand)
    {
        add_diagnostic(diagnostic_op::error_E022("variable symbol definition", stmt.instruction_ref().field_range));
        return false;
    }

    return true;
}

void ca_processor::process_ANOP(const semantics::complete_statement& stmt)
{
    assert(stmt.operands_ref().value.empty());
    register_seq_sym(stmt);
}

bool ca_processor::prepare_ACTR(const semantics::complete_statement& stmt, context::A_t& ctr)
{
    if (stmt.operands_ref().value.size() != 1)
    {
        add_diagnostic(diagnostic_op::error_E020("operand", stmt.instruction_ref().field_range));
        return false;
    }

    const auto* ca_op = stmt.operands_ref().value[0]->access_ca();
    assert(ca_op);

    if (ca_op->kind == semantics::ca_kind::EXPR || ca_op->kind == semantics::ca_kind::VAR)
    {
        ctr = ca_op->access_expr()->expression->evaluate<context::A_t>(eval_ctx);
        return true;
    }
    else
    {
        add_diagnostic(diagnostic_op::error_E010("operand", ca_op->operand_range));
        return false;
    }
}

void ca_processor::process_ACTR(const semantics::complete_statement& stmt)
{
    register_seq_sym(stmt);

    context::A_t ctr;
    bool ok = prepare_ACTR(stmt, ctr);

    if (ok)
        hlasm_ctx.set_branch_counter(ctr);
}

bool ca_processor::prepare_AGO(const semantics::complete_statement& stmt,
    context::A_t& branch,
    std::vector<std::pair<context::id_index, range>>& targets)
{
    if (stmt.operands_ref().value.empty())
    {
        add_diagnostic(diagnostic_op::error_E022("AGO", stmt.instruction_ref().field_range));
        return false;
    }

    for (const auto& op : stmt.operands_ref().value)
    {
        if (op->type == semantics::operand_type::EMPTY)
        {
            add_diagnostic(diagnostic_op::error_E010("operand", op->operand_range));
            return false;
        }
    }

    auto ca_op = stmt.operands_ref().value[0]->access_ca();
    assert(ca_op);

    if (ca_op->kind == semantics::ca_kind::SEQ)
    {
        if (stmt.operands_ref().value.size() != 1)
        {
            add_diagnostic(diagnostic_op::error_E010("operand", ca_op->operand_range));
            return false;
        }

        auto& symbol = ca_op->access_seq()->sequence_symbol;
        targets.emplace_back(symbol.name, symbol.symbol_range);
        branch = 1;
        return true;
    }

    if (ca_op->kind == semantics::ca_kind::BRANCH)
    {
        auto br_op = ca_op->access_branch();
        branch = br_op->expression->evaluate<context::A_t>(eval_ctx);
        targets.emplace_back(br_op->sequence_symbol.name, br_op->sequence_symbol.symbol_range);

        for (size_t i = 1; i < stmt.operands_ref().value.size(); ++i)
        {
            auto tmp = stmt.operands_ref().value[i]->access_ca();
            assert(tmp);

            if (tmp->kind == semantics::ca_kind::SEQ)
            {
                auto& symbol = tmp->access_seq()->sequence_symbol;
                targets.emplace_back(symbol.name, symbol.symbol_range);
            }
            else
            {
                add_diagnostic(diagnostic_op::error_E010("operand", tmp->operand_range));
                return false;
            }
        }
        return true;
    }
    return false;
}

void ca_processor::process_AGO(const semantics::complete_statement& stmt)
{
    register_seq_sym(stmt);

    context::A_t branch;
    std::vector<std::pair<context::id_index, range>> targets;
    bool ok = prepare_AGO(stmt, branch, targets);
    if (!ok)
        return;

    if (branch > 0 && branch <= (context::A_t)targets.size())
        branch_provider.jump_in_statements(targets[branch - 1].first, targets[branch - 1].second);
}

bool ca_processor::prepare_AIF(
    const semantics::complete_statement& stmt, context::B_t& condition, context::id_index& target, range& target_range)
{
    condition = false;

    if (stmt.operands_ref().value.empty())
    {
        add_diagnostic(diagnostic_op::error_E022("AIF", stmt.instruction_ref().field_range));
        return false;
    }

    bool has_operand = false;
    for (auto it = stmt.operands_ref().value.begin(); it != stmt.operands_ref().value.end(); ++it)
    {
        const auto& op = *it;

        if (op->type == semantics::operand_type::EMPTY)
        {
            if (it == stmt.operands_ref().value.end() - 1)
                continue;

            add_diagnostic(diagnostic_op::error_E010("operand", op->operand_range));
            return false;
        }
        has_operand = true;

        auto ca_op = op->access_ca();
        assert(ca_op);

        if (ca_op->kind == semantics::ca_kind::BRANCH)
        {
            if (!condition)
            {
                auto br = ca_op->access_branch();
                condition = br->expression->evaluate<context::B_t>(eval_ctx);

                target = br->sequence_symbol.name;
                target_range = br->sequence_symbol.symbol_range;
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_E010("operand", ca_op->operand_range));
            return false;
        }
    }

    if (!has_operand)
    {
        add_diagnostic(diagnostic_op::error_E022("variable symbol definition", stmt.instruction_ref().field_range));
        return false;
    }

    return true;
}

void ca_processor::process_AIF(const semantics::complete_statement& stmt)
{
    register_seq_sym(stmt);

    context::B_t condition;
    context::id_index target;
    range target_range;
    bool ok = prepare_AIF(stmt, condition, target, target_range);

    if (!ok)
        return;

    if (condition)
        branch_provider.jump_in_statements(target, target_range);
}

void ca_processor::process_MACRO(const semantics::complete_statement& stmt)
{
    register_seq_sym(stmt);
    listener_.start_macro_definition({});
}

void ca_processor::process_MEXIT(const semantics::complete_statement& stmt)
{
    if (!hlasm_ctx.is_in_macro())
        add_diagnostic(diagnostic_op::error_E054(stmt.stmt_range_ref()));
    else
        hlasm_ctx.leave_macro();
}

void ca_processor::process_MEND(const semantics::complete_statement& stmt)
{
    if (!hlasm_ctx.is_in_macro())
        add_diagnostic(diagnostic_op::error_E054(stmt.stmt_range_ref()));
}

void ca_processor::process_AEJECT(const semantics::complete_statement&)
{
    // TODO
}

void ca_processor::process_ASPACE(const semantics::complete_statement& stmt)
{
    // TODO
    (void)stmt;
}

namespace {
struct AREAD_operand_visitor final : public semantics::operand_visitor
{
    explicit AREAD_operand_visitor(expressions::evaluation_context* ectx)
        : eval_ctx(ectx)
    {}

    expressions::evaluation_context* eval_ctx = nullptr;
    context::id_index value = nullptr;

    void visit(const semantics::empty_operand&) override {}
    void visit(const semantics::model_operand&) override {}
    void visit(const semantics::expr_machine_operand&) override {}
    void visit(const semantics::address_machine_operand&) override {}
    void visit(const semantics::expr_assembler_operand&) override {}
    void visit(const semantics::using_instr_assembler_operand&) override {}
    void visit(const semantics::complex_assembler_operand&) override {}
    void visit(const semantics::string_assembler_operand&) override {}
    void visit(const semantics::data_def_operand&) override {}
    void visit(const semantics::var_ca_operand&) override {}
    void visit(const semantics::expr_ca_operand& op) override
    {
        if (const auto* symbol = dynamic_cast<const expressions::ca_symbol*>(op.expression.get()))
            value = symbol->symbol;
    }
    void visit(const semantics::seq_ca_operand&) override {}
    void visit(const semantics::branch_ca_operand&) override {}
    void visit(const semantics::macro_operand_chain&) override {}
    void visit(const semantics::macro_operand_string&) override {}
};
} // namespace

void ca_processor::process_AREAD(const semantics::complete_statement& stmt)
{
    if (stmt.label_ref().type != semantics::label_si_type::VAR)
    {
        add_diagnostic(diagnostic_op::error_E010("label", stmt.label_ref().field_range));
        return;
    }
    if (!hlasm_ctx.current_scope().is_in_macro())
    {
        add_diagnostic(diagnostic_op::error_E069(stmt.instruction_ref().field_range));
        return;
    }

    auto& ops = stmt.operands_ref();
    enum class aread_variant
    {
        invalid,
        reader,
        clockb,
        clockd,
    } const variant = [&eval_ctx = eval_ctx, &ops]() {
        if (ops.value.size() == 0)
            return aread_variant::reader;
        if (ops.value.size() == 2 && ops.value[0]->type == semantics::operand_type::EMPTY
            && ops.value[1]->type == semantics::operand_type::EMPTY)
            return aread_variant::reader;

        AREAD_operand_visitor op(&eval_ctx);
        ops.value.at(0)->apply(op);

        if (op.value == nullptr)
            return aread_variant::invalid;

        static const std::initializer_list<std::pair<std::string_view, aread_variant>> allowed_operands = {
            { "NOSTMT", aread_variant::reader },
            { "NOPRINT", aread_variant::reader },
            { "CLOCKB", aread_variant::clockb },
            { "CLOCKD", aread_variant::clockd },
        };
        auto idx = std::find_if(allowed_operands.begin(),
            allowed_operands.end(),
            [value = std::string_view(*op.value)](const auto& p) { return p.first == value; });
        if (idx == allowed_operands.end())
            return aread_variant::invalid;
        return idx->second;
    }();

    if (variant == aread_variant::invalid)
    {
        add_diagnostic(diagnostic_op::error_E070(ops.field_range));
        return;
    }

    auto [set_symbol, name, index] = get_SET_symbol<context::C_t>(stmt);

    if (!set_symbol)
        return;

    constexpr const auto since_midnight = []() -> std::chrono::nanoseconds {
        using namespace std::chrono;

        const auto now = system_clock::now();

        const auto now_t = system_clock::to_time_t(now);
        const auto tm = *std::localtime(&now_t);

        const auto subsecond = now - std::chrono::floor<std::chrono::seconds>(now);

        return hours(tm.tm_hour) + minutes(tm.tm_min) + seconds(tm.tm_sec) + subsecond;
    };

    std::string value_to_set;
    switch (variant)
    {
        case aread_variant::reader:
            value_to_set = open_code_->aread();
            break;

        case aread_variant::clockb:
            value_to_set = time_to_clockb(since_midnight());
            break;

        case aread_variant::clockd:
            value_to_set = time_to_clockd(since_midnight());
            break;
    }
    set_symbol->access_set_symbol<context::C_t>()->set_value(std::move(value_to_set), index - 1);
}

void ca_processor::process_empty(const semantics::complete_statement&) {}

template<typename T>
void ca_processor::process_SET(const semantics::complete_statement& stmt)
{
    std::vector<expressions::ca_expression*> expr_values;
    auto [set_symbol, name, index] = get_SET_symbol<T>(stmt);

    if (!set_symbol)
        return;

    if (!prepare_SET_operands(stmt, expr_values))
        return;

    for (size_t i = 0; i < expr_values.size(); i++)
    {
        // first obtain a place to put the result in
        auto& val = set_symbol->template access_set_symbol<T>()->reserve_value(index - 1 + i);
        // then evaluate the new value and save it
        val = expr_values[i]->template evaluate<T>(eval_ctx);
    }
}

template void ca_processor::process_SET<context::A_t>(const semantics::complete_statement& stmt);
template void ca_processor::process_SET<context::B_t>(const semantics::complete_statement& stmt);
template void ca_processor::process_SET<context::C_t>(const semantics::complete_statement& stmt);

template<typename T, bool global>
void ca_processor::process_GBL_LCL(const semantics::complete_statement& stmt)
{
    register_seq_sym(stmt);

    std::vector<context::id_index> ids;
    std::vector<bool> scalar_info;
    bool ok = prepare_GBL_LCL(stmt, ids, scalar_info);

    if (!ok)
        return;

    for (size_t i = 0; i < ids.size(); ++i)
    {
        if constexpr (global)
            hlasm_ctx.create_global_variable<T>(ids[i], scalar_info[i]);
        else
            hlasm_ctx.create_local_variable<T>(ids[i], scalar_info[i]);
    }
}

template void ca_processor::process_GBL_LCL<context::A_t, false>(const semantics::complete_statement& stmt);
template void ca_processor::process_GBL_LCL<context::B_t, false>(const semantics::complete_statement& stmt);
template void ca_processor::process_GBL_LCL<context::C_t, false>(const semantics::complete_statement& stmt);
template void ca_processor::process_GBL_LCL<context::A_t, true>(const semantics::complete_statement& stmt);
template void ca_processor::process_GBL_LCL<context::B_t, true>(const semantics::complete_statement& stmt);
template void ca_processor::process_GBL_LCL<context::C_t, true>(const semantics::complete_statement& stmt);
