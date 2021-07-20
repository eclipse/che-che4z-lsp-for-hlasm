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

#ifndef CONTEXT_SET_SYMBOL_H
#define CONTEXT_SET_SYMBOL_H

#include <map>
#include <vector>

#include "variable.h"

namespace hlasm_plugin::parser_library::context {

template<typename T>
class set_symbol;
using set_sym_ptr = std::shared_ptr<set_symbol_base>;

// base class for set_symbols
class set_symbol_base : public variable_symbol
{
public:
    // describes whether set symbol is scalar
    // when scalar, sublist notation is not allowed
    const bool is_scalar;

    // returns type of set symbol
    const SET_t_enum type;

    // casts this to specialized set symbol
    template<typename T>
    set_symbol<T>* access_set_symbol()
    {
        return (type == object_traits<T>::type_enum) ? static_cast<set_symbol<T>*>(this) : nullptr;
    }

    template<typename T>
    const set_symbol<T>* access_set_symbol() const
    {
        return (type == object_traits<T>::type_enum) ? static_cast<const set_symbol<T>*>(this) : nullptr;
    }

    virtual std::vector<size_t> keys() const = 0;
    virtual size_t size() const = 0;

protected:
    set_symbol_base(id_index name, bool is_scalar, bool is_global, SET_t_enum type);
};

// specialized set symbol holding data T (int = A_t, bool = B_t, std::string=C_t)
template<typename T>
class set_symbol : public set_symbol_base
{
    static_assert(object_traits<T>::type_enum != SET_t_enum::UNDEF_TYPE, "Not a SET variable type.");

    // data holding this set_symbol
    // can be scalar or only array of scallars - no other nesting allowed
    std::map<size_t, T> data;

public:
    set_symbol(id_index name, bool is_scalar, bool is_global)
        : set_symbol_base(name, is_scalar, is_global, object_traits<T>::type_enum)
    {}

    // gets value from non scalar set symbol
    // if data at idx is not set or it does not exists, default is returned
    const T& get_value(size_t idx) const
    {
        if (is_scalar)
            return object_traits<T>::default_v();

        auto tmp = data.find(idx);
        if (tmp == data.end())
            return object_traits<T>::default_v();
        return tmp->second;
    }

    // gets value from scalar set symbol
    const T& get_value() const
    {
        if (!is_scalar)
            return object_traits<T>::default_v();

        auto tmp = data.find(0);
        if (tmp == data.end())
            return object_traits<T>::default_v();
        return tmp->second;
    }

    // sets value to scalar set symbol
    void set_value(T value) { data.insert_or_assign(0, std::move(value)); }

    // sets value to non scalar set symbol
    // any index can be accessed
    void set_value(T value, size_t idx)
    {
        if (is_scalar)
            data.insert_or_assign(0, std::move(value));
        else
            data.insert_or_assign(idx, std::move(value));
    }

    // reserves storage for the object value
    T& reserve_value() { return data[0]; }

    // reserves storage for the object value
    // any index can be accessed
    T& reserve_value(size_t idx)
    {
        if (is_scalar)
            return data[0];
        else
            return data[idx];
    }

    // N' attribute of the symbol
    A_t number(std::vector<size_t>) const override
    {
        return (A_t)(is_scalar || data.empty() ? 0 : data.rbegin()->first + 1);
    }

    // K' attribute of the symbol
    A_t count(std::vector<size_t> offset) const override;

    size_t size() const override { return data.size(); };

    std::vector<size_t> keys() const override
    {
        std::vector<size_t> keys;
        keys.reserve(data.size());
        for (auto& [key, value] : data)
            keys.push_back(key);
        return keys;
    }

private:
    const T* get_data(std::vector<size_t> offset) const
    {
        if ((is_scalar && !offset.empty()) || (!is_scalar && offset.size() != 1))
            return nullptr;

        auto tmp_offs = is_scalar ? 0 : offset.front() - 1;

        if (data.find(tmp_offs) == data.end())
            return nullptr;

        return &data.at(tmp_offs);
    }
};


template<>
inline A_t set_symbol<A_t>::count(std::vector<size_t> offset) const
{
    auto tmp = get_data(std::move(offset));
    return tmp ? (A_t)std::to_string(*tmp).size() : (A_t)1;
}

template<>
inline A_t set_symbol<B_t>::count(std::vector<size_t>) const
{
    return (A_t)1;
}

template<>
inline A_t set_symbol<C_t>::count(std::vector<size_t> offset) const
{
    auto tmp = get_data(std::move(offset));
    return tmp ? (A_t)tmp->size() : (A_t)0;
}

} // namespace hlasm_plugin::parser_library::context

#endif
