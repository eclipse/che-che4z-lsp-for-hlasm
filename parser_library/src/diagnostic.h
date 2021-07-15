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

#ifndef HLASMPLUGIN_PARSERLIBRARY_DIAGNOSTIC_H
#define HLASMPLUGIN_PARSERLIBRARY_DIAGNOSTIC_H

// This file contains class diagnostic, which represents LSP diagnostic.
// It also contains definitions (static methods) of almost all diagnostics
// reported by analyzer.


#include <string>
#include <vector>

#include "protocol.h"

namespace hlasm_plugin::parser_library {

/*
diagnostic_op errors:
I000 - NONE
ASSEMBLER INSTR:
A0xx - general errors
        - A00x - complexity of operands
                - A000 - simple operand expected
                - A001 - complex operand expected
                - A002 - simple parameter expected
                - A003 - complex parameter expected
        - A01x - number of operands
                - A010 - number of operands is min x
                - A011 - number of operands is exactly x
                - A012 - number of operands is from x to y
                - A013 - number of operands is either x or y
                - A014 - number of operands is lower than x
                - A015 - number of parameters is min x
                - A016 - number of parameters is exactly x
                - A017 - number of parameters is from x to y
                - A018 - number of parameters is either x or y
                - A019 - number of parameters is lower than x
A1xx - specific instruction errors
        - A10x - XATTR
                - A100 - XATTR identifier error

A2xx - specific option errors
        - A200 - SCOPE option error

MACHINE INSTR:
M0xx - general overall issues
        M00x - wrong instruction
                - M000 - wrong number of operands - has to be number
                - M001 - wrong number of operands - either one or two
                - M002 - wrong number of operands - from one to two
                - M003 - empty operand
        M01x - validation problem
                - M010 - address operand is not valid
M1xx - format problems
        M10x - address expected
                - M100 - D(X,B)
                - M101 - D(L,B)
                - M102 - D(R,B)
                - M103 - D(V,B)
                - M104 - D(B)
        M11x - simple expected
                - M110 - register
                - M111 - mask
                - M112 - immediate value
                - M113 - register immediate
                - M114 - vector register
        M12x - simple operand value errors
                - M120 - register
                - M121 - mask
                - M122 - immediate value
                - M123 - register immediate
                - M124 - vector register
        M13x - address operand value errors
                - M130 - wrong value of displacement
                - M131 - wrong value of base register parameter
                - M132 - wrong value of length parameter
                - M133 - wrong value of register parameter
                - M134 - wrong value of vector register parameter
                - M135 - wrong value of displacement register

M2xx - custom instruction problems
         - M200 - VNOT instruction, last too operands are not equal

I999 - should never happen
         - implementation problem
*/

struct diagnostic_op
{
    diagnostic_severity severity = diagnostic_severity::unspecified;
    std::string code;
    std::string message;
    range diag_range;
    diagnostic_op() = default;

    diagnostic_op(diagnostic_severity severity, std::string code, std::string message, range diag_range = {})
        : severity(severity)
        , code(std::move(code))
        , message(std::move(message))
        , diag_range(std::move(diag_range)) {};

    static diagnostic_op error_I999(const std::string& instr_name, const range& range);

    static diagnostic_op error_A001_complex_op_expected(const std::string& instr_name, const range& range);

    static diagnostic_op error_A004_data_def_expected();

    static diagnostic_op error_A010_minimum(const std::string& instr_name, size_t min_params, const range& range);

    static diagnostic_op error_A011_exact(const std::string& instr_name, size_t number_of_params, const range& range);

    static diagnostic_op error_A012_from_to(
        const std::string& instr_name, size_t number_from, size_t number_to, const range& range);

    static diagnostic_op error_A013_either(
        const std::string& instr_name, int option_one, int option_two, const range& range);

    static diagnostic_op error_A014_lower_than(const std::string& instr_name, size_t number, const range& range);

    static diagnostic_op error_A015_minimum(
        const std::string& instr_name, const std::string& op_name, size_t min_params, const range& range);

    static diagnostic_op error_A016_exact(
        const std::string& instr_name, const std::string& op_name, size_t number_of_params, const range& range);

