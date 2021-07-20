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

#include "data_definition.h"

#include <assert.h>
#include <limits>
#include <set>
#include <stdexcept>

#include "checking/data_definition/data_def_fields.h"
#include "checking/data_definition/data_def_type_base.h"
#include "mach_expr_term.h"
#include "semantics/collector.h"

using namespace hlasm_plugin::parser_library::expressions;
using namespace hlasm_plugin::parser_library;


data_definition data_definition::create(
    semantics::collector& coll, std::string format, mach_expr_list exprs, nominal_value_ptr nominal, position begin)
{
    parser p(coll, std::move(format), std::move(exprs), std::move(nominal), begin);
    return p.parse();
}

void insert_deps(
    context::dependency_collector& into, context::dependency_solver& solver, const context::dependable* from)
{
    if (from)
        into = into + from->get_dependencies(solver);
}

constexpr char V_type = 'V';

context::dependency_collector data_definition::get_dependencies(context::dependency_solver& solver) const
{
    context::dependency_collector conjuction;
    // In V type, the symbols are external, it is not defined in current program and does not
    // have any dependencies.
    if (type != V_type)
        insert_deps(conjuction, solver, nominal_value.get());
    insert_deps(conjuction, solver, dupl_factor.get());
    insert_deps(conjuction, solver, length.get());
    insert_deps(conjuction, solver, scale.get());
    insert_deps(conjuction, solver, exponent.get());
    return conjuction;
}

context::dependency_collector data_definition::get_length_dependencies(context::dependency_solver& solver) const
{
    context::dependency_collector conjuction;
    insert_deps(conjuction, solver, dupl_factor.get());
    insert_deps(conjuction, solver, length.get());
    return conjuction;
}

const checking::data_def_type* data_definition::access_data_def_type() const
{
    return checking::data_def_type::access_data_def_type(type, extension);
}

context::alignment data_definition::get_alignment() const
{
    auto def_type = access_data_def_type();
    if (def_type)
        return def_type->get_alignment(length != nullptr);
    else
        return context::no_align;
}

char hlasm_plugin::parser_library::expressions::data_definition::get_type_attribute() const
{
    switch (type)
    {
        case 'B':
        case 'C':
        case 'P':
        case 'X':
        case 'Z':
            return type;
        case 'G':
            return '@';
        default:
            break;
    }
    if (length == nullptr)
    {
        switch (type)
        {
            case 'A':
            case 'J':
                return 'A';
            case 'D':
            case 'E':
            case 'F':
            case 'H':
            case 'L':
            case 'Q':
            case 'S':
            case 'V':
            case 'Y':
                return type;
            case 'R':
                return 'V';
            default:
                break;
        }
    }
    else
    {
        switch (type)
        {
            case 'F':
            case 'H':
                return 'G';
            case 'E':
            case 'D':
            case 'L':
                return 'K';
            case 'A':
            case 'S':
            case 'Q':
            case 'J':
            case 'R':
            case 'V':
            case 'Y':
                return 'R';
            default:
                break;
        }
    }
    return 'U';
}

int32_t data_definition::get_scale_attribute(expressions::mach_evaluate_info info) const
{
    auto def_type = access_data_def_type();
    if (def_type)
        return def_type->get_scale_attribute(evaluate_scale(info), evaluate_nominal_value(info));
    else
        return 0;
}

uint32_t data_definition::get_length_attribute(expressions::mach_evaluate_info info) const
{
    auto def_type = access_data_def_type();
    if (def_type)
        return def_type->get_length_attribute(evaluate_length(info), evaluate_nominal_value(info));
    else
        return 0;
}

int32_t data_definition::get_integer_attribute(expressions::mach_evaluate_info info) const
{
    auto def_type = access_data_def_type();
    if (def_type)
        return def_type->get_integer_attribute(
            evaluate_length(info), evaluate_scale(info), evaluate_nominal_value(info));
    else
        return 0;
}

bool data_definition::expects_single_symbol() const
{
    auto def_type = access_data_def_type();
    if (def_type)
        return def_type->expects_single_symbol();
    else
        return false;
}

