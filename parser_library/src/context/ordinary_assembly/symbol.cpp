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

#include "symbol.h"

#include <assert.h>
#include <stdexcept>

using namespace hlasm_plugin::parser_library;
using namespace hlasm_plugin::parser_library::context;

symbol_value::symbol_value(abs_value_t value)
    : value_(value)
{}

symbol_value::symbol_value(reloc_value_t value)
    : value_(std::move(value))
{}

symbol_value::symbol_value() = default;

symbol_value symbol_value::operator+(const symbol_value& value) const
{
    if (value_kind() == symbol_value_kind::UNDEF || value.value_kind() == symbol_value_kind::UNDEF)
        return symbol_value();

    if (value_kind() == symbol_value_kind::ABS)
    {
        if (value.value_kind() == symbol_value_kind::ABS)
            return get_abs() + value.get_abs();
        else
            return value.get_reloc() + get_abs();
    }
    else if (value_kind() == symbol_value_kind::RELOC)
    {
        if (value.value_kind() == symbol_value_kind::ABS)
            return get_reloc() + value.get_abs();
        else
        {
            auto tmp_val(get_reloc() + value.get_reloc());
            if (tmp_val.bases().empty() && !tmp_val.has_unresolved_space())
                return tmp_val.offset();
            else
                return tmp_val;
        }
    }
    else
    {
        assert(false);
        return symbol_value();
    }
}

symbol_value symbol_value::operator-(const symbol_value& value) const
{
    if (value_kind() == symbol_value_kind::UNDEF || value.value_kind() == symbol_value_kind::UNDEF)
        return symbol_value();

    if (value_kind() == symbol_value_kind::ABS)
    {
        if (value.value_kind() == symbol_value_kind::ABS)
            return get_abs() - value.get_abs();
        else
            return -value.get_reloc() + get_abs();
    }
    else if (value_kind() == symbol_value_kind::RELOC)
    {
        if (value.value_kind() == symbol_value_kind::ABS)
            return get_reloc() - value.get_abs();
        else
        {
            auto tmp_val(get_reloc() - value.get_reloc());
            if (tmp_val.bases().empty() && !tmp_val.has_unresolved_space())
                return tmp_val.offset();
            else
                return tmp_val;
        }
    }
    else
    {
        assert(false);
        return symbol_value();
    }
}

symbol_value symbol_value::operator*(const symbol_value& value) const
{
    if (value_kind() == symbol_value_kind::ABS && value.value_kind() == symbol_value_kind::ABS)
        return get_abs() * value.get_abs();

    return symbol_value();
}

symbol_value symbol_value::operator/(const symbol_value& value) const
{
    if (value_kind() == symbol_value_kind::ABS && value.value_kind() == symbol_value_kind::ABS)
        return value.get_abs() == 0 ? 0 : get_abs() / value.get_abs();

    return symbol_value();
}

symbol_value symbol_value::operator-() const
{
    switch (value_kind())
    {
        case symbol_value_kind::ABS:
            return -get_abs();
        case symbol_value_kind::RELOC:
            return -get_reloc();
        case symbol_value_kind::UNDEF:
            return symbol_value();
        default:
            assert(false);
            return symbol_value();
    }
}

const symbol_value::abs_value_t& symbol_value::get_abs() const { return std::get<abs_value_t>(value_); }

const symbol_value::reloc_value_t& symbol_value::get_reloc() const { return std::get<reloc_value_t>(value_); }

symbol_value_kind symbol_value::value_kind() const { return static_cast<symbol_value_kind>(value_.index()); }

symbol::symbol(id_index name, symbol_value value, symbol_attributes attributes, location symbol_location)
    : name(name)
    , symbol_location(std::move(symbol_location))
    , value_(std::move(value))
    , attributes_(attributes)
{}

const symbol_value& symbol::value() const { return value_; }

const symbol_attributes& symbol::attributes() const { return attributes_; }

symbol_value_kind symbol::kind() const { return value_.value_kind(); }

void symbol::set_value(symbol_value value)
{
    if (value.value_kind() == symbol_value_kind::UNDEF)
        throw std::runtime_error("can not assign undefined value");

    if (kind() != symbol_value_kind::UNDEF)
        throw std::runtime_error("can not assign value to already defined symbol");

    value_ = std::move(value);
}

void symbol::set_length(symbol_attributes::len_attr value) { attributes_.length(value); }

void symbol::set_scale(symbol_attributes::scale_attr value) { attributes_.scale(value); }
