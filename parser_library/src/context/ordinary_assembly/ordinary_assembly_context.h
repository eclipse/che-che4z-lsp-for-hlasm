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

#ifndef CONTEXT_ORDINARY_ASSEMBLY_CONTEXT_H
#define CONTEXT_ORDINARY_ASSEMBLY_CONTEXT_H

#include <unordered_map>

#include "alignment.h"
#include "dependable.h"
#include "location_counter.h"
#include "loctr_dependency_resolver.h"
#include "section.h"
#include "symbol.h"
#include "symbol_dependency_tables.h"


namespace hlasm_plugin::parser_library::context {

class hlasm_context;

// class holding complete information about the 'ordinary assembly' (assembler and machine instructions)
// it contains 'sections' ordinary 'symbols' and all dependencies between them
class ordinary_assembly_context : public dependency_solver
{
    // list of visited sections
    std::vector<std::unique_ptr<section>> sections_;
    // list of visited symbols
    std::unordered_map<id_index, symbol> symbols_;
    // list of lookaheaded symbols
    std::unordered_map<id_index, symbol> symbol_refs_;

    section* curr_section_;

public:
    // access id storage
    id_storage& ids;
    const hlasm_context& hlasm_ctx_;
    // access sections
    const std::vector<std::unique_ptr<section>>& sections() const;

    // access symbol dependency table
    symbol_dependency_tables symbol_dependencies;

    ordinary_assembly_context(id_storage& storage, const hlasm_context& hlasm_ctx);

    // creates symbol
    // returns false if loctr cycle has occured
    [[nodiscard]] bool create_symbol(
        id_index name, symbol_value value, symbol_attributes attributes, location symbol_location);

    void add_symbol_reference(symbol sym);
    const symbol* get_symbol_reference(context::id_index name) const;

    // gets symbol by name
    virtual const symbol* get_symbol(id_index name) const override;
    symbol* get_symbol(id_index name);

    // access current section
    const section* current_section() const;

    // sets current section
    void set_section(id_index name, const section_kind kind, location symbol_location);

    // sets current location counter of current section
    void set_location_counter(id_index name, location symbol_location);

    // sets value of the current location counter
    void set_location_counter_value(const address& addr,
        size_t boundary,
        int offset,
        const resolvable* undefined_address,
        post_stmt_ptr dependency_source);
    space_ptr set_location_counter_value_space(const address& addr,
        size_t boundary,
        int offset,
        const resolvable* undefined_address,
        post_stmt_ptr dependency_source);

    // sets next available value for the current location counter
    void set_available_location_counter_value(size_t boundary, int offset);

    // check whether symbol is already defined
    bool symbol_defined(id_index name);
    // check whether section is already defined
    bool section_defined(id_index name, const section_kind kind);
    // check whether location counter is already defined
    bool counter_defined(id_index name);

    // reserves storage area of specified length and alignment
    address reserve_storage_area(size_t length, alignment align);

    // aligns storage
    address align(alignment align);

    // adds space to the current location counter
    space_ptr register_ordinary_space(alignment align);

    // creates layout of every section
    void finish_module_layout(loctr_dependency_resolver* resolver);

    const std::unordered_map<id_index, symbol>& get_all_symbols();

private:
    void create_private_section();
    std::pair<address, space_ptr> reserve_storage_area_space(size_t length, alignment align);
};

} // namespace hlasm_plugin::parser_library::context

#endif