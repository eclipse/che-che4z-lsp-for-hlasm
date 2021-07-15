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

#include "diagnostic.h"

#include <string>

namespace hlasm_plugin::parser_library {

// diagnostic_op errors

// asembler instruction errors

diagnostic_op diagnostic_op::error_I999(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "I999",
        "Fatal error at " + instr_name + " instruction: implementation error.",
        range);
}

diagnostic_op diagnostic_op::error_A001_complex_op_expected(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A001",
        "Error at " + instr_name + ": operand in a form identifier(parameters) expected",
        range);
}

diagnostic_op diagnostic_op::error_A004_data_def_expected()
{
    return diagnostic_op(diagnostic_severity::error, "A004", "Data definition operand expected");
}

diagnostic_op diagnostic_op::error_A010_minimum(const std::string& instr_name, size_t min_params, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A010",
        "Error at " + instr_name + " instruction: number of operands has to be at least " + std::to_string(min_params),
        range);
}

diagnostic_op diagnostic_op::error_A011_exact(
    const std::string& instr_name, size_t number_of_params, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A011",
        "Error at " + instr_name + " instruction: number of operands has to be " + std::to_string(number_of_params),
        range);
}

diagnostic_op diagnostic_op::error_A012_from_to(
    const std::string& instr_name, size_t number_from, size_t number_to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A012",
        "Error at " + instr_name + " instruction: number of operands has to be from " + std::to_string(number_from)
            + " to " + std::to_string(number_to),
        range);
}

diagnostic_op diagnostic_op::error_A013_either(
    const std::string& instr_name, int option_one, int option_two, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A013",
        "Error at " + instr_name + " instruction: number of operands has to be either " + std::to_string(option_one)
            + " or " + std::to_string(option_two),
        range);
}

diagnostic_op diagnostic_op::error_A014_lower_than(const std::string& instr_name, size_t number, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A014",
        "Error at " + instr_name + " instruction: number of operands has to be lower than " + std::to_string(number),
        range);
}

diagnostic_op diagnostic_op::error_A015_minimum(
    const std::string& instr_name, const std::string& op_name, size_t min_params, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A015",
        "Error at " + instr_name + " instruction at " + op_name + " operand: number of parameters has to be at least "
            + std::to_string(min_params),
        range);
}

diagnostic_op diagnostic_op::error_A016_exact(
    const std::string& instr_name, const std::string& op_name, size_t number_of_params, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A016",
        "Error at " + instr_name + " instruction at " + op_name + " operand: number of parameters has to be "
            + std::to_string(number_of_params),
        range);
}

diagnostic_op diagnostic_op::error_A017_from_to(
    const std::string& instr_name, const std::string& op_name, size_t number_from, size_t number_to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A017",
        "Error at " + instr_name + " instruction at " + op_name + " operand: number of parameters has to be from "
            + std::to_string(number_from) + " to " + std::to_string(number_to),
        range);
}

diagnostic_op diagnostic_op::error_A018_either(
    const std::string& instr_name, const std::string& op_name, int option_one, int option_two, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A018",
        "Error at " + instr_name + " instruction at " + op_name + " operand: number of parameters has to be either "
            + std::to_string(option_one) + " or " + std::to_string(option_two),
        range);
}

diagnostic_op diagnostic_op::error_A019_lower_than(
    const std::string& instr_name, const std::string& op_name, size_t number, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A019",
        "Error at " + instr_name + " instruction at " + op_name + " operand: number of parameters has to be lower than "
            + std::to_string(number),
        range);
}

diagnostic_op diagnostic_op::error_A020_absolute_val_or_empty_expected(
    const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A020",
        "Error at " + instr_name + " instruction: operand must either specify an absolute value or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A021_cannot_be_empty(const std::string& instr_name, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "A021", "Error at " + instr_name + " instruction: operand cannot be empty", range);
}

diagnostic_op diagnostic_op::error_A100_XATTR_identifier(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A100",
        "Error at XATTR instruction: operand identifier value must be one of the following: ATTRIBUTES|ATTR, "
        "LINKAGE|LINK, SCOPE, PSECT, REFERENCE|REF",
        range);
}

diagnostic_op diagnostic_op::error_A101_USING_base_val(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A101",
        "Error at USING instruction: first operand base value must lie between 0 and 2^31-1",
        range);
}

diagnostic_op diagnostic_op::error_A102_USING_end_val(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A102",
        "Error at USING instruction: first operand end value must lie between 0 and 2^31-1 or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A103_USING_end_exceed(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A103",
        "Error at USING instruction: first operand end value must exceed base value or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A104_USING_first_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A104",
        "Error at USING instruction: first operand format must either be an absolute base value, or must be in a form "
        "(base,end)",
        range);
}

diagnostic_op diagnostic_op::error_A105_USING_base_register_val(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A105",
        "Error at USING instruction: base register value must lie between 0 and 15",
        range);
}

diagnostic_op diagnostic_op::error_A106_TITLE_string_chars(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A106",
        "Error at TITLE instruction: operand value must be a title string of 1 to 100 characters",
        range);
}

diagnostic_op diagnostic_op::error_A107_RMODE_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A108",
        "Error at RMODE instruction: operand value must be one of the following: 24, 31, 64, ANY",
        range);
}

diagnostic_op diagnostic_op::error_A108_PUNCH_string_chars(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A108",
        "Error at PUNCH instruction: operand value must be a string of maximum 80 characters",
        range);
}

diagnostic_op diagnostic_op::error_A109_PRINT_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A109",
        "Error at PRINT instruction: operand value must be one of the following: GEN|NOGEN, DATA|NODATA, "
        "MCALL|NOMCALL, MSOURCE|NOMSOURCE, UHEAD|NOUHEAD, ON, OFF, NOPRINT",
        range);
}

diagnostic_op diagnostic_op::error_A110_STACK_last_op_format_val(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A110",
        "Error at " + instr_name
            + " instruction: last operand value must be one of the following: " + "PRINT, USING, ACONTROL, NOPRINT",
        range);
}

diagnostic_op diagnostic_op::error_A111_STACK_other_op_format_val(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A111",
        "Error at " + instr_name + " instruction: operand value must be one of the following:"
            + " PRINT, USING, ACONTROL (NOPRINT can be specified for last operand of " + instr_name + " only)",
        range);
}