bool data_definition::check_single_symbol_ok(const diagnostic_collector& add_diagnostic) const
{
    if (!expects_single_symbol() || !nominal_value)
        return true;
    if (!nominal_value->access_exprs())
        return true;

    bool ret = true;
    for (const auto& expr_or_addr : nominal_value->access_exprs()->exprs)
    {
        if (!std::holds_alternative<mach_expr_ptr>(expr_or_addr))
        {
            add_diagnostic(
                diagnostic_op::error_D030({ std::get<address_nominal>(expr_or_addr).base->get_range().start,
                                              std::get<address_nominal>(expr_or_addr).base->get_range().end },
                    std::string(1, type)));
            ret = false;
            continue;
        }
        const mach_expression* expr = std::get<mach_expr_ptr>(expr_or_addr).get();
        auto symbol = dynamic_cast<const mach_expr_symbol*>(expr);
        if (!symbol)
        {
            add_diagnostic(diagnostic_op::error_D030(expr->get_range(), std::string(1, type)));
            ret = false;
        }
    }
    return ret;
}

std::vector<context::id_index> data_definition::get_single_symbol_names() const
{
    // expects that check_single_symbol_ok returned true
    assert(check_single_symbol_ok(diagnostic_collector()));

    std::vector<context::id_index> symbols;
    symbols.reserve(nominal_value->access_exprs()->exprs.size());
    for (const auto& expr_or_addr : nominal_value->access_exprs()->exprs)
    {
        const mach_expression* expr = std::get<mach_expr_ptr>(expr_or_addr).get();
        const auto& symbol = dynamic_cast<const mach_expr_symbol&>(*expr);
        symbols.push_back(symbol.value);
    }
    return symbols;
}

void data_definition::assign_location_counter(context::address loctr_value)
{
    if (dupl_factor)
        dupl_factor->fill_location_counter(loctr_value);
    if (program_type)
        program_type->fill_location_counter(loctr_value);
    if (length)
        length->fill_location_counter(loctr_value);
    if (scale)
        scale->fill_location_counter(loctr_value);
    if (exponent)
        exponent->fill_location_counter(loctr_value);
    if (nominal_value && nominal_value->access_exprs())
    {
        for (auto& entry : nominal_value->access_exprs()->exprs)
        {
            if (std::holds_alternative<mach_expr_ptr>(entry))
                std::get<mach_expr_ptr>(entry)->fill_location_counter(loctr_value);
            else
            {
                std::get<address_nominal>(entry).base->fill_location_counter(loctr_value);
                std::get<address_nominal>(entry).displacement->fill_location_counter(loctr_value);
            }
        }
    }
}

void data_definition::collect_diags() const {}

checking::data_def_field<int32_t> set_data_def_field(
    const expressions::mach_expression* e, expressions::mach_evaluate_info info)
{
    using namespace checking;
    data_def_field<int32_t> field;
    // if the expression cannot be evaluated, we return field as if it was not there
    field.present = e != nullptr && !e->get_dependencies(info).contains_dependencies();
    if (field.present)
    {
        field.rng = e->get_range();
        // TODO get_reloc get_abs
        auto ret(e->evaluate(info));
        if (ret.value_kind() == context::symbol_value_kind::ABS)
            field.value = e->evaluate(info).get_abs();
    }
    return field;
}

checking::dupl_factor_modifier_t data_definition::evaluate_dupl_factor(expressions::mach_evaluate_info info) const
{
    return set_data_def_field(dupl_factor.get(), info);
}

checking::data_def_length_t data_definition::evaluate_length(expressions::mach_evaluate_info info) const
{
    checking::data_def_length_t len(set_data_def_field(length.get(), info));
    len.len_type = length_type == expressions::data_definition::length_type::BIT ? checking::data_def_length_t::BIT
                                                                                 : checking::data_def_length_t::BYTE;
    return len;
}

