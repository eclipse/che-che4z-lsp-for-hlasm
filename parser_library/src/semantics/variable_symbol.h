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

#ifndef SEMANTICS_VARIABLE_SYMBOL_H
#define SEMANTICS_VARIABLE_SYMBOL_H

#include <memory>
#include <utility>
#include <vector>

#include "concatenation.h"
#include "context/id_storage.h"
#include "expressions/conditional_assembly/ca_expression.h"
#include "range.h"

// this file is a composition of structures that create concat_chain
// concat_chain is used to represent model statement fields

namespace hlasm_plugin::parser_library::semantics {

struct variable_symbol;
struct basic_variable_symbol;
struct created_variable_symbol;

using vs_ptr = std::unique_ptr<variable_symbol>;
using vs_eval = std::pair<context::id_index, std::vector<context::A_t>>;

struct variable_symbol
{
    const bool created;

    std::vector<expressions::ca_expr_ptr> subscript;

    const range symbol_range;

    basic_variable_symbol* access_basic();
    const basic_variable_symbol* access_basic() const;
    created_variable_symbol* access_created();
    const created_variable_symbol* access_created() const;

    vs_eval evaluate_symbol(const expressions::evaluation_context& eval_ctx) const;
    std::vector<context::A_t> evaluate_subscript(const expressions::evaluation_context& eval_ctx) const;
    context::SET_t evaluate(const expressions::evaluation_context& eval_ctx) const;

    virtual context::id_index evaluate_name(const expressions::evaluation_context& eval_ctx) const = 0;

    virtual ~variable_symbol() = default;

protected:
    variable_symbol(const bool created, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range);
};

struct basic_variable_symbol : variable_symbol
{
    basic_variable_symbol(context::id_index name, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range);

    const context::id_index name;

    context::id_index evaluate_name(const expressions::evaluation_context& eval_ctx) const override;
};

struct created_variable_symbol : variable_symbol
{
    created_variable_symbol(
        concat_chain created_name, std::vector<expressions::ca_expr_ptr> subscript, range symbol_range);

    const concat_chain created_name;

    context::id_index evaluate_name(const expressions::evaluation_context& eval_ctx) const override;
};

} // namespace hlasm_plugin::parser_library::semantics

#endif
