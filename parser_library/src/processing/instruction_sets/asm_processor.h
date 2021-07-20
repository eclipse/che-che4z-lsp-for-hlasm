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

#ifndef PROCESSING_ASM_PROCESSOR_H
#define PROCESSING_ASM_PROCESSOR_H

#include "low_language_processor.h"
#include "processing/opencode_provider.h"
#include "semantics/operand_impls.h"
#include "workspaces/parse_lib_provider.h"

namespace hlasm_plugin::parser_library::processing {

// processor of assembler instructions
class asm_processor : public low_language_processor
{
    using process_table_t = std::unordered_map<context::id_index, std::function<void(rebuilt_statement)>>;

    const process_table_t table_;
    checking::assembler_checker checker_;

public:
    asm_processor(analyzing_context ctx,
        branching_provider& branch_provider,
        workspaces::parse_lib_provider& lib_provider,
        statement_fields_parser& parser,
        opencode_provider& open_code);

    void process(context::shared_stmt_ptr stmt) override;

    static bool process_copy(const semantics::complete_statement& stmt,
        analyzing_context ctx,
        workspaces::parse_lib_provider& lib_provider,
        diagnosable_ctx* diagnoser);

private:
    opencode_provider* open_code_;
    process_table_t create_table(context::hlasm_context& hlasm_ctx);

    context::id_index find_sequence_symbol(const rebuilt_statement& stmt);

    void process_sect(const context::section_kind kind, rebuilt_statement stmt);
    void process_LOCTR(rebuilt_statement stmt);
    void process_EQU(rebuilt_statement stmt);
    void process_DC(rebuilt_statement stmt);
    void process_DS(rebuilt_statement stmt);
    void process_COPY(rebuilt_statement stmt);
    void process_EXTRN(rebuilt_statement stmt);
    void process_WXTRN(rebuilt_statement stmt);
    void process_ORG(rebuilt_statement stmt);
    void process_OPSYN(rebuilt_statement stmt);
    void process_AINSERT(rebuilt_statement stmt);

    template<checking::data_instr_type instr_type>
    void process_data_instruction(rebuilt_statement stmt);

    std::optional<context::A_t> try_get_abs_value(const semantics::simple_expr_operand* op) const;
};

} // namespace hlasm_plugin::parser_library::processing
#endif