diagnostic_op diagnostic_op::error_A112_STACK_option_specified(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A112",
        "Error at " + instr_name + " instruction: " + op_name + " can be specified only once",
        range);
}

diagnostic_op diagnostic_op::error_A113_STACK_NOPRINT_end(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A113",
        "Error at " + instr_name + " instruction: " + "NOPRINT option can be specified for last operand only",
        range);
}

diagnostic_op diagnostic_op::error_A114_STACK_NOPRINT_solo(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A114",
        "Error at " + instr_name + " instruction: "
            + "NOPRINT option cannot be the only option specified. Other possible operand values: PRINT, USING, "
              "ACONTROL.",
        range);
}

diagnostic_op diagnostic_op::error_A115_ORG_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A115",
        "Error at ORG instruction: operand value must be either an absolute expression or the operand must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A116_ORG_boundary_operand(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A116",
        "Error at ORG instruction: boundary operand value must be either a number that is a power of 2 with a range "
        "from 2 to 4096, or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A117_MNOTE_message_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A117",
        "Error at MNOTE instruction: operand must specify a message string of maximum 1020 characters",
        range);
}

diagnostic_op diagnostic_op::error_A118_MNOTE_operands_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A118",
        "Error at MNOTE instruction: the total size of all operands cannot exceed 1024 bytes",
        range);
}

diagnostic_op diagnostic_op::error_A119_MNOTE_first_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A119",
        "Error at MNOTE instruction: first operand must be either a severity value in the range 0 through 255, an "
        "asterisk or the operand must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A120_ISEQ_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A120",
        "Error at ISEQ instruction: operands must be both either omitted, or they both must be decimal self-defining "
        "terms with value in the range 1 through 80",
        range);
}

diagnostic_op diagnostic_op::error_A121_ISEQ_right_GT_left(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A121",
        "Error at ISEQ instruction: right operand value must be greater than left operand value, or both operands must "
        "be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A122_ICTL_op_format_first(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A122",
        "Error at ICTL instruction: operand value must be a decimal self-defining term",
        range);
}

diagnostic_op diagnostic_op::error_A123_ICTL_begin_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A123",
        "Error at ICTL instruction: begin operand must be a self-defining term with value in the range 1 through 40",
        range);
}

diagnostic_op diagnostic_op::error_A124_ICTL_end_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A124",
        "Error at ICTL instruction: end operand must be a self-defining term with value in the range 41 through 80",
        range);
}

diagnostic_op diagnostic_op::error_A125_ICTL_begin_end_diff(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A125",
        "Error at ICTL instruction: end operand value cannot not be less than begin+5",
        range);
}

diagnostic_op diagnostic_op::error_A126_ICTL_continuation_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A126",
        "Error at ICTL instruction: continuation operand must be a self-defining term with value in the range 2 "
        "through 40",
        range);
}

diagnostic_op diagnostic_op::error_A127_ICTL_begin_continuation_diff(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A127",
        "Error at ICTL instruction: continuation operand value must be greater than begin value",
        range);
}

diagnostic_op diagnostic_op::error_A129_EXTRN_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A129",
        "Error at EXTRN instruction: operand must either specify an external symbol, or it must be in a format "
        "PART(external symbols), where external symbols must also be a reference to a part as defined on the CATTR "
        "instruction.",
        range);
}

diagnostic_op diagnostic_op::error_A130_EXITCTL_exit_type_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A130",
        "Error at EXITCTL instruction: first operand must specify an exit type, which must have one of the following "
        "values: SOURCE, LIBRARY, LISTING, PUNCH, ADATA, TERM, OBJECT",
        range);
}

diagnostic_op diagnostic_op::error_A131_EXITCTL_control_value_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A131",
        "Error at EXITCTL instruction: operand must either be omitted or must specify a control value in the range "
        "-2^31 through 2^31-1.",
        range);
}

diagnostic_op diagnostic_op::error_A132_EQU_value_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A132",
        "Error at EQU instruction: first operand must represent either an absolute value, a relocatable value, or a "
        "complexly relocatable value",
        range);
}

diagnostic_op diagnostic_op::error_A133_EQU_len_att_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A133",
        "Error at EQU instruction: operand representing length attribute value must either be an absolute value in the "
        "range 0 throught 65535 or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A134_EQU_type_att_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A134",
        "Error at EQU instruction: operand representing type attribute value must either be an absolute value in the "
        "range 0 throught 255 or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A135_EQU_asm_type_val_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A135",
        "Error at EQU instruction: operand representing assembler type value must either be omitted or must have one "
        "of the following values: AR, CR, CR32, CR64, FPR, GR, VR, GR32, GR64",
        range);
}

diagnostic_op diagnostic_op::error_A136_ENTRY_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A136",
        "Error at ENTRY instruction: operand must specify an entry point, which must be a valid relocatable symbol",
        range);
}

diagnostic_op diagnostic_op::error_A137_END_lang_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A137",
        "Error at END instruction: language operand must be in the following format: (char10, char4, char5)",
        range);
}

diagnostic_op diagnostic_op::error_A138_END_lang_first(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A138",
        "Error at END instruction: first parameter of language operand (language translator identifier) must be a 1 to "
        "10 character code",
        range);
}

diagnostic_op diagnostic_op::error_A139_END_lang_second(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A139",
        "Error at END instruction: second parameter of language operand (version and release code) must be exactly 4 "
        "character long",
        range);
}

diagnostic_op diagnostic_op::error_A140_END_lang_third(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A140",
        "Error at END instruction: third parameter of language operand must be exactly 5 character long and must "
        "specify the compile date in the \"YYDDD\" format",
        range);
}

diagnostic_op diagnostic_op::error_A141_DROP_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A141",
        "Error at DROP instruction: operand must either specify a base register (value 0 through 15), label (either an "
        "ordinary or a variable symbol) or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A142_COPY_op_format(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "A142", "Error at COPY instruction: operand must specify a member name", range);
}

diagnostic_op diagnostic_op::error_A143_must_be_absolute_expr(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A143",
        "Error at " + instr_name + " instruction: operand value must be an absolute expression",
        range);
}

diagnostic_op diagnostic_op::error_A144_CNOP_byte_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A144",
        "Error at CNOP instruction: byte operand must specify an even positive value",
        range);
}

diagnostic_op diagnostic_op::error_A145_CNOP_boundary_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A145",
        "Error at CNOP instruction: boundary operand must specify a value that is a power of 2 with a range from 2 to "
        "4096",
        range);
}