    static diagnostic_op error_A017_from_to(const std::string& instr_name,
        const std::string& op_name,
        size_t number_from,
        size_t number_to,
        const range& range);

    static diagnostic_op error_A018_either(
        const std::string& instr_name, const std::string& op_name, int option_one, int option_two, const range& range);

    static diagnostic_op error_A019_lower_than(
        const std::string& instr_name, const std::string& op_name, size_t number, const range& range);

    static diagnostic_op error_A020_absolute_val_or_empty_expected(const std::string& instr_name, const range& range);

    static diagnostic_op error_A021_cannot_be_empty(const std::string& instr_name, const range& range);

    // operands

    static diagnostic_op error_A100_XATTR_identifier(const range& range);

    static diagnostic_op error_A101_USING_base_val(const range& range);

    static diagnostic_op error_A102_USING_end_val(const range& range);

    static diagnostic_op error_A103_USING_end_exceed(const range& range);

    static diagnostic_op error_A104_USING_first_format(const range& range);

    static diagnostic_op error_A105_USING_base_register_val(const range& range);

    static diagnostic_op error_A106_TITLE_string_chars(const range& range);

    static diagnostic_op error_A107_RMODE_op_format(const range& range);

    static diagnostic_op error_A108_PUNCH_string_chars(const range& range);

    static diagnostic_op error_A109_PRINT_op_format(const range& range);

    static diagnostic_op error_A110_STACK_last_op_format_val(const std::string& instr_name, const range& range);

    static diagnostic_op error_A111_STACK_other_op_format_val(const std::string& instr_name, const range& range);

