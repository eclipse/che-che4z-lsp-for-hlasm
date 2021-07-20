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


#include "asm_instr_check.h"

#include <regex>

namespace {
const std::vector<std::string> rmode_options = { "24", "31", "64", "ANY" };
}

namespace hlasm_plugin::parser_library::checking {

xattr::xattr(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool xattr::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    for (const auto& operand : to_check)
    {
        // instruction can have only complex operands
        auto current_operand = get_complex_operand(operand);
        if (current_operand == nullptr)
        {
            add_diagnostic(diagnostic_op::error_A001_complex_op_expected(name_of_instruction, operand->operand_range));
            return false;
        }
        const static std::vector<std::string> possible_identifiers = {
            "ATTRIBUTES", "ATTR", "LINK", "LINKAGE", "SCOPE", "PSECT", "REFERENCE", "REF"
        };
        if (!is_param_in_vector(current_operand->operand_identifier, possible_identifiers))
        {
            add_diagnostic(diagnostic_op::error_A100_XATTR_identifier(current_operand->operand_range));
            return false;
        }
        if (current_operand->operand_identifier == "ATTRIBUTES" || current_operand->operand_identifier == "ATTR"
            || current_operand->operand_identifier == "LINKAGE" || current_operand->operand_identifier == "LINK"
            || current_operand->operand_identifier == "SCOPE" || current_operand->operand_identifier == "PSECT")
        {
            if (current_operand->operand_parameters.size() != 1)
            {
                add_diagnostic(diagnostic_op::error_A016_exact(
                    current_operand->operand_identifier, current_operand->operand_identifier, 1, stmt_range));
                return false;
            }
            // get the simple operand
            auto param = get_simple_operand(current_operand->operand_parameters[0].get());
            if (current_operand->operand_identifier == "SCOPE")
            {
                const static std::vector<std::string> scope_operands = {
                    "SECTION", "MODULE", "LIBRARY", "IMPORT", "EXPORT", "S", "M", "L", "X"
                };
                if (param == nullptr || !is_param_in_vector(param->operand_identifier, scope_operands))
                {
                    add_diagnostic(diagnostic_op::error_A200_SCOPE_param(
                        name_of_instruction, current_operand->operand_parameters[0]->operand_range));
                    return false;
                }
            }
            else if (current_operand->operand_identifier == "LINKAGE" || current_operand->operand_identifier == "LINK")
            {
                if (param == nullptr || (param->operand_identifier != "OS" && param->operand_identifier != "XPLINK"))
                {
                    add_diagnostic(diagnostic_op::error_A201_LINKAGE_param(
                        name_of_instruction, current_operand->operand_parameters[0]->operand_range));
                    return false;
                }
            }
        }
        else if (current_operand->operand_identifier == "REFERENCE" || current_operand->operand_identifier == "REF")
        {
            if ((current_operand->operand_parameters.empty()) || (current_operand->operand_parameters.size() > 2))
            {
                add_diagnostic(diagnostic_op::error_A018_either(
                    name_of_instruction, current_operand->operand_identifier, 1, 2, stmt_range));
                return false;
            }
            bool code_data_option = false;
            bool direct_option = false;
            for (const auto& parameter : current_operand->operand_parameters)
            {
                // check whether it is simple
                auto param = get_simple_operand(parameter.get());
                if (param == nullptr)
                {
                    add_diagnostic(diagnostic_op::error_A238_REF_format(name_of_instruction, parameter->operand_range));
                    return false;
                }
                if (param->operand_identifier == "DIRECT" || param->operand_identifier == "INDIRECT")
                {
                    if (!direct_option)
                        direct_option = true;
                    else
                    {
                        add_diagnostic(
                            diagnostic_op::error_A202_REF_direct(name_of_instruction, parameter->operand_range));
                        return false;
                    }
                }
                else if (param->operand_identifier == "DATA" || param->operand_identifier == "CODE")
                {
                    if (!code_data_option)
                        code_data_option = true;
                    else
                    {
                        add_diagnostic(
                            diagnostic_op::error_A203_REF_data(name_of_instruction, parameter->operand_range));
                        return false;
                    }
                }
                else
                {
                    add_diagnostic(diagnostic_op::error_A238_REF_format(name_of_instruction, param->operand_range));
                    return false;
                }
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_I999(name_of_instruction, stmt_range));
            assert(false);
            return false;
        }
    }
    return true;
};

using_instr::using_instr(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 2, -1) {};

bool using_instr::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    /*
    // check first operand
    // TO DO, waiting for the evaluation of relocatable expressions, symbols can be used
    if (auto first_operand = get_complex_operand(to_check[0]); first_operand) //first operand must be therefore in the
    form of (base, end)
    {
            if (first_operand->operand_identifier != "" ||
    is_operand_complex(first_operand->operand_parameters[0].get()) ||
    is_operand_complex(first_operand->operand_parameters[1].get())
                    || first_operand->operand_parameters.size() != 2)
            {
                    add_diagnostic(diagnostic_op::error_A104_USING_first_format());
                    return false;
            }
            if (!is_positive_value(get_simple_operand(first_operand->operand_parameters[0].get())->operand_identifier))
            {
                    add_diagnostic(diagnostic_op::error_A101_USING_base_val());
                    return false;
            }
            if (!is_positive_value(get_simple_operand(first_operand->operand_parameters[1].get())->operand_identifier))
            {
                    add_diagnostic(diagnostic_op::error_A102_USING_end_val());
                    return false;
            }
            if (std::stoi(get_simple_operand(first_operand->operand_parameters[1].get())->operand_identifier)
                    <= std::stoi(get_simple_operand(first_operand->operand_parameters[0].get())->operand_identifier))
            {
                    add_diagnostic(diagnostic_op::error_A103_USING_end_exceed());
                    return false;
            }
    }
    else if (auto simple_op = get_simple_operand(to_check[0]); simple_op)//first operand specifies only base
    {
            if (!is_positive_value(simple_op->operand_identifier))
            {
                    add_diagnostic(diagnostic_op::error_A101_USING_base_val());
                    return false;
            }
    }
    else
            assert(false);
    //check other operands
    if (to_check.size() == 2) //therefore there can be either address or one base register
    {
            auto simple_op = get_simple_operand(to_check[1]);
            if (simple_op == nullptr)
            {
                    add_diagnostic(diagnostic_op::error_A000_simple_op_expected(name_of_instruction));
                    return false;
            }
            // can also be address, maybe TO DO
            return true;

            if (!is_base_register(simple_op->operand_identifier) || !is_positive_value(simple_op->operand_identifier))
            {
                    add_diagnostic(diagnostic_op::error_I020_format(name_of_instruction, true));
                    return false;
            }
    }
    else //all other parameters are base registers
    {
            for (size_t i = 1; i < to_check.size(); i++)
            {
                    auto simple_op = get_simple_operand(to_check[i]);
                    if (simple_op == nullptr)
                    {
                            add_diagnostic(diagnostic_op::error_A000_simple_op_expected(name_of_instruction));
                            return false;
                    }
                    if (!is_base_register(simple_op->operand_identifier))
                    {
                            add_diagnostic(diagnostic_op::error_A105_USING_base_register_val());
                            return false;
                    }
            }
    }*/
    return true;
}