diagnostic_op diagnostic_op::error_A146_CNOP_byte_GT_boundary(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A146",
        "Error at CNOP instruction: the value of byte operand value must be in range 0 through boundary-2",
        range);
}

diagnostic_op diagnostic_op::error_A147_CCW_op_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A147",
        "Error at " + instr_name + " instruction: all operands must be specified",
        range);
}

diagnostic_op diagnostic_op::error_A148_EXPR_op_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A148",
        "Error at " + instr_name
            + " instruction: operand must either specify a non-negative absolute value, or it must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A149_CATTR_identifier_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A149",
        "Error at CATTR instruction: binder attribute operand must be in one of the following formats: DEFLOAD, "
        "EXECUTABLE, NOTEXECUTABLE, MOVABLE, NOLOAD, NOTREUS, READONLY, REFR, REMOVABLE, RENT, REUS, "
        "RMODE(24|31|64|ANY), ALIGN(n), FILL(nnn), PART(part-name), PRIORITY(nnnnn)",
        range);
}

diagnostic_op diagnostic_op::error_A150_AMODE_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A150",
        "Error at AMODE instruction: operand must have one of the following values: 24, 31, 64, ANY, ANY31, ANY64",
        range);
}

diagnostic_op diagnostic_op::error_A151_ALIAS_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A151",
        "Error at ALIAS instruction: operand must specify an alias string in one of the following formats: "
        "C'aaaaaaaa', X'xxxxxxxx'",
        range);
}

diagnostic_op diagnostic_op::error_A152_ALIAS_C_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A152",
        "Error at ALIAS instruction: C'aaaaaaaa' operands format supports only characters with hexadecimal value in "
        "the range X'42' - X'FE'",
        range);
}

diagnostic_op diagnostic_op::error_A153_ALIAS_X_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A153",
        "Error at ALIAS instruction: the string 'xxxxxxxx' of operand format X'xxxxxxxx' must contain only hexadecimal "
        "digits",
        range);
}

diagnostic_op diagnostic_op::error_A154_ALIAS_X_format_no_of_chars(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A154",
        "Error at ALIAS instruction: the string 'xxxxxxxx' of X'xxxxxxxx' operand format must have an even length",
        range);
}

diagnostic_op diagnostic_op::error_A155_ALIAS_X_format_range(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A155",
        "Error at ALIAS instruction: each character pair of the string 'xxxxxxxx' of operand format X'xxxxxxxx' must "
        "be in the range X'42' - X'FE'",
        range);
}

diagnostic_op diagnostic_op::error_A156_AINSERT_second_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A156",
        "Error at AINSERT instruction: second operand must have one of the following values: BACK, FRONT",
        range);
}

diagnostic_op diagnostic_op::error_A157_AINSERT_first_op_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A157",
        "Error at AINSERT instruction: record operand must be a string enclosed in apostrophes of maximum 80 "
        "characters",
        range);
}

diagnostic_op diagnostic_op::error_A158_ADATA_val_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A158",
        "Error at ADATA instruction: operand value must either be a decimal self defining term, or the operand must be "
        "omitted",
        range);
}

diagnostic_op diagnostic_op::error_A159_ADATA_val_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A159",
        "Error at ADATA instruction: operand value must be in range -2^32 through 2^32-1",
        range);
}

diagnostic_op diagnostic_op::error_A160_ADATA_char_string_size(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A160",
        "Error at ADATA instruction: the maximum size of character string operand is 255 bytes",
        range);
}

diagnostic_op diagnostic_op::error_A161_ACONTROL_op_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A161",
        "Error at ACONTROL instruction: operand must be in one of the following formats: AFPR|NOAFPR, RA2|NORA2, "
        "LIBMAC|LMAC, NOLIBMAC|NOLMAC, (COMPAT|CPAT)(keywords), NOCOMPAT|NOCPAT, FLAG(flags), OPTABLE(opcode, "
        "listing?), (TYPECHECK|TC)(parameters), NOTYPECHECK|NOTC",
        range);
}

diagnostic_op diagnostic_op::error_A162_PROCESS_uknown_option(const std::string& option, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "A162", "Error at *PROCESS instruction: unknown assembler option " + option, range);
}

diagnostic_op diagnostic_op::error_A200_SCOPE_param(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A200",
        "Error at " + instr_name + " instruction: SCOPE operand parameter name must be one of the following: "
            + "SECTION|S, MODULE|M, LIBRARY|L, IMPORT|X, EXPORT|X",
        range);
}

diagnostic_op diagnostic_op::error_A201_LINKAGE_param(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A201",
        "Error at " + instr_name
            + " instruction: LINKAGE operand parameter value must be one of the following: " + "OS, XPLINK",
        range);
}

diagnostic_op diagnostic_op::error_A202_REF_direct(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A202",
        "Error at " + instr_name
            + " instruction at REFERENCE operand: both DIRECT and INDIRECT parameter cannot be specified",
        range);
}

diagnostic_op diagnostic_op::error_A203_REF_data(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A203",
        "Error at " + instr_name
            + " instruction at REFERENCE operand: both DATA and CODE parameter cannot be specified",
        range);
}

diagnostic_op diagnostic_op::error_A204_RMODE_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A204",
        "Error at " + instr_name
            + " instruction: RMODE operand parameter value must be one of the following: 24, 31, 64, ANY",
        range);
}

diagnostic_op diagnostic_op::error_A205_ALIGN_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A205",
        "Error at " + instr_name
            + " instruction: ALIGN operand parameter value must be one of the following: 0, 1, 2, 3, 4, 12",
        range);
}

diagnostic_op diagnostic_op::error_A206_FILL_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A206",
        "Error at " + instr_name
            + " instruction: FILL operand parameter must be an unsigned decimal number in range 0 through 255",
        range);
}

diagnostic_op diagnostic_op::error_A207_PART_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A207",
        "Error at " + instr_name
            + " instruction: PART operand parameter value must be a part-name of maximum 63 characters",
        range);
}

diagnostic_op diagnostic_op::error_A208_PRIORITY_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A208",
        "Error at " + instr_name
            + " instruction: PRIORITY operand parameter value must be an unsigned decimal number in range 0 through "
              "2^31-1",
        range);
}

