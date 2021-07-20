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

#ifndef PROCESSING_CA_PROCESSOR_H
#define PROCESSING_CA_PROCESSOR_H

#include "aread_time.h"
#include "instruction_processor.h"
#include "processing/context_manager.h"
#include "processing/opencode_provider.h"
#include "processing/processing_state_listener.h"
#include "semantics/operand_visitor.h"

namespace hlasm_plugin::parser_library::processing {

// processor of conditional assembly instructions
class ca_processor : public instruction_processor
{
    using process_table_t =
        std::unordered_map<context::id_index, std::function<void(const semantics::complete_statement&)>>;

    const process_table_t table_;
    processing_state_listener& listener_;

public:
    ca_processor(analyzing_context ctx,
        branching_provider& branch_provider,
        workspaces::parse_lib_provider& lib_provider,
        processing_state_listener& listener,
        opencode_provider& open_code);

    void process(context::shared_stmt_ptr stmt) override;

private:
    opencode_provider* open_code_;

    process_table_t create_table(context::hlasm_context& hlasm_ctx);

    void register_seq_sym(const semantics::complete_statement& stmt);

    struct SET_info
    {
        context::set_symbol_base* symbol;
        context::id_index name;
        int index;
    };

    template<typename T>
    SET_info get_SET_symbol(const semantics::complete_statement& stmt);
    bool prepare_SET_operands(
        const semantics::complete_statement& stmt, std::vector<expressions::ca_expression*>& expr_values);

    template<typename T>
    void process_SET(const semantics::complete_statement& stmt);

    bool prepare_GBL_LCL(
        const semantics::complete_statement& stmt, std::vector<context::id_index>& ids, std::vector<bool>& scalar_info);

    template<typename T, bool global>
    void process_GBL_LCL(const semantics::complete_statement& stmt);

    void process_ANOP(const semantics::complete_statement& stmt);

    bool prepare_ACTR(const semantics::complete_statement& stmt, context::A_t& ctr);
    void process_ACTR(const semantics::complete_statement& stmt);

    bool prepare_AGO(const semantics::complete_statement& stmt,
        context::A_t& branch,
        std::vector<std::pair<context::id_index, range>>& targets);
    void process_AGO(const semantics::complete_statement& stmt);

    bool prepare_AIF(const semantics::complete_statement& stmt,
        context::B_t& condition,
        context::id_index& target,
        range& target_range);
    void process_AIF(const semantics::complete_statement& stmt);

    void process_MACRO(const semantics::complete_statement& stmt);
    void process_MEXIT(const semantics::complete_statement& stmt);
    void process_MEND(const semantics::complete_statement& stmt);
    void process_AEJECT(const semantics::complete_statement& stmt);
    void process_ASPACE(const semantics::complete_statement& stmt);
    void process_AREAD(const semantics::complete_statement& stmt);

    void process_empty(const semantics::complete_statement&);
};

} // namespace hlasm_plugin::parser_library::processing

#endif