title::title(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool title::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    // first operand must be simple
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr)
    {
        add_diagnostic(diagnostic_op::error_A106_TITLE_string_chars(to_check[0]->operand_range));
        return false;
    }
    const auto& op_id = first->operand_identifier;
    if (op_id.size() > TITLE_max_length || op_id.size() < 2)
    {
        add_diagnostic(diagnostic_op::error_A106_TITLE_string_chars(first->operand_range));
        return false;
    }
    if (op_id.front() != '\'' || op_id.back() != '\'')
    {
        add_diagnostic(diagnostic_op::warning_A300_op_apostrophes_missing(name_of_instruction, first->operand_range));
        return false;
    }
    return true;
}

rmode::rmode(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool rmode::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr || !is_param_in_vector(first->operand_identifier, rmode_options))
    {
        add_diagnostic(diagnostic_op::error_A107_RMODE_op_format(to_check[0]->operand_range));
        return false;
    }
    return true;
}

punch::punch(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool punch::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic)) // check number of operands
        return false;
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr)
    {
        add_diagnostic(diagnostic_op::error_A244_PUNCH_char_string(to_check[0]->operand_range));
        return false;
    }
    if (first->operand_identifier.size() < 2 || first->operand_identifier.front() != '\''
        || first->operand_identifier.back() != '\'')
    {
        add_diagnostic(diagnostic_op::warning_A300_op_apostrophes_missing(name_of_instruction, first->operand_range));
        return false;
    }
    int size_of_string = 0;
    size_t i = 0;
    for (const auto& op_id = first->operand_identifier; i < op_id.size() - 1; ++size_of_string)
    {
        if (op_id[i] == '&' && op_id[i + 1] == '&')
            i += 2;
        else if (op_id[i] == '\'' && op_id[i + 1] == '\'')
            i += 2;
        else
            i++;
    }
    if (size_of_string <= string_max_length)
        return true;
    add_diagnostic(diagnostic_op::error_A108_PUNCH_string_chars(to_check[0]->operand_range));
    return false;
}

print::print(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool print::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    const static std::vector<std::string> print_pair_operands = {
        "GEN", "DATA", "MCALL", "MSOURCE", "UHEAD", "NOGEN", "NODATA", "NOMCALL", "NOMSOURCE", "NOUHEAD"
    };
    const static std::vector<std::string> print_other_operands = { "ON", "OFF", "NOPRINT" };
    for (const auto& operand : to_check)
    {
        auto simple = get_simple_operand(operand);
        if (simple == nullptr
            || (!is_param_in_vector(simple->operand_identifier, print_pair_operands)
                && !is_param_in_vector(simple->operand_identifier, print_other_operands)))
        {
            add_diagnostic(diagnostic_op::error_A109_PRINT_op_format(operand->operand_range));
            return false;
        }
    }
    return true;
}

stack_instr::stack_instr(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 4) {};