diagnostic_op diagnostic_op::error_A209_COMPAT_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A209",
        "Error at " + instr_name
            + " instruction: COMPAT option parameter must have one of the following values: CASE, NOCASE, LITTYPE|LIT, "
              "NOLITTYPE|NOLIT, MACROCASE|MC, NOMACROCASE|NOMC, SYSLIST|SYSL, NOSYSLIST|NOSYSL, TRANSDT|TRS, "
              "NOTRANSDT|NOTRS",
        range);
}

diagnostic_op diagnostic_op::error_A210_FLAG_integer_size(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A210",
        "Error at " + instr_name
            + " instruction: the value of FLAG option parameter specifying error diagnostic message must be in range 0 "
              "through 255",
        range);
}

diagnostic_op diagnostic_op::error_A211_FLAG_op_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A211",
        "Error at " + instr_name
            + " instruction: FLAG option parameter must have one of the following values: ALIGN|AL, NOALIGN|NOAL, "
              "CONT, NOCONT, PAGE0, NOPAGE0, SUB, NOSUB, USING0|US0, NOUSING0|NOUS0, IMPLEN, NOIMPLEN, EXLITW, "
              "NOEXLITW, SUBSTR|NOSUBSTR",
        range);
}

diagnostic_op diagnostic_op::error_A212_OPTABLE_first_op(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A212",
        "Error at " + instr_name
            + " instruction: first parameter of the OPTABLE option must be one of the following values: DOS, ESA, XA, "
              "370, YOP, ZOP, ZS3, ZS4, ZS5, ZS6, ZS7, ZS8",
        range);
}

diagnostic_op diagnostic_op::error_A213_OPTABLE_second_op(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A213",
        "Error at " + instr_name
            + " instruction: second parameter of the OPTABLE option must either be omitted or must have one of the "
              "following values: LIST, NOLIST",
        range);
}

diagnostic_op diagnostic_op::error_A214_TYPECHECK_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A214",
        "Error at " + instr_name
            + " instruction: the parameter of the TYPECHECK option must be one of the following values: MAGNITUDE|MAG, "
              "REGISTER|REG, NOMAGNITUDE|NOMAG, NOREGISTER|NOREG",
        range);
}

diagnostic_op diagnostic_op::error_A215_CODEPAGE_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A215",
        "Error at " + instr_name
            + " instruction: the parameter of the CODEPAGE option must be either in a nnnnn format specifying a "
              "decimal value, or a X'xxxx' format specifying a hexadecimal value",
        range);
}

diagnostic_op diagnostic_op::error_A216_CODEPAGE_value(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A216",
        "Error at " + instr_name
            + " instruction: the value of the parameter of the CODEPAGE option must evaluate to an absolute value in "
              "the range 1140 through 1148",
        range);
}

diagnostic_op diagnostic_op::error_A217_INFO_value(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A217",
        "Error at " + instr_name
            + " instruction: INFO parameter must either be in a yyyymmdd format specifying date, or it must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A218_MXREF_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A218",
        "Error at " + instr_name
            + " instruction: parameter of the MXREF option must either be omitted or must be one of the following "
              "values: FULL, XREF, SOURCE",
        range);
}

diagnostic_op diagnostic_op::error_A219_SECTALGN_par_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A219",
        "Error at " + instr_name
            + " instruction at SECTALGN option: alignment parameter must specify a positive absolute value",
        range);
}

diagnostic_op diagnostic_op::error_A220_SECTALGN_par_value(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A220",
        "Error at " + instr_name
            + " instruction at SECTALGN option: alignment parameter value must be a power of 2 in the range 8 through "
              "4096",
        range);
}

diagnostic_op diagnostic_op::error_A221_MACH_second_par_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A221",
        "Error at " + instr_name
            + " instruction: second parameter of the MACHINE option must either be omitted or must have one of the "
              "following values: LIST, NOLIST",
        range);
}

diagnostic_op diagnostic_op::error_A222_MACH_first_par_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A222",
        "Error at " + instr_name
            + " instruction: first parameter of the MACHINE option must have one of the following values: S370, "
              "S370XA, S370ESA, S390, S390E, ZSERIES, ZS, (ZS|ZSERIES)-(2|3|4|5|6|7|8)",
        range);
}

diagnostic_op diagnostic_op::error_A223_PCONTROL_par_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A223",
        "Error at " + instr_name
            + " instruction: PCONTROL option parameter must have one of the following values: ON, OFF, MCALL|MC, "
              "NOMCALL|NOMC, MSOURCE|MS, NOMSOURCE|NOMS, UHEAD|UHD, NOUHEAD|NOUHD, GEN, NOGEN, DATA, NODATA",
        range);
}

diagnostic_op diagnostic_op::error_A224_XREF_par_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A224",
        "Error at " + instr_name
            + " instruction: XREF option must either have exactly one parameter with FULL value, or multiple "
              "parameters with either SHORT or UNREFS value",
        range);
}

diagnostic_op diagnostic_op::error_A225_SUPRWARN_par_format(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A225",
        "Error at " + instr_name + " instruction at " + op_name
            + " option: operand must specify a 1-4 digit message number",
        range);
}

diagnostic_op diagnostic_op::error_A226_SUPRWARN_par_size(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A226",
        "Error at " + instr_name + " instruction at " + op_name + " option: message parameter must contain 1-4 digits",
        range);
}

diagnostic_op diagnostic_op::error_A227_USING_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A227",
        "Error at " + instr_name
            + " instruction: USING option parameter must have one of the following values: MAP, NOMAP, WARN(n), "
              "NOWARN, LIMIT(xxxx), NOLIMIT",
        range);
}

diagnostic_op diagnostic_op::error_A228_USING_complex_param_no(
    const std::string& instr_name, const std::string& param_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A228",
        "Error at " + instr_name + " instruction at USING option: " + param_name
            + " parameter must be specified in the following format " + param_name + "(value)",
        range);
}

diagnostic_op diagnostic_op::error_A229_USING_WARN_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A229",
        "Error at " + instr_name
            + " instruction at USING option: the condition number associated with the WARN(n) suboption must be in "
              "range 0 through 15",
        range);
}

diagnostic_op diagnostic_op::error_A230_USING_LIMIT_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A230",
        "Error at " + instr_name
            + " instruction at USING option: value associated with the LIMIT(xxxx) suboption must be either a decimal "
              "value, or must be in a X'xxx' format specifying a hexadecimal value",
        range);
}

