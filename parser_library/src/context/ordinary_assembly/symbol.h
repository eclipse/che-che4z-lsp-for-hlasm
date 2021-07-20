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

#ifndef CONTEXT_SYMBOL_H
#define CONTEXT_SYMBOL_H

#include <limits>
#include <variant>

#include "address.h"
#include "location.h"
#include "symbol_attributes.h"

namespace hlasm_plugin::parser_library::context {

// defines kind of symbol value, absolute or relocatable or undefined
enum class symbol_value_kind
{
    UNDEF = 0,
    ABS = 1,
    RELOC = 2
};

// structure holding value of a symbol
struct symbol_value
{
    using abs_value_t = int32_t;
    using reloc_value_t = address;

    symbol_value(abs_value_t value);
    symbol_value(reloc_value_t value);
    symbol_value();

    symbol_value operator+(const symbol_value& value) const;
    symbol_value operator-(const symbol_value& value) const;
    symbol_value operator*(const symbol_value& value) const;
    symbol_value operator/(const symbol_value& value) const;
    symbol_value operator-() const;

    const abs_value_t& get_abs() const;
    const reloc_value_t& get_reloc() const;

    symbol_value_kind value_kind() const;

private:
    std::variant<std::monostate, abs_value_t, reloc_value_t> value_;
};

// class representing ordinary symbol
// the value and attributes fields have the same semantics as described in symbol_attributes
class symbol
{
public:
    symbol(id_index name, symbol_value value, symbol_attributes attributes, location symbol_location);

    const symbol_value& value() const;
    const symbol_attributes& attributes() const;

    symbol_value_kind kind() const;

    void set_value(symbol_value value);
    void set_length(symbol_attributes::len_attr value);
    void set_scale(symbol_attributes::scale_attr value);

    const id_index name;
    const location symbol_location;

private:
    symbol_value value_;
    symbol_attributes attributes_;
};

} // namespace hlasm_plugin::parser_library::context
#endif