bool stack_instr::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    bool acontrol_operand = false;
    bool print_operand = false;
    bool using_operand = false;
    for (size_t i = 0; i < to_check.size(); i++)
    {
        auto simple = get_simple_operand(to_check[i]);
        if (simple == nullptr)
        {
            // whether are at the last operand
            if (i == to_check.size() - 1)
                add_diagnostic(diagnostic_op::error_A110_STACK_last_op_format_val(
                    name_of_instruction, to_check[i]->operand_range));
            else
                add_diagnostic(diagnostic_op::error_A111_STACK_other_op_format_val(
                    name_of_instruction, to_check[i]->operand_range));
            return false;
        }
        if (simple->operand_identifier == "ACONTROL")
        {
            if (!acontrol_operand)
                acontrol_operand = true;
            else
            {
                add_diagnostic(diagnostic_op::error_A112_STACK_option_specified(
                    name_of_instruction, simple->operand_identifier, stmt_range));
                return false;
            }
        }
        else if (simple->operand_identifier == "PRINT")
        {
            if (!print_operand)
                print_operand = true;
            else
            {
                add_diagnostic(diagnostic_op::error_A112_STACK_option_specified(
                    name_of_instruction, simple->operand_identifier, stmt_range));
                return false;
            }
        }
        else if (simple->operand_identifier == "USING")
        {
            if (!using_operand)
                using_operand = true;
            else
            {
                add_diagnostic(diagnostic_op::error_A112_STACK_option_specified(
                    name_of_instruction, simple->operand_identifier, stmt_range));
                return false;
            }
        }
        else if (simple->operand_identifier == "NOPRINT")
        {
            // must be specified at end
            if (i != to_check.size() - 1)
            {
                add_diagnostic(
                    diagnostic_op::error_A113_STACK_NOPRINT_end(name_of_instruction, to_check[i]->operand_range));
                return false;
            }
            // cannot be the only option specified
            if (to_check.size() == 1)
            {
                add_diagnostic(diagnostic_op::error_A114_STACK_NOPRINT_solo(name_of_instruction, stmt_range));
                return false;
            }
        }
        else
        {
            // whether are at the last operand
            if (i == to_check.size() - 1)
                add_diagnostic(diagnostic_op::error_A110_STACK_last_op_format_val(
                    name_of_instruction, to_check[i]->operand_range));
            else
                add_diagnostic(diagnostic_op::error_A111_STACK_other_op_format_val(
                    name_of_instruction, to_check[i]->operand_range));
            return false;
        }
    }
    return true;
}

org::org(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 3) {};

bool org::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    if (has_one_comma(to_check))
        return true;
    if (to_check.empty())
        return true;
    const one_operand* second = nullptr;

    for (size_t i = 0; i < to_check.size(); i++)
    {
        // boundary can be empty
        if (i == 1 && is_operand_empty(to_check[i]))
            continue;
        auto simple_op = get_simple_operand(to_check[i]);
        if (simple_op == nullptr)
        {
            if (i == 0)
                add_diagnostic(diagnostic_op::error_A245_ORG_expression(to_check[i]->operand_range));
            else
                add_diagnostic(diagnostic_op::error_A020_absolute_val_or_empty_expected(
                    name_of_instruction, to_check[i]->operand_range));
            return false;
        }
        if (simple_op->is_default) // check whether operands are numbers
        {
            add_diagnostic(diagnostic_op::error_A115_ORG_op_format(to_check[i]->operand_range));
            return false;
        }
        switch (i)
        {
            case 1:
                second = simple_op;
                break;
            case 2:
                break;
        }
    }
    if (second != nullptr)
    {
        // check the boundary operand value in case it is specified
        auto second_val = second->value;
        if (!is_power_of_two(second_val) || second_val > ORG_max_boundary_val || second_val < ORG_min_boundary_val)
        {
            add_diagnostic(diagnostic_op::error_A116_ORG_boundary_operand(second->operand_range));
            return false;
        }
    }
    return true;
}

opsyn::opsyn(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 1) {};

bool opsyn::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    if (has_one_comma(to_check))
        return true;
    if (to_check.size() == 1)
    {
        if (is_operand_complex(to_check[0]))
        {
            add_diagnostic(diagnostic_op::error_A246_OPSYN(to_check[0]->operand_range));
            return false;
        }
        // TO DO - check operation code parameter
        return true;
    }
    return true;
}

mnote::mnote(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 2) {};

bool mnote::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    if (!is_operand_simple(to_check.back()))
    {
        add_diagnostic(diagnostic_op::error_A117_MNOTE_message_size(to_check.back()->operand_range));
        return false;
    }
    if (to_check.size() == 2)
    {
        if (!is_operand_simple(to_check[0]) && !is_operand_empty(to_check[0]))
        {
            add_diagnostic(diagnostic_op::error_A119_MNOTE_first_op_format(to_check[0]->operand_range));
            return false;
        }
    }
    // last is a message
    auto last = get_simple_operand(to_check.back());
    // first does not have to be specified
    const one_operand* first = nullptr;
    if (to_check.size() == 2)
        first = dynamic_cast<const one_operand*>(to_check[0]);
    // check message
    if (last->operand_identifier.size() < 2 || last->operand_identifier[0] != '\''
        || last->operand_identifier.back() != '\'')
    {
        add_diagnostic(diagnostic_op::warning_A300_op_apostrophes_missing(name_of_instruction, last->operand_range));
        return false;
    }
    if (last->operand_identifier.size() > MNOTE_max_message_length)
    {
        add_diagnostic(diagnostic_op::error_A117_MNOTE_message_size(last->operand_range));
        return false;
    }
    // move onto the first operand
    if (to_check.size() == 1 || first == nullptr)
        return true;
    if (first->operand_identifier.size() + last->operand_identifier.size() > MNOTE_max_operands_length)
    {
        add_diagnostic(diagnostic_op::error_A118_MNOTE_operands_size(stmt_range));
        return false;
    }
    // this means that severity is not specified
    if (first->operand_identifier == "*")
        return true;
    // severity is specified
    if (!has_all_digits(first->operand_identifier))
    {
        add_diagnostic(diagnostic_op::error_A119_MNOTE_first_op_format(first->operand_range));
        return false;
    }
    if (!first->is_default && is_byte_value(first->value))
        return true;
    add_diagnostic(diagnostic_op::error_A119_MNOTE_first_op_format(first->operand_range));
    return false;
}