diagnostic_op diagnostic_op::error_A231_USING_LIMIT_decimal(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A231",
        "Error at " + instr_name
            + " instruction at USING option: the maximum value of the decimal value xxxx associated with the "
              "LIMIT(xxxx) suboption is 4095",
        range);
}

diagnostic_op diagnostic_op::error_A232_USING_LIMIT_hexa(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A232",
        "Error at " + instr_name
            + " instruction at USING option: the maximum value of the hexadecimal value xxxx associated with the "
              "LIMIT(xxxx) suboption is FFF",
        range);
}

diagnostic_op diagnostic_op::error_A233_FAIL_param_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A233",
        "Error at " + instr_name
            + " instruction: FAIL option parameter must have one of the following values: MSG(msgsev), NOMSG, "
              "MNOTE(mnotesev), NOMNOTE, MAXERRS(maxerrs), NOMAXERRS(maxerrs)",
        range);
}

diagnostic_op diagnostic_op::error_A234_FAIL_complex_param_no(
    const std::string& instr_name, const std::string& param_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A234",
        "Error at " + instr_name + " instruction at FAIL option: " + param_name
            + " parameter must be specified in the following format " + param_name + "(value)",
        range);
}

diagnostic_op diagnostic_op::error_A235_FAIL_param_number_format(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A235",
        "Error at " + instr_name + " instruction at FAIL option: the parameter of the " + op_name
            + " suboption must be an absolute value",
        range);
}

diagnostic_op diagnostic_op::error_A236_FAIL_MAXXERS_value(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A236",
        "Error at " + instr_name + " instruction at FAIL option: the value of the maxerrs parameter specified in "
            + op_name + "(maxerrs) suboption must be in range 32 through 65535",
        range);
}

diagnostic_op diagnostic_op::error_A237_FAIL_severity_message(
    const std::string& instr_name, const std::string& op_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A237",
        "Error at " + instr_name + " instruction at FAIL option: the value of the parameter of the " + op_name
            + " suboption must be in range 0 through 7",
        range);
}

diagnostic_op diagnostic_op::error_A238_REF_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A288",
        "Error at " + instr_name
            + " instruction at REFERENCE option: parameter must be in one of the following formats: DIRECT|INDIRECT, "
              "CODE|DATA",
        range);
}

diagnostic_op diagnostic_op::error_A239_ADATA_char_string_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A239",
        "Error at ADATA instruction: the last operand must either be a character string enclosed in single quotes, or "
        "it must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A240_expression_format(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A240",
        "Error at " + instr_name + " instruction: operand must either specify an absolute value or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A242_ICTL_op_format_second_third(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A242",
        "Error at ICTL instruction: operand value must either be a decimal self-defining term or the operand must be "
        "omitted",
        range);
}

diagnostic_op diagnostic_op::error_A243_END_expr_format(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A243",
        "Error at END instruction: first operand must either be an expresison or the operand must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A244_PUNCH_char_string(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A244",
        "Error at PUNCH instruction: operand must specify a character string",
        range);
}

diagnostic_op diagnostic_op::error_A245_ORG_expression(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A245",
        "Error at ORG instruction: operand must specify relocatable expression or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A246_OPSYN(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A246",
        "Error at OPSYN instruction: operand must either specify an operation code or must be omitted",
        range);
}

diagnostic_op diagnostic_op::error_A247_must_be_rel_abs_expr(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A247",
        "Error at " + instr_name + " instruction: operand must be a relocatable or an absolute expression",
        range);
}

diagnostic_op diagnostic_op::error_A248_END_lang_char_sequence(const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A248",
        "Error at END instruction at language operand: parameter must be a character sequence",
        range);
}

diagnostic_op diagnostic_op::warning_A300_op_apostrophes_missing(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::warning,
        "A300",
        "Warning at " + instr_name + " instruction: operand not properly enclosed in quotes",
        range);
}

diagnostic_op diagnostic_op::error_A301_op_apostrophes_missing(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "A301",
        "Error at " + instr_name + " instruction: operand not properly enclosed in quotes",
        range);
}

diagnostic_op diagnostic_op::error_NOERR(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "NOERR", "No error found", range);
}

bool diagnostic_op::is_error(const diagnostic_op& diag) { return diag.code != "NOERR"; }


// machine instruction errors

diagnostic_op diagnostic_op::error_M100(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M100",
        "Error at " + instr_name + " instruction: operand must be in an address D(X,B) format",
        range);
}

diagnostic_op diagnostic_op::error_M101(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M101",
        "Error at " + instr_name + " instruction: operand must be in an address D(L,B) format",
        range);
}

diagnostic_op diagnostic_op::error_M102(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M102",
        "Error at " + instr_name + " instruction: operand must be in an address D(R,B) format",
        range);
}

diagnostic_op diagnostic_op::error_M103(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M103",
        "Error at " + instr_name + " instruction: operand must be in an address D(V,B) format",
        range);
}

diagnostic_op diagnostic_op::error_M104(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M104",
        "Error at " + instr_name + " instruction: operand must be in an address D(B) format",
        range);
}

diagnostic_op diagnostic_op::error_M110(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M110",
        "Error at " + instr_name + " instruction: operand must be an absolute register value",
        range);
}

diagnostic_op diagnostic_op::error_M111(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M111",
        "Error at " + instr_name + " instruction: operand must be an absolute mask value",
        range);
}

diagnostic_op diagnostic_op::error_M112(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M112",
        "Error at " + instr_name + " instruction: operand must be an absolute immediate value",
        range);
}

diagnostic_op diagnostic_op::error_M113(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M113",
        "Error at " + instr_name + " instruction: operand must be relocatable symbol or an absolute  immediate value",
        range);
}

diagnostic_op diagnostic_op::error_M114(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M114",
        "Error at " + instr_name + " instruction: operand must be an absolute vector register value",
        range);
}

diagnostic_op diagnostic_op::error_M120(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M120",
        "Error at " + instr_name + " instruction: register operand absolute value must be between 0 and 15",
        range);
}

diagnostic_op diagnostic_op::error_M121(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M121",
        "Error at " + instr_name + " instruction: mask operand absolute value must be between 0 and 15",
        range);
}

diagnostic_op diagnostic_op::error_M122(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M122",
        "Error at " + instr_name + " instruction: immediate operand absolute value must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_M123(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M123",
        "Error at " + instr_name + " instruction: relocatable symbol or immediate absolute value must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_M124(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M124",
        "Error at " + instr_name + " instruction: vector register operand absolute value must be between 0 and 15",
        range);
}