checking::scale_modifier_t data_definition::evaluate_scale(expressions::mach_evaluate_info info) const
{
    auto common = set_data_def_field(scale.get(), info);
    return checking::scale_modifier_t(common.present, (int16_t)common.value, common.rng);
}

checking::exponent_modifier_t data_definition::evaluate_exponent(expressions::mach_evaluate_info info) const
{
    return set_data_def_field(exponent.get(), info);
}

checking::nominal_value_t data_definition::evaluate_nominal_value(expressions::mach_evaluate_info info) const
{
    checking::nominal_value_t nom;
    if (nominal_value)
    {
        nom.present = true;
        if (nominal_value->access_string())
        {
            nom.value = nominal_value->access_string()->value;
            nom.rng = nominal_value->access_string()->value_range;
        }
        else if (nominal_value->access_exprs())
        {
            checking::nominal_value_expressions values;
            for (auto& e_or_a : nominal_value->access_exprs()->exprs)
            {
                if (std::holds_alternative<expressions::mach_expr_ptr>(e_or_a))
                {
                    expressions::mach_expr_ptr& e = std::get<expressions::mach_expr_ptr>(e_or_a);
                    bool ignored = e->get_dependencies(info).has_error
                        || e->get_dependencies(info).contains_dependencies(); // ignore values with dependencies
                    auto ev = e->evaluate(info);
                    auto kind = ev.value_kind();
                    if (kind == context::symbol_value_kind::ABS)
                        values.push_back(checking::data_def_expr {
                            ev.get_abs(), checking::expr_type::ABS, e->get_range(), ignored });

                    else if (kind == context::symbol_value_kind::RELOC)
                    {
                        checking::expr_type ex_type;
                        const auto& reloc = ev.get_reloc();
                        if (reloc.is_complex())
                            ex_type = checking::expr_type::COMPLEX;
                        else
                            ex_type = checking::expr_type::RELOC;
                        // TO DO value of the relocatable expression
                        // maybe push back data_def_addr?
                        values.push_back(checking::data_def_expr { 0, ex_type, e->get_range(), ignored });
                    }
                    else if (kind == context::symbol_value_kind::UNDEF)
                    {
                        values.push_back(checking::data_def_expr { 0, checking::expr_type::ABS, e->get_range(), true });
                    }
                    else
                    {
                        assert(false);
                        continue;
                    }
                }
                else // there is an address D(B)
                {
                    auto& a = std::get<expressions::address_nominal>(e_or_a);
                    checking::data_def_address ch_adr;

                    ch_adr.base = set_data_def_field(a.base.get(), info);
                    ch_adr.displacement = set_data_def_field(a.displacement.get(), info);
                    if (!ch_adr.base.present || !ch_adr.displacement.present)
                        ch_adr.ignored = true; // ignore values with dependencies
                    values.push_back(ch_adr);
                }
            }
            nom.value = std::move(values);
        }
        else
            assert(false);
    }
    else
        nom.present = false;
    return nom;
}

data_definition::parser::parser(
    semantics::collector& coll, std::string format, mach_expr_list exprs, nominal_value_ptr nominal, position begin)
    : collector_(coll)
    , format_(std::move(format))
    , exprs_(std::move(exprs))
    , nominal_(std::move(nominal))
    , pos_(begin)
    , p_(0)
    , exprs_i_(0)
{
    context::to_upper(format_);
}

constexpr unsigned char digit_to_value(unsigned char c)
{
    static_assert((unsigned char)'0' + 0 == '0');
    static_assert((unsigned char)'0' + 1 == '1');
    static_assert((unsigned char)'0' + 2 == '2');
    static_assert((unsigned char)'0' + 3 == '3');
    static_assert((unsigned char)'0' + 4 == '4');
    static_assert((unsigned char)'0' + 5 == '5');
    static_assert((unsigned char)'0' + 6 == '6');
    static_assert((unsigned char)'0' + 7 == '7');
    static_assert((unsigned char)'0' + 8 == '8');
    static_assert((unsigned char)'0' + 9 == '9');
    return c - '0';
}