iseq::iseq(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 2) {};

bool iseq::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (to_check.empty())
        return true;
    if (has_one_comma(to_check))
        return true;
    if (to_check.size() == 2)
    {
        auto first = get_simple_operand(to_check[0]);
        auto second = get_simple_operand(to_check[1]);
        if (first == nullptr || second == nullptr || first->is_default || second->is_default)
        {
            add_diagnostic(diagnostic_op::error_A120_ISEQ_op_format(stmt_range));
            return false;
        }
        int left = first->value;
        int right = second->value;
        if (left >= ISEQ_min_op_val && left <= ISEQ_max_op_val && right >= ISEQ_min_op_val && right <= ISEQ_max_op_val
            && right >= left)
            return true;
        else if (right < left)
        {
            add_diagnostic(diagnostic_op::error_A121_ISEQ_right_GT_left(stmt_range));
            return false;
        }
        add_diagnostic(diagnostic_op::error_A120_ISEQ_op_format(stmt_range));
        return false;
    }
    add_diagnostic(diagnostic_op::error_A013_either(name_of_instruction, 0, 2, stmt_range));
    return false;
}

ictl::ictl(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 3) {};

bool ictl::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    int begin = 1;
    int end = 72;
    int continuation = 16;
    if (is_operand_empty(to_check[0]))
    {
        add_diagnostic(diagnostic_op::error_A021_cannot_be_empty(name_of_instruction, to_check[0]->operand_range));
        return false;
    }
    for (size_t i = 0; i < to_check.size(); i++)
    {
        auto simple_op = get_simple_operand(to_check[i]);
        if (simple_op == nullptr || !has_all_digits(simple_op->operand_identifier) || simple_op->is_default)
        {
            if (i == 0)
                add_diagnostic(diagnostic_op::error_A122_ICTL_op_format_first(to_check[0]->operand_range));
            else
                add_diagnostic(diagnostic_op::error_A242_ICTL_op_format_second_third(to_check[i]->operand_range));
            return false;
        }
    }
    // transform first operand
    begin = get_simple_operand(to_check[0])->value;
    // transform second and third operand
    if (to_check.size() > 1)
        end = get_simple_operand(to_check[1])->value;
    if (to_check.size() > 2)
        continuation = get_simple_operand(to_check[2])->value;
    // check ICTL parameters
    if (to_check.size() == 2 || end == 80)
        continuation = -1;
    if (begin < ICTL_begin_min_val || begin > ICTL_begin_max_val)
    {
        add_diagnostic(diagnostic_op::error_A123_ICTL_begin_format(to_check[0]->operand_range));
        return false;
    }
    if (end < ICTL_end_min_val || end > ICTL_end_max_val)
    {
        add_diagnostic(diagnostic_op::error_A124_ICTL_end_format(to_check[1]->operand_range));
        return false;
    }
    if (end < begin + ICTL_begin_end_diff)
    {
        range begin_end_range = range(to_check[0]->operand_range.start, to_check[1]->operand_range.end);
        add_diagnostic(diagnostic_op::error_A125_ICTL_begin_end_diff(begin_end_range));
        return false;
    }
    if (continuation == -1)
        return true;
    if (continuation < ICTL_continuation_min_val || continuation > ICTL_continuation_max_val)
    {
        add_diagnostic(diagnostic_op::error_A126_ICTL_continuation_format(to_check[2]->operand_range));
        return false;
    }
    if (continuation <= begin)
    {
        add_diagnostic(diagnostic_op::error_A127_ICTL_begin_continuation_diff(stmt_range));
        return false;
    }
    return true;
}

external::external(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool external::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    for (const auto& operand : to_check)
    {
        if (auto complex_op = get_complex_operand(operand); complex_op)
        {
            // check PART operand
            if (complex_op->operand_identifier != "PART")
            {
                add_diagnostic(diagnostic_op::error_A129_EXTRN_format(operand->operand_range));
                return false;
            }
            for (const auto& parameter : complex_op->operand_parameters)
            {
                if (is_operand_empty(parameter.get()) || is_operand_complex(parameter.get())
                    || get_simple_operand(parameter.get())->operand_identifier == "")
                {
                    add_diagnostic(diagnostic_op::error_A129_EXTRN_format(operand->operand_range));
                    return false;
                }
            }
        }
        else if (auto simple_op = get_simple_operand(operand); simple_op)
        {
            // check simple external symbol
            if (simple_op->operand_identifier == "")
            {
                add_diagnostic(diagnostic_op::error_A129_EXTRN_format(operand->operand_range));
                return false;
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_A021_cannot_be_empty(name_of_instruction, operand->operand_range));
            return false;
        }
    }
    return true;
}

exitctl::exitctl(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 2, 5) {};