diagnostic_op diagnostic_op::error_M130(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M130",
        "Error at " + instr_name + " instruction: value of the address operand displacement value must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_M131(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M131",
        "Error at " + instr_name
            + " instruction: value of the address operand base register parameter must be between 0 and 15",
        range);
}

diagnostic_op diagnostic_op::error_M132(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M132",
        "Error at " + instr_name + " instruction: value of the address operand length parameter must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_M133(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M133",
        "Error at " + instr_name + " instruction: value of the address operand register parameter must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_M134(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M134",
        "Error at " + instr_name
            + " instruction: value of the address operand vector register parameter must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_D001(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D001", "Integer out of range", range);
}

diagnostic_op diagnostic_op::error_D002(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D002", "Expected an integer", range);
}

diagnostic_op diagnostic_op::error_D003(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "D003", "Expected an integer or an expression after modifier", range);
}

diagnostic_op diagnostic_op::error_D004(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D004", "Wrong order of modifiers", range);
}

diagnostic_op diagnostic_op::error_D005(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D005", "Unexpected '.' in modifier other than length", range);
}

diagnostic_op diagnostic_op::error_D006(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "D006", "Unexpected character. Nominal value or modifier expected", range);
}

diagnostic_op diagnostic_op::error_D007(const range& range, const std::string& type)
{
    return diagnostic_op(diagnostic_severity::error, "D007", "Bit length not allowed with type " + type, range);
}

diagnostic_op diagnostic_op::error_D008(
    const range& range, const std::string& type, const std::string& modifier, int min, int max)
{
    if (min == max)
        return diagnostic_op(diagnostic_severity::error,
            "D008",
            "The " + modifier + " modifier of type " + type + " must be " + std::to_string(min),
            range);
    else
        return diagnostic_op(diagnostic_severity::error,
            "D008",
            "The " + modifier + " modifier of type " + type + " must be between " + std::to_string(min) + " and "
                + std::to_string(max),
            range);
}

diagnostic_op diagnostic_op::error_D009(const range& range, const std::string& type, const std::string& modifier)
{
    return diagnostic_op(
        diagnostic_severity::error, "D009", "The " + modifier + " modifier not allowed with type " + type, range);
}

diagnostic_op diagnostic_op::error_D010(const range& range, const std::string& type)
{
    return diagnostic_op(diagnostic_severity::error, "D010", "Wrong format of nominal value of type " + type, range);
}

diagnostic_op diagnostic_op::error_D011(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D011", "Duplication factor must be non-negative", range);
}

diagnostic_op diagnostic_op::error_D012(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D012", "Unknown type of data definition", range);
}

diagnostic_op diagnostic_op::error_D013(const range& range, const std::string& type)
{
    return diagnostic_op(diagnostic_severity::error, "D013", "Invalid type extension for type " + type, range);
}

diagnostic_op diagnostic_op::error_D014(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D014", "The length modifier must be even with type " + type, range);
}

diagnostic_op diagnostic_op::error_D015(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D015", "Only hexadecimal digits allowed in nominal value of type " + type, range);
}

diagnostic_op diagnostic_op::error_D016(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D016", "Nominal value expected", range);
}

diagnostic_op diagnostic_op::error_D017(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D017", "Nominal value enclosed in parentheses expected with type " + type, range);
}

diagnostic_op diagnostic_op::error_D018(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D018", "Nominal value enclosed in apostrophes expected with type " + type, range);
}

diagnostic_op diagnostic_op::error_D019(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D019", "Name of a symbol expected", range);
}

diagnostic_op diagnostic_op::error_D020(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D020", "Address in form D(B) is not allowed with type " + type, range);
}

diagnostic_op diagnostic_op::error_D021(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D021", "Only lengths 3, 4 or 8 are allowed with type " + type, range);
}

diagnostic_op diagnostic_op::error_D022(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D022", "Displacement out of range", range);
}

diagnostic_op diagnostic_op::error_D023(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D023", "Base register must be between 0 and 15", range);
}

diagnostic_op diagnostic_op::error_D024(const range& range, const std::string& type)
{
    return diagnostic_op(
        diagnostic_severity::error, "D024", "With type " + type + " only lengths 2 to 4 and 8 are allowed", range);
}

diagnostic_op diagnostic_op::warn_D025(const range& range, const std::string& type, const std::string& modifier)
{
    return diagnostic_op(
        diagnostic_severity::warning, "D025", "The " + modifier + " modifier is ignored with type " + type, range);
}

diagnostic_op diagnostic_op::warn_D031(const range& range, const std::string& operand_value)
{
    return diagnostic_op(diagnostic_severity::warning,
        "D031",
        "Using absolute value '" + operand_value + "' as relative immediate value",
        range);
}

diagnostic_op diagnostic_op::error_D026(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "D026", "Invalid round mode", range);
}

diagnostic_op diagnostic_op::error_D027(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "D027", "Graphic constant must be enclosed in SI '<' and SO '>'", range);
}

diagnostic_op diagnostic_op::error_D028(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "D028", "Data definition operand is longer than 2^31 bytes", range);
}

diagnostic_op diagnostic_op::error_D029(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "D029", "Data definition operands are longer than 2^31 bytes", range);
}

diagnostic_op diagnostic_op::error_D030(const range& range, const std::string& type)
{
    return diagnostic_op(diagnostic_severity::error, "D030", "Only single symbol expected with type " + type, range);
}

diagnostic_op diagnostic_op::error_M135(const std::string& instr_name, long long from, long long to, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M135",
        "Error at " + instr_name
            + " instruction: value of the address operand displacement register parameter must be between "
            + std::to_string(from) + " and " + std::to_string(to),
        range);
}

diagnostic_op diagnostic_op::error_optional_number_of_operands(
    const std::string& instr_name, int optional_no, int operands_no, const range& range)
{
    if (optional_no == 0)
        return error_M000(instr_name, operands_no, range);
    else if (optional_no == 1)
        return error_M001(instr_name, operands_no - 1, operands_no, range);
    else
        return error_M002(instr_name, operands_no - optional_no, operands_no, range);
}

diagnostic_op hlasm_plugin::parser_library::diagnostic_op::error_M000(
    const std::string& instr_name, int number, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M000",
        "Incorrect number of operands at " + instr_name + " instruction: number of operands has to be "
            + std::to_string(number),
        range);
}