    static diagnostic_op error_A112_STACK_option_specified(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A113_STACK_NOPRINT_end(const std::string& instr_name, const range& range);

    static diagnostic_op error_A114_STACK_NOPRINT_solo(const std::string& instr_name, const range& range);

    static diagnostic_op error_A115_ORG_op_format(const range& range);

    static diagnostic_op error_A116_ORG_boundary_operand(const range& range);

    static diagnostic_op error_A117_MNOTE_message_size(const range& range);

    static diagnostic_op error_A118_MNOTE_operands_size(const range& range);

    static diagnostic_op error_A119_MNOTE_first_op_format(const range& range);

    static diagnostic_op error_A120_ISEQ_op_format(const range& range);

    static diagnostic_op error_A121_ISEQ_right_GT_left(const range& range);

    static diagnostic_op error_A122_ICTL_op_format_first(const range& range);

    static diagnostic_op error_A123_ICTL_begin_format(const range& range);

    static diagnostic_op error_A124_ICTL_end_format(const range& range);

    static diagnostic_op error_A125_ICTL_begin_end_diff(const range& range);

    static diagnostic_op error_A126_ICTL_continuation_format(const range& range);

    static diagnostic_op error_A127_ICTL_begin_continuation_diff(const range& range);

    static diagnostic_op error_A129_EXTRN_format(const range& range);

    static diagnostic_op error_A130_EXITCTL_exit_type_format(const range& range);

    static diagnostic_op error_A131_EXITCTL_control_value_format(const range& range);

    static diagnostic_op error_A132_EQU_value_format(const range& range);

    static diagnostic_op error_A133_EQU_len_att_format(const range& range);

    static diagnostic_op error_A134_EQU_type_att_format(const range& range);

    static diagnostic_op error_A135_EQU_asm_type_val_format(const range& range);

    static diagnostic_op error_A136_ENTRY_op_format(const range& range);

    static diagnostic_op error_A137_END_lang_format(const range& range);

    static diagnostic_op error_A138_END_lang_first(const range& range);

    static diagnostic_op error_A139_END_lang_second(const range& range);

    static diagnostic_op error_A140_END_lang_third(const range& range);

    static diagnostic_op error_A141_DROP_op_format(const range& range);

    static diagnostic_op error_A142_COPY_op_format(const range& range);

    static diagnostic_op error_A143_must_be_absolute_expr(const std::string& instr_name, const range& range);

    static diagnostic_op error_A144_CNOP_byte_size(const range& range);

    static diagnostic_op error_A145_CNOP_boundary_size(const range& range);

    static diagnostic_op error_A146_CNOP_byte_GT_boundary(const range& range);

    static diagnostic_op error_A147_CCW_op_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A148_EXPR_op_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A149_CATTR_identifier_format(const range& range);

    static diagnostic_op error_A150_AMODE_op_format(const range& range);

    static diagnostic_op error_A151_ALIAS_op_format(const range& range);

    static diagnostic_op error_A152_ALIAS_C_format(const range& range);

    static diagnostic_op error_A153_ALIAS_X_format(const range& range);

    static diagnostic_op error_A154_ALIAS_X_format_no_of_chars(const range& range);

    static diagnostic_op error_A155_ALIAS_X_format_range(const range& range);

    static diagnostic_op error_A156_AINSERT_second_op_format(const range& range);

    static diagnostic_op error_A157_AINSERT_first_op_size(const range& range);

    static diagnostic_op error_A158_ADATA_val_format(const range& range);

    static diagnostic_op error_A159_ADATA_val_size(const range& range);

    static diagnostic_op error_A160_ADATA_char_string_size(const range& range);

    static diagnostic_op error_A161_ACONTROL_op_format(const range& range);

    static diagnostic_op error_A162_PROCESS_uknown_option(const std::string& option, const range& range);

    // operand parameters

    static diagnostic_op error_A200_SCOPE_param(const std::string& instr_name, const range& range);

    static diagnostic_op error_A201_LINKAGE_param(const std::string& instr_name, const range& range);

    static diagnostic_op error_A202_REF_direct(const std::string& instr_name, const range& range);

    static diagnostic_op error_A203_REF_data(const std::string& instr_name, const range& range);

    static diagnostic_op error_A204_RMODE_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A205_ALIGN_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A206_FILL_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A207_PART_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A208_PRIORITY_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A209_COMPAT_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A210_FLAG_integer_size(const std::string& instr_name, const range& range);

    static diagnostic_op error_A211_FLAG_op_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A212_OPTABLE_first_op(const std::string& instr_name, const range& range);

    static diagnostic_op error_A213_OPTABLE_second_op(const std::string& instr_name, const range& range);

    static diagnostic_op error_A214_TYPECHECK_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A215_CODEPAGE_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A216_CODEPAGE_value(const std::string& instr_name, const range& range);

    static diagnostic_op error_A217_INFO_value(const std::string& instr_name, const range& range);

    static diagnostic_op error_A218_MXREF_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A219_SECTALGN_par_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A220_SECTALGN_par_value(const std::string& instr_name, const range& range);

    static diagnostic_op error_A221_MACH_second_par_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A222_MACH_first_par_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A223_PCONTROL_par_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A224_XREF_par_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A225_SUPRWARN_par_format(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A226_SUPRWARN_par_size(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A227_USING_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A228_USING_complex_param_no(
        const std::string& instr_name, const std::string& param_name, const range& range);

    static diagnostic_op error_A229_USING_WARN_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A230_USING_LIMIT_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A231_USING_LIMIT_decimal(const std::string& instr_name, const range& range);

    static diagnostic_op error_A232_USING_LIMIT_hexa(const std::string& instr_name, const range& range);

    static diagnostic_op error_A233_FAIL_param_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A234_FAIL_complex_param_no(
        const std::string& instr_name, const std::string& param_name, const range& range);

    static diagnostic_op error_A235_FAIL_param_number_format(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A236_FAIL_MAXXERS_value(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A237_FAIL_severity_message(
        const std::string& instr_name, const std::string& op_name, const range& range);

    static diagnostic_op error_A238_REF_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A239_ADATA_char_string_format(const range& range);

    static diagnostic_op error_A240_expression_format(const std::string& instr_name, const range& range);

    static diagnostic_op error_A242_ICTL_op_format_second_third(const range& range);

    static diagnostic_op error_A243_END_expr_format(const range& range);

    static diagnostic_op error_A244_PUNCH_char_string(const range& range);

    static diagnostic_op error_A245_ORG_expression(const range& range);

    static diagnostic_op error_A246_OPSYN(const range& range);

    static diagnostic_op error_A247_must_be_rel_abs_expr(const std::string& instr_name, const range& range);

    static diagnostic_op error_A248_END_lang_char_sequence(const range& range);

    // other

    static diagnostic_op warning_A300_op_apostrophes_missing(const std::string& instr_name, const range& range);

    static diagnostic_op error_A301_op_apostrophes_missing(const std::string& instr_name, const range& range);

    static diagnostic_op error_NOERR(const range& range);

    static bool is_error(const diagnostic_op& diag);

    static diagnostic_op error_M000(const std::string& instr_name, int number, const range& range);

    static diagnostic_op error_M001(const std::string& instr_name, int one, int two, const range& range);

    static diagnostic_op error_M002(const std::string& instr_name, int one, int two, const range& range);

    static diagnostic_op error_M003(const std::string& instr_name, const range& range);

    static diagnostic_op error_M004(const std::string& instr_name, const range& range);

    static diagnostic_op error_M100(const std::string& instr_name, const range& range);

    static diagnostic_op error_M101(const std::string& instr_name, const range& range);

    static diagnostic_op error_D001(const range& range);
    static diagnostic_op error_D002(const range& range);
    static diagnostic_op error_D003(const range& range);
    static diagnostic_op error_D004(const range& range);
    static diagnostic_op error_D005(const range& range);
    static diagnostic_op error_D006(const range& range);
    static diagnostic_op error_D007(const range& range, const std::string& type);
    static diagnostic_op error_D008(
        const range& range, const std::string& type, const std::string& modifier, int min, int max);
    static diagnostic_op error_D009(const range& range, const std::string& type, const std::string& modifier);
    static diagnostic_op error_D010(const range& range, const std::string& type);
    static diagnostic_op error_D011(const range& range);
    static diagnostic_op error_D012(const range& range);
    static diagnostic_op error_D013(const range& range, const std::string& type);
    static diagnostic_op error_D014(const range& range, const std::string& type);
    static diagnostic_op error_D015(const range& range, const std::string& type);
    static diagnostic_op error_D016(const range& range);
    static diagnostic_op error_D017(const range& range, const std::string& type);
    static diagnostic_op error_D018(const range& range, const std::string& type);
    static diagnostic_op error_D019(const range& range);
    static diagnostic_op error_D020(const range& range, const std::string& type);
    static diagnostic_op error_D021(const range& range, const std::string& type);
    static diagnostic_op error_D022(const range& range);
    static diagnostic_op error_D023(const range& range);
    static diagnostic_op error_D024(const range& range, const std::string& type);
    static diagnostic_op warn_D025(const range& range, const std::string& type, const std::string& modifier);
    static diagnostic_op error_D026(const range& range);
    static diagnostic_op error_D027(const range& range);
    static diagnostic_op error_D028(const range& range);
    static diagnostic_op error_D029(const range& range);
    static diagnostic_op error_D030(const range& range, const std::string& type);
    static diagnostic_op warn_D031(const range& range, const std::string& modifier);

    static diagnostic_op error_M102(const std::string& instr_name, const range& range);

    static diagnostic_op error_M103(const std::string& instr_name, const range& range);

    static diagnostic_op error_M104(const std::string& instr_name, const range& range);

    static diagnostic_op error_M110(const std::string& instr_name, const range& range);

    static diagnostic_op error_M111(const std::string& instr_name, const range& range);

    static diagnostic_op error_M112(const std::string& instr_name, const range& range);

    static diagnostic_op error_M113(const std::string& instr_name, const range& range);

    static diagnostic_op error_M114(const std::string& instr_name, const range& range);

    static diagnostic_op error_M120(const std::string& instr_name, const range& range);

    static diagnostic_op error_M121(const std::string& instr_name, const range& range);

    static diagnostic_op error_M122(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M123(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M124(const std::string& instr_name, const range& range);

    static diagnostic_op error_M130(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M131(const std::string& instr_name, const range& range);

    static diagnostic_op error_M132(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M133(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M134(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_M135(const std::string& instr_name, long long from, long long to, const range& range);

    static diagnostic_op error_optional_number_of_operands(
        const std::string& instr_name, int optional_no, int operands_no, const range& range);

    static diagnostic_op error_M010(const std::string& instr_name, const range& range);

    static diagnostic_op warning_M041(const std::string& instr_name, const range& range);

    static diagnostic_op error_M200(const std::string& instr_name, const range& range);

    static diagnostic_op error_E001(const range& range);

    static diagnostic_op error_E010(const std::string& message, const range& range);

    static diagnostic_op error_E011(const std::string& message, const range& range);

    static diagnostic_op error_E012(const std::string& message, const range& range);

    static diagnostic_op error_E013(const std::string& message, const range& range);

    static diagnostic_op error_E020(const std::string& message, const range& range);

    static diagnostic_op error_E021(const std::string& message, const range& range);

    static diagnostic_op error_E022(const std::string& message, const range& range);

    static diagnostic_op error_E030(const std::string& message, const range& range);

    static diagnostic_op error_E031(const std::string& message, const range& range);

    static diagnostic_op error_E032(const std::string& message, const range& range);

    static diagnostic_op error_E033(const range& range);

    static diagnostic_op error_E042(const range& range);

    static diagnostic_op error_E043(const range& range);

    static diagnostic_op error_E044(const range& range);

    static diagnostic_op error_E045(const std::string& message, const range& range);

    static diagnostic_op error_E046(const std::string& message, const range& range);

    static diagnostic_op error_E047(const std::string& message, const range& range);

    static diagnostic_op error_E048(const std::string& message, const range& range);

    static diagnostic_op error_E049(const std::string& message, const range& range);

    static diagnostic_op error_E050(const range& range);

    static diagnostic_op error_E051(const std::string& message, const range& range);

    static diagnostic_op error_E052(const std::string& message, const range& range);

    static diagnostic_op error_E053(const range& range);

    static diagnostic_op error_E054(const range& range);

    static diagnostic_op error_E055(const range& range);

    static diagnostic_op error_E056(const range& range);

    static diagnostic_op error_E057(const range& range);

    static diagnostic_op error_E058(const range& range);

    static diagnostic_op error_E059(const std::string& message, const range& range);

    static diagnostic_op error_E060(const std::string& message, const range& range);

    static diagnostic_op error_E061(const std::string& message, const range& range);

    static diagnostic_op error_E062(const range& range);

    static diagnostic_op error_E063(const range& range);

    static diagnostic_op error_E064(const range& range);

    static diagnostic_op error_E065(const range& range);

    static diagnostic_op error_E066(const range& range);

    static diagnostic_op error_E067(const range& range);

    static diagnostic_op error_E068(const range& range);

    static diagnostic_op error_E069(const range& range);

    static diagnostic_op error_E070(const range& range);

    static diagnostic_op warning_W010(const std::string& message, const range& range);

    static diagnostic_op warning_W011(const range& range);

    static diagnostic_op warning_W012(const range& range);

    static diagnostic_op warning_W013(const range& range);

    static diagnostic_op warning_W014(const range& range);

    static diagnostic_op error_EQU1(const range& range);

    static diagnostic_op error_EQU2(const range& range);

    static diagnostic_op error_ME001(const range& range);

    static diagnostic_op error_ME002(const range& range);

    static diagnostic_op error_ME003(const range& range);

    static diagnostic_op error_CE001(const range& range);

    static diagnostic_op error_CE002(const std::string& message, const range& range);

    static diagnostic_op error_CE003(const range& range);

    static diagnostic_op error_CE004(const range& range);

    static diagnostic_op error_CE005(const range& range);

    static diagnostic_op error_CE006(const range& range);

    static diagnostic_op error_CE007(const range& range);

    static diagnostic_op error_CE008(const range& range);

    static diagnostic_op error_CE009(const range& range);

    static diagnostic_op error_CE010(const range& range);

    static diagnostic_op error_CE011(const range& range);

    static diagnostic_op error_CE012(const range& range);

    static diagnostic_op error_CE013(const range& range);

    static diagnostic_op error_CE014(const range& range);

    static diagnostic_op error_CE015(const range& range);

    static diagnostic_op error_CE016_logical_expression_parenthesis(const range& range);

    static diagnostic_op error_CW001(const range& range);

    static diagnostic_op error_S100(const std::string& message, const range& range);

    static diagnostic_op error_P0001(const range& range);

    static diagnostic_op error_P0002(const range& range, std::string_view lib);

    static diagnostic_op error_P0003(const range& range, std::string_view lib);
};

struct range_uri_s
{
    range_uri_s() {};
    range_uri_s(std::string uri, range range)
        : uri(std::move(uri))
        , rang(range)
    {}

    std::string uri;
    range rang;
};

// Represents related info (location with message) of LSP diagnostic.
class diagnostic_related_info_s
{
public:
    diagnostic_related_info_s() {}
    diagnostic_related_info_s(range_uri_s location, std::string message)
        : location(std::move(location))
        , message(std::move(message))
    {}
    range_uri_s location;
    std::string message;
};

// Represents a LSP diagnostic.
class diagnostic_s
{
public:
    diagnostic_s()
        : severity(diagnostic_severity::unspecified)
    {}
    diagnostic_s(std::string file_name, range range, std::string code, std::string message)
        : file_name(std::move(file_name))
        , diag_range(range)
        , severity(diagnostic_severity::unspecified)
        , code(code)
        , message(std::move(message))
    {}
    diagnostic_s(std::string file_name,
        range range,
        diagnostic_severity severity,
        std::string code,
        std::string message,
        std::vector<diagnostic_related_info_s> related)
        : file_name(std::move(file_name))
        , diag_range(range)
        , severity(severity)
        , code(std::move(code))
        , source("HLASM Plugin")
        , message(std::move(message))
        , related(std::move(related))
    {}
    diagnostic_s(std::string file_name, diagnostic_op diag_op)
        : file_name(std::move(file_name))
        , diag_range(std::move(diag_op.diag_range))
        , severity(diag_op.severity)
        , code(std::move(diag_op.code))
        , source("HLASM Plugin")
        , message(std::move(diag_op.message))
    {}
    diagnostic_s(diagnostic_op diag_op)
        : diag_range(std::move(diag_op.diag_range))
        , severity(diag_op.severity)
        , code(std::move(diag_op.code))
        , source("HLASM Plugin")
        , message(std::move(diag_op.message))
    {}


    std::string file_name;
    range diag_range;
    diagnostic_severity severity;
    std::string code;
    std::string source;
    std::string message;
    std::vector<diagnostic_related_info_s> related;

    /*
    Lxxxx - local library messages
    - L0001 - Error loading library
    - L0002 - Library does not exist
    - L0003 - Deprecated file extension specification was used
    - L0004 - Macro with multiple definitions
    */
    static diagnostic_s error_L0001(const std::string& path);

    static diagnostic_s error_L0002(const std::string& path);

    static diagnostic_s warning_L0003(const std::string& path);

    static diagnostic_s warning_L0004(const std::string& path, const std::string& macro_name);

    static diagnostic_s error_L0005(const std::string& path);

    static diagnostic_s error_W002(const std::string& file_name, const std::string& ws_name);

    static diagnostic_s error_W003(const std::string& file_name, const std::string& ws_name);

    static diagnostic_s error_W004(const std::string& file_name, const std::string& ws_name);

    static diagnostic_s error_W005(const std::string& file_name, const std::string& proc_group);

    /*
    E01x - wrong format
    - E010 - unknown name
    - E011 - operand/param already exist and are defined
    - E012 - wrong format (exists but contains space etc)
    - E013 - inconsistent format (with instruction, operation etc)

    E02x - operand/parameter/instruction number issues
    - E020 - operands number problem (too many)
    - E021 - operands number problem (too little)
    - E022 - missing operand

    E03x - runtime problems
    - E030 - assigment not allowed
    - E031 - naming problem - name already exists

    W01x - wrong format
    - W010 - unexpected field/name/instr

    */
};

} // namespace hlasm_plugin::parser_library

#endif