bool exitctl::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    // check first operand representing exit type
    const static std::vector<std::string> exit_type = {
        "SOURCE", "LIBRARY", "LISTING", "PUNCH", "ADATA", "TERM", "OBJECT"
    };
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr || !is_param_in_vector(first->operand_identifier, exit_type))
    {
        add_diagnostic(diagnostic_op::error_A130_EXITCTL_exit_type_format(to_check[0]->operand_range));
        return false;
    }
    // check other operands representing control values
    for (size_t i = 1; i < to_check.size(); i++)
    {
        if (is_operand_empty(to_check[i]))
            continue;
        auto operand = get_simple_operand(to_check[i]);
        if (operand == nullptr)
        {
            add_diagnostic(diagnostic_op::error_A020_absolute_val_or_empty_expected(
                name_of_instruction, to_check[i]->operand_range));
            return false;
        }
        if (operand->is_default)
        {
            add_diagnostic(diagnostic_op::error_A131_EXITCTL_control_value_format(to_check[i]->operand_range));
            return false;
        }
    }
    return true;
}

equ::equ(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 5) {};

bool equ::check(const std::vector<const asm_operand*>&, const range&, const diagnostic_collector&) const
{
    return true;
}

entry::entry(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool entry::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    if (to_check.size() > ENTRY_max_operands)
    {
        add_diagnostic(diagnostic_op::error_A014_lower_than(name_of_instruction, ENTRY_max_operands, stmt_range));
        return false;
    }
    for (const auto& operand : to_check)
    {
        auto simple = get_simple_operand(operand);
        if (simple == nullptr || simple->operand_identifier == "")
        {
            add_diagnostic(diagnostic_op::error_A136_ENTRY_op_format(operand->operand_range));
            return false;
        }
    }
    return true;
}

end::end(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 2) {};

bool end::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    // check the first expression operand
    if (!to_check.empty())
    {
        if (!is_operand_simple(to_check[0]) && !is_operand_empty(to_check[0])) // first operand must be simple
        {
            add_diagnostic(diagnostic_op::error_A243_END_expr_format(to_check[0]->operand_range));
            return false;
        }
    }
    // check the second language operand
    if (to_check.size() == 2)
    {
        // second operand must be complex or empty
        if (is_operand_empty(to_check[1]))
            return true;
        auto language_operand = get_complex_operand(to_check[1]);
        if (language_operand == nullptr)
        {
            add_diagnostic(
                diagnostic_op::error_A001_complex_op_expected(name_of_instruction, to_check[1]->operand_range));
            return false;
        }
        if (language_operand->operand_parameters.size() != 3)
        {
            add_diagnostic(diagnostic_op::error_A016_exact(name_of_instruction, "language", 3, stmt_range));
            return false;
        }
        if (language_operand->operand_identifier != "")
        {
            add_diagnostic(diagnostic_op::error_A137_END_lang_format(language_operand->operand_range));
            return false;
        }
        for (const auto& param : language_operand->operand_parameters)
        {
            // all parameters must be simple
            if (!is_operand_simple(param.get()))
            {
                add_diagnostic(diagnostic_op::error_A248_END_lang_char_sequence(param->operand_range));
                return false;
            }
        }
        if (get_simple_operand(language_operand->operand_parameters[0].get())->operand_identifier.empty()
            || get_simple_operand(language_operand->operand_parameters[0].get())->operand_identifier.size()
                > END_lang_first_par_size)
        {
            add_diagnostic(
                diagnostic_op::error_A138_END_lang_first(language_operand->operand_parameters[0]->operand_range));
            return false;
        }
        if (get_simple_operand(language_operand->operand_parameters[1].get())->operand_identifier.size()
            != END_lang_second_par_size)
        {
            add_diagnostic(
                diagnostic_op::error_A139_END_lang_second(language_operand->operand_parameters[1]->operand_range));
            return false;
        }
        auto third_op = get_simple_operand(language_operand->operand_parameters[2].get());
        if (third_op->operand_identifier.size() != END_lang_third_par_size
            || !has_all_digits(third_op->operand_identifier))
        {
            add_diagnostic(
                diagnostic_op::error_A140_END_lang_third(language_operand->operand_parameters[2]->operand_range));
            return false;
        }
    }
    return true;
}

drop::drop(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, -1) {};

bool drop::check(
    const std::vector<const asm_operand*>& to_check, const range&, const diagnostic_collector& add_diagnostic) const
{
    if (has_one_comma(to_check))
        return true;
    for (const auto& operand : to_check)
    {
        auto simple = get_simple_operand(operand);
        if (simple == nullptr)
        {
            add_diagnostic(diagnostic_op::error_A141_DROP_op_format(operand->operand_range));
            return false;
        }
        if (!simple->is_default)
        {
            // base register must be specified
            if (is_byte_value(simple->value))
                continue;
        }
        else
        {
            // label must be specified
            if (is_ord_symbol(simple->operand_identifier) || is_var_symbol(simple->operand_identifier))
                continue;
        }
        add_diagnostic(diagnostic_op::error_A141_DROP_op_format(operand->operand_range));
        return false;
    }
    return true;
}

copy::copy(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool copy::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    // cannot take string
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr || !first->is_default
        || (first->operand_identifier.size() > 1 && first->operand_identifier[0] == '\''
            && first->operand_identifier.back() == '\''))
    {
        add_diagnostic(diagnostic_op::error_A142_COPY_op_format(to_check[0]->operand_range));
        return false;
    }
    return true;
}

cnop::cnop(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 2, 2) {};

