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

#include "variable_symbol.h"

#include "expressions/conditional_assembly/terms/ca_constant.h"
#include "processing/context_manager.h"

namespace hlasm_plugin::parser_library::semantics {

basic_variable_symbol::basic_variable_symbol(
    context::id_index name, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range)
    : variable_symbol(false, std::move(subscript), std::move(symbol_range))
    , name(name)
{}

context::id_index basic_variable_symbol::evaluate_name(const expressions::evaluation_context&) const { return name; }

created_variable_symbol::created_variable_symbol(
    concat_chain created_name, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range)
    : variable_symbol(true, std::move(subscript), std::move(symbol_range))
    , created_name(std::move(created_name))
{}

context::id_index created_variable_symbol::evaluate_name(const expressions::evaluation_context& eval_ctx) const
{
    auto str_name = concatenation_point::evaluate(created_name, eval_ctx);

    auto mngr = processing::context_manager(&eval_ctx);

    return mngr.get_symbol_name(str_name, symbol_range);
}

basic_variable_symbol* variable_symbol::access_basic()
{
    return created ? nullptr : static_cast<basic_variable_symbol*>(this);
}

const basic_variable_symbol* variable_symbol::access_basic() const
{
    return created ? nullptr : static_cast<const basic_variable_symbol*>(this);
}

created_variable_symbol* variable_symbol::access_created()
{
    return created ? static_cast<created_variable_symbol*>(this) : nullptr;
}

const created_variable_symbol* variable_symbol::access_created() const
{
    return created ? static_cast<const created_variable_symbol*>(this) : nullptr;
}

vs_eval variable_symbol::evaluate_symbol(const expressions::evaluation_context& eval_ctx) const
{
    return vs_eval(evaluate_name(eval_ctx), evaluate_subscript(eval_ctx));
}

std::vector<context::A_t> variable_symbol::evaluate_subscript(const expressions::evaluation_context& eval_ctx) const
{
    std::vector<context::A_t> eval_subscript;
    for (const auto& expr : subscript)
    {
        auto val = expr->evaluate<context::A_t>(eval_ctx);
        eval_subscript.push_back(val);
    }

    return eval_subscript;
}

context::SET_t variable_symbol::evaluate(const expressions::evaluation_context& eval_ctx) const
{
    auto [name, evaluated_subscript] = evaluate_symbol(eval_ctx);

    processing::context_manager mngr(&eval_ctx);

    auto val = mngr.get_var_sym_value(name, evaluated_subscript, symbol_range);

    return val;
}

variable_symbol::variable_symbol(
    const bool created, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range)
    : created(created)
    , subscript(std::move(subscript))
    , symbol_range(std::move(symbol_range))
{}

} // namespace hlasm_plugin::parser_library::semantics