// parses a number that begins on index begin in format_ string. Leaves index of the first character after the number in
// begin.
std::optional<int> data_definition::parser::parse_number()
{
    constexpr long long min_l = -(1LL << 31);
    constexpr long long max_l = (1LL << 31) - 1;
    constexpr long long parse_limit_l = (1LL << 31);
    static_assert(std::numeric_limits<int>::min() <= min_l);
    static_assert(std::numeric_limits<int>::max() >= max_l);

    const auto initial_p = p_;
    const position initial_pos = pos_;

    long long v = 0;
    const bool negative = format_[p_] == '-';
    bool parsed_one = false;
    if (format_[p_] == '+' || negative)
        ++p_;
    while (p_ < format_.size())
    {
        unsigned char c = format_[p_];
        if (!isdigit(c))
            break;
        parsed_one = true;
        ++p_;

        if (v > parse_limit_l)
            continue;

        v = v * 10 + digit_to_value(c);
    }
    pos_.column += p_ - initial_p;
    if (!parsed_one)
    {
        result_.add_diagnostic(diagnostic_op::error_D002({ initial_pos, pos_ }));
        return std::nullopt;
    }
    if (negative)
        v = -v;
    if (v < min_l || v > max_l)
    {
        result_.add_diagnostic(diagnostic_op::error_D001({ initial_pos, pos_ }));
        return std::nullopt;
    }
    collector_.add_hl_symbol(token_info(range(initial_pos, pos_), semantics::hl_scopes::number));

    return (int)v;
}

void data_definition::parser::update_position(const mach_expression& e)
{
    pos_ = e.get_range().end;
    pos_.column += 1;
}
void data_definition::parser::update_position_by_one() { ++pos_.column; }

void data_definition::parser::parse_duplication_factor()
{
    if (isdigit(format_[0]) || format_[0] == '-') // duplication factor is present
    {
        position old_pos = pos_;
        auto dupl_factor_num = parse_number();

        if (dupl_factor_num)
            result_.dupl_factor = std::make_unique<mach_expr_constant>(*dupl_factor_num, range(old_pos, pos_));
        else
            result_.dupl_factor = std::make_unique<mach_expr_constant>(1, range(old_pos, pos_));
    }
    else if (format_[0] == *data_definition::expr_placeholder)
    { // duplication factor as expression
        result_.dupl_factor = move_next_expression();
    }
    else
    {
        result_.dupl_factor = std::make_unique<mach_expr_constant>(1, range(pos_, pos_));
    }
}

bool is_type_extension(char ch)
{
    static std::set<char> type_extensions({ 'A', 'E', 'U', 'H', 'B', 'D', 'Q', 'Y' });
    return type_extensions.find(ch) != type_extensions.end();
}

bool is_modifier_or_prog(char ch) { return ch == 'P' || ch == 'L' || ch == 'S' || ch == 'E'; }

mach_expr_ptr data_definition::parser::move_next_expression()
{
    assert(exprs_i_ < exprs_.size());
    auto res = std::move(exprs_[exprs_i_]);
    ++exprs_i_;
    update_position(*res);
    ++p_;

    return res;
}

mach_expr_ptr data_definition::parser::parse_modifier_num_or_expr()
{
    if (auto c = format_[p_]; isdigit(c) || c == '-' || c == '+')
    {
        position old_pos = pos_;
        auto modifier = parse_number();

        if (modifier)
            return std::make_unique<mach_expr_constant>(*modifier, range(old_pos, pos_));
        // else leave expr null
    }
    else if (format_[p_] == expr_placeholder[0])
    {
        return move_next_expression();
    }
    else if (format_[p_] == nominal_placeholder[0])
    {
        auto exprs = nominal_->access_exprs();
        if (exprs && exprs->exprs.size() == 1 && std::holds_alternative<mach_expr_ptr>(exprs->exprs[0]))
        {
            // it is possible that a modifier has been parsed as nominal value,
            // if nominal value is not present at all and duplication factor is 0 or with DS instruction
            mach_expr_ptr& modifier = std::get<mach_expr_ptr>(exprs->exprs[0]);
            ++p_;
            update_position(*modifier);
            nominal_parsed_ = true;
            return std::move(modifier);
        }
        else
        {
            result_.add_diagnostic(diagnostic_op::error_D003({ pos_, { pos_.line, pos_.column + 1 } }));
            ++p_;
            // no need to update pos_, nominal value (if present) is the last character of the format string
        }
    }
    return nullptr;
}