bool cnop::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    auto second = get_simple_operand(to_check[1]);
    if (first == nullptr || first->is_default)
    {
        add_diagnostic(
            diagnostic_op::error_A143_must_be_absolute_expr(name_of_instruction, to_check[0]->operand_range));
        return false;
    }
    if (second == nullptr || second->is_default)
    {
        add_diagnostic(
            diagnostic_op::error_A143_must_be_absolute_expr(name_of_instruction, to_check[1]->operand_range));
        return false;
    }
    int byte = first->value;
    int boundary = second->value;
    if (byte < 0 || byte % 2 == 1)
    {
        add_diagnostic(diagnostic_op::error_A144_CNOP_byte_size(to_check[0]->operand_range));
        return false;
    }
    if (boundary < CNOP_min_boundary_val || boundary > CNOP_max_boundary_val || !is_power_of_two(boundary))
    {
        add_diagnostic(diagnostic_op::error_A145_CNOP_boundary_size(to_check[1]->operand_range));
        return false;
    }
    if (byte > boundary - CNOP_byte_boundary_diff)
    {
        add_diagnostic(diagnostic_op::error_A146_CNOP_byte_GT_boundary(to_check[0]->operand_range));
        return false;
    }
    return true;
}

ccw::ccw(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 4, 4) {};

bool ccw::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    for (size_t i = 0; i < to_check.size(); i++)
    {
        if (is_operand_empty(to_check[i]))
        {
            add_diagnostic(diagnostic_op::error_A147_CCW_op_format(name_of_instruction, to_check[i]->operand_range));
            return false;
        }
        auto simple = get_simple_operand(to_check[i]);
        if (simple == nullptr)
        {
            if (i == 1)
                add_diagnostic(
                    diagnostic_op::error_A247_must_be_rel_abs_expr(name_of_instruction, to_check[i]->operand_range));
            else
                add_diagnostic(
                    diagnostic_op::error_A143_must_be_absolute_expr(name_of_instruction, to_check[i]->operand_range));
            return false;
        }
    }
    return true;
}

// instructions like SPACE, CEJECT, START

expression_instruction::expression_instruction(
    const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 1) {};

bool expression_instruction::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (to_check.empty())
        return true;
    // an if for the specific "SPACE , " case which should return true
    if (has_one_comma(to_check))
        return true;
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr)
    {
        add_diagnostic(diagnostic_op::error_A240_expression_format(name_of_instruction, to_check[0]->operand_range));
        return false;
    }
    if (first->is_default || !is_positive_number(first->value))
    {
        add_diagnostic(diagnostic_op::error_A148_EXPR_op_format(name_of_instruction, first->operand_range));
        return false;
    }
    return true;
}

cattr::cattr(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool cattr::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    const static std::vector<std::string> simple_operands = { "DEFLOAD",
        "EXECUTABLE",
        "MOVABLE",
        "NOLOAD",
        "NOTEXECUTABLE",
        "NOTREUS",
        "READONLY",
        "REFR",
        "REMOVABLE",
        "RENT",
        "REUS" };
    for (const auto& operand : to_check)
    {
        if (auto simple_op = get_simple_operand(operand); simple_op)
        {
            // operand is simple
            if (!is_param_in_vector(simple_op->operand_identifier, simple_operands))
            {
                add_diagnostic(diagnostic_op::error_A149_CATTR_identifier_format(simple_op->operand_range));
                return false;
            }
        }
        else if (auto complex_op = get_complex_operand(operand); complex_op)
        {
            // operand is complex
            const static std::vector<std::string> complex_operands = { "RMODE", "ALIGN", "FILL", "PART", "PRIORITY" };
            if (!is_param_in_vector(complex_op->operand_identifier, complex_operands))
            {
                add_diagnostic(diagnostic_op::error_A149_CATTR_identifier_format(complex_op->operand_range));
                return false;
            }
            if (complex_op->operand_parameters.size() != 1) // has to have exactly one operand
            {
                add_diagnostic(diagnostic_op::error_A016_exact(
                    name_of_instruction, complex_op->operand_identifier, 1, stmt_range));
                return false;
            }
            auto parameter = get_simple_operand(complex_op->operand_parameters[0].get());
            if (complex_op->operand_identifier == "RMODE")
            {
                if (parameter == nullptr || !is_param_in_vector(parameter->operand_identifier, rmode_options))
                {
                    add_diagnostic(diagnostic_op::error_A204_RMODE_param_format(
                        name_of_instruction, complex_op->operand_parameters[0]->operand_range));
                    return false;
                }
            }
            else if (complex_op->operand_identifier
                == "ALIGN") // upon sending, an empty operand has to come in a vector (not an empty vector)
            {
                const static std::vector<std::string> align_options = { "0", "1", "2", "3", "4", "12" };
                if (parameter == nullptr || !is_param_in_vector(parameter->operand_identifier, align_options))
                {
                    add_diagnostic(diagnostic_op::error_A205_ALIGN_param_format(
                        name_of_instruction, complex_op->operand_parameters[0]->operand_range));
                    return false;
                }
            }
            else if (complex_op->operand_identifier == "FILL")
            {
                if (parameter == nullptr || !has_all_digits(parameter->operand_identifier) || parameter->is_default
                    || !is_byte_value(parameter->value))
                {
                    add_diagnostic(diagnostic_op::error_A206_FILL_param_format(
                        name_of_instruction, complex_op->operand_parameters[0]->operand_range));
                    return false;
                }
            }
            else if (complex_op->operand_identifier == "PART")
            {
                if (parameter == nullptr || parameter->operand_identifier.empty()
                    || parameter->operand_identifier.length() > 63)
                {
                    add_diagnostic(diagnostic_op::error_A207_PART_param_format(
                        name_of_instruction, complex_op->operand_parameters[0]->operand_range));
                    return false;
                }
            }
            else if (complex_op->operand_identifier == "PRIORITY")
            {
                if (parameter == nullptr || !has_all_digits(parameter->operand_identifier) || parameter->is_default
                    || !is_positive_number(parameter->value))
                {
                    add_diagnostic(diagnostic_op::error_A208_PRIORITY_param_format(
                        name_of_instruction, complex_op->operand_parameters[0]->operand_range));
                    return false;
                }
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_A021_cannot_be_empty(name_of_instruction, operand->operand_range));
            return false;
        }
    }
    return true;
}