diagnostic_op diagnostic_op::error_M001(const std::string& instr_name, int one, int two, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M001",
        "Incorrect number of operands at " + instr_name + " instruction: number of operands has to be either "
            + std::to_string(one) + " or " + std::to_string(two),
        range);
}

diagnostic_op hlasm_plugin::parser_library::diagnostic_op::error_M010(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M010",
        "Error at " + instr_name + " instruction: address operand is not valid",
        range);
}

diagnostic_op diagnostic_op::error_M002(const std::string& instr_name, int one, int two, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M002",
        "Incorrect number of operands at " + instr_name + " instruction: number of operands has to be from "
            + std::to_string(one) + " to " + std::to_string(two),
        range);
}

diagnostic_op diagnostic_op::error_M003(const std::string& instr_name, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "M003", "Error at " + instr_name + " instruction: operand cannot be empty", range);
}

diagnostic_op diagnostic_op::error_M004(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M004",
        "Error at " + instr_name + " instruction: operand format D(X,) not allowed",
        range);
}

diagnostic_op diagnostic_op::warning_M041(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::warning,
        "M041",
        "Warning at " + instr_name + " instruction: non-standard address format",
        range);
}

diagnostic_op diagnostic_op::error_M200(const std::string& instr_name, const range& range)
{
    return diagnostic_op(diagnostic_severity::error,
        "M200",
        "Error at " + instr_name + " instruction: second and third operand must be equal",
        range);
}

// diagnostic_s errors
diagnostic_op diagnostic_op::error_E001(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E001", "Continued line does not begin with required number of blanks", range);
}

diagnostic_op diagnostic_op::error_E010(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E010", "Unknown name of " + message, range);
}

diagnostic_op diagnostic_op::error_E011(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E011", message + " already specified", range);
}

diagnostic_op diagnostic_op::error_E012(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E012", "Wrong format: " + message, range);
}

diagnostic_op diagnostic_op::error_E013(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E013", "Inconsistent format: " + message, range);
}

diagnostic_op diagnostic_op::error_E020(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E020", "Error at " + message + " - too many operands", range);
}

diagnostic_op diagnostic_op::error_E021(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E021", "Error at " + message + " - operand number too low", range);
}

diagnostic_op diagnostic_op::error_E022(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E022", "Error at " + message + " - operand missing", range);
}

diagnostic_op diagnostic_op::error_E030(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E030", "Can't assign value to " + message, range);
}

diagnostic_op diagnostic_op::error_E031(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E031", "Cannot declare " + message + " with the same name", range);
}

diagnostic_op diagnostic_op::error_E032(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E032", "Undefined symbol - " + message, range);
}

diagnostic_op diagnostic_op::error_E033(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E033", "Cyclic symbol definition", range);
}

diagnostic_op diagnostic_op::error_E042(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E042", "Macro name ommited - ASPACE instead", range);
}

diagnostic_op diagnostic_op::error_E043(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E043", "Invalid name of variable in macro prototype", range);
}

diagnostic_op diagnostic_op::error_E048(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E048", "Undefined sequence symbol, macro aborted - " + message, range);
}

diagnostic_op diagnostic_op::error_E049(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E049", "Operation code not found - " + message, range);
}

diagnostic_op diagnostic_op::error_E050(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E050", "Illegal set symbol name", range);
}

diagnostic_op diagnostic_op::error_E051(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E051", "Duplicate SET symbol declaration, first is retained - " + message, range);
}

diagnostic_op diagnostic_op::error_E052(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E052", "Illegal use of symbolic parameter - " + message, range);
}

diagnostic_op diagnostic_op::error_E053(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E053", "Required name missing", range);
}

diagnostic_op diagnostic_op::error_E054(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E054", "Illegal statement outside macro definition", range);
}

diagnostic_op diagnostic_op::error_E055(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E055", "Too many nested macro calls, continuing opencode", range);
}

diagnostic_op diagnostic_op::error_E056(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E055", "ACTR counter exceeded", range);
}

diagnostic_op diagnostic_op::error_E057(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E057", "Symbol not an ordinary or sequence symbol", range);
}

diagnostic_op diagnostic_op::error_E058(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E058", "Copy member not found", range);
}

diagnostic_op diagnostic_op::error_E059(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E059", "First statement not MACRO in library " + message, range);
}

diagnostic_op diagnostic_op::error_E060(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E060", "Library macro name incorrect, expected " + message, range);
}

diagnostic_op diagnostic_op::error_E061(const std::string& message, const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E061", "Unbalanced MACRO MEND statements in copy member " + message, range);
}

diagnostic_op diagnostic_op::error_E062(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E062", "Recursive COPY", range);
}

diagnostic_op diagnostic_op::error_E063(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E063", "Too many ACTR calls, exiting", range);
}

diagnostic_op diagnostic_op::error_E064(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E064", "Illegal use of ampersand", range);
}

diagnostic_op diagnostic_op::error_E065(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E065", "Invalid symbol name", range);
}

diagnostic_op diagnostic_op::error_E066(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E066", "Illegal attribute reference", range);
}

diagnostic_op diagnostic_op::error_E067(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E067", "Bad symbol in instruction", range);
}

diagnostic_op diagnostic_op::error_E068(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E068", "Operand value falls outside of current section/LOCTR", range);
}

diagnostic_op diagnostic_op::error_E069(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E069", "AREAD instruction can only be called from within a macro", range);
}

diagnostic_op diagnostic_op::error_E070(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "E070", "Invalid AREAD operand. Use AREAD [NOSTMT|NOPRINT|CLOCKB|CLOCKD].", range);
}

diagnostic_op diagnostic_op::error_E044(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E044", "Illegal name field in macro prototype, discarded", range);
}

diagnostic_op diagnostic_op::error_E045(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E045", "Sequence symbol already defined - " + message, range);
}

diagnostic_op diagnostic_op::error_E046(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E046", "Missing MEND in " + message, range);
}

diagnostic_op diagnostic_op::error_E047(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "E047", "Lookahead failed, symbol not found - " + message, range);
}

diagnostic_op diagnostic_op::warning_W010(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::warning, "W010", message + " not expected", range);
}

diagnostic_op diagnostic_op::warning_W011(const range& range)
{
    return diagnostic_op(diagnostic_severity::warning,
        "W011",
        "Wrong type of constant for S or I attribute reference, default is 0",
        range);
}

diagnostic_op diagnostic_op::warning_W012(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::warning, "W012", "Length of EQUated symbol undefined, default is 1", range);
}