void data_definition::parser::assign_expr_to_modifier(char modifier, mach_expr_ptr expr)
{
    switch (modifier)
    {
        case 'P':
            result_.program_type = std::move(expr);
            break;
        case 'L':
            result_.length = std::move(expr);
            break;
        case 'S':
            result_.scale = std::move(expr);
            break;
        case 'E':
            result_.exponent = std::move(expr);
            break;
        default:
            assert(false);
    }
}



void data_definition::parser::parse_modifier()
{
    // we assume, that format_[p_] determines one of modifiers PLSE
    position begin_pos = pos_;
    char modifier = format_[p_++];
    collector_.add_hl_symbol(token_info(
        range { begin_pos, { begin_pos.line, begin_pos.column + 1 } }, semantics::hl_scopes::data_def_modifier));
    if (p_ >= format_.size())
    {
        // expected something after modifier character
        result_.add_diagnostic(diagnostic_op::error_D003({ begin_pos, { begin_pos.line, begin_pos.column + 1 } }));
        return;
    }

    update_position_by_one();

    // parse bit length
    if (format_[p_] == '.')
    {
        ++p_;

        if (modifier == 'L')
            result_.length_type = length_type::BIT;
        else
            result_.add_diagnostic(diagnostic_op::error_D005({ begin_pos, { begin_pos.line, begin_pos.column + 1 } }));

        update_position_by_one();

        if (p_ >= format_.size())
        {
            // expected something after modifier character
            result_.add_diagnostic(diagnostic_op::error_D003({ begin_pos, { begin_pos.line, begin_pos.column + 1 } }));
            return;
        }
    }
    mach_expr_ptr expr = parse_modifier_num_or_expr();

    size_t rem_pos = remaining_modifiers_.find(modifier);
    if (rem_pos == std::string::npos) // wrong order
        result_.add_diagnostic(diagnostic_op::error_D004({ begin_pos, pos_ }));
    else
        remaining_modifiers_ = remaining_modifiers_.substr(rem_pos + 1);

    assign_expr_to_modifier(modifier, std::move(expr));
}

data_definition data_definition::parser::parse()
{
    parse_duplication_factor();

    result_.type = format_[p_++];
    result_.type_range = { pos_, { pos_.line, pos_.column + 1 } };
    range unified_type_range = result_.type_range;
    update_position_by_one();


    if (p_ >= format_.size())
        return std::move(result_);

    if (is_type_extension(format_[p_]))
    {
        result_.extension = format_[p_];
        result_.extension_range = { pos_, { pos_.line, pos_.column + 1 } };
        unified_type_range.end = result_.extension_range.end;
        ++p_;
        update_position_by_one();
    }

    collector_.add_hl_symbol(token_info(unified_type_range, semantics::hl_scopes::data_def_type));

    for (size_t i = 0; i < 5 && p_ < format_.size(); ++i)
    {
        if (is_modifier_or_prog(format_[p_]))
        {
            parse_modifier();
        }
        else if (format_[p_] == nominal_placeholder[0])
        {
            result_.nominal_value = std::move(nominal_);
            ++p_;
            break;
        }
        else
        {
            // consume all invalid characters and produce one diagnostic regarding all of them
            auto begin_pos = pos_;
            while (p_ < format_.size() && !is_modifier_or_prog(format_[p_]) && format_[p_] != nominal_placeholder[0])
            {
                ++p_;
                update_position_by_one();
            }

            result_.add_diagnostic(diagnostic_op::error_D006({ begin_pos, pos_ }));
        }
    }

    return std::move(result_);
}