amode::amode(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool amode::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    const static std::vector<std::string> amode_options = { "24", "31", "64", "ANY", "ANY31", "ANY64" };
    if (first == nullptr || !is_param_in_vector(first->operand_identifier, amode_options))
    {
        add_diagnostic(diagnostic_op::error_A150_AMODE_op_format(to_check[0]->operand_range));
        return false;
    }
    return true;
}

alias::alias(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, 1) {};

bool alias::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    if (first == nullptr || first->operand_identifier.size() < 3)
    {
        add_diagnostic(diagnostic_op::error_A151_ALIAS_op_format(to_check[0]->operand_range));
        return false;
    }
    if (first->operand_identifier[1] == '\'' && first->operand_identifier[first->operand_identifier.size() - 1] == '\'')
    {
        if (first->operand_identifier[0] == 'C')
        {
            // TO DO - no support for four characters in EBCDIC (¢, ¬, ±, ¦) - we throw an error although it should not
            // be
            std::regex regex(R"([\.<¢\(\+\|&!\$\*\);¬\-\/¦,%_>\?`,:#@\=\"~±\[\]\{\}\^\\a-zA-Z0-9]*)");
            std::string substr = first->operand_identifier.substr(2, first->operand_identifier.size() - 3);
            if (!std::regex_match(substr, regex))
            {
                add_diagnostic(diagnostic_op::error_A152_ALIAS_C_format(first->operand_range));
                return false;
            }
            return true;
        }
        else if (first->operand_identifier[0] == 'X')
        {
            if (first->operand_identifier.size() % 2 == 1)
            {
                add_diagnostic(diagnostic_op::error_A154_ALIAS_X_format_no_of_chars(first->operand_range));
                return false;
            }
            int max_value = ALIAS_max_val;
            int min_value = ALIAS_min_val;
            for (size_t i = 2; i < first->operand_identifier.size() - 1; i += 2)
            {
                std::string tocomp = "";
                tocomp.push_back(first->operand_identifier[i]);
                tocomp.push_back(first->operand_identifier[i + 1]);
                if (tocomp == "0x" || tocomp == "0X")
                {
                    add_diagnostic(diagnostic_op::error_A153_ALIAS_X_format(first->operand_range));
                    return false;
                }
                int comparing = 0;
                try
                {
                    comparing = std::stoul(tocomp, nullptr, 16);
                }
                catch (...)
                {
                    add_diagnostic(diagnostic_op::error_A153_ALIAS_X_format(first->operand_range));
                    return false;
                }
                if (comparing < min_value || comparing > max_value)
                {
                    add_diagnostic(diagnostic_op::error_A155_ALIAS_X_format_range(first->operand_range));
                    return false;
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    add_diagnostic(diagnostic_op::error_A151_ALIAS_op_format(first->operand_range));
    return false;
}

ainsert::ainsert(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 2, 2) {};

bool ainsert::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    auto first = get_simple_operand(to_check[0]);
    auto second = get_simple_operand(to_check[1]);
    // check first operand
    if (first == nullptr || first->operand_identifier.size() < 2
        || first->operand_identifier.size() > string_max_length)
    {
        add_diagnostic(diagnostic_op::error_A157_AINSERT_first_op_size(to_check[0]->operand_range));
        return false;
    }
    if (first->operand_identifier.front() != '\'' || first->operand_identifier.back() != '\'')
    {
        add_diagnostic(
            diagnostic_op::error_A301_op_apostrophes_missing(name_of_instruction, to_check[0]->operand_range));
        return false;
    }
    if (first->operand_identifier.size() == 2) // empty string
    {
        add_diagnostic(diagnostic_op::error_A021_cannot_be_empty(name_of_instruction, to_check[0]->operand_range));
        return false;
    }
    // check second operand
    if (second == nullptr || (second->operand_identifier != "BACK" && second->operand_identifier != "FRONT"))
    {
        add_diagnostic(diagnostic_op::error_A156_AINSERT_second_op_format(to_check[1]->operand_range));
        return false;
    }
    return true;
}

adata::adata(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 5, 5) {};

bool adata::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    // first four operands must be values or empty
    for (size_t i = 0; i < to_check.size() - 1; ++i)
    {
        // operand must be simple or empty
        if (is_operand_empty(to_check[i]))
            continue;
        auto simple = get_simple_operand(to_check[i]);
        if (simple == nullptr || simple->is_default)
        {
            add_diagnostic(diagnostic_op::error_A158_ADATA_val_format(to_check[i]->operand_range));
            return false;
        }
    }
    // last operand must be character_string
    if (is_operand_empty(to_check.back()))
        return true;
    auto last_op = get_simple_operand(to_check.back());
    if (last_op == nullptr)
    {
        add_diagnostic(diagnostic_op::error_A239_ADATA_char_string_format(to_check.back()->operand_range));
        return false;
    }
    if (last_op->operand_identifier.size() == 1
        || (last_op->operand_identifier.size() > 1
            && (last_op->operand_identifier.front() != '\'' || last_op->operand_identifier.back() != '\'')))
        add_diagnostic(diagnostic_op::warning_A300_op_apostrophes_missing(name_of_instruction, last_op->operand_range));
    if (!is_character_string(last_op->operand_identifier))
    {
        add_diagnostic(diagnostic_op::error_A160_ADATA_char_string_size(last_op->operand_range));
        return false;
    }
    return true;
}

no_operands::no_operands(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 0, 0) {};

bool no_operands::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    return operands_size_corresponding(to_check, stmt_range, add_diagnostic);
}