diagnostic_op diagnostic_op::warning_W013(const range& range)
{
    return diagnostic_op(diagnostic_severity::warning, "W013", "Undefined symbol attribute, default used", range);
}

diagnostic_op diagnostic_op::warning_W014(const range& range)
{
    return diagnostic_op(diagnostic_severity::warning,
        "W013",
        "Undefined keyword parameter, default to positional including keyword",
        range);
}

diagnostic_op diagnostic_op::error_EQU1(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "EQU1", "Constant redefinition", range);
}

diagnostic_op diagnostic_op::error_EQU2(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "EQU2", "Label redefinition", range);
}

diagnostic_op diagnostic_op::error_ME001(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "ME001", "Constant number overflow.", range);
}

diagnostic_op diagnostic_op::error_ME002(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "ME002", "multiplication or division of address", range);
}

diagnostic_op diagnostic_op::error_ME003(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "ME003", "Relative Immediate operand must evaluate into an even offset.", range);
}

diagnostic_op diagnostic_op::error_CE001(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE001", "Operator expected", range);
}

diagnostic_op diagnostic_op::error_CE002(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE002", "Undefined operator - " + message, range);
}

diagnostic_op diagnostic_op::error_CE003(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE003", "Operand expected", range);
}

diagnostic_op diagnostic_op::error_CE004(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "CE004", "Invalid use of operator - different return type expected", range);
}

diagnostic_op diagnostic_op::error_CE005(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE005", "Illegal duplication factor", range);
}

diagnostic_op diagnostic_op::error_CE006(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE006", "Bad number of operands", range);
}

diagnostic_op diagnostic_op::error_CE007(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE007", "Bad operand value", range);
}

diagnostic_op diagnostic_op::error_CE008(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE008", "Bad substring expression", range);
}

diagnostic_op diagnostic_op::error_CE009(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE009", "Substring start points past string end", range);
}

diagnostic_op diagnostic_op::error_CE010(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE010", "Negative duplication factor", range);
}

diagnostic_op diagnostic_op::error_CE011(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE011", "Maximum string size exceeded", range);
}

diagnostic_op diagnostic_op::error_CE012(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE012", "Only absolute and defined symbols allowed", range);
}

diagnostic_op diagnostic_op::error_CE013(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE013", "Arithmetic overflow", range);
}

diagnostic_op diagnostic_op::error_CE014(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE014", "Arithmetic underflow", range);
}

diagnostic_op diagnostic_op::error_CE015(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "CE015", "Invalid self-defining term", range);
}

diagnostic_op diagnostic_op::error_CE016_logical_expression_parenthesis(const range& range)
{
    return diagnostic_op(
        diagnostic_severity::error, "CE016", "Logical expression must be enclosed in parenthesis.", range);
}

diagnostic_op diagnostic_op::error_CW001(const range& range)
{
    return diagnostic_op(diagnostic_severity::warning, "CW001", "Substring count points past string end", range);
}

diagnostic_op diagnostic_op::error_P0001(const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "P0001", "DB2 preprocessor - invalid line continuation", range);
}

diagnostic_op diagnostic_op::error_P0002(const range& range, std::string_view lib)
{
    return diagnostic_op(diagnostic_severity::error,
        "P0002",
        std::string("DB2 preprocessor - unable to find library '").append(lib).append("'"),
        range);
}

diagnostic_op diagnostic_op::error_P0003(const range& range, std::string_view lib)
{
    return diagnostic_op(diagnostic_severity::error,
        "P0003",
        std::string("DB2 preprocessor - nested include '").append(lib).append("' requested"),
        range);
}

diagnostic_s diagnostic_s::error_W002(const std::string& ws_uri, const std::string& ws_name)
{
    return diagnostic_s(ws_uri,
        {},
        diagnostic_severity::error,
        "W0002",
        "The configuration file proc_grps for workspace " + ws_name + " is malformed.",
        {});
}

diagnostic_s diagnostic_s::error_W003(const std::string& file_name, const std::string& ws_name)
{
    return diagnostic_s(file_name,
        {},
        diagnostic_severity::error,
        "W0003",
        "The configuration file pgm_conf for workspace " + ws_name + " is malformed.",
        {});
}

diagnostic_s diagnostic_s::error_W004(const std::string& file_name, const std::string& ws_name)
{
    return diagnostic_s(file_name,
        {},
        diagnostic_severity::warning,
        "W0004",
        "The configuration file pgm_conf for workspace " + ws_name
            + " refers to a processor group, that is not defined in proc_grps",
        {});
}

diagnostic_s diagnostic_s::error_W005(const std::string& file_name, const std::string& proc_group)
{
    return diagnostic_s(file_name,
        {},
        diagnostic_severity::warning,
        "W0005",
        "The processor group '" + proc_group + "' from '" + file_name + "' defines invalid assembler options.",
        {});
}

diagnostic_s diagnostic_s::error_L0001(const std::string& path)
{
    return diagnostic_s(path, {}, "L0001", "Unable to load library: " + path + ".");
}

diagnostic_s diagnostic_s::error_L0002(const std::string& path)
{
    return diagnostic_s(
        "", {}, "L0002", "Unable to load library: " + path + ". Error: The path does not point to directory.");
}

diagnostic_s diagnostic_s::warning_L0003(const std::string& path)
{
    return diagnostic_s("",
        {},
        diagnostic_severity::warning,
        "L0003",
        "Macros from library '" + path
            + "' were selected by a deprecated mechanism to specify file extensions (alwaysRecognize in "
              "pgm_conf.json).",
        {});
}

diagnostic_s diagnostic_s::warning_L0004(const std::string& path, const std::string& macro_name)
{
    return diagnostic_s("",
        {},
        diagnostic_severity::warning,
        "L0004",
        "Library '" + path + "' contains multiple definitions of the macro '" + macro_name + "'.",
        {});
}
diagnostic_s diagnostic_s::error_L0005(const std::string& path)
{
    return diagnostic_s("",
        {},
        diagnostic_severity::warning,
        "L0005",
        "Library '" + path + "' contains recursive search more than once '",
        {});
}

diagnostic_op diagnostic_op::error_S100(const std::string& message, const range& range)
{
    return diagnostic_op(diagnostic_severity::error, "S100", "Long ordinary symbol name - " + message, range);
}

} // namespace hlasm_plugin::parser_library
