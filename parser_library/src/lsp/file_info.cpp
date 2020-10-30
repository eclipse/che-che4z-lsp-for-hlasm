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

#include "file_info.h"

namespace hlasm_plugin::parser_library::lsp {

file_info::file_info(context::macro_def_ptr owner)
    : name(owner->definition_location.file)
    , type(file_type::MACRO)
    , owner(std::move(owner))
{}

file_info::file_info(context::copy_member_ptr owner)
    : name(owner->definition_location.file)
    , type(file_type::COPY)
    , owner(std::move(owner))
{}

file_slice_t file_slice_t::transform_slice(const macro_slice_t& slice, const macro_info& macro_i)
{
    file_slice_t fslice;

    fslice.begin_idx = slice.begin_statement;
    fslice.end_idx = slice.end_statement;

    fslice.begin_line = macro_i.macro_definition->copy_nests[fslice.begin_idx].back().pos.line;
    fslice.begin_line = macro_i.macro_definition->copy_nests[fslice.end_idx].back().pos.line;

    fslice.type = slice.inner_macro ? scope_type::INNER_MACRO : scope_type::MACRO;
    fslice.macro_context = macro_i;
}

std::vector<file_slice_t> file_slice_t::transform_slices(
    const std::vector<macro_slice_t>& slices, const macro_info& macro_i)
{
    std::vector<file_slice_t> ret;
    for (const auto& s : slices)
        ret.push_back(transform_slice(s, macro_i));
    return ret;
}

} // namespace hlasm_plugin::parser_library::lsp