process::process(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool process::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    if (to_check.size() == 1 && is_operand_complex(to_check[0])
        && get_complex_operand(to_check[0])->operand_identifier
            == "OVERRIDE") // everything parsed is parameter of operand
    {
        auto process_operands = get_complex_operand(to_check[0]);
        return std::all_of(process_operands->operand_parameters.cbegin(),
            process_operands->operand_parameters.cend(),
            [this, &add_diagnostic](
                const auto& parameter) { return check_assembler_process_operand(parameter.get(), add_diagnostic); });
    }
    // everything is an operand
    return std::all_of(to_check.cbegin(), to_check.cend(), [this, &add_diagnostic](const auto& operand) {
        return check_assembler_process_operand(operand, add_diagnostic);
    });
}

acontrol::acontrol(const std::vector<label_types>& allowed_types, const std::string& name_of_instruction)
    : assembler_instruction(allowed_types, name_of_instruction, 1, -1) {};

bool acontrol::check(const std::vector<const asm_operand*>& to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    if (!operands_size_corresponding(to_check, stmt_range, add_diagnostic))
        return false;
    for (const auto& operand : to_check)
    {
        auto simple_op = get_simple_operand(operand);
        auto complex_op = get_complex_operand(operand);
        if (simple_op != nullptr) // checking simple operands
        {
            // possible simple operands to check, need to also check for NOCOMPAT and NOTYPECHECK
            const static std::vector<std::string> pair_option_vector = {
                "AFPR", "LIBMAC", "RA2", "LMAC", "NOAFPR", "NOLIBMAC", "NORA2", "NOLMAC"
            };
            if (is_param_in_vector(simple_op->operand_identifier, pair_option_vector)
                || (simple_op->operand_identifier == "NOCOMPAT" || simple_op->operand_identifier == "NOTYPECHECK"
                    || simple_op->operand_identifier == "NOCPAT" || simple_op->operand_identifier == "NOTC"))
                continue;
            add_diagnostic(diagnostic_op::error_A161_ACONTROL_op_format(operand->operand_range));
            return false;
        }
        else if (complex_op != nullptr) // checking complex operands
        {
            const static std::vector<std::string> complex_op_id = {
                "CPAT", "COMPAT", "FLAG", "TC", "TYPECHECK", "OPTABLE"
            };
            if (!is_param_in_vector(complex_op->operand_identifier, complex_op_id))
            {
                add_diagnostic(diagnostic_op::error_A161_ACONTROL_op_format(operand->operand_range));
                return false;
            }
            if (complex_op->operand_parameters.empty())
            {
                add_diagnostic(diagnostic_op::error_A015_minimum(
                    name_of_instruction, complex_op->operand_identifier, 1, stmt_range));
                return false;
            }
            if (complex_op->operand_identifier == "COMPAT" || complex_op->operand_identifier == "CPAT")
            {
                if (!check_compat_operands(complex_op->operand_parameters, name_of_instruction, add_diagnostic))
                    return false;
            }
            else if (complex_op->operand_identifier == "FLAG")
            {
                for (size_t j = 0; j < complex_op->operand_parameters.size(); j++)
                {
                    auto flag_param = get_simple_operand(complex_op->operand_parameters[j].get());
                    if (flag_param == nullptr)
                    {
                        add_diagnostic(diagnostic_op::error_A211_FLAG_op_format(
                            name_of_instruction, complex_op->operand_parameters[j]->operand_range));
                        return false;
                    }
                    if (!check_flag_operand(flag_param, name_of_instruction, add_diagnostic))
                        return false;
                }
            }
            else if (complex_op->operand_identifier == "OPTABLE")
            {
                if (complex_op->operand_parameters.size() > 2 || complex_op->operand_parameters.empty())
                {
                    add_diagnostic(diagnostic_op::error_A018_either(
                        name_of_instruction, complex_op->operand_identifier, 1, 2, stmt_range));
                    return false;
                }
                if (!check_optable_operands(complex_op->operand_parameters, name_of_instruction, add_diagnostic))
                    return false;
            }
            else if (complex_op->operand_identifier == "TYPECHECK" || complex_op->operand_identifier == "TC")
            {
                if (!check_typecheck_operands(complex_op->operand_parameters,
                        name_of_instruction,
                        complex_op->operand_identifier,
                        add_diagnostic))
                    return false;
            }
            else
            {
                assert(false);
                add_diagnostic(diagnostic_op::error_I999(name_of_instruction, stmt_range));
                return false;
            }
        }
        else
        {
            add_diagnostic(diagnostic_op::error_A021_cannot_be_empty(name_of_instruction, operand->operand_range));
            return false;
        }
    }
    return true;
}

} // namespace hlasm_plugin::parser_library::checking
