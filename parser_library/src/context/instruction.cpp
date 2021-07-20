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

#include "instruction.h"

#include <algorithm>

using namespace hlasm_plugin::parser_library::context;
using namespace hlasm_plugin::parser_library::checking;
using namespace hlasm_plugin::parser_library;

const std::map<mach_format, std::string> instruction::mach_format_to_string = {
    { mach_format::E, "E" },
    { mach_format::I, "I" },
    { mach_format::IE, "IE" },
    { mach_format::MII, "MII" },
    { mach_format::RI_a, "RI-a" },
    { mach_format::RI_b, "RI-b" },
    { mach_format::RI_c, "RI-c" },
    { mach_format::RIE_a, "RIE-a" },
    { mach_format::RIE_b, "RIE-b" },
    { mach_format::RIE_c, "RIE-c" },
    { mach_format::RIE_d, "RIE-d" },
    { mach_format::RIE_e, "RIE-e" },
    { mach_format::RIE_f, "RIE-f" },
    { mach_format::RIE_g, "RIE-g" },
    { mach_format::RIL_a, "RIL-a" },
    { mach_format::RIL_b, "RIL-b" },
    { mach_format::RIL_c, "RIL-c" },
    { mach_format::RIS, "RIS" },
    { mach_format::RR, "RR" },
    { mach_format::RRD, "RRD" },
    { mach_format::RRE, "RRE" },
    { mach_format::RRF_a, "RRF-a" },
    { mach_format::RRF_b, "RRF-b" },
    { mach_format::RRF_c, "RRF-c" },
    { mach_format::RRF_d, "RRF-d" },
    { mach_format::RRF_e, "RRF-e" },
    { mach_format::RRS, "RRS" },
    { mach_format::RS_a, "RS-a" },
    { mach_format::RS_b, "RS-b" },
    { mach_format::RSI, "RSI" },
    { mach_format::RSL_a, "RSL-a" },
    { mach_format::RSL_b, "RSL-b" },
    { mach_format::RSY_a, "RSY-a" },
    { mach_format::RSY_b, "RSY-b" },
    { mach_format::RX_a, "RX-a" },
    { mach_format::RX_b, "RX-b" },
    { mach_format::RXE, "RXE" },
    { mach_format::RXF, "RXF" },
    { mach_format::RXY_a, "RXY-a" },
    { mach_format::RXY_b, "RXY-b" },
    { mach_format::S, "S" },
    { mach_format::SI, "SI" },
    { mach_format::SIL, "SIL" },
    { mach_format::SIY, "SIY" },
    { mach_format::SMI, "SMI" },
    { mach_format::SS_a, "SS-a" },
    { mach_format::SS_b, "SS-b" },
    { mach_format::SS_c, "SS-c" },
    { mach_format::SS_d, "SS-d" },
    { mach_format::SS_e, "SS-e" },
    { mach_format::SS_f, "SS-f" },
    { mach_format::SSE, "SSE" },
    { mach_format::SSF, "SSF" },
    { mach_format::VRI_a, "VRI-a" },
    { mach_format::VRI_b, "VRI-b" },
    { mach_format::VRI_c, "VRI-c" },
    { mach_format::VRI_d, "VRI-d" },
    { mach_format::VRI_e, "VRI-e" },
    { mach_format::VRI_f, "VRI-f" },
    { mach_format::VRI_g, "VRI-g" },
    { mach_format::VRI_h, "VRI-h" },
    { mach_format::VRI_i, "VRI-i" },
    { mach_format::VRR_a, "VRR-a" },
    { mach_format::VRR_b, "VRR-b" },
    { mach_format::VRR_c, "VRR-c" },
    { mach_format::VRR_d, "VRR-d" },
    { mach_format::VRR_e, "VRR-e" },
    { mach_format::VRR_f, "VRR-f" },
    { mach_format::VRR_g, "VRR-g" },
    { mach_format::VRR_h, "VRR-h" },
    { mach_format::VRR_i, "VRR-i" },
    { mach_format::VRS_a, "VRS-a" },
    { mach_format::VRS_b, "VRS-b" },
    { mach_format::VRS_c, "VRS-c" },
    { mach_format::VRS_d, "VRS-d" },
    { mach_format::VSI, "VSI" },
    { mach_format::VRV, "VRV" },
    { mach_format::VRX, "VRX" },
};

const std::vector<ca_instruction> instruction::ca_instructions = {
    { "AIF", false },
    { "AGO", false },
    { "ACTR", false },
    { "SETA", false },
    { "SETB", false },
    { "SETC", false },
    { "ANOP", true },
    { "LCLA", false },
    { "LCLB", false },
    { "LCLC", false },
    { "GBLA", false },
    { "GBLB", false },
    { "GBLC", false },
    { "MACRO", true },
    { "MEND", true },
    { "MEXIT", true },
    { "AEJECT", true },
    { "AREAD", false },
    { "ASPACE", false },
};

const std::map<std::string, assembler_instruction> instruction::assembler_instructions = {
    { "*PROCESS", assembler_instruction(1, -1, true, "") }, // TO DO
    { "ACONTROL", assembler_instruction(1, -1, false, "<selection>+") },
    { "ADATA", assembler_instruction(5, 5, false, "value1,value2,value3,value4,character_string") },
    { "AINSERT", assembler_instruction(2, 2, false, "'record',BACK|FRONT") },
    { "ALIAS", assembler_instruction(1, 1, false, "alias_string") },
    { "AMODE", assembler_instruction(1, 1, false, "amode_option") },
    { "CATTR", assembler_instruction(1, -1, false, "attribute+") },
    { "CCW", assembler_instruction(4, 4, true, "command_code,data_address,flags,data_count") },
    { "CCW0", assembler_instruction(4, 4, true, "command_code,data_address,flags,data_count") },
    { "CCW1", assembler_instruction(4, 4, true, "command_code,data_address,flags,data_count") },
    { "CEJECT", assembler_instruction(0, 1, true, "?number_of_lines") },
    { "CNOP", assembler_instruction(2, 2, true, "byte,boundary") },
    { "COM", assembler_instruction(0, 0, false, "") },
    { "COPY", assembler_instruction(1, 1, false, "member") },
    { "CSECT", assembler_instruction(0, 0, false, "") },
    { "CXD", assembler_instruction(0, 0, false, "") },
    { "DC", assembler_instruction(1, -1, true, "<operand>+") },
    { "DROP", assembler_instruction(0, -1, true, "?<<base_register|label>+>") },
    { "DS", assembler_instruction(1, -1, true, "<operand>+") },
    { "DSECT", assembler_instruction(0, 0, false, "") },
    { "DXD", assembler_instruction(1, -1, true, "<operand>+") },
    { "EJECT", assembler_instruction(0, 0, false, "") },
    { "END", assembler_instruction(0, 2, true, "?expression,?language") },
    { "ENTRY", assembler_instruction(1, -1, true, "entry_point+") },
    { "EQU",
        assembler_instruction(1,
            5,
            true,
            "value,?<length_attribute_value>,?<type_attribute_value>,?<program_type_value>,?<assembler_type_"
            "value>") },
    { "EXITCTL", assembler_instruction(2, 5, false, "exit_type,control_value+") },
    { "EXTRN", assembler_instruction(1, -1, false, "<external_symbol>+|PART(<external_symbol>+)") },
    { "ICTL", assembler_instruction(1, 3, false, "begin,?<end>,?<continue>") },
    { "ISEQ", assembler_instruction(0, 2, false, "?<left,right>") },
    { "LOCTR", assembler_instruction(0, 0, false, "") },
    { "LTORG", assembler_instruction(0, 0, false, "") },
    { "MNOTE", assembler_instruction(1, 2, false, "?<<severity|*|>,>message") },
    { "OPSYN", assembler_instruction(0, 1, false, "?operation_code_2") },
    { "ORG", assembler_instruction(0, 3, true, "expression?<,boundary?<,offset>>") },
    { "POP", assembler_instruction(1, 4, false, "<PRINT|USING|ACONTROL>+,?NOPRINT") },
    { "PRINT", assembler_instruction(1, -1, false, "operand+") },
    { "PUNCH", assembler_instruction(1, 1, false, "string") },
    { "PUSH", assembler_instruction(1, 4, false, "<PRINT|USING|ACONTROL>+,?NOPRINT") },
    { "REPRO", assembler_instruction(0, 0, false, "") },
    { "RMODE", assembler_instruction(1, 1, false, "rmode_option") },
    { "RSECT", assembler_instruction(0, 0, false, "") },
    { "SPACE", assembler_instruction(0, 1, true, "?number_of_lines") },
    { "START", assembler_instruction(0, 1, true, "?expression") },
    { "TITLE", assembler_instruction(1, 1, false, "title_string") },
    { "USING", assembler_instruction(2, -1, true, "operand+") },
    { "WXTRN", assembler_instruction(1, -1, false, "<external_symbol>+|PART(<external_symbol>+)") },
    { "XATTR", assembler_instruction(1, -1, false, "attribute+") },
};


bool hlasm_plugin::parser_library::context::machine_instruction::check_nth_operand(
    size_t place, const checking::machine_operand* operand)
{
    diagnostic_op diag;
    const range stmt_range = range();
    if (operand->check(diag, operands[place], instr_name, stmt_range))
        return true;
    return false;
}

bool hlasm_plugin::parser_library::context::machine_instruction::check(const std::string& name_of_instruction,
    const std::vector<const checking::machine_operand*> to_check,
    const range& stmt_range,
    const diagnostic_collector& add_diagnostic) const
{
    // check size of operands
    int diff = (int)operands.size() - (int)to_check.size();
    if (diff > no_optional || diff < 0)
    {
        add_diagnostic(diagnostic_op::error_optional_number_of_operands(
            name_of_instruction, no_optional, (int)operands.size(), stmt_range));
        return false;
    }
    bool error = false;
    for (size_t i = 0; i < to_check.size(); i++)
    {
        assert(to_check[i] != nullptr);
        diagnostic_op diag;
        if (!(*to_check[i]).check(diag, operands[i], name_of_instruction, stmt_range))
        {
            add_diagnostic(diag);
            error = true;
        }
    };
    return (!error);
}

void add_machine_instr(std::map<std::string, machine_instruction>& result,
    const std::string& instruction_name,
    mach_format format,
    std::vector<machine_operand_format> op_format,
    size_t page_no)
{
    result.insert(std::pair<std::string, machine_instruction>(
        instruction_name, machine_instruction(instruction_name, format, op_format, page_no)));
}
void add_machine_instr(std::map<std::string, machine_instruction>& result,
    const std::string& instruction_name,
    mach_format format,
    std::vector<machine_operand_format> op_format,
    int optional,
    size_t page_no)
{
    result.insert(std::pair<std::string, machine_instruction>(
        instruction_name, machine_instruction(instruction_name, format, std::move(op_format), optional, page_no)));
}


std::map<std::string, machine_instruction>
hlasm_plugin::parser_library::context::instruction::get_machine_instructions()
{
    std::map<std::string, machine_instruction> result;
    add_machine_instr(result, "AR", mach_format::RR, { reg_4_U, reg_4_U }, 510);
    add_machine_instr(result, "ADDFRR", mach_format::RRE, { reg_4_U, reg_4_U }, 7);
    add_machine_instr(result, "AGR", mach_format::RRE, { reg_4_U, reg_4_U }, 510);
    add_machine_instr(result, "AGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 510);
    add_machine_instr(result, "ARK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 510);
    add_machine_instr(result, "AGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 510);
    add_machine_instr(result, "A", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 510);
    add_machine_instr(result, "AY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 511);
    add_machine_instr(result, "AG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 511);
    add_machine_instr(result, "AGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 511);
    add_machine_instr(result, "AFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 511);
    add_machine_instr(result, "AGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 511);
    add_machine_instr(result, "AHIK", mach_format::RIE_d, { reg_4_U, reg_4_U, imm_16_S }, 511);
    add_machine_instr(result, "AGHIK", mach_format::RIE_d, { reg_4_U, reg_4_U, imm_16_S }, 511);
    add_machine_instr(result, "ASI", mach_format::SIY, { db_20_4_S, imm_8_S }, 511);
    add_machine_instr(result, "AGSI", mach_format::SIY, { db_20_4_S, imm_8_S }, 511);
    add_machine_instr(result, "AH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 512);
    add_machine_instr(result, "AHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 512);
    add_machine_instr(result, "AGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 512);
    add_machine_instr(result, "AHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 512);
    add_machine_instr(result, "AGHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 513);
    add_machine_instr(result, "AHHHR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 513);
    add_machine_instr(result, "AHHLR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 513);
    add_machine_instr(result, "AIH", mach_format::RIL_a, { reg_4_U, imm_32_S }, 513);
    add_machine_instr(result, "ALR", mach_format::RR, { reg_4_U, reg_4_U }, 514);
    add_machine_instr(result, "ALGR", mach_format::RRE, { reg_4_U, reg_4_U }, 514);
    add_machine_instr(result, "ALGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 514);
    add_machine_instr(result, "ALRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 514);
    add_machine_instr(result, "ALGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 514);
    add_machine_instr(result, "AL", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 514);
    add_machine_instr(result, "ALY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 514);
    add_machine_instr(result, "ALG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 514);
    add_machine_instr(result, "ALGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 514);
    add_machine_instr(result, "ALFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 514);
    add_machine_instr(result, "ALGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 514);
    add_machine_instr(result, "ALHHHR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 515);
    add_machine_instr(result, "ALHHLR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 515);
    add_machine_instr(result, "ALCR", mach_format::RRE, { reg_4_U, reg_4_U }, 515);
    add_machine_instr(result, "ALCGR", mach_format::RRE, { reg_4_U, reg_4_U }, 515);
    add_machine_instr(result, "ALC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 515);
    add_machine_instr(result, "ALCG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 515);
    add_machine_instr(result, "ALSI", mach_format::SIY, { db_20_4_S, imm_8_S }, 516);
    add_machine_instr(result, "ALGSI", mach_format::SIY, { db_20_4_S, imm_8_S }, 516);
    add_machine_instr(result, "ALHSIK", mach_format::RIE_d, { reg_4_U, reg_4_U, imm_16_S }, 516);
    add_machine_instr(result, "ALGHSIK", mach_format::RIE_d, { reg_4_U, reg_4_U, imm_16_S }, 516);
    add_machine_instr(result, "ALSIH", mach_format::RIL_a, { reg_4_U, imm_32_S }, 517);
    add_machine_instr(result, "ALSIHN", mach_format::RIL_a, { reg_4_U, imm_32_S }, 517);
    add_machine_instr(result, "NR", mach_format::RR, { reg_4_U, reg_4_U }, 517);
    add_machine_instr(result, "NGR", mach_format::RRE, { reg_4_U, reg_4_U }, 517);
    add_machine_instr(result, "NRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 517);
    add_machine_instr(result, "NGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 517);
    add_machine_instr(result, "N", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 517);
    add_machine_instr(result, "NY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 517);
    add_machine_instr(result, "NG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 517);
    add_machine_instr(result, "NI", mach_format::SI, { db_12_4_U, imm_8_U }, 517);
    add_machine_instr(result, "NIY", mach_format::SIY, { db_20_4_S, imm_8_U }, 518);
    add_machine_instr(result, "NC", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 518);
    add_machine_instr(result, "NIHF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 518);
    add_machine_instr(result, "NIHH", mach_format::RI_a, { reg_4_U, imm_16_U }, 518);
    add_machine_instr(result, "NIHL", mach_format::RI_a, { reg_4_U, imm_16_U }, 518);
    add_machine_instr(result, "NILF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 519);
    add_machine_instr(result, "NILH", mach_format::RI_a, { reg_4_U, imm_16_U }, 519);
    add_machine_instr(result, "NILL", mach_format::RI_a, { reg_4_U, imm_16_U }, 519);
    add_machine_instr(result, "BALR", mach_format::RR, { reg_4_U, reg_4_U }, 519);
    add_machine_instr(result, "BAL", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 519);
    add_machine_instr(result, "BASR", mach_format::RR, { reg_4_U, reg_4_U }, 520);
    add_machine_instr(result, "BAS", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 520);
    add_machine_instr(result, "BASSM", mach_format::RX_a, { reg_4_U, reg_4_U }, 520);
    add_machine_instr(result, "BSM", mach_format::RR, { reg_4_U, reg_4_U }, 522);
    add_machine_instr(result, "BIC", mach_format::RXY_b, { mask_4_U, dxb_20_4x4_S }, 523);
    add_machine_instr(result, "BCR", mach_format::RR, { mask_4_U, reg_4_U }, 524);
    add_machine_instr(result, "BC", mach_format::RX_b, { mask_4_U, dxb_12_4x4_U }, 524);
    add_machine_instr(result, "BCTR", mach_format::RR, { reg_4_U, reg_4_U }, 525);
    add_machine_instr(result, "BCTGR", mach_format::RRE, { reg_4_U, reg_4_U }, 525);
    add_machine_instr(result, "BCT", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 525);
    add_machine_instr(result, "BCTG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 525);
    add_machine_instr(result, "BXH", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 526);
    add_machine_instr(result, "BXHG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 526);
    add_machine_instr(result, "BXLE", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 526);
    add_machine_instr(result, "BXLEG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 526);
    add_machine_instr(result, "BPP", mach_format::SMI, { mask_4_U, rel_addr_imm_16_S, db_12_4_U }, 527);
    add_machine_instr(result, "BPRP", mach_format::MII, { mask_4_U, rel_addr_imm_12_S, rel_addr_imm_24_S }, 527);
    add_machine_instr(result, "BRAS", mach_format::RI_b, { reg_4_U, rel_addr_imm_16_S }, 530);
    add_machine_instr(result, "BRASL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 530);
    add_machine_instr(result, "BRC", mach_format::RI_c, { mask_4_U, rel_addr_imm_16_S }, 530);
    add_machine_instr(result, "BRCL", mach_format::RIL_c, { mask_4_U, rel_addr_imm_32_S }, 530);
    add_machine_instr(result, "BRCT", mach_format::RI_b, { reg_4_U, rel_addr_imm_16_S }, 531);
    add_machine_instr(result, "BRCTG", mach_format::RI_b, { reg_4_U, rel_addr_imm_16_S }, 531);
    add_machine_instr(result, "BRCTH", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 531);
    add_machine_instr(result, "BRXH", mach_format::RSI, { reg_4_U, reg_4_U, rel_addr_imm_16_S }, 532);
    add_machine_instr(result, "BRXHG", mach_format::RIE_e, { reg_4_U, reg_4_U, rel_addr_imm_16_S }, 532);
    add_machine_instr(result, "BRXLE", mach_format::RSI, { reg_4_U, reg_4_U, rel_addr_imm_16_S }, 532);
    add_machine_instr(result, "BRXLG", mach_format::RIE_e, { reg_4_U, reg_4_U, rel_addr_imm_16_S }, 532);
    add_machine_instr(result, "CKSM", mach_format::RRE, { reg_4_U, reg_4_U }, 533);
    add_machine_instr(result, "KM", mach_format::RRE, { reg_4_U, reg_4_U }, 537);
    add_machine_instr(result, "KMC", mach_format::RRE, { reg_4_U, reg_4_U }, 537);
    add_machine_instr(result, "KMA", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U }, 562);
    add_machine_instr(result, "KMF", mach_format::RRE, { reg_4_U, reg_4_U }, 576);
    add_machine_instr(result, "KMCTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U }, 591);
    add_machine_instr(result, "KMO", mach_format::RRE, { reg_4_U, reg_4_U }, 604);
    add_machine_instr(result, "CR", mach_format::RR, { reg_4_U, reg_4_U }, 618);
    add_machine_instr(result, "CGR", mach_format::RRE, { reg_4_U, reg_4_U }, 618);
    add_machine_instr(result, "CGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 618);
    add_machine_instr(result, "C", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 618);
    add_machine_instr(result, "CY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 618);
    add_machine_instr(result, "CG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 618);
    add_machine_instr(result, "CGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 618);
    add_machine_instr(result, "CFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 618);
    add_machine_instr(result, "CGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 619);
    add_machine_instr(result, "CRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 619);
    add_machine_instr(result, "CGRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 619);
    add_machine_instr(result, "CGFRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 619);
    add_machine_instr(result, "CRB", mach_format::RRS, { reg_4_U, reg_4_U, mask_4_U, db_12_4_U }, 619);
    add_machine_instr(result, "CGRB", mach_format::RRS, { reg_4_U, reg_4_U, mask_4_U, db_12_4_U }, 619);
    add_machine_instr(result, "CRJ", mach_format::RIE_b, { reg_4_U, reg_4_U, mask_4_U, rel_addr_imm_16_S }, 619);
    add_machine_instr(result, "CGRJ", mach_format::RIE_b, { reg_4_U, reg_4_U, mask_4_U, rel_addr_imm_16_S }, 620);
    add_machine_instr(result, "CIB", mach_format::RIS, { reg_4_U, imm_8_S, mask_4_U, db_12_4_U }, 620);
    add_machine_instr(result, "CGIB", mach_format::RIS, { reg_4_U, imm_8_S, mask_4_U, db_12_4_U }, 620);
    add_machine_instr(result, "CIJ", mach_format::RIE_c, { reg_4_U, imm_8_S, mask_4_U, rel_addr_imm_16_S }, 620);
    add_machine_instr(result, "CGIJ", mach_format::RIE_c, { reg_4_U, imm_8_S, mask_4_U, rel_addr_imm_16_S }, 620);
    add_machine_instr(result, "CFC", mach_format::S, { db_12_4_U }, 621);
    add_machine_instr(result, "CS", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 628);
    add_machine_instr(result, "CSY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 628);
    add_machine_instr(result, "CSG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 628);
    add_machine_instr(result, "CDS", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 628);
    add_machine_instr(result, "CDSY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 628);
    add_machine_instr(result, "CDSG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 628);
    add_machine_instr(result, "CSST", mach_format::SSF, { db_12_4_U, db_12_4_U, reg_4_U }, 630);
    add_machine_instr(result, "CRT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 633);
    add_machine_instr(result, "CGRT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 633);
    add_machine_instr(result, "CIT", mach_format::RIE_a, { reg_4_U, imm_16_S, mask_4_U }, 633);
    add_machine_instr(result, "CGIT", mach_format::RIE_a, { reg_4_U, imm_16_S, mask_4_U }, 633);
    add_machine_instr(result, "CH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 634);
    add_machine_instr(result, "CHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 634);
    add_machine_instr(result, "CGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 634);
    add_machine_instr(result, "CHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 634);
    add_machine_instr(result, "CGHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 634);
    add_machine_instr(result, "CHHSI", mach_format::SIL, { db_12_4_U, imm_16_S }, 634);
    add_machine_instr(result, "CHSI", mach_format::SIL, { db_12_4_U, imm_16_S }, 634);
    add_machine_instr(result, "CGHSI", mach_format::SIL, { db_12_4_U, imm_16_S }, 634);
    add_machine_instr(result, "CHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 634);
    add_machine_instr(result, "CGHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 634);
    add_machine_instr(result, "CHHR", mach_format::RRE, { reg_4_U, reg_4_U }, 635);
    add_machine_instr(result, "CHLR", mach_format::RRE, { reg_4_U, reg_4_U }, 635);
    add_machine_instr(result, "CHF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 635);
    add_machine_instr(result, "CIH", mach_format::RIL_a, { reg_4_U, imm_32_S }, 635);
    add_machine_instr(result, "CLR", mach_format::RR, { reg_4_U, reg_4_U }, 636);
    add_machine_instr(result, "CLGR", mach_format::RRE, { reg_4_U, reg_4_U }, 636);
    add_machine_instr(result, "CLGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 636);
    add_machine_instr(result, "CL", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 636);
    add_machine_instr(result, "CLY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 636);
    add_machine_instr(result, "CLG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 636);
    add_machine_instr(result, "CLGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 636);
    add_machine_instr(result, "CLC", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 636);
    add_machine_instr(result, "CLFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 636);
    add_machine_instr(result, "CLGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 636);
    add_machine_instr(result, "CLI", mach_format::SI, { db_12_4_U, imm_8_U }, 636);
    add_machine_instr(result, "CLIY", mach_format::SIY, { db_12_4_U, imm_8_U }, 636);
    add_machine_instr(result, "CLFHSI", mach_format::SIL, { db_12_4_U, imm_16_U }, 636);
    add_machine_instr(result, "CLGHSI", mach_format::SIL, { db_12_4_U, imm_16_U }, 636);
    add_machine_instr(result, "CLHHSI", mach_format::SIL, { db_12_4_U, imm_16_U }, 636);
    add_machine_instr(result, "CLRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 637);
    add_machine_instr(result, "CLGRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 637);
    add_machine_instr(result, "CLGFRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 637);
    add_machine_instr(result, "CLHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 637);
    add_machine_instr(result, "CLGHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 637);
    add_machine_instr(result, "CLRB", mach_format::RRS, { reg_4_U, reg_4_U, mask_4_U, db_12_4_U }, 638);
    add_machine_instr(result, "CLGRB", mach_format::RRS, { reg_4_U, reg_4_U, mask_4_U, db_12_4_U }, 638);
    add_machine_instr(result, "CLRJ", mach_format::RIE_b, { reg_4_U, reg_4_U, mask_4_U, rel_addr_imm_16_S }, 638);
    add_machine_instr(result, "CLGRJ", mach_format::RIE_b, { reg_4_U, reg_4_U, mask_4_U, rel_addr_imm_16_S }, 638);
    add_machine_instr(result, "CLIB", mach_format::RIS, { reg_4_U, imm_8_S, mask_4_U, db_12_4_U }, 638);
    add_machine_instr(result, "CLGIB", mach_format::RIS, { reg_4_U, imm_8_S, mask_4_U, db_12_4_U }, 638);
    add_machine_instr(result, "CLIJ", mach_format::RIE_c, { reg_4_U, imm_8_S, mask_4_U, rel_addr_imm_16_S }, 638);
    add_machine_instr(result, "CLGIJ", mach_format::RIE_c, { reg_4_U, imm_8_S, mask_4_U, rel_addr_imm_16_S }, 638);
    add_machine_instr(result, "CLRT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 639);
    add_machine_instr(result, "CLGRT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 639);
    add_machine_instr(result, "CLT", mach_format::RSY_b, { reg_4_U, mask_4_U, dxb_20_4x4_S }, 639);
    add_machine_instr(result, "CLGT", mach_format::RSY_b, { reg_4_U, mask_4_U, dxb_20_4x4_S }, 639);
    add_machine_instr(result, "CLFIT", mach_format::RIE_a, { reg_4_U, imm_16_S, mask_4_U }, 640);
    add_machine_instr(result, "CLGIT", mach_format::RIE_a, { reg_4_U, imm_16_S, mask_4_U }, 640);
    add_machine_instr(result, "CLM", mach_format::RS_b, { reg_4_U, mask_4_U, db_12_4_U }, 641);
    add_machine_instr(result, "CLMY", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 641);
    add_machine_instr(result, "CLMH", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 641);
    add_machine_instr(result, "CLHHR", mach_format::RRE, { reg_4_U, reg_4_U }, 641);
    add_machine_instr(result, "CLHLR", mach_format::RRE, { reg_4_U, reg_4_U }, 641);
    add_machine_instr(result, "CLHF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 641);
    add_machine_instr(result, "CLCL", mach_format::RR, { reg_4_U, reg_4_U }, 642);
    add_machine_instr(result, "CLIH", mach_format::RIL_a, { reg_4_U, imm_32_S }, 642);
    add_machine_instr(result, "CLCLE", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 644);
    add_machine_instr(result, "CLCLU", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 647);
    add_machine_instr(result, "CLST", mach_format::RRE, { reg_4_U, reg_4_U }, 650);
    add_machine_instr(result, "CUSE", mach_format::RRE, { reg_4_U, reg_4_U }, 651);
    add_machine_instr(result, "CMPSC", mach_format::RRE, { reg_4_U, reg_4_U }, 654);
    add_machine_instr(result, "KIMD", mach_format::RRE, { reg_4_U, reg_4_U }, 672);
    add_machine_instr(result, "KLMD", mach_format::RRE, { reg_4_U, reg_4_U }, 685);
    add_machine_instr(result, "KMAC", mach_format::RRE, { reg_4_U, reg_4_U }, 703);
    add_machine_instr(result, "CVB", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 714);
    add_machine_instr(result, "CVBY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 714);
    add_machine_instr(result, "CVBG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 714);
    add_machine_instr(result, "CVD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 715);
    add_machine_instr(result, "CVDY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 715);
    add_machine_instr(result, "CVDG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 715);
    add_machine_instr(result, "CU24", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 715);
    add_machine_instr(result, "CUUTF", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 718);
    add_machine_instr(result, "CU21", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 718);
    add_machine_instr(result, "CU42", mach_format::RRE, { reg_4_U, reg_4_U }, 722);
    add_machine_instr(result, "CU41", mach_format::RRE, { reg_4_U, reg_4_U }, 725);
    add_machine_instr(result, "CUTFU", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 728);
    add_machine_instr(result, "CU12", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 728);
    add_machine_instr(result, "CU14", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 732);
    add_machine_instr(result, "CPYA", mach_format::RRE, { reg_4_U, reg_4_U }, 736);
    add_machine_instr(result, "DR", mach_format::RR, { reg_4_U, reg_4_U }, 736);
    add_machine_instr(result, "D", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 736);
    add_machine_instr(result, "DLR", mach_format::RRE, { reg_4_U, reg_4_U }, 737);
    add_machine_instr(result, "DLGR", mach_format::RRE, { reg_4_U, reg_4_U }, 737);
    add_machine_instr(result, "DL", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 737);
    add_machine_instr(result, "DLG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 737);
    add_machine_instr(result, "DSGR", mach_format::RRE, { reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "DSGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "DSG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 738);
    add_machine_instr(result, "DSGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 738);
    add_machine_instr(result, "HIO", mach_format::S, { db_12_4_U }, 129);
    add_machine_instr(result, "HDV", mach_format::S, { db_12_4_U }, 129);
    add_machine_instr(result, "SIO", mach_format::S, { db_12_4_U }, 129);
    add_machine_instr(result, "SIOF", mach_format::S, { db_12_4_U }, 129);
    add_machine_instr(result, "STIDC", mach_format::S, { db_12_4_U }, 129);
    add_machine_instr(result, "CLRCH", mach_format::S, { db_12_4_U }, 367);
    add_machine_instr(result, "CLRIO", mach_format::S, { db_12_4_U }, 368);
    add_machine_instr(result, "TCH", mach_format::S, { db_12_4_U }, 384);
    add_machine_instr(result, "TIO", mach_format::S, { db_12_4_U }, 385);
    add_machine_instr(result, "RRB", mach_format::S, { db_12_4_U }, 295);
    add_machine_instr(result, "CONCS", mach_format::S, { db_12_4_U }, 263);
    add_machine_instr(result, "DISCS", mach_format::S, { db_12_4_U }, 265);
    add_machine_instr(result, "XR", mach_format::RR, { reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "XGR", mach_format::RRE, { reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "XRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "XGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 738);
    add_machine_instr(result, "X", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 738);
    add_machine_instr(result, "XY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 738);
    add_machine_instr(result, "XG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 738);
    add_machine_instr(result, "XI", mach_format::SI, { db_12_4_U, imm_8_U }, 739);
    add_machine_instr(result, "XIY", mach_format::SIY, { db_20_4_S, imm_8_U }, 739);
    add_machine_instr(result, "XC", mach_format::SS_a, { db_12_8x4L_U, db_20_4_S }, 739);
    add_machine_instr(result, "EX", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 740);
    add_machine_instr(result, "XIHF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 740);
    add_machine_instr(result, "XILF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 740);
    add_machine_instr(result, "EXRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 740);
    add_machine_instr(result, "EAR", mach_format::RRE, { reg_4_U, reg_4_U }, 741);
    add_machine_instr(result, "ECAG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 741);
    add_machine_instr(result, "ECTG", mach_format::SSF, { db_12_4_U, db_12_4_U, reg_4_U }, 744);
    add_machine_instr(result, "EPSW", mach_format::RRE, { reg_4_U, reg_4_U }, 745);
    add_machine_instr(result, "ETND", mach_format::RRE, { reg_4_U }, 745);
    add_machine_instr(result, "FLOGR", mach_format::RRE, { reg_4_U, reg_4_U }, 746);
    add_machine_instr(result, "IC", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 746);
    add_machine_instr(result, "ICY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 746);
    add_machine_instr(result, "ICM", mach_format::RS_b, { reg_4_U, mask_4_U, db_12_4_U }, 746);
    add_machine_instr(result, "ICMY", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 746);
    add_machine_instr(result, "ICMH", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 746);
    add_machine_instr(result, "IIHF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 747);
    add_machine_instr(result, "IIHH", mach_format::RI_a, { reg_4_U, imm_16_U }, 747);
    add_machine_instr(result, "IIHL", mach_format::RI_a, { reg_4_U, imm_16_U }, 747);
    add_machine_instr(result, "IILF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 747);
    add_machine_instr(result, "IILH", mach_format::RI_a, { reg_4_U, imm_16_U }, 747);
    add_machine_instr(result, "IILL", mach_format::RI_a, { reg_4_U, imm_16_U }, 747);
    add_machine_instr(result, "IPM", mach_format::RRE, { reg_4_U }, 748);
    add_machine_instr(result, "LR", mach_format::RR, { reg_4_U, reg_4_U }, 748);
    add_machine_instr(result, "LGR", mach_format::RRE, { reg_4_U, reg_4_U }, 748);
    add_machine_instr(result, "LGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 748);
    add_machine_instr(result, "L", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 748);
    add_machine_instr(result, "LY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 748);
    add_machine_instr(result, "LG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 748);
    add_machine_instr(result, "LGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 748);
    add_machine_instr(result, "LGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 748);
    add_machine_instr(result, "LRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 748);
    add_machine_instr(result, "LGRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 748);
    add_machine_instr(result, "LGFRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 748);
    add_machine_instr(result, "LAM", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 749);
    add_machine_instr(result, "LAMY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 749);
    add_machine_instr(result, "LA", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 750);
    add_machine_instr(result, "LAY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 750);
    add_machine_instr(result, "LAE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 750);
    add_machine_instr(result, "LAEY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 750);
    add_machine_instr(result, "LARL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 751);
    add_machine_instr(result, "LAA", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 752);
    add_machine_instr(result, "LAAG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 752);
    add_machine_instr(result, "LAAL", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 752);
    add_machine_instr(result, "LAALG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 752);
    add_machine_instr(result, "LAN", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 753);
    add_machine_instr(result, "LANG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 753);
    add_machine_instr(result, "LAX", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 753);
    add_machine_instr(result, "LAXG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 753);
    add_machine_instr(result, "LAO", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 754);
    add_machine_instr(result, "LAOG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 754);
    add_machine_instr(result, "LTR", mach_format::RR, { reg_4_U, reg_4_U }, 754);
    add_machine_instr(result, "LTGR", mach_format::RRE, { reg_4_U, reg_4_U }, 754);
    add_machine_instr(result, "LTGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 754);
    add_machine_instr(result, "LT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LTG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LTGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LAT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LGAT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LZRF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LZRG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 755);
    add_machine_instr(result, "LBR", mach_format::RRE, { reg_4_U, reg_4_U }, 756);
    add_machine_instr(result, "LGBR", mach_format::RRE, { reg_4_U, reg_4_U }, 756);
    add_machine_instr(result, "LB", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 756);
    add_machine_instr(result, "LGB", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 756);
    add_machine_instr(result, "LBH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 756);
    add_machine_instr(result, "LCR", mach_format::RR, { reg_4_U, reg_4_U }, 756);
    add_machine_instr(result, "LCGR", mach_format::RRE, { reg_4_U, reg_4_U }, 757);
    add_machine_instr(result, "LCGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 757);
    add_machine_instr(result, "LCBB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U, mask_4_U }, 757);
    add_machine_instr(result, "LGG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 758);
    add_machine_instr(result, "LLGFSG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 758);
    add_machine_instr(result, "LGSC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 759);
    add_machine_instr(result, "LHR", mach_format::RRE, { reg_4_U, reg_4_U }, 760);
    add_machine_instr(result, "LGHR", mach_format::RRE, { reg_4_U, reg_4_U }, 760);
    add_machine_instr(result, "LH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 760);
    add_machine_instr(result, "LHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 760);
    add_machine_instr(result, "LGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 760);
    add_machine_instr(result, "LHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 760);
    add_machine_instr(result, "LGHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 760);
    add_machine_instr(result, "LHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 760);
    add_machine_instr(result, "LGHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 760);
    add_machine_instr(result, "LHH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 761);
    add_machine_instr(result, "LOCHI", mach_format::RIE_g, { reg_4_U, imm_16_S, mask_4_U }, 761);
    add_machine_instr(result, "LOCGHI", mach_format::RIE_g, { reg_4_U, imm_16_S, mask_4_U }, 761);
    add_machine_instr(result, "LOCHHI", mach_format::RIE_g, { reg_4_U, imm_16_S, mask_4_U }, 761);
    add_machine_instr(result, "LFH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 762);
    add_machine_instr(result, "LFHAT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 762);
    add_machine_instr(result, "LLGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 762);
    add_machine_instr(result, "LLGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 762);
    add_machine_instr(result, "LLGFRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 762);
    add_machine_instr(result, "LLGFAT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 763);
    add_machine_instr(result, "LLCR", mach_format::RRE, { reg_4_U, reg_4_U }, 763);
    add_machine_instr(result, "LLGCR", mach_format::RRE, { reg_4_U, reg_4_U }, 763);
    add_machine_instr(result, "LLC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 763);
    add_machine_instr(result, "LLGC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 763);
    add_machine_instr(result, "LLZRGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 763);
    add_machine_instr(result, "LLCH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 764);
    add_machine_instr(result, "LLHR", mach_format::RRE, { reg_4_U, reg_4_U }, 764);
    add_machine_instr(result, "LLGHR", mach_format::RRE, { reg_4_U, reg_4_U }, 764);
    add_machine_instr(result, "LLH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 764);
    add_machine_instr(result, "LLGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 764);
    add_machine_instr(result, "LLHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 764);
    add_machine_instr(result, "LLGHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 764);
    add_machine_instr(result, "LLHH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 765);
    add_machine_instr(result, "LLIHF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 765);
    add_machine_instr(result, "LLIHH", mach_format::RI_a, { reg_4_U, imm_16_S }, 765);
    add_machine_instr(result, "LLIHL", mach_format::RI_a, { reg_4_U, imm_16_S }, 765);
    add_machine_instr(result, "LLILF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 765);
    add_machine_instr(result, "LLILH", mach_format::RI_a, { reg_4_U, imm_16_S }, 765);
    add_machine_instr(result, "LLILL", mach_format::RI_a, { reg_4_U, imm_16_S }, 765);
    add_machine_instr(result, "LLGTR", mach_format::RRE, { reg_4_U, reg_4_U }, 765);
    add_machine_instr(result, "LLGT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 766);
    add_machine_instr(result, "LLGTAT", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 766);
    add_machine_instr(result, "LM", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 766);
    add_machine_instr(result, "LMY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 766);
    add_machine_instr(result, "LMG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 766);
    add_machine_instr(result, "LMD", mach_format::SS_e, { reg_4_U, reg_4_U, db_12_4_U, db_12_4_U }, 767);
    add_machine_instr(result, "LMH", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 767);
    add_machine_instr(result, "LNR", mach_format::RR, { reg_4_U, reg_4_U }, 767);
    add_machine_instr(result, "LNGR", mach_format::RRE, { reg_4_U, reg_4_U }, 767);
    add_machine_instr(result, "LNGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 768);
    add_machine_instr(result, "LOCFHR", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 768);
    add_machine_instr(result, "LOCFH", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 768);
    add_machine_instr(result, "LOCR", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 768);
    add_machine_instr(result, "LOCGR", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 768);
    add_machine_instr(result, "LOC", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 768);
    add_machine_instr(result, "LOCG", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 768);
    add_machine_instr(result, "LPD", mach_format::SSF, { reg_4_U, db_12_4_U, db_12_4_U }, 769);
    add_machine_instr(result, "LPDG", mach_format::SSF, { reg_4_U, db_12_4_U, db_12_4_U }, 769);
    add_machine_instr(result, "LPQ", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 770);
    add_machine_instr(result, "LPR", mach_format::RR, { reg_4_U, reg_4_U }, 771);
    add_machine_instr(result, "LPGR", mach_format::RRE, { reg_4_U, reg_4_U }, 771);
    add_machine_instr(result, "LPGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 771);
    add_machine_instr(result, "LRVR", mach_format::RRE, { reg_4_U, reg_4_U }, 771);
    add_machine_instr(result, "LRVGR", mach_format::RRE, { reg_4_U, reg_4_U }, 771);
    add_machine_instr(result, "LRVH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 771);
    add_machine_instr(result, "LRV", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 771);
    add_machine_instr(result, "LRVG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 771);
    add_machine_instr(result, "MC", mach_format::SI, { db_12_4_U, imm_8_S }, 772);
    add_machine_instr(result, "MVC", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 773);
    add_machine_instr(result, "MVCRL", mach_format::SSE, { db_12_4_U, db_12_4_U }, 788);
    add_machine_instr(result, "MVHHI", mach_format::SIL, { db_12_4_U, imm_16_S }, 773);
    add_machine_instr(result, "MVHI", mach_format::SIL, { db_12_4_U, imm_16_S }, 773);
    add_machine_instr(result, "MVGHI", mach_format::SIL, { db_12_4_U, imm_16_S }, 773);
    add_machine_instr(result, "MVI", mach_format::SI, { db_12_4_U, imm_8_U }, 773);
    add_machine_instr(result, "MVIY", mach_format::SIY, { db_12_4_U, imm_8_U }, 773);
    add_machine_instr(result, "MVCIN", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 774);
    add_machine_instr(result, "MVCL", mach_format::RR, { reg_4_U, reg_4_U }, 774);
    add_machine_instr(result, "MVCLE", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 778);
    add_machine_instr(result, "MVCLU", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 781);
    add_machine_instr(result, "MVN", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 785);
    add_machine_instr(result, "MVST", mach_format::RRE, { reg_4_U, reg_4_U }, 785);
    add_machine_instr(result, "MVO", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 786);
    add_machine_instr(result, "MVZ", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 787);
    add_machine_instr(result, "MR", mach_format::RR, { reg_4_U, reg_4_U }, 788);
    add_machine_instr(result, "MGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 788);
    add_machine_instr(result, "M", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 788);
    add_machine_instr(result, "MFY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 788);
    add_machine_instr(result, "MG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 788);
    add_machine_instr(result, "MH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 789);
    add_machine_instr(result, "MHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 789);
    add_machine_instr(result, "MGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 789);
    add_machine_instr(result, "MHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 789);
    add_machine_instr(result, "MGHI", mach_format::RI_a, { reg_4_U, imm_16_S }, 789);
    add_machine_instr(result, "MLR", mach_format::RRE, { reg_4_U, reg_4_U }, 790);
    add_machine_instr(result, "MLGR", mach_format::RRE, { reg_4_U, reg_4_U }, 790);
    add_machine_instr(result, "ML", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 790);
    add_machine_instr(result, "MLG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 790);
    add_machine_instr(result, "MSR", mach_format::RRE, { reg_4_U, reg_4_U }, 791);
    add_machine_instr(result, "MSRKC", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 791);
    add_machine_instr(result, "MSGR", mach_format::RRE, { reg_4_U, reg_4_U }, 791);
    add_machine_instr(result, "MSGRKC", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 791);
    add_machine_instr(result, "MSGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 791);
    add_machine_instr(result, "MS", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 791);
    add_machine_instr(result, "MSC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 791);
    add_machine_instr(result, "MSY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 791);
    add_machine_instr(result, "MSG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 791);
    add_machine_instr(result, "MSGC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 791);
    add_machine_instr(result, "MSGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 791);
    add_machine_instr(result, "MSFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 791);
    add_machine_instr(result, "MSGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 791);
    add_machine_instr(result, "NIAI", mach_format::IE, { imm_4_U, imm_4_U }, 792);
    add_machine_instr(result, "NTSTG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 794);
    add_machine_instr(result, "NCGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 522);
    add_machine_instr(result, "NCRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 522);
    add_machine_instr(result, "NNRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 796);
    add_machine_instr(result, "NNGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 796);
    add_machine_instr(result, "NOGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 799);
    add_machine_instr(result, "NORK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 799);
    add_machine_instr(result, "NXRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 799);
    add_machine_instr(result, "NXGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 799);
    add_machine_instr(result, "OR", mach_format::RR, { reg_4_U, reg_4_U }, 794);
    add_machine_instr(result, "OGR", mach_format::RRE, { reg_4_U, reg_4_U }, 794);
    add_machine_instr(result, "ORK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 794);
    add_machine_instr(result, "OCGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 802);
    add_machine_instr(result, "OCRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 802);
    add_machine_instr(result, "OGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 794);
    add_machine_instr(result, "O", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 794);
    add_machine_instr(result, "OY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 794);
    add_machine_instr(result, "OG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 795);
    add_machine_instr(result, "OI", mach_format::SI, { db_12_4_U, imm_8_U }, 795);
    add_machine_instr(result, "OIY", mach_format::SIY, { db_20_4_S, imm_8_U }, 795);
    add_machine_instr(result, "OC", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 795);
    add_machine_instr(result, "OIHF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 796);
    add_machine_instr(result, "OIHH", mach_format::RI_a, { reg_4_U, imm_16_U }, 796);
    add_machine_instr(result, "OIHL", mach_format::RI_a, { reg_4_U, imm_16_U }, 796);
    add_machine_instr(result, "OILF", mach_format::RIL_a, { reg_4_U, imm_32_S }, 796);
    add_machine_instr(result, "OILH", mach_format::RI_a, { reg_4_U, imm_16_U }, 796);
    add_machine_instr(result, "OILL", mach_format::RI_a, { reg_4_U, imm_16_U }, 796);
    add_machine_instr(result, "PACK", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 796);
    add_machine_instr(result, "PKA", mach_format::SS_f, { db_12_4_U, db_12_8x4L_U }, 797);
    add_machine_instr(result, "PKU", mach_format::SS_f, { db_12_4_U, db_12_8x4L_U }, 798);
    add_machine_instr(result, "PCC", mach_format::RRE, {}, 799);
    add_machine_instr(result, "PLO", mach_format::SS_e, { reg_4_U, db_12_4_U, reg_4_U, db_12_4_U }, 815);
    add_machine_instr(result, "PPA", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 829);
    add_machine_instr(result, "PRNO", mach_format::RRE, { reg_4_U, reg_4_U }, 830);
    add_machine_instr(result, "PPNO", mach_format::RRE, { reg_4_U, reg_4_U }, 830);
    add_machine_instr(result, "POPCNT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 853);
    add_machine_instr(result, "PFD", mach_format::RXY_b, { mask_4_U, dxb_20_4x4_S }, 843);
    add_machine_instr(result, "PFDRL", mach_format::RIL_c, { mask_4_U, rel_addr_imm_32_S }, 843);
    add_machine_instr(result, "RLL", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 845);
    add_machine_instr(result, "RLLG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 845);
    add_machine_instr(result, "RNSBG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 845);
    add_machine_instr(result, "RXSBG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 846);
    add_machine_instr(result, "ROSBG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 846);
    add_machine_instr(result, "RISBG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 847);
    add_machine_instr(result, "RISBGN", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 847);
    add_machine_instr(result, "RISBHG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 848);
    add_machine_instr(result, "RISBLG", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 849);
    add_machine_instr(result, "RNSBGT", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 845);
    add_machine_instr(result, "RXSBGT", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 846);
    add_machine_instr(result, "ROSBGT", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 858);
    add_machine_instr(result, "RISBGZ", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 858);
    add_machine_instr(result, "RISBGNZ", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 860);
    add_machine_instr(result, "RISBHGZ", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 860);
    add_machine_instr(result, "RISBLGZ", mach_format::RIE_f, { reg_4_U, reg_4_U, imm_8_S, imm_8_S, imm_8_S }, 1, 860);
    add_machine_instr(result, "SRST", mach_format::RRE, { reg_4_U, reg_4_U }, 850);
    add_machine_instr(result, "SRSTU", mach_format::RRE, { reg_4_U, reg_4_U }, 852);
    add_machine_instr(result, "SAR", mach_format::RRE, { reg_4_U, reg_4_U }, 854);
    add_machine_instr(result, "SAM24", mach_format::E, {}, 854);
    add_machine_instr(result, "SAM31", mach_format::E, {}, 854);
    add_machine_instr(result, "SAM64", mach_format::E, {}, 854);
    add_machine_instr(result, "SPM", mach_format::RR, { reg_4_U }, 855);
    add_machine_instr(result, "SLDA", mach_format::RS_a, { reg_4_U, db_12_4_U }, 855);
    add_machine_instr(result, "SLA", mach_format::RS_a, { reg_4_U, db_12_4_U }, 856);
    add_machine_instr(result, "SLAK", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 856);
    add_machine_instr(result, "SLAG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 856);
    add_machine_instr(result, "SLDL", mach_format::RS_a, { reg_4_U, db_12_4_U }, 856);
    add_machine_instr(result, "SLL", mach_format::RS_a, { reg_4_U, db_12_4_U }, 857);
    add_machine_instr(result, "SLLK", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 857);
    add_machine_instr(result, "SLLG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 857);
    add_machine_instr(result, "SRDA", mach_format::RS_a, { reg_4_U, db_12_4_U }, 858);
    add_machine_instr(result, "SRDL", mach_format::RS_a, { reg_4_U, db_12_4_U }, 858);
    add_machine_instr(result, "SRA", mach_format::RS_a, { reg_4_U, db_12_4_U }, 859);
    add_machine_instr(result, "SRAK", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 859);
    add_machine_instr(result, "SRAG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 859);
    add_machine_instr(result, "SRL", mach_format::RS_a, { reg_4_U, db_12_4_U }, 860);
    add_machine_instr(result, "SRLK", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 860);
    add_machine_instr(result, "SRLG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 860);
    add_machine_instr(result, "ST", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 860);
    add_machine_instr(result, "STY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 861);
    add_machine_instr(result, "STG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 861);
    add_machine_instr(result, "STRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 861);
    add_machine_instr(result, "STGRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 861);
    add_machine_instr(result, "STAM", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 861);
    add_machine_instr(result, "STAMY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 861);
    add_machine_instr(result, "STC", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 862);
    add_machine_instr(result, "STCY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 862);
    add_machine_instr(result, "STCH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 862);
    add_machine_instr(result, "STCM", mach_format::RS_b, { reg_4_U, mask_4_U, db_12_4_U }, 862);
    add_machine_instr(result, "STCMY", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 862);
    add_machine_instr(result, "STCMH", mach_format::RSY_b, { reg_4_U, mask_4_U, db_20_4_S }, 862);
    add_machine_instr(result, "STCK", mach_format::S, { db_12_4_U }, 863);
    add_machine_instr(result, "STCKF", mach_format::S, { db_12_4_U }, 863);
    add_machine_instr(result, "STCKE", mach_format::S, { db_12_4_U }, 864);
    add_machine_instr(result, "STFLE", mach_format::S, { db_20_4_S }, 866);
    add_machine_instr(result, "STGSC", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 867);
    add_machine_instr(result, "STH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 867);
    add_machine_instr(result, "STHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 868);
    add_machine_instr(result, "STHRL", mach_format::RIL_b, { reg_4_U, rel_addr_imm_32_S }, 868);
    add_machine_instr(result, "STHH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 868);
    add_machine_instr(result, "STFH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 868);
    add_machine_instr(result, "STM", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 869);
    add_machine_instr(result, "STMY", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 869);
    add_machine_instr(result, "STMG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 869);
    add_machine_instr(result, "STMH", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 869);
    add_machine_instr(result, "STOC", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 869);
    add_machine_instr(result, "STOCG", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 869);
    add_machine_instr(result, "STOCFH", mach_format::RSY_b, { reg_4_U, db_20_4_S, mask_4_U }, 870);
    add_machine_instr(result, "STPQ", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 870);
    add_machine_instr(result, "STRVH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 871);
    add_machine_instr(result, "STRV", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 871);
    add_machine_instr(result, "STRVG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 871);
    add_machine_instr(result, "SR", mach_format::RR, { reg_4_U, reg_4_U }, 871);
    add_machine_instr(result, "SGR", mach_format::RRE, { reg_4_U, reg_4_U }, 871);
    add_machine_instr(result, "SGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 871);
    add_machine_instr(result, "SRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 871);
    add_machine_instr(result, "SGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 872);
    add_machine_instr(result, "S", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 872);
    add_machine_instr(result, "SY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 872);
    add_machine_instr(result, "SG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 872);
    add_machine_instr(result, "SGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 872);
    add_machine_instr(result, "SH", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 872);
    add_machine_instr(result, "SHY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 872);
    add_machine_instr(result, "SGH", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 872);
    add_machine_instr(result, "SHHHR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SHHLR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SLR", mach_format::RR, { reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SLGR", mach_format::RRE, { reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SLGFR", mach_format::RRE, { reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SLRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SLGRK", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 873);
    add_machine_instr(result, "SL", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 874);
    add_machine_instr(result, "SLY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 874);
    add_machine_instr(result, "SLG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 874);
    add_machine_instr(result, "SLGF", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 874);
    add_machine_instr(result, "SLFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 874);
    add_machine_instr(result, "SLGFI", mach_format::RIL_a, { reg_4_U, imm_32_S }, 874);
    add_machine_instr(result, "SLHHHR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 875);
    add_machine_instr(result, "SLHHLR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 875);
    add_machine_instr(result, "SLBR", mach_format::RRE, { reg_4_U, reg_4_U }, 875);
    add_machine_instr(result, "SLBGR", mach_format::RRE, { reg_4_U, reg_4_U }, 875);
    add_machine_instr(result, "SLB", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 875);
    add_machine_instr(result, "SLBG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 875);
    add_machine_instr(result, "SVC", mach_format::I, { imm_8_U }, 876);
    add_machine_instr(result, "TS", mach_format::SI, { db_12_4_U }, 876);
    add_machine_instr(result, "TAM", mach_format::E, {}, 876);
    add_machine_instr(result, "TM", mach_format::SI, { db_12_4_U, imm_8_U }, 877);
    add_machine_instr(result, "TMY", mach_format::SIY, { db_20_4_S, imm_8_U }, 877);
    add_machine_instr(result, "TMHH", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TMHL", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TMH", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TMLH", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TML", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TMLL", mach_format::RI_a, { reg_4_U, imm_16_U }, 877);
    add_machine_instr(result, "TABORT", mach_format::S, { db_12_4_U }, 878);
    add_machine_instr(result, "TBEGIN", mach_format::SIL, { db_12_4_U, imm_16_S }, 879);
    add_machine_instr(result, "TBEGINC", mach_format::SIL, { db_12_4_U, imm_16_S }, 883);
    add_machine_instr(result, "TEND", mach_format::S, {}, 885);
    add_machine_instr(result, "TR", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 886);
    add_machine_instr(result, "TRT", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 887);
    add_machine_instr(result, "TRTE", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 887);
    add_machine_instr(result, "TRTRE", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 888);
    add_machine_instr(result, "TRTR", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 892);
    add_machine_instr(result, "TRE", mach_format::RRE, { reg_4_U, reg_4_U }, 893);
    add_machine_instr(result, "TROO", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 895);
    add_machine_instr(result, "TROT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 895);
    add_machine_instr(result, "TRTO", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 895);
    add_machine_instr(result, "TRTT", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 895);
    add_machine_instr(result, "UNPK", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 900);
    add_machine_instr(result, "UNPKA", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 901);
    add_machine_instr(result, "UNPKU", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 902);
    add_machine_instr(result, "UPT", mach_format::E, {}, 903);
    add_machine_instr(result, "AP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 920);
    add_machine_instr(result, "CP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 921);
    add_machine_instr(result, "DP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 921);
    add_machine_instr(result, "DFLTCC", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1714);
    add_machine_instr(result, "ED", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 922);
    add_machine_instr(result, "EDMK", mach_format::SS_a, { db_12_8x4L_U, db_12_4_U }, 925);
    add_machine_instr(result, "SRP", mach_format::SS_c, { db_12_4x4L_U, db_12_4_U, imm_4_U }, 926);
    add_machine_instr(result, "MP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 926);
    add_machine_instr(result, "SP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 927);
    add_machine_instr(result, "TP", mach_format::RSL_a, { db_12_4x4L_U }, 928);
    add_machine_instr(result, "ZAP", mach_format::SS_b, { db_12_4x4L_U, db_12_4x4L_U }, 928);
    add_machine_instr(result, "THDR", mach_format::RRE, { reg_4_U, reg_4_U }, 955);
    add_machine_instr(result, "THDER", mach_format::RRE, { reg_4_U, reg_4_U }, 955);
    add_machine_instr(result, "TBEDR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 956);
    add_machine_instr(result, "TBDR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 956);
    add_machine_instr(result, "CPSDR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U }, 958);
    add_machine_instr(result, "EFPC", mach_format::RRE, { reg_4_U }, 958);
    add_machine_instr(result, "LER", mach_format::RR, { reg_4_U, reg_4_U }, 959);
    add_machine_instr(result, "LDR", mach_format::RR, { reg_4_U, reg_4_U }, 959);
    add_machine_instr(result, "LXR", mach_format::RRE, { reg_4_U, reg_4_U }, 959);
    add_machine_instr(result, "LE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 959);
    add_machine_instr(result, "LD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 959);
    add_machine_instr(result, "LEY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 959);
    add_machine_instr(result, "LDY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 959);
    add_machine_instr(result, "LCDFR", mach_format::RRE, { reg_4_U, reg_4_U }, 959);
    add_machine_instr(result, "LFPC", mach_format::S, { db_12_4_U }, 959);
    add_machine_instr(result, "LFAS", mach_format::S, { db_12_4_U }, 960);
    add_machine_instr(result, "LDGR", mach_format::RRE, { reg_4_U, reg_4_U }, 962);
    add_machine_instr(result, "LGDR", mach_format::RRE, { reg_4_U, reg_4_U }, 962);
    add_machine_instr(result, "LNDFR", mach_format::RRE, { reg_4_U, reg_4_U }, 962);
    add_machine_instr(result, "LPDFR", mach_format::RRE, { reg_4_U, reg_4_U }, 962);
    add_machine_instr(result, "LZER", mach_format::RRE, { reg_4_U }, 963);
    add_machine_instr(result, "LZXR", mach_format::RRE, { reg_4_U }, 963);
    add_machine_instr(result, "LZDR", mach_format::RRE, { reg_4_U }, 963);
    add_machine_instr(result, "PFPO", mach_format::E, {}, 963);
    add_machine_instr(result, "SRNM", mach_format::S, { db_12_4_U }, 975);
    add_machine_instr(result, "SRNMB", mach_format::S, { db_12_4_U }, 975);
    add_machine_instr(result, "SRNMT", mach_format::S, { db_12_4_U }, 975);
    add_machine_instr(result, "SFPC", mach_format::RRE, { reg_4_U }, 975);
    add_machine_instr(result, "SFASR", mach_format::RRE, { reg_4_U }, 976);
    add_machine_instr(result, "STE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 976);
    add_machine_instr(result, "STD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 976);
    add_machine_instr(result, "STDY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 977);
    add_machine_instr(result, "STEY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 977);
    add_machine_instr(result, "STFPC", mach_format::S, { db_12_4_U }, 977);
    add_machine_instr(result, "BSA", mach_format::RRE, { reg_4_U, reg_4_U }, 989);
    add_machine_instr(result, "BAKR", mach_format::RRE, { reg_4_U, reg_4_U }, 993);
    add_machine_instr(result, "BSG", mach_format::RRE, { reg_4_U, reg_4_U }, 995);
    add_machine_instr(result, "CRDTE", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1, 999);
    add_machine_instr(result, "CSP", mach_format::RRE, { reg_4_U, reg_4_U }, 1003);
    add_machine_instr(result, "CSPG", mach_format::RRE, { reg_4_U, reg_4_U }, 1003);
    add_machine_instr(result, "ESEA", mach_format::RRE, { reg_4_U }, 1006);
    add_machine_instr(result, "EPAR", mach_format::RRE, { reg_4_U }, 1006);
    add_machine_instr(result, "EPAIR", mach_format::RRE, { reg_4_U }, 1006);
    add_machine_instr(result, "ESAR", mach_format::RRE, { reg_4_U }, 1006);
    add_machine_instr(result, "ESAIR", mach_format::RRE, { reg_4_U }, 1007);
    add_machine_instr(result, "EREG", mach_format::RRE, { reg_4_U, reg_4_U }, 1007);
    add_machine_instr(result, "EREGG", mach_format::RRE, { reg_4_U, reg_4_U }, 1007);
    add_machine_instr(result, "ESTA", mach_format::RRE, { reg_4_U, reg_4_U }, 1008);
    add_machine_instr(result, "IAC", mach_format::RRE, { reg_4_U }, 1011);
    add_machine_instr(result, "IPK", mach_format::S, {}, 1012);
    add_machine_instr(result, "IRBM", mach_format::RRE, { reg_4_U, reg_4_U }, 1012);
    add_machine_instr(result, "ISK", mach_format::RR, { reg_4_U, reg_4_U }, 268);
    add_machine_instr(result, "ISKE", mach_format::RRE, { reg_4_U, reg_4_U }, 1012);
    add_machine_instr(result, "IVSK", mach_format::RRE, { reg_4_U, reg_4_U }, 1013);
    add_machine_instr(result, "IDTE", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1, 1014);
    add_machine_instr(result, "IPTE", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 2, 1019);
    add_machine_instr(result, "LASP", mach_format::SSE, { db_12_4_U, db_12_4_U }, 1023);
    add_machine_instr(result, "LCTL", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 1032);
    add_machine_instr(result, "LCTLG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 1032);
    add_machine_instr(result, "LPTEA", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1032);
    add_machine_instr(result, "LPSW", mach_format::SI, { db_12_4_U }, 1036);
    add_machine_instr(result, "LPSWE", mach_format::S, { db_12_4_U }, 1037);
    add_machine_instr(result, "LRA", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1038);
    add_machine_instr(result, "LRAY", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 1038);
    add_machine_instr(result, "LRAG", mach_format::RXY_a, { reg_4_U, dxb_20_4x4_S }, 1038);
    add_machine_instr(result, "LURA", mach_format::RRE, { reg_4_U, reg_4_U }, 1042);
    add_machine_instr(result, "LURAG", mach_format::RRE, { reg_4_U, reg_4_U }, 1042);
    add_machine_instr(result, "MSTA", mach_format::RRE, { reg_4_U }, 1043);
    add_machine_instr(result, "MVPG", mach_format::RRE, { reg_4_U, reg_4_U }, 1044);
    add_machine_instr(result, "MVCP", mach_format::SS_d, { drb_12_4x4_U, db_12_4_U, reg_4_U }, 1046);
    add_machine_instr(result, "MVCS", mach_format::SS_d, { drb_12_4x4_U, db_12_4_U, reg_4_U }, 1046);
    add_machine_instr(result, "MVCDK", mach_format::SSE, { db_12_4_U, db_12_4_U }, 1048);
    add_machine_instr(result, "MVCK", mach_format::SS_d, { drb_12_4x4_U, db_12_4_U, reg_4_U }, 1049);
    add_machine_instr(result, "MVCOS", mach_format::SSF, { db_12_4_U, db_12_4_U, reg_4_U }, 1050);
    add_machine_instr(result, "MVCSK", mach_format::SSE, { db_12_4_U, db_12_4_U }, 1053);
    add_machine_instr(result, "PGIN", mach_format::RRE, { reg_4_U, reg_4_U }, 1054);
    add_machine_instr(result, "PGOUT", mach_format::RRE, { reg_4_U, reg_4_U }, 1055);
    add_machine_instr(result, "PCKMO", mach_format::RRE, {}, 1056);
    add_machine_instr(result, "PFMF", mach_format::RRE, { reg_4_U, reg_4_U }, 1059);
    add_machine_instr(result, "PTFF", mach_format::E, {}, 1063);
    add_machine_instr(result, "PTF", mach_format::RRE, { reg_4_U }, 1071);
    add_machine_instr(result, "PC", mach_format::S, { db_12_4_U }, 1072);
    add_machine_instr(result, "PR", mach_format::E, {}, 1085);
    add_machine_instr(result, "PTI", mach_format::RRE, { reg_4_U, reg_4_U }, 1089);
    add_machine_instr(result, "PT", mach_format::RRE, { reg_4_U, reg_4_U }, 1089);
    add_machine_instr(result, "PALB", mach_format::RRE, {}, 1098);
    add_machine_instr(result, "PTLB", mach_format::S, {}, 1098);
    add_machine_instr(result, "RRBE", mach_format::RRE, { reg_4_U, reg_4_U }, 1098);
    add_machine_instr(result, "RRBM", mach_format::RRE, { reg_4_U, reg_4_U }, 1099);
    add_machine_instr(result, "RP", mach_format::S, { db_12_4_U }, 1099);
    add_machine_instr(result, "SAC", mach_format::S, { db_12_4_U }, 1102);
    add_machine_instr(result, "SACF", mach_format::S, { db_12_4_U }, 1102);
    add_machine_instr(result, "SCK", mach_format::S, { db_12_4_U }, 1103);
    add_machine_instr(result, "SCKC", mach_format::S, { db_12_4_U }, 1104);
    add_machine_instr(result, "SCKPF", mach_format::E, {}, 1105);
    add_machine_instr(result, "SPX", mach_format::S, { db_12_4_U }, 1105);
    add_machine_instr(result, "SPT", mach_format::S, { db_12_4_U }, 1105);
    add_machine_instr(result, "SPKA", mach_format::S, { db_12_4_U }, 1106);
    add_machine_instr(result, "SSAR", mach_format::RRE, { reg_4_U }, 1107);
    add_machine_instr(result, "SSAIR", mach_format::RRE, { reg_4_U }, 1107);
    add_machine_instr(result, "SSK", mach_format::RR, { reg_4_U, reg_4_U }, 304);
    add_machine_instr(result, "SSKE", mach_format::RRF_c, { reg_4_U, reg_4_U, mask_4_U }, 1, 1112);
    add_machine_instr(result, "SSM", mach_format::SI, { db_12_4_U }, 1115);
    add_machine_instr(result, "SIGP", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 1115);
    add_machine_instr(result, "STCKC", mach_format::S, { db_12_4_U }, 1117);
    add_machine_instr(result, "STCTL", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 1117);
    add_machine_instr(result, "STCTG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 1117);
    add_machine_instr(result, "STAP", mach_format::S, { db_12_4_U }, 1118);
    add_machine_instr(result, "STIDP", mach_format::S, { db_12_4_U }, 1118);
    add_machine_instr(result, "STPT", mach_format::S, { db_12_4_U }, 1120);
    add_machine_instr(result, "STFL", mach_format::S, { db_12_4_U }, 1120);
    add_machine_instr(result, "STPX", mach_format::S, { db_12_4_U }, 1121);
    add_machine_instr(result, "STRAG", mach_format::SSE, { db_12_4_U, db_12_4_U }, 1121);
    add_machine_instr(result, "STSI", mach_format::S, { db_12_4_U }, 1122);
    add_machine_instr(result, "STOSM", mach_format::SI, { db_12_4_U, imm_8_S }, 1146);
    add_machine_instr(result, "STNSM", mach_format::SI, { db_12_4_U, imm_8_S }, 1146);
    add_machine_instr(result, "STURA", mach_format::RRE, { reg_4_U, reg_4_U }, 1147);
    add_machine_instr(result, "STURG", mach_format::RRE, { reg_4_U, reg_4_U }, 1147);
    add_machine_instr(result, "TAR", mach_format::RRE, { reg_4_U, reg_4_U }, 1147);
    add_machine_instr(result, "TB", mach_format::RRE, { reg_4_U, reg_4_U }, 1149);
    add_machine_instr(result, "TPEI", mach_format::RRE, { reg_4_U, reg_4_U }, 1151);
    add_machine_instr(result, "TPROT", mach_format::SSE, { db_12_4_U, db_12_4_U }, 1152);
    add_machine_instr(result, "TRACE", mach_format::RS_a, { reg_4_U, reg_4_U, db_12_4_U }, 1155);
    add_machine_instr(result, "TRACG", mach_format::RSY_a, { reg_4_U, reg_4_U, db_20_4_S }, 1155);
    add_machine_instr(result, "TRAP2", mach_format::E, {}, 1156);
    add_machine_instr(result, "TRAP4", mach_format::S, { db_12_4_U }, 1156);
    add_machine_instr(result, "XSCH", mach_format::S, {}, 1215);
    add_machine_instr(result, "CSCH", mach_format::S, {}, 1217);
    add_machine_instr(result, "HSCH", mach_format::S, {}, 1218);
    add_machine_instr(result, "MSCH", mach_format::S, { db_12_4_U }, 1219);
    add_machine_instr(result, "RCHP", mach_format::S, {}, 1221);
    add_machine_instr(result, "RSCH", mach_format::S, {}, 1222);
    add_machine_instr(result, "SAL", mach_format::S, {}, 1224);
    add_machine_instr(result, "SCHM", mach_format::S, {}, 1225);
    add_machine_instr(result, "SSCH", mach_format::S, { db_12_4_U }, 1227);
    add_machine_instr(result, "STCPS", mach_format::S, { db_12_4_U }, 1228);
    add_machine_instr(result, "STCRW", mach_format::S, { db_12_4_U }, 1229);
    add_machine_instr(result, "STSCH", mach_format::S, { db_12_4_U }, 1230);
    add_machine_instr(result, "TPI", mach_format::S, { db_12_4_U }, 1231);
    add_machine_instr(result, "TSCH", mach_format::S, { db_12_4_U }, 1232);

    add_machine_instr(result, "AER", mach_format::RR, { reg_4_U, reg_4_U }, 1412);
    add_machine_instr(result, "ADR", mach_format::RR, { reg_4_U, reg_4_U }, 1412);
    add_machine_instr(result, "AXR", mach_format::RR, { reg_4_U, reg_4_U }, 1412);
    add_machine_instr(result, "AE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1412);
    add_machine_instr(result, "AD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1412);
    add_machine_instr(result, "AWR", mach_format::RR, { reg_4_U, reg_4_U }, 1413);
    add_machine_instr(result, "AUR", mach_format::RR, { reg_4_U, reg_4_U }, 1413);
    add_machine_instr(result, "AU", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1413);
    add_machine_instr(result, "AW", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1413);
    add_machine_instr(result, "CER", mach_format::RR, { reg_4_U, reg_4_U }, 1414);
    add_machine_instr(result, "CDR", mach_format::RR, { reg_4_U, reg_4_U }, 1414);
    add_machine_instr(result, "CXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1414);
    add_machine_instr(result, "CE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1414);
    add_machine_instr(result, "CD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1414);
    add_machine_instr(result, "CEFR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CDFR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CXFR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CEGR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CDGR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CXGR", mach_format::RRE, { reg_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CFER", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CFDR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CFXR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CGER", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CGDR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "CGXR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1415);
    add_machine_instr(result, "DDR", mach_format::RR, { reg_4_U, reg_4_U }, 1416);
    add_machine_instr(result, "DER", mach_format::RR, { reg_4_U, reg_4_U }, 1416);
    add_machine_instr(result, "DXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1416);
    add_machine_instr(result, "DD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1416);
    add_machine_instr(result, "DE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1416);
    add_machine_instr(result, "HDR", mach_format::RR, { reg_4_U, reg_4_U }, 1417);
    add_machine_instr(result, "HER", mach_format::RR, { reg_4_U, reg_4_U }, 1417);
    add_machine_instr(result, "LTER", mach_format::RR, { reg_4_U, reg_4_U }, 1417);
    add_machine_instr(result, "LTDR", mach_format::RR, { reg_4_U, reg_4_U }, 1417);
    add_machine_instr(result, "LTXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1418);
    add_machine_instr(result, "LCER", mach_format::RR, { reg_4_U, reg_4_U }, 1418);
    add_machine_instr(result, "LCDR", mach_format::RR, { reg_4_U, reg_4_U }, 1418);
    add_machine_instr(result, "LCXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1418);
    add_machine_instr(result, "FIER", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "FIDR", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "FIXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "LDER", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "LXDR", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "LXER", mach_format::RRE, { reg_4_U, reg_4_U }, 1419);
    add_machine_instr(result, "LDE", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1419);
    add_machine_instr(result, "LXD", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1419);
    add_machine_instr(result, "LXE", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1419);
    add_machine_instr(result, "LNDR", mach_format::RR, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LNER", mach_format::RR, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LPDR", mach_format::RR, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LPER", mach_format::RR, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LNXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LPXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1420);
    add_machine_instr(result, "LEDR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "LDXR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "LRER", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "LRDR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "LEXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MEER", mach_format::RRE, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MDR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MXR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MDER", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MER", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MXDR", mach_format::RR, { reg_4_U, reg_4_U }, 1421);
    add_machine_instr(result, "MEE", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1422);
    add_machine_instr(result, "MD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1422);
    add_machine_instr(result, "MDE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1422);
    add_machine_instr(result, "MXD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1422);
    add_machine_instr(result, "ME", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1422);
    add_machine_instr(result, "MAER", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1423);
    add_machine_instr(result, "MADR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1423);
    add_machine_instr(result, "MAD", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1423);
    add_machine_instr(result, "MAE", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1423);
    add_machine_instr(result, "MSER", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1423);
    add_machine_instr(result, "MSDR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1423);
    add_machine_instr(result, "MSE", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1423);
    add_machine_instr(result, "MSD", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1423);
    add_machine_instr(result, "MAYR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1424);
    add_machine_instr(result, "MAYHR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1424);
    add_machine_instr(result, "MAYLR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1424);
    add_machine_instr(result, "MAY", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1424);
    add_machine_instr(result, "MAYH", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1424);
    add_machine_instr(result, "MAYL", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1424);
    add_machine_instr(result, "MYR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1426);
    add_machine_instr(result, "MYHR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1426);
    add_machine_instr(result, "MYLR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1426);
    add_machine_instr(result, "MY", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1426);
    add_machine_instr(result, "MYH", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1426);
    add_machine_instr(result, "MYL", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1426);
    add_machine_instr(result, "SQER", mach_format::RRE, { reg_4_U, reg_4_U }, 1427);
    add_machine_instr(result, "SQDR", mach_format::RRE, { reg_4_U, reg_4_U }, 1427);
    add_machine_instr(result, "SQXR", mach_format::RRE, { reg_4_U, reg_4_U }, 1427);
    add_machine_instr(result, "SQE", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1427);
    add_machine_instr(result, "SQD", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1427);
    add_machine_instr(result, "SER", mach_format::RR, { reg_4_U, reg_4_U }, 1428);
    add_machine_instr(result, "SDR", mach_format::RR, { reg_4_U, reg_4_U }, 1428);
    add_machine_instr(result, "SXR", mach_format::RR, { reg_4_U, reg_4_U }, 1428);
    add_machine_instr(result, "SE", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1428);
    add_machine_instr(result, "SD", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1428);
    add_machine_instr(result, "SUR", mach_format::RR, { reg_4_U, reg_4_U }, 1429);
    add_machine_instr(result, "SWR", mach_format::RR, { reg_4_U, reg_4_U }, 1429);
    add_machine_instr(result, "SU", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1429);
    add_machine_instr(result, "SW", mach_format::RX_a, { reg_4_U, dxb_12_4x4_U }, 1429);
    add_machine_instr(result, "AEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1445);
    add_machine_instr(result, "ADBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1445);
    add_machine_instr(result, "AXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1445);
    add_machine_instr(result, "AEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1445);
    add_machine_instr(result, "ADB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1445);
    add_machine_instr(result, "CEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1447);
    add_machine_instr(result, "CDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1447);
    add_machine_instr(result, "CXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1447);
    add_machine_instr(result, "CDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1447);
    add_machine_instr(result, "CEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1447);
    add_machine_instr(result, "KEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1448);
    add_machine_instr(result, "KDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1448);
    add_machine_instr(result, "KDSA", mach_format::RRE, { reg_4_U, reg_4_U }, 1700);
    add_machine_instr(result, "KXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1448);
    add_machine_instr(result, "KDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1448);
    add_machine_instr(result, "KEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1448);
    add_machine_instr(result, "CEFBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CDFBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CXFBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CEGBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CDGBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CXGBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1449);
    add_machine_instr(result, "CEFBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CDFBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CXFBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CEGBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CDGBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CXGBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1449);
    add_machine_instr(result, "CELFBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CDLFBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CXLFBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CELGBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CDLGBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CXLGBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1451);
    add_machine_instr(result, "CFEBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CFDBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CFXBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CGEBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CGDBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CGXBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1452);
    add_machine_instr(result, "CFEBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CFDBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CFXBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CGEBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CGDBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CGXBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1452);
    add_machine_instr(result, "CLFEBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "CLFDBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "CLFXBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "CLGEBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "CLGDBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "CLGXBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1455);
    add_machine_instr(result, "DEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1457);
    add_machine_instr(result, "DDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1457);
    add_machine_instr(result, "DXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1457);
    add_machine_instr(result, "DEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1457);
    add_machine_instr(result, "DDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1457);
    add_machine_instr(result, "DIEBR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1458);
    add_machine_instr(result, "DIDBR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1458);
    add_machine_instr(result, "LTEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "LTDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "LTXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "LCEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "LCDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "LCXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1461);
    add_machine_instr(result, "ECCTR", mach_format::RRE, { reg_4_U, reg_4_U }, 39);
    add_machine_instr(result, "EPCTR", mach_format::RRE, { reg_4_U, reg_4_U }, 39);
    add_machine_instr(result, "ECPGA", mach_format::RRE, { reg_4_U, reg_4_U }, 39);
    add_machine_instr(result, "FIEBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1462);
    add_machine_instr(result, "FIDBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1462);
    add_machine_instr(result, "FIXBR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1462);
    add_machine_instr(result, "FIEBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1462);
    add_machine_instr(result, "FIDBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1462);
    add_machine_instr(result, "FIXBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1462);
    add_machine_instr(result, "LSCTL", mach_format::S, { db_12_4_U }, 42);
    add_machine_instr(result, "LDEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1463);
    add_machine_instr(result, "LXDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1463);
    add_machine_instr(result, "LPCTL", mach_format::S, { db_12_4_U }, 41);
    add_machine_instr(result, "LCCTL", mach_format::S, { db_12_4_U }, 40);
    add_machine_instr(result, "LXEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1463);
    add_machine_instr(result, "LDEB", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LXDB", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LXEB", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LNEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LNDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LNXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1464);
    add_machine_instr(result, "LPEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LPDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LPXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LEDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LDXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LEXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1465);
    add_machine_instr(result, "LEDBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1465);
    add_machine_instr(result, "LDXBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1465);
    add_machine_instr(result, "LEXBRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1465);
    add_machine_instr(result, "LPP", mach_format::S, { db_12_4_U }, 11);
    add_machine_instr(result, "MEEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1467);
    add_machine_instr(result, "MDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1467);
    add_machine_instr(result, "MXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1467);
    add_machine_instr(result, "MDEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1467);
    add_machine_instr(result, "MXDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1467);
    add_machine_instr(result, "MEEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1467);
    add_machine_instr(result, "MDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1467);
    add_machine_instr(result, "MDEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1467);
    add_machine_instr(result, "MXDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1467);
    add_machine_instr(result, "MADBR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1468);
    add_machine_instr(result, "MAEBR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1468);
    add_machine_instr(result, "MAEB", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1468);
    add_machine_instr(result, "MADB", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1468);
    add_machine_instr(result, "MSEBR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1468);
    add_machine_instr(result, "MSDBR", mach_format::RRD, { reg_4_U, reg_4_U, reg_4_U }, 1468);
    add_machine_instr(result, "MSEB", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1468);
    add_machine_instr(result, "MSDB", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1468);
    add_machine_instr(result, "QCTRI", mach_format::S, { db_12_4_U }, 43);
    add_machine_instr(result, "QSI", mach_format::S, { db_12_4_U }, 45);
    add_machine_instr(result, "SCCTR", mach_format::RRE, { reg_4_U, reg_4_U }, 46);
    add_machine_instr(result, "SPCTR", mach_format::RRE, { reg_4_U, reg_4_U }, 47);
    add_machine_instr(result, "SQEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SQDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SQXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SQEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1470);
    add_machine_instr(result, "SQDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1470);
    add_machine_instr(result, "SEBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SDBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SXBR", mach_format::RRE, { reg_4_U, reg_4_U }, 1470);
    add_machine_instr(result, "SEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1470);
    add_machine_instr(result, "SDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1470);
    add_machine_instr(result, "SORTL", mach_format::RRE, { reg_4_U, reg_4_U }, 19);
    add_machine_instr(result, "TCEB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1471);
    add_machine_instr(result, "TCDB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1471);
    add_machine_instr(result, "TCXB", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1471);
    add_machine_instr(result, "ADTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1491);
    add_machine_instr(result, "AXTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1491);
    add_machine_instr(result, "ADTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1491);
    add_machine_instr(result, "AXTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1491);
    add_machine_instr(result, "CDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1494);
    add_machine_instr(result, "CXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1494);
    add_machine_instr(result, "KDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1495);
    add_machine_instr(result, "KXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1495);
    add_machine_instr(result, "CEDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1495);
    add_machine_instr(result, "CEXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1495);
    add_machine_instr(result, "CDGTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1496);
    add_machine_instr(result, "CXGTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1496);
    add_machine_instr(result, "CDGTRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1496);
    add_machine_instr(result, "CXGTRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1496);
    add_machine_instr(result, "CDFTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1496);
    add_machine_instr(result, "CXFTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1496);
    add_machine_instr(result, "CDLGTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1497);
    add_machine_instr(result, "CXLGTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1497);
    add_machine_instr(result, "CDLFTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1497);
    add_machine_instr(result, "CXLFTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1497);
    add_machine_instr(result, "CDPT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1498);
    add_machine_instr(result, "CXPT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1498);
    add_machine_instr(result, "CDSTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1500);
    add_machine_instr(result, "CXSTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1500);
    add_machine_instr(result, "CDUTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1500);
    add_machine_instr(result, "CXUTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1500);
    add_machine_instr(result, "CDZT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1501);
    add_machine_instr(result, "CXZT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1501);
    add_machine_instr(result, "CGDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1501);
    add_machine_instr(result, "CGXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U }, 1501);
    add_machine_instr(result, "CGDTRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1502);
    add_machine_instr(result, "CGXTRA", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1502);
    add_machine_instr(result, "CFDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1502);
    add_machine_instr(result, "CFXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1502);
    add_machine_instr(result, "CLGDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1504);
    add_machine_instr(result, "CLGXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1504);
    add_machine_instr(result, "CLFDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1504);
    add_machine_instr(result, "CLFXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1504);
    add_machine_instr(result, "CPDT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1505);
    add_machine_instr(result, "CPXT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1505);
    add_machine_instr(result, "CSDTR", mach_format::RRF_d, { reg_4_U, reg_4_U, mask_4_U }, 1507);
    add_machine_instr(result, "CSXTR", mach_format::RRF_d, { reg_4_U, reg_4_U, mask_4_U }, 1507);
    add_machine_instr(result, "CUDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1507);
    add_machine_instr(result, "CUXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1507);
    add_machine_instr(result, "CZDT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1508);
    add_machine_instr(result, "CZXT", mach_format::RSL_b, { reg_4_U, db_12_8x4L_U, mask_4_U }, 1508);
    add_machine_instr(result, "DDTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1509);
    add_machine_instr(result, "DXTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1509);
    add_machine_instr(result, "DDTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1509);
    add_machine_instr(result, "DXTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1509);
    add_machine_instr(result, "EEXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1511);
    add_machine_instr(result, "EEDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1511);
    add_machine_instr(result, "ESDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1511);
    add_machine_instr(result, "ESXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1511);
    add_machine_instr(result, "IEDTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U }, 1512);
    add_machine_instr(result, "IEXTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U }, 1512);
    add_machine_instr(result, "LTDTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1513);
    add_machine_instr(result, "LTXTR", mach_format::RRE, { reg_4_U, reg_4_U }, 1513);
    add_machine_instr(result, "FIDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1514);
    add_machine_instr(result, "FIXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1514);
    add_machine_instr(result, "LDETR", mach_format::RRF_d, { reg_4_U, reg_4_U, mask_4_U }, 1517);
    add_machine_instr(result, "LXDTR", mach_format::RRF_d, { reg_4_U, reg_4_U, mask_4_U }, 1517);
    add_machine_instr(result, "LEDTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1518);
    add_machine_instr(result, "LDXTR", mach_format::RRF_e, { reg_4_U, mask_4_U, reg_4_U, mask_4_U }, 1518);
    add_machine_instr(result, "MDTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1519);
    add_machine_instr(result, "MXTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1519);
    add_machine_instr(result, "MDTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1520);
    add_machine_instr(result, "MXTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1520);
    add_machine_instr(result, "QADTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1521);
    add_machine_instr(result, "QAXTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1521);
    add_machine_instr(result, "RRDTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1524);
    add_machine_instr(result, "RRXTR", mach_format::RRF_b, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1524);
    add_machine_instr(result, "SELFHR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 864);
    add_machine_instr(result, "SELGR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 864);
    add_machine_instr(result, "SELR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 864);
    add_machine_instr(result, "SLDT", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1526);
    add_machine_instr(result, "SLXT", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1526);
    add_machine_instr(result, "SRDT", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1526);
    add_machine_instr(result, "SRXT", mach_format::RXF, { reg_4_U, reg_4_U, dxb_12_4x4_U }, 1526);
    add_machine_instr(result, "SDTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1527);
    add_machine_instr(result, "SXTR", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U }, 1527);
    add_machine_instr(result, "SDTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1527);
    add_machine_instr(result, "SXTRA", mach_format::RRF_a, { reg_4_U, reg_4_U, reg_4_U, mask_4_U }, 1527);
    add_machine_instr(result, "TDCET", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1528);
    add_machine_instr(result, "TDCDT", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1528);
    add_machine_instr(result, "TDCXT", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1528);
    add_machine_instr(result, "TDGET", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1529);
    add_machine_instr(result, "TDGDT", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1529);
    add_machine_instr(result, "TDGXT", mach_format::RXE, { reg_4_U, dxb_12_4x4_U }, 1529);
    add_machine_instr(result, "VBPERM", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1536);
    add_machine_instr(result, "VGEF", mach_format::VRV, { vec_reg_5_U, dvb_12_5x4_U, mask_4_U }, 1536);
    add_machine_instr(
        result, "VCFPS", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1641);
    add_machine_instr(
        result, "VCLFP", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1611);
    add_machine_instr(result, "VGEG", mach_format::VRV, { vec_reg_5_U, dvb_12_5x4_U, mask_4_U }, 1536);
    add_machine_instr(result, "VGBM", mach_format::VRI_a, { vec_reg_5_U, imm_16_U }, 1537);
    add_machine_instr(result, "VGM", mach_format::VRI_b, { vec_reg_5_U, imm_8_U, imm_8_U, mask_4_U }, 1537);
    add_machine_instr(result, "VL", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1, 1538);
    add_machine_instr(result, "VSTEBRF", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1576);
    add_machine_instr(result, "VSTEBRG", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1576);
    add_machine_instr(result, "VLLEBRZ", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1562);
    add_machine_instr(result, "VLREP", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1538);
    add_machine_instr(result, "VLR", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U }, 1538);
    add_machine_instr(result, "VLEB", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1538);
    add_machine_instr(result, "VLEBRH", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1561);
    add_machine_instr(result, "VLEBRG", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1561);
    add_machine_instr(result, "VLBRREP", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1562);
    add_machine_instr(result, "VLER", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1564);
    add_machine_instr(result, "VLBR", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1563);
    add_machine_instr(result, "VLEH", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1539);
    add_machine_instr(result, "VLEIH", mach_format::VRI_a, { vec_reg_5_U, imm_16_S, mask_4_U }, 1539);
    add_machine_instr(result, "VLEF", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1539);
    add_machine_instr(result, "VLEIF", mach_format::VRI_a, { vec_reg_5_U, imm_16_S, mask_4_U }, 1539);
    add_machine_instr(result, "VLEG", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1539);
    add_machine_instr(result, "VLEIG", mach_format::VRI_a, { vec_reg_5_U, imm_16_S, mask_4_U }, 1539);
    add_machine_instr(result, "VLEIB", mach_format::VRI_a, { vec_reg_5_U, imm_16_S, mask_4_U }, 1539);
    add_machine_instr(result, "VLGV", mach_format::VRS_c, { reg_4_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1539);
    add_machine_instr(result, "VLLEZ", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1540);
    add_machine_instr(result, "VLM", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1, 1541);
    add_machine_instr(result, "VLRLR", mach_format::VRS_d, { vec_reg_5_U, reg_4_U, db_12_4_U }, 1541);
    add_machine_instr(result, "VLRL", mach_format::VSI, { vec_reg_5_U, db_12_4_U, imm_8_U }, 1541);
    add_machine_instr(result, "VLBB", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1542);
    add_machine_instr(result, "VLVG", mach_format::VRS_b, { vec_reg_5_U, reg_4_U, db_12_4_U, mask_4_U }, 1543);
    add_machine_instr(result, "VLVGP", mach_format::VRR_f, { vec_reg_5_U, reg_4_U, reg_4_U }, 1543);
    add_machine_instr(result, "VLL", mach_format::VRS_b, { vec_reg_5_U, reg_4_U, db_12_4_U }, 1543);
    add_machine_instr(result, "VMRH", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1544);
    add_machine_instr(result, "VMRL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1544);
    add_machine_instr(result, "VPK", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1545);
    add_machine_instr(
        result, "VPKS", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1545);
    add_machine_instr(
        result, "VPKLS", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1546);
    add_machine_instr(
        result, "VPERM", mach_format::VRR_e, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1547);
    add_machine_instr(result, "VPDI", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1547);
    add_machine_instr(result, "VREP", mach_format::VRI_c, { vec_reg_5_U, vec_reg_5_U, imm_16_U, mask_4_U }, 1547);
    add_machine_instr(result, "VREPI", mach_format::VRI_a, { vec_reg_5_U, imm_16_S, mask_4_U }, 1548);
    add_machine_instr(result, "VSCEF", mach_format::VRV, { vec_reg_5_U, dvb_12_5x4_U, mask_4_U }, 1548);
    add_machine_instr(result, "VSCEG", mach_format::VRV, { vec_reg_5_U, dvb_12_5x4_U, mask_4_U }, 1548);
    add_machine_instr(result, "VSEL", mach_format::VRR_e, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1549);
    add_machine_instr(result, "VSEG", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1549);
    add_machine_instr(result, "VSTBR", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1576);
    add_machine_instr(result, "VST", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1, 1550);
    add_machine_instr(result, "VSTEB", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1550);
    add_machine_instr(result, "VSTEBRH", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1576);
    add_machine_instr(result, "VSTEH", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1550);
    add_machine_instr(result, "VSTEF", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1550);
    add_machine_instr(result, "VSTEG", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1550);
    add_machine_instr(result, "VSTER", mach_format::VRX, { vec_reg_5_U, dxb_12_4x4_U, mask_4_U }, 1578);
    add_machine_instr(result, "VSTM", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1, 1551);
    add_machine_instr(result, "VSTRLR", mach_format::VRS_d, { vec_reg_5_U, reg_4_U, db_12_4_U }, 1551);
    add_machine_instr(result, "VSTRL", mach_format::VSI, { vec_reg_5_U, db_12_4_U, imm_8_U }, 1551);
    add_machine_instr(result, "VSTL", mach_format::VRS_b, { vec_reg_5_U, reg_4_U, db_12_4_U }, 1552);
    add_machine_instr(result, "VUPH", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1552);
    add_machine_instr(result, "VUPL", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1553);
    add_machine_instr(result, "VUPLH", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1553);
    add_machine_instr(result, "VUPLL", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1554);
    add_machine_instr(result, "VA", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1557);
    add_machine_instr(result, "VACC", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1558);
    add_machine_instr(
        result, "VAC", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1558);
    add_machine_instr(
        result, "VACCC", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1559);
    add_machine_instr(result, "VN", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1559);
    add_machine_instr(result, "VNC", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1559);
    add_machine_instr(result, "VAVG", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1560);
    add_machine_instr(result, "VAVGL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1560);
    add_machine_instr(result, "VCKSM", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1560);
    add_machine_instr(result, "VEC", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1561);
    add_machine_instr(result, "VECL", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1561);
    add_machine_instr(
        result, "VCEQ", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1561);
    add_machine_instr(
        result, "VCH", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1562);
    add_machine_instr(
        result, "VCHL", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1563);
    add_machine_instr(result, "VCLZ", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1564);
    add_machine_instr(result, "VCTZ", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1564);
    add_machine_instr(result, "VGFM", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1565);
    add_machine_instr(result, "VX", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1565);
    add_machine_instr(result, "VLC", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1566);
    add_machine_instr(
        result, "VGFMA", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1566);
    add_machine_instr(result, "VLP", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1566);
    add_machine_instr(result, "VMX", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1567);
    add_machine_instr(result, "VMXL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1567);
    add_machine_instr(result, "VMN", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1567);
    add_machine_instr(result, "VMNL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1568);
    add_machine_instr(
        result, "VMAL", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1568);
    add_machine_instr(
        result, "VMAH", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1569);
    add_machine_instr(
        result, "VMALH", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1569);
    add_machine_instr(
        result, "VMAE", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1569);
    add_machine_instr(
        result, "VMALE", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1569);
    add_machine_instr(
        result, "VMAO", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1570);
    add_machine_instr(
        result, "VMALO", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1570);
    add_machine_instr(result, "VMH", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1570);
    add_machine_instr(result, "VMLH", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1571);
    add_machine_instr(result, "VML", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1571);
    add_machine_instr(result, "VME", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1572);
    add_machine_instr(result, "VMLE", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1572);
    add_machine_instr(result, "VMO", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1572);
    add_machine_instr(result, "VMLO", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1572);
    add_machine_instr(result,
        "VMSL",
        mach_format::VRR_d,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1573);
    add_machine_instr(result, "VNN", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1574);
    add_machine_instr(result, "VNO", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1574);
    add_machine_instr(result, "VNX", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1574);
    add_machine_instr(result, "VO", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1574);
    add_machine_instr(result, "VOC", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1575);
    add_machine_instr(result, "VPOPCT", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1575);
    add_machine_instr(result, "VERLLV", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1575);
    add_machine_instr(result, "VERLL", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1575);
    add_machine_instr(
        result, "VERIM", mach_format::VRI_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1576);
    add_machine_instr(result, "VESLV", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1577);
    add_machine_instr(result, "VESL", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1577);
    add_machine_instr(result, "VESRAV", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1577);
    add_machine_instr(result, "VESRA", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1577);
    add_machine_instr(result, "VESRLV", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1578);
    add_machine_instr(result, "VESRL", mach_format::VRS_a, { vec_reg_5_U, vec_reg_5_U, db_12_4_U, mask_4_U }, 1578);
    add_machine_instr(result, "VSLD", mach_format::VRI_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U }, 1607);
    add_machine_instr(result, "VSRD", mach_format::VRI_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U }, 1608);
    add_machine_instr(result, "VSLDB", mach_format::VRI_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U }, 1579);
    add_machine_instr(result, "VSL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1579);
    add_machine_instr(result, "VSLB", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1579);
    add_machine_instr(result, "VSRA", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1579);
    add_machine_instr(result, "VSRAB", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1580);
    add_machine_instr(result, "VSRL", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1580);
    add_machine_instr(result, "VSRLB", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U }, 1580);
    add_machine_instr(result, "VS", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1580);
    add_machine_instr(result, "VSCBI", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1581);
    add_machine_instr(
        result, "VCSFP", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1644);
    add_machine_instr(
        result, "VSBI", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1581);
    add_machine_instr(
        result, "VSBCBI", mach_format::VRR_d, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1582);
    add_machine_instr(result, "VSUMG", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1582);
    add_machine_instr(result, "VSUMQ", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1583);
    add_machine_instr(result, "VSUM", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1583);
    add_machine_instr(result, "VTM", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U }, 1584);
    add_machine_instr(
        result, "VFAE", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1, 1585);
    add_machine_instr(
        result, "VFEE", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1, 1587);
    add_machine_instr(
        result, "VFENE", mach_format::VRR_b, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1, 1588);
    add_machine_instr(result, "VISTR", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1, 1589);
    add_machine_instr(result,
        "VSTRC",
        mach_format::VRR_d,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1,
        1590);
    add_machine_instr(result,
        "VSTRS",
        mach_format::VRR_d,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1,
        1622);
    add_machine_instr(
        result, "VFA", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1595);
    add_machine_instr(result, "WFC", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1599);
    add_machine_instr(result, "WFK", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1600);
    add_machine_instr(result,
        "VFCE",
        mach_format::VRR_c,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U },
        1601);
    add_machine_instr(result,
        "VFCH",
        mach_format::VRR_c,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U },
        1603);
    add_machine_instr(result,
        "VFCHE",
        mach_format::VRR_c,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U },
        1605);
    add_machine_instr(
        result, "VCFPS", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1607);
    add_machine_instr(
        result, "VCFPL", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1643);
    add_machine_instr(
        result, "VCLGD", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1611);
    add_machine_instr(
        result, "VFD", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1613);
    add_machine_instr(
        result, "VFI", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1615);
    add_machine_instr(result, "VFLL", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1617);
    add_machine_instr(
        result, "VFLR", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1618);
    add_machine_instr(result,
        "VFMAX",
        mach_format::VRR_c,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U },
        1619);
    add_machine_instr(result,
        "VFMIN",
        mach_format::VRR_c,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U },
        1625);
    add_machine_instr(
        result, "VFM", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1631);
    add_machine_instr(result,
        "VFMA",
        mach_format::VRR_e,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1633);
    add_machine_instr(result,
        "VFMS",
        mach_format::VRR_e,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1633);
    add_machine_instr(result,
        "VFNMA",
        mach_format::VRR_e,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1633);
    add_machine_instr(result,
        "VFNMS",
        mach_format::VRR_e,
        { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U },
        1633);
    add_machine_instr(
        result, "VFPSO", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U, mask_4_U }, 1635);
    add_machine_instr(result, "VFSQ", mach_format::VRR_a, { vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1636);
    add_machine_instr(
        result, "VFS", mach_format::VRR_c, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, mask_4_U, mask_4_U }, 1637);
    add_machine_instr(
        result, "VFTCI", mach_format::VRI_e, { vec_reg_5_U, vec_reg_5_U, imm_12_S, mask_4_U, mask_4_U }, 1638);
    add_machine_instr(
        result, "VAP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1643);
    add_machine_instr(result, "VCP", mach_format::VRR_h, { vec_reg_5_U, vec_reg_5_U, mask_4_U }, 1644);
    add_machine_instr(result, "VCVB", mach_format::VRR_i, { reg_4_U, vec_reg_5_U, mask_4_U }, 1645);
    add_machine_instr(result, "VCVBG", mach_format::VRR_i, { reg_4_U, vec_reg_5_U, mask_4_U }, 1645);
    add_machine_instr(result, "VCVD", mach_format::VRI_i, { vec_reg_5_U, reg_4_U, imm_8_S, mask_4_U }, 1646);
    add_machine_instr(result, "VCVDG", mach_format::VRI_i, { vec_reg_5_U, reg_4_U, imm_8_S, mask_4_U }, 1646);
    add_machine_instr(
        result, "VDP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1648);
    add_machine_instr(
        result, "VMP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1650);
    add_machine_instr(
        result, "VMSP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1651);
    add_machine_instr(
        result, "VRP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1654);
    add_machine_instr(
        result, "VSDP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1656);
    add_machine_instr(
        result, "VSP", mach_format::VRI_f, { vec_reg_5_U, vec_reg_5_U, vec_reg_5_U, imm_8_U, mask_4_U }, 1658);
    add_machine_instr(result, "VLIP", mach_format::VRI_h, { vec_reg_5_U, imm_16_S, imm_4_U }, 1649);
    add_machine_instr(result, "VPKZ", mach_format::VSI, { vec_reg_5_U, db_12_4_U, imm_8_U }, 1652);
    add_machine_instr(
        result, "VPSOP", mach_format::VRI_g, { vec_reg_5_U, vec_reg_5_U, imm_8_U, imm_8_U, mask_4_U }, 1653);
    add_machine_instr(
        result, "VSRP", mach_format::VRI_g, { vec_reg_5_U, vec_reg_5_U, imm_8_U, imm_8_S, mask_4_U }, 1657);
    add_machine_instr(result, "SIE", mach_format::S, { db_12_4_U }, 7);
    add_machine_instr(result, "VAD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VADS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VDDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMADS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMSDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VCDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VAS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VNS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VOS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VXS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VCS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMSD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMSE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMSES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLINT", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VDD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VDDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VDE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VDES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMXAD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMXAE", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMXSE", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VNVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VOVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLCVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLELE", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLELD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLI", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLID", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLBIX", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLVCU", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLVCA", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VLVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMNSD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMNSE", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMRRS", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VMRSV", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VACRS", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VACSV", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSTVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSTVP", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSVMM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VTVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VXELE", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VXELD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VXVC", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VXVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VXVMM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSTI", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSTID", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VRCL", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VRRS", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VRSV", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VRSVC", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSLL", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSPSD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VZPSD", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSRRS", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VSRSV", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VOVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VCVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VCZVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VCOVM", mach_format::RRE, { reg_4_U, reg_4_U }, 0);
    add_machine_instr(result, "VTP", mach_format::VRR_g, { vec_reg_5_U }, 1660);
    add_machine_instr(result, "VUPKZ", mach_format::VSI, { vec_reg_5_U, db_12_4_U, imm_8_U }, 1660);
    add_machine_instr(result, "VSTK", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSDS", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSTD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSTKD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSTMD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VSTH", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLH", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLMD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLY", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VLYD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VM", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMAD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMAES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMCD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMCE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VMES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VACD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VACE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VAE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VAES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VC", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VCD", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VCE", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);
    add_machine_instr(result, "VCES", mach_format::RI_a, { reg_4_U, imm_16_U }, 0);

    return result;
}

void add_mnemonic_code(std::map<std::string, mnemonic_code>& result, std::string instr, mnemonic_code code)
{
    assert(std::is_sorted(
        code.replaced.begin(), code.replaced.end(), [](const auto& l, const auto& r) { return l.first < r.first; }));
    result.insert(std::make_pair<std::string, mnemonic_code>(std::move(instr), std::move(code)));
}

std::map<std::string, mnemonic_code> hlasm_plugin::parser_library::context::instruction::get_mnemonic_codes(
    const std::map<std::string, machine_instruction>& i)
{
    std::map<std::string, mnemonic_code> result;
    add_mnemonic_code(result, "B", { &i.at("BC"), { { 0, 15 } } });
    add_mnemonic_code(result, "BR", { &i.at("BCR"), { { 0, 15 } } });
    add_mnemonic_code(result, "J", { &i.at("BRC"), { { 0, 15 } } });
    add_mnemonic_code(result, "NOP", { &i.at("BC"), { { 0, 0 } } });
    add_mnemonic_code(result, "NOPR", { &i.at("BCR"), { { 0, 0 } } });
    add_mnemonic_code(result, "JNOP", { &i.at("BRC"), { { 0, 0 } } });
    add_mnemonic_code(result, "BH", { &i.at("BC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BHR", { &i.at("BCR"), { { 0, 2 } } });
    add_mnemonic_code(result, "JH", { &i.at("BRC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BL", { &i.at("BC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BLR", { &i.at("BCR"), { { 0, 4 } } });
    add_mnemonic_code(result, "JL", { &i.at("BRC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BE", { &i.at("BC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BER", { &i.at("BCR"), { { 0, 8 } } });
    add_mnemonic_code(result, "JE", { &i.at("BRC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BNH", { &i.at("BC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BNHR", { &i.at("BCR"), { { 0, 13 } } });
    add_mnemonic_code(result, "JNH", { &i.at("BRC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BNL", { &i.at("BC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BNLR", { &i.at("BCR"), { { 0, 11 } } });
    add_mnemonic_code(result, "JNL", { &i.at("BRC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BNE", { &i.at("BC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BNER", { &i.at("BCR"), { { 0, 7 } } });
    add_mnemonic_code(result, "JNE", { &i.at("BRC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BP", { &i.at("BC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BPR", { &i.at("BCR"), { { 0, 2 } } });
    add_mnemonic_code(result, "JP", { &i.at("BRC"), { { 0, 2 } } });
    add_mnemonic_code(result, "JM", { &i.at("BRC"), { { 0, 4 } } });
    add_mnemonic_code(result, "JZ", { &i.at("BRC"), { { 0, 8 } } });
    add_mnemonic_code(result, "JO", { &i.at("BRC"), { { 0, 1 } } });
    add_mnemonic_code(result, "BNP", { &i.at("BC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BNPR", { &i.at("BCR"), { { 0, 13 } } });
    add_mnemonic_code(result, "JNP", { &i.at("BRC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BNM", { &i.at("BC"), { { 0, 11 } } });
    add_mnemonic_code(result, "JNM", { &i.at("BRC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BNZ", { &i.at("BC"), { { 0, 7 } } });
    add_mnemonic_code(result, "JNZ", { &i.at("BRC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BNO", { &i.at("BC"), { { 0, 14 } } });
    add_mnemonic_code(result, "JNO", { &i.at("BRC"), { { 0, 14 } } });
    add_mnemonic_code(result, "XHLR", { &i.at("RXSBG"), { { 2, 0 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "XHHR", { &i.at("RXSBG"), { { 2, 0 }, { 3, 31 } } });
    add_mnemonic_code(result, "XLHR", { &i.at("RXSBG"), { { 2, 32 }, { 3, 63 }, { 4, 32 } } });
    add_mnemonic_code(result, "OHLR", { &i.at("ROSBG"), { { 2, 0 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "OHHR", { &i.at("ROSBG"), { { 2, 0 }, { 3, 31 } } });
    add_mnemonic_code(result, "OLHR", { &i.at("ROSBG"), { { 2, 32 }, { 3, 63 }, { 4, 32 } } });
    add_mnemonic_code(result, "NHLR", { &i.at("RNSBG"), { { 2, 0 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "NHHR", { &i.at("RNSBG"), { { 2, 0 }, { 3, 31 } } });
    add_mnemonic_code(result, "NLHR", { &i.at("RNSBG"), { { 2, 32 }, { 3, 63 }, { 4, 32 } } });
    add_mnemonic_code(result, "BO", { &i.at("BC"), { { 0, 1 } } });
    add_mnemonic_code(result, "BOR", { &i.at("BCR"), { { 0, 1 } } });
    add_mnemonic_code(result, "BM", { &i.at("BC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BMR", { &i.at("BCR"), { { 0, 4 } } });
    add_mnemonic_code(result, "BZ", { &i.at("BC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BZR", { &i.at("BCR"), { { 0, 8 } } });
    add_mnemonic_code(result, "BNOR", { &i.at("BCR"), { { 0, 14 } } });
    add_mnemonic_code(result, "BNMR", { &i.at("BCR"), { { 0, 11 } } });
    add_mnemonic_code(result, "BNZR", { &i.at("BCR"), { { 0, 7 } } });
    add_mnemonic_code(result, "BRUL", { &i.at("BRCL"), { { 0, 15 } } });
    add_mnemonic_code(result, "BRHL", { &i.at("BRCL"), { { 0, 2 } } });
    add_mnemonic_code(result, "BRLL", { &i.at("BRCL"), { { 0, 4 } } });
    add_mnemonic_code(result, "BREL", { &i.at("BRCL"), { { 0, 8 } } });
    add_mnemonic_code(result, "BRNHL", { &i.at("BRCL"), { { 0, 13 } } });
    add_mnemonic_code(result, "BRNLL", { &i.at("BRCL"), { { 0, 11 } } });
    add_mnemonic_code(result, "BRNEL", { &i.at("BRCL"), { { 0, 7 } } });
    add_mnemonic_code(result, "BRPL", { &i.at("BRCL"), { { 0, 2 } } });
    add_mnemonic_code(result, "BRML", { &i.at("BRCL"), { { 0, 4 } } });
    add_mnemonic_code(result, "BRZL", { &i.at("BRCL"), { { 0, 8 } } });
    add_mnemonic_code(result, "BROL", { &i.at("BRCL"), { { 0, 1 } } });
    add_mnemonic_code(result, "BRNPL", { &i.at("BRCL"), { { 0, 13 } } });
    add_mnemonic_code(result, "BRNML", { &i.at("BRCL"), { { 0, 11 } } });
    add_mnemonic_code(result, "BRNZL", { &i.at("BRCL"), { { 0, 7 } } });
    add_mnemonic_code(result, "BRNOL", { &i.at("BRCL"), { { 0, 14 } } });
    add_mnemonic_code(result, "BRO", { &i.at("BRC"), { { 0, 1 } } });
    add_mnemonic_code(result, "BRP", { &i.at("BRC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BRH", { &i.at("BRC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BRL", { &i.at("BRC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BRM", { &i.at("BRC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BRNE", { &i.at("BRC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BRNZ", { &i.at("BRC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BRE", { &i.at("BRC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BRZ", { &i.at("BRC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BRNL", { &i.at("BRC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BRNM", { &i.at("BRC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BRNH", { &i.at("BRC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BRNP", { &i.at("BRC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BRNO", { &i.at("BRC"), { { 0, 14 } } });
    add_mnemonic_code(result, "BRU", { &i.at("BRC"), { { 0, 15 } } });
    add_mnemonic_code(result, "JLU", { &i.at("BRCL"), { { 0, 15 } } });
    add_mnemonic_code(result, "JLNOP", { &i.at("BRCL"), { { 0, 0 } } });
    add_mnemonic_code(result, "JLH", { &i.at("BRCL"), { { 0, 2 } } });
    add_mnemonic_code(result, "JLL", { &i.at("BRCL"), { { 0, 4 } } });
    add_mnemonic_code(result, "JLE", { &i.at("BRCL"), { { 0, 8 } } });
    add_mnemonic_code(result, "JLNH", { &i.at("BRCL"), { { 0, 13 } } });
    add_mnemonic_code(result, "JLNL", { &i.at("BRCL"), { { 0, 11 } } });
    add_mnemonic_code(result, "JLNE", { &i.at("BRCL"), { { 0, 7 } } });
    add_mnemonic_code(result, "JLP", { &i.at("BRCL"), { { 0, 2 } } });
    add_mnemonic_code(result, "JLM", { &i.at("BRCL"), { { 0, 4 } } });
    add_mnemonic_code(result, "JLZ", { &i.at("BRCL"), { { 0, 8 } } });
    add_mnemonic_code(result, "JLO", { &i.at("BRCL"), { { 0, 1 } } });
    add_mnemonic_code(result, "JLNP", { &i.at("BRCL"), { { 0, 13 } } });
    add_mnemonic_code(result, "JLNM", { &i.at("BRCL"), { { 0, 11 } } });
    add_mnemonic_code(result, "JLNZ", { &i.at("BRCL"), { { 0, 7 } } });
    add_mnemonic_code(result, "JLNO", { &i.at("BRCL"), { { 0, 14 } } });
    add_mnemonic_code(result, "JAS", { &i.at("BRAS"), {} });
    add_mnemonic_code(result, "JASL", { &i.at("BRASL"), {} });
    add_mnemonic_code(result, "JC", { &i.at("BRC"), {} });
    add_mnemonic_code(result, "JCT", { &i.at("BRCT"), {} });
    add_mnemonic_code(result, "JCTG", { &i.at("BRCTG"), {} });
    add_mnemonic_code(result, "JXH", { &i.at("BRXH"), {} });
    add_mnemonic_code(result, "JXHG", { &i.at("BRXHG"), {} });
    add_mnemonic_code(result, "JXLE", { &i.at("BRXLE"), {} });
    add_mnemonic_code(result, "JXLEG", { &i.at("BRXLG"), {} });
    add_mnemonic_code(result, "VCDG", { &i.at("VCFPS"), {} });
    add_mnemonic_code(result, "VCGD", { &i.at("VCSFP"), {} });
    add_mnemonic_code(result, "BIO", { &i.at("BIC"), { { 0, 1 } } });
    add_mnemonic_code(result, "BIP", { &i.at("BIC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BIH", { &i.at("BIC"), { { 0, 2 } } });
    add_mnemonic_code(result, "BIM", { &i.at("BIC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BIL", { &i.at("BIC"), { { 0, 4 } } });
    add_mnemonic_code(result, "BINZ", { &i.at("BIC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BINE", { &i.at("BIC"), { { 0, 7 } } });
    add_mnemonic_code(result, "BIZ", { &i.at("BIC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BIE", { &i.at("BIC"), { { 0, 8 } } });
    add_mnemonic_code(result, "BINM", { &i.at("BIC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BINL", { &i.at("BIC"), { { 0, 11 } } });
    add_mnemonic_code(result, "BINP", { &i.at("BIC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BINH", { &i.at("BIC"), { { 0, 13 } } });
    add_mnemonic_code(result, "BINO", { &i.at("BIC"), { { 0, 14 } } });
    add_mnemonic_code(result, "BI", { &i.at("BIC"), { { 0, 15 } } });
    add_mnemonic_code(result, "VSTBRH", { &i.at("VSTBR"), { { 2, 1 } } });
    add_mnemonic_code(result, "VSTBRF", { &i.at("VSTBR"), { { 2, 2 } } });
    add_mnemonic_code(result, "VSTBRG", { &i.at("VSTBR"), { { 2, 3 } } });
    add_mnemonic_code(result, "VSTBRQ", { &i.at("VSTBR"), { { 2, 4 } } });
    add_mnemonic_code(result, "VSTERH", { &i.at("VSTER"), { { 2, 1 } } });
    add_mnemonic_code(result, "VSTERF", { &i.at("VSTER"), { { 2, 2 } } });
    add_mnemonic_code(result, "VSTERG", { &i.at("VSTER"), { { 2, 3 } } });
    add_mnemonic_code(result, "STERV", { &i.at("VSTEBRF"), { { 2, 0 } } });
    add_mnemonic_code(result, "STDRV", { &i.at("VSTEBRG"), { { 2, 0 } } });
    add_mnemonic_code(result, "SELFHRE", { &i.at("SELFHR"), { { 3, 8 } } });
    add_mnemonic_code(result, "SELFHRH", { &i.at("SELFHR"), { { 3, 2 } } });
    add_mnemonic_code(result, "SELFHRL", { &i.at("SELFHR"), { { 3, 4 } } });
    add_mnemonic_code(result, "SELFHRNE", { &i.at("SELFHR"), { { 3, 7 } } });
    add_mnemonic_code(result, "SELFHRNH", { &i.at("SELFHR"), { { 3, 13 } } });
    add_mnemonic_code(result, "SELFHRNL", { &i.at("SELFHR"), { { 3, 11 } } });
    add_mnemonic_code(result, "SELFHRNO", { &i.at("SELFHR"), { { 3, 14 } } });
    add_mnemonic_code(result, "SELFHRO", { &i.at("SELFHR"), { { 3, 1 } } });
    add_mnemonic_code(result, "LHHR", { &i.at("RISBHGZ"), { { 2, 0 }, { 3, 31 } } });
    add_mnemonic_code(result, "LHLR", { &i.at("RISBHGZ"), { { 2, 0 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LLHHHR", { &i.at("RISBHGZ"), { { 2, 16 }, { 3, 31 } } });
    add_mnemonic_code(result, "LLHHLR", { &i.at("RISBHGZ"), { { 2, 16 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LLCHHR", { &i.at("RISBHGZ"), { { 2, 24 }, { 3, 31 } } });
    add_mnemonic_code(result, "LLCHLR", { &i.at("RISBHGZ"), { { 2, 24 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LLHFR", { &i.at("RISBLGZ"), { { 2, 0 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LLHLHR", { &i.at("RISBLGZ"), { { 2, 16 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LLCLHR", { &i.at("RISBLGZ"), { { 2, 24 }, { 3, 31 }, { 4, 32 } } });
    add_mnemonic_code(result, "LOCO", { &i.at("LOC"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCNO", { &i.at("LOC"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCGO", { &i.at("LOCG"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCGNO", { &i.at("LOCG"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCGHIH", { &i.at("LOCGHI"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCGHIL", { &i.at("LOCGHI"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCGHIE", { &i.at("LOCGHI"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCGHINE", { &i.at("LOCGHI"), { { 2, 7 } } });
    add_mnemonic_code(result, "LOCGHINL", { &i.at("LOCGHI"), { { 2, 11 } } });
    add_mnemonic_code(result, "LOCGHINH", { &i.at("LOCGHI"), { { 2, 13 } } });
    add_mnemonic_code(result, "LOCGHINO", { &i.at("LOCGHI"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCGHIO", { &i.at("LOCGHI"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCGRO", { &i.at("LOCGR"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCGRNO", { &i.at("LOCGR"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCHHIE", { &i.at("LOCHHI"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCHHIH", { &i.at("LOCHHI"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCHHIL", { &i.at("LOCHHI"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCHHINE", { &i.at("LOCHHI"), { { 2, 7 } } });
    add_mnemonic_code(result, "LOCHHINH", { &i.at("LOCHHI"), { { 2, 13 } } });
    add_mnemonic_code(result, "LOCHHINL", { &i.at("LOCHHI"), { { 2, 11 } } });
    add_mnemonic_code(result, "LOCHHINO", { &i.at("LOCHHI"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCHHIO", { &i.at("LOCHHI"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCHIE", { &i.at("LOCHI"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCHIH", { &i.at("LOCHI"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCHIL", { &i.at("LOCHI"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCHINE", { &i.at("LOCHI"), { { 2, 7 } } });
    add_mnemonic_code(result, "LOCHINH", { &i.at("LOCHI"), { { 2, 13 } } });
    add_mnemonic_code(result, "LOCHINL", { &i.at("LOCHI"), { { 2, 11 } } });
    add_mnemonic_code(result, "LOCHINO", { &i.at("LOCHI"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCHIO", { &i.at("LOCHI"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCRNO", { &i.at("LOCR"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCRO", { &i.at("LOCR"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCFHE", { &i.at("LOCFH"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCFHH", { &i.at("LOCFH"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCFHL", { &i.at("LOCFH"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCFHNE", { &i.at("LOCFH"), { { 2, 7 } } });
    add_mnemonic_code(result, "LOCFHNH", { &i.at("LOCFH"), { { 2, 13 } } });
    add_mnemonic_code(result, "LOCFHNL", { &i.at("LOCFH"), { { 2, 11 } } });
    add_mnemonic_code(result, "LOCFHNO", { &i.at("LOCFH"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCFHO", { &i.at("LOCFH"), { { 2, 1 } } });
    add_mnemonic_code(result, "LOCFHRH", { &i.at("LOCFHR"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCFHRL", { &i.at("LOCFHR"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCFHRE", { &i.at("LOCFHR"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCFHRNE", { &i.at("LOCFHR"), { { 2, 7 } } });
    add_mnemonic_code(result, "LOCFHRNH", { &i.at("LOCFHR"), { { 2, 13 } } });
    add_mnemonic_code(result, "LOCFHRNL", { &i.at("LOCFHR"), { { 2, 11 } } });
    add_mnemonic_code(result, "LOCFHRNO", { &i.at("LOCFHR"), { { 2, 14 } } });
    add_mnemonic_code(result, "LOCFHRO", { &i.at("LOCFHR"), { { 2, 1 } } });
    add_mnemonic_code(result, "STOCFHE", { &i.at("STOCFH"), { { 2, 8 } } });
    add_mnemonic_code(result, "STOCFHH", { &i.at("STOCFH"), { { 2, 2 } } });
    add_mnemonic_code(result, "STOCFHL", { &i.at("STOCFH"), { { 2, 4 } } });
    add_mnemonic_code(result, "STOCFHNE", { &i.at("STOCFH"), { { 2, 7 } } });
    add_mnemonic_code(result, "STOCFHNH", { &i.at("STOCFH"), { { 2, 13 } } });
    add_mnemonic_code(result, "STOCFHNL", { &i.at("STOCFH"), { { 2, 11 } } });
    add_mnemonic_code(result, "STOCFHNO", { &i.at("STOCFH"), { { 2, 14 } } });
    add_mnemonic_code(result, "STOCFHO", { &i.at("STOCFH"), { { 2, 1 } } });
    add_mnemonic_code(result, "STOCGNO", { &i.at("STOCG"), { { 2, 14 } } });
    add_mnemonic_code(result, "STOCGO", { &i.at("STOCG"), { { 2, 1 } } });
    add_mnemonic_code(result, "STOCNO", { &i.at("STOC"), { { 2, 14 } } });
    add_mnemonic_code(result, "STOCO", { &i.at("STOC"), { { 2, 1 } } });
    add_mnemonic_code(result, "SELGRE", { &i.at("SELGR"), { { 3, 8 } } });
    add_mnemonic_code(result, "SELGRH", { &i.at("SELGR"), { { 3, 2 } } });
    add_mnemonic_code(result, "SELGRL", { &i.at("SELGR"), { { 3, 4 } } });
    add_mnemonic_code(result, "SELGRNE", { &i.at("SELGR"), { { 3, 7 } } });
    add_mnemonic_code(result, "SELGRNH", { &i.at("SELGR"), { { 3, 13 } } });
    add_mnemonic_code(result, "SELGRNL", { &i.at("SELGR"), { { 3, 11 } } });
    add_mnemonic_code(result, "SELGRNO", { &i.at("SELGR"), { { 3, 14 } } });
    add_mnemonic_code(result, "SELGRO", { &i.at("SELGR"), { { 3, 1 } } });
    add_mnemonic_code(result, "SELRE", { &i.at("SELR"), { { 3, 8 } } });
    add_mnemonic_code(result, "SELRH", { &i.at("SELR"), { { 3, 2 } } });
    add_mnemonic_code(result, "SELRL", { &i.at("SELR"), { { 3, 4 } } });
    add_mnemonic_code(result, "SELRNE", { &i.at("SELR"), { { 3, 7 } } });
    add_mnemonic_code(result, "SELRNH", { &i.at("SELR"), { { 3, 13 } } });
    add_mnemonic_code(result, "SELRNL", { &i.at("SELR"), { { 3, 11 } } });
    add_mnemonic_code(result, "SELRNO", { &i.at("SELR"), { { 3, 14 } } });
    add_mnemonic_code(result, "SELRO", { &i.at("SELR"), { { 3, 1 } } });
    add_mnemonic_code(result, "VZERO", { &i.at("VGBM"), { { 0, 1 } } });
    add_mnemonic_code(result, "VONE", { &i.at("VGBM"), { { 1, 65535 } } });
    add_mnemonic_code(result, "VGMB", { &i.at("VGM"), { { 3, 0 } } });
    add_mnemonic_code(result, "VGMH", { &i.at("VGM"), { { 3, 1 } } });
    add_mnemonic_code(result, "VGMF", { &i.at("VGM"), { { 3, 2 } } });
    add_mnemonic_code(result, "VGMG", { &i.at("VGM"), { { 3, 3 } } });
    add_mnemonic_code(result, "VLREPB", { &i.at("VLREP"), { { 2, 0 } } });
    add_mnemonic_code(result, "VLREPH", { &i.at("VLREP"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLREPF", { &i.at("VLREP"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLREPG", { &i.at("VLREP"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLGVB", { &i.at("VLGV"), { { 3, 0 } } });
    add_mnemonic_code(result, "VLGVH", { &i.at("VLGV"), { { 3, 1 } } });
    add_mnemonic_code(result, "VLGVF", { &i.at("VLGV"), { { 3, 2 } } });
    add_mnemonic_code(result, "VLGVG", { &i.at("VLGV"), { { 3, 3 } } });
    add_mnemonic_code(result, "VLLEZB", { &i.at("VLLEZ"), { { 2, 0 } } });
    add_mnemonic_code(result, "VLLEZH", { &i.at("VLLEZ"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLLEZF", { &i.at("VLLEZ"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLLEZG", { &i.at("VLLEZ"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLLEZLF", { &i.at("VLLEZ"), { { 2, 6 } } });
    add_mnemonic_code(result, "VLLEBRZE", { &i.at("VLLEBRZ"), { { 2, 6 } } });
    add_mnemonic_code(result, "VLLEBRZG", { &i.at("VLLEBRZ"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLLEBRZF", { &i.at("VLLEBRZ"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLLEBRZH", { &i.at("VLLEBRZ"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLVGB", { &i.at("VLVG"), { { 3, 0 } } });
    add_mnemonic_code(result, "VLVGH", { &i.at("VLVG"), { { 3, 1 } } });
    add_mnemonic_code(result, "VLVGF", { &i.at("VLVG"), { { 3, 2 } } });
    add_mnemonic_code(result, "VLVGG", { &i.at("VLVG"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMRHB", { &i.at("VMRH"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMRHH", { &i.at("VMRH"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMRHF", { &i.at("VMRH"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMRHG", { &i.at("VMRH"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMRLB", { &i.at("VMRL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMRLH", { &i.at("VMRL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMRLF", { &i.at("VMRL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMRLG", { &i.at("VMRL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VPKSH", { &i.at("VPKS"), { { 3, 1 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKSF", { &i.at("VPKS"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKSG", { &i.at("VPKS"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKSHS", { &i.at("VPKS"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VPKSFS", { &i.at("VPKS"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VPKSGS", { &i.at("VPKS"), { { 3, 3 }, { 4, 1 } } });
    add_mnemonic_code(result, "VPKLSH", { &i.at("VPKLS"), { { 3, 1 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKLSF", { &i.at("VPKLS"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKLSG", { &i.at("VPKLS"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "VPKLSHS", { &i.at("VPKLS"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VPKLSFS", { &i.at("VPKLS"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VPKLSGS", { &i.at("VPKLS"), { { 3, 3 }, { 4, 1 } } });
    add_mnemonic_code(result, "VREPB", { &i.at("VREP"), { { 3, 0 } } });
    add_mnemonic_code(result, "VREPH", { &i.at("VREP"), { { 3, 1 } } });
    add_mnemonic_code(result, "VREPF", { &i.at("VREP"), { { 3, 2 } } });
    add_mnemonic_code(result, "VREPG", { &i.at("VREP"), { { 3, 3 } } });
    add_mnemonic_code(result, "VREPIB", { &i.at("VREPI"), { { 2, 0 } } });
    add_mnemonic_code(result, "VREPIH", { &i.at("VREPI"), { { 2, 1 } } });
    add_mnemonic_code(result, "VREPIF", { &i.at("VREPI"), { { 2, 2 } } });
    add_mnemonic_code(result, "VREPIG", { &i.at("VREPI"), { { 2, 3 } } });
    add_mnemonic_code(result, "VSEGB", { &i.at("VSEG"), { { 2, 0 } } });
    add_mnemonic_code(result, "VSEGH", { &i.at("VSEG"), { { 2, 1 } } });
    add_mnemonic_code(result, "VSEGF", { &i.at("VSEG"), { { 2, 2 } } });
    add_mnemonic_code(result, "VUPHB", { &i.at("VUPH"), { { 2, 0 } } });
    add_mnemonic_code(result, "VUPHH", { &i.at("VUPH"), { { 2, 1 } } });
    add_mnemonic_code(result, "VUPHF", { &i.at("VUPH"), { { 2, 2 } } });
    add_mnemonic_code(result, "VUPLHB", { &i.at("VUPLH"), { { 2, 0 } } });
    add_mnemonic_code(result, "VUPLHG", { &i.at("VUPLH"), { { 2, 1 } } });
    add_mnemonic_code(result, "VUPLHF", { &i.at("VUPLH"), { { 2, 2 } } });
    add_mnemonic_code(result, "VUPLB", { &i.at("VUPL"), { { 2, 0 } } });
    add_mnemonic_code(result, "VUPLHW", { &i.at("VUPL"), { { 2, 1 } } });
    add_mnemonic_code(result, "VUPLF", { &i.at("VUPL"), { { 2, 2 } } });
    add_mnemonic_code(result, "VUPLLB", { &i.at("VUPLL"), { { 2, 0 } } });
    add_mnemonic_code(result, "VUPLLH", { &i.at("VUPLL"), { { 2, 1 } } });
    add_mnemonic_code(result, "VUPLLF", { &i.at("VUPLL"), { { 2, 2 } } });
    add_mnemonic_code(result, "VAB", { &i.at("VA"), { { 3, 0 } } });
    add_mnemonic_code(result, "VAH", { &i.at("VA"), { { 3, 1 } } });
    add_mnemonic_code(result, "VAF", { &i.at("VA"), { { 3, 2 } } });
    add_mnemonic_code(result, "VAG", { &i.at("VA"), { { 3, 3 } } });
    add_mnemonic_code(result, "VAQ", { &i.at("VA"), { { 3, 4 } } });
    add_mnemonic_code(result, "VACCB", { &i.at("VACC"), { { 3, 0 } } });
    add_mnemonic_code(result, "VACCH", { &i.at("VACC"), { { 3, 1 } } });
    add_mnemonic_code(result, "VACCF", { &i.at("VACC"), { { 3, 2 } } });
    add_mnemonic_code(result, "VACCG", { &i.at("VACC"), { { 3, 3 } } });
    add_mnemonic_code(result, "VACCQ", { &i.at("VACC"), { { 3, 4 } } });
    add_mnemonic_code(result, "VACQ", { &i.at("VAC"), { { 3, 4 } } });
    add_mnemonic_code(result, "VACCCQ", { &i.at("VACCC"), { { 3, 4 } } });
    add_mnemonic_code(result, "VAVGB", { &i.at("VAVG"), { { 3, 0 } } });
    add_mnemonic_code(result, "VAVGH", { &i.at("VAVG"), { { 3, 1 } } });
    add_mnemonic_code(result, "VAVGF", { &i.at("VAVG"), { { 3, 2 } } });
    add_mnemonic_code(result, "VAVGG", { &i.at("VAVG"), { { 3, 3 } } });
    add_mnemonic_code(result, "VAVGLB", { &i.at("VAVGL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VAVGLH", { &i.at("VAVGL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VAVGLF", { &i.at("VAVGL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VAVGLG", { &i.at("VAVGL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VECB", { &i.at("VEC"), { { 2, 0 } } });
    add_mnemonic_code(result, "VECH", { &i.at("VEC"), { { 2, 1 } } });
    add_mnemonic_code(result, "VECF", { &i.at("VEC"), { { 2, 2 } } });
    add_mnemonic_code(result, "VECG", { &i.at("VEC"), { { 2, 3 } } });
    add_mnemonic_code(result, "VECLB", { &i.at("VECL"), { { 2, 0 } } });
    add_mnemonic_code(result, "VECLH", { &i.at("VECL"), { { 2, 1 } } });
    add_mnemonic_code(result, "VECLF", { &i.at("VECL"), { { 2, 2 } } });
    add_mnemonic_code(result, "VECLG", { &i.at("VECL"), { { 2, 3 } } });
    add_mnemonic_code(result, "VCEQB", { &i.at("VCEQ"), { { 3, 0 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCEQH", { &i.at("VCEQ"), { { 3, 1 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCEQF", { &i.at("VCEQ"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCEQG", { &i.at("VCEQ"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCEQBS", { &i.at("VCEQ"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCEQHS", { &i.at("VCEQ"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCEQFS", { &i.at("VCEQ"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCEQGS", { &i.at("VCEQ"), { { 3, 3 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHB", { &i.at("VCH"), { { 3, 0 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHH", { &i.at("VCH"), { { 3, 1 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHF", { &i.at("VCH"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHG", { &i.at("VCH"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHBS", { &i.at("VCH"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHHS", { &i.at("VCH"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHFS", { &i.at("VCH"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHGS", { &i.at("VCH"), { { 3, 3 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHLB", { &i.at("VCHL"), { { 3, 0 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHLH", { &i.at("VCHL"), { { 3, 1 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHLF", { &i.at("VCHL"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHLG", { &i.at("VCHL"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCHLBS", { &i.at("VCHL"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHLHS", { &i.at("VCHL"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHLFS", { &i.at("VCHL"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCHLGS", { &i.at("VCHL"), { { 3, 3 }, { 4, 1 } } });
    add_mnemonic_code(result, "VCLZB", { &i.at("VCLZ"), { { 2, 0 } } });
    add_mnemonic_code(result, "VCLZH", { &i.at("VCLZ"), { { 2, 1 } } });
    add_mnemonic_code(result, "VCLZF", { &i.at("VCLZ"), { { 2, 2 } } });
    add_mnemonic_code(result, "VCLZG", { &i.at("VCLZ"), { { 2, 3 } } });
    add_mnemonic_code(result, "VGFMB", { &i.at("VGFM"), { { 3, 0 } } });
    add_mnemonic_code(result, "VGFMH", { &i.at("VGFM"), { { 3, 1 } } });
    add_mnemonic_code(result, "VGFMF", { &i.at("VGFM"), { { 3, 2 } } });
    add_mnemonic_code(result, "VGFMG", { &i.at("VGFM"), { { 3, 3 } } });
    add_mnemonic_code(result, "VGFMAB", { &i.at("VGFMA"), { { 4, 0 } } });
    add_mnemonic_code(result, "VGFMAH", { &i.at("VGFMA"), { { 4, 1 } } });
    add_mnemonic_code(result, "VGFMAF", { &i.at("VGFMA"), { { 4, 2 } } });
    add_mnemonic_code(result, "VGFMAG", { &i.at("VGFMA"), { { 4, 3 } } });
    add_mnemonic_code(result, "VLCB", { &i.at("VLC"), { { 2, 0 } } });
    add_mnemonic_code(result, "VLCH", { &i.at("VLC"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLCF", { &i.at("VLC"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLCG", { &i.at("VLC"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLPB", { &i.at("VLP"), { { 2, 0 } } });
    add_mnemonic_code(result, "VLPH", { &i.at("VLP"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLPF", { &i.at("VLP"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLPG", { &i.at("VLP"), { { 2, 3 } } });
    add_mnemonic_code(result, "VMXB", { &i.at("VMX"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMXH", { &i.at("VMX"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMXF", { &i.at("VMX"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMXG", { &i.at("VMX"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMXLB", { &i.at("VMXL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMXLH", { &i.at("VMXL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMXLF", { &i.at("VMXL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMXLG", { &i.at("VMXL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMNB", { &i.at("VMN"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMNH", { &i.at("VMN"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMNF", { &i.at("VMN"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMNG", { &i.at("VMN"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMNLB", { &i.at("VMNL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMNLH", { &i.at("VMNL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMNLF", { &i.at("VMNL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMNLG", { &i.at("VMNL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VMALB", { &i.at("VMAL"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMALHW", { &i.at("VMAL"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMALF", { &i.at("VMAL"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMAHB", { &i.at("VMAH"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMAHH", { &i.at("VMAH"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMAHF", { &i.at("VMAH"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMALHB", { &i.at("VMALH"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMALHH", { &i.at("VMALH"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMALHF", { &i.at("VMALH"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMAEB", { &i.at("VMAE"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMAEH", { &i.at("VMAE"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMAEF", { &i.at("VMAE"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMALEB", { &i.at("VMALE"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMALEH", { &i.at("VMALE"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMALEF", { &i.at("VMALE"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMAOB", { &i.at("VMAO"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMAOH", { &i.at("VMAO"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMAOF", { &i.at("VMAO"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMALOB", { &i.at("VMALO"), { { 4, 0 } } });
    add_mnemonic_code(result, "VMALOH", { &i.at("VMALO"), { { 4, 1 } } });
    add_mnemonic_code(result, "VMALOF", { &i.at("VMALO"), { { 4, 2 } } });
    add_mnemonic_code(result, "VMHB", { &i.at("VMH"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMHH", { &i.at("VMH"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMHF", { &i.at("VMH"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMLHB", { &i.at("VMLH"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMLHH", { &i.at("VMLH"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMLHF", { &i.at("VMLH"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMLB", { &i.at("VML"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMLHW", { &i.at("VML"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMLF", { &i.at("VML"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMEB", { &i.at("VME"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMEH", { &i.at("VME"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMEF", { &i.at("VME"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMLEB", { &i.at("VMLE"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMLEH", { &i.at("VMLE"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMLEF", { &i.at("VMLE"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMSLG", { &i.at("VMSL"), { { 4, 3 } } });
    add_mnemonic_code(result, "VMOB", { &i.at("VMO"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMOH", { &i.at("VMO"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMOF", { &i.at("VMO"), { { 3, 2 } } });
    add_mnemonic_code(result, "VMLOB", { &i.at("VMLO"), { { 3, 0 } } });
    add_mnemonic_code(result, "VMLOH", { &i.at("VMLO"), { { 3, 1 } } });
    add_mnemonic_code(result, "VMLOF", { &i.at("VMLO"), { { 3, 2 } } });
    add_mnemonic_code(result, "VPOPCTB", { &i.at("VPOPCT"), { { 2, 0 } } });
    add_mnemonic_code(result, "VPOPCTH", { &i.at("VPOPCT"), { { 2, 1 } } });
    add_mnemonic_code(result, "VPOPCTF", { &i.at("VPOPCT"), { { 2, 2 } } });
    add_mnemonic_code(result, "VPOPCTG", { &i.at("VPOPCT"), { { 2, 3 } } });
    add_mnemonic_code(result, "VERLLVB", { &i.at("VERLLV"), { { 3, 0 } } });
    add_mnemonic_code(result, "VERLLVH", { &i.at("VERLLV"), { { 3, 1 } } });
    add_mnemonic_code(result, "VERLLVF", { &i.at("VERLLV"), { { 3, 2 } } });
    add_mnemonic_code(result, "VERLLVG", { &i.at("VERLLV"), { { 3, 3 } } });
    add_mnemonic_code(result, "VERLLB", { &i.at("VERLL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VERLLH", { &i.at("VERLL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VERLLF", { &i.at("VERLL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VERLLG", { &i.at("VERLL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VERIMB", { &i.at("VERIM"), { { 4, 0 } } });
    add_mnemonic_code(result, "VERIMH", { &i.at("VERIM"), { { 4, 1 } } });
    add_mnemonic_code(result, "VERIMF", { &i.at("VERIM"), { { 4, 2 } } });
    add_mnemonic_code(result, "VERIMG", { &i.at("VERIM"), { { 4, 3 } } });
    add_mnemonic_code(result, "VESLVB", { &i.at("VESLV"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESLVH", { &i.at("VESLV"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESLVF", { &i.at("VESLV"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESLVG", { &i.at("VESLV"), { { 3, 3 } } });
    add_mnemonic_code(result, "VESLB", { &i.at("VESL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESLH", { &i.at("VESL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESLF", { &i.at("VESL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESLG", { &i.at("VESL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VESRAVB", { &i.at("VESRAV"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESRAVH", { &i.at("VESRAV"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESRAVF", { &i.at("VESRAV"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESRAVG", { &i.at("VESRAV"), { { 3, 3 } } });
    add_mnemonic_code(result, "VESRAB", { &i.at("VESRA"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESRAH", { &i.at("VESRA"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESRAF", { &i.at("VESRA"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESRAG", { &i.at("VESRA"), { { 3, 3 } } });
    add_mnemonic_code(result, "VESRLVB", { &i.at("VESRLV"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESRLVH", { &i.at("VESRLV"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESRLVF", { &i.at("VESRLV"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESRLVG", { &i.at("VESRLV"), { { 3, 3 } } });
    add_mnemonic_code(result, "VESRLB", { &i.at("VESRL"), { { 3, 0 } } });
    add_mnemonic_code(result, "VESRLH", { &i.at("VESRL"), { { 3, 1 } } });
    add_mnemonic_code(result, "VESRLF", { &i.at("VESRL"), { { 3, 2 } } });
    add_mnemonic_code(result, "VESRLG", { &i.at("VESRL"), { { 3, 3 } } });
    add_mnemonic_code(result, "VCEFB", { &i.at("VCFPS"), { { 2, 0 } } });
    add_mnemonic_code(result, "VSB", { &i.at("VS"), { { 3, 0 } } });
    add_mnemonic_code(result, "VSH", { &i.at("VS"), { { 3, 1 } } });
    add_mnemonic_code(result, "VSF", { &i.at("VS"), { { 3, 2 } } });
    add_mnemonic_code(result, "VSG", { &i.at("VS"), { { 3, 3 } } });
    add_mnemonic_code(result, "VSQ", { &i.at("VS"), { { 3, 4 } } });
    add_mnemonic_code(result, "VLERH", { &i.at("VLER"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLERF", { &i.at("VLER"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLERG", { &i.at("VLER"), { { 2, 3 } } });
    add_mnemonic_code(result, "VSCBIB", { &i.at("VSCBI"), { { 3, 0 } } });
    add_mnemonic_code(result, "VCFEB", { &i.at("VCSFP"), { { 2, 2 } } });
    add_mnemonic_code(result, "VSCBIH", { &i.at("VSCBI"), { { 3, 1 } } });
    add_mnemonic_code(result, "VSCBIF", { &i.at("VSCBI"), { { 3, 2 } } });
    add_mnemonic_code(result, "VSCBIG", { &i.at("VSCBI"), { { 3, 3 } } });
    add_mnemonic_code(result, "VSCBIQ", { &i.at("VSCBI"), { { 3, 4 } } });
    add_mnemonic_code(result, "VSBIQ", { &i.at("VSBI"), { { 4, 4 } } });
    add_mnemonic_code(result, "VSBCBIQ", { &i.at("VSBCBI"), { { 4, 4 } } });
    add_mnemonic_code(result, "VSUMQF", { &i.at("VSUMQ"), { { 3, 2 } } });
    add_mnemonic_code(result, "VSUMQG", { &i.at("VSUMQ"), { { 3, 3 } } });
    add_mnemonic_code(result, "VSUMGH", { &i.at("VSUMG"), { { 3, 1 } } });
    add_mnemonic_code(result, "VSUMGF", { &i.at("VSUMG"), { { 3, 2 } } });
    add_mnemonic_code(result, "VSUMB", { &i.at("VSUM"), { { 3, 0 } } });
    add_mnemonic_code(result, "VSUMH", { &i.at("VSUM"), { { 3, 1 } } });
    add_mnemonic_code(result, "VFAEB", { &i.at("VFAE"), { { 3, 0 } } });
    add_mnemonic_code(result, "VFAEH", { &i.at("VFAE"), { { 3, 1 } } });
    add_mnemonic_code(result, "VFAEF", { &i.at("VFAE"), { { 3, 2 } } });
    add_mnemonic_code(result, "VFEEB", { &i.at("VFEE"), { { 3, 0 } } });
    add_mnemonic_code(result, "VFEEH", { &i.at("VFEE"), { { 3, 1 } } });
    add_mnemonic_code(result, "VFEEF", { &i.at("VFEE"), { { 3, 2 } } });
    add_mnemonic_code(result, "VLBRH", { &i.at("VLBR"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLBRF", { &i.at("VLBR"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLBRG", { &i.at("VLBR"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLBRQ", { &i.at("VLBR"), { { 2, 4 } } });
    add_mnemonic_code(result, "VFEEBS", { &i.at("VFEE"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFEEGS", { &i.at("VFEE"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFEEFS", { &i.at("VFEE"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFEEZB", { &i.at("VFEE"), { { 3, 0 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFEEZH", { &i.at("VFEE"), { { 3, 1 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFEEZF", { &i.at("VFEE"), { { 3, 2 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFEEZBS", { &i.at("VFEE"), { { 3, 0 }, { 4, 3 } } });
    add_mnemonic_code(result, "VFEEZHS", { &i.at("VFEE"), { { 3, 1 }, { 4, 3 } } });
    add_mnemonic_code(result, "VFEEZFS", { &i.at("VFEE"), { { 3, 2 }, { 4, 3 } } });
    add_mnemonic_code(result, "VFENEB", { &i.at("VFENE"), { { 3, 0 } } });
    add_mnemonic_code(result, "VFENEH", { &i.at("VFENE"), { { 3, 1 } } });
    add_mnemonic_code(result, "VFENEF", { &i.at("VFENE"), { { 3, 2 } } });
    add_mnemonic_code(result, "VFENEBS", { &i.at("VFENE"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFENEHS", { &i.at("VFENE"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFENEFS", { &i.at("VFENE"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFENEZB", { &i.at("VFENE"), { { 3, 0 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFENEZH", { &i.at("VFENE"), { { 3, 1 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFENEZF", { &i.at("VFENE"), { { 3, 2 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFENEZBS", { &i.at("VFENE"), { { 3, 0 }, { 4, 3 } } });
    add_mnemonic_code(result, "VFENEZHS", { &i.at("VFENE"), { { 3, 1 }, { 4, 3 } } });
    add_mnemonic_code(result, "VFENEZFS", { &i.at("VFENE"), { { 3, 2 }, { 4, 3 } } });
    add_mnemonic_code(result, "VISTRB", { &i.at("VISTR"), { { 3, 0 } } });
    add_mnemonic_code(result, "VISTRH", { &i.at("VISTR"), { { 3, 1 } } });
    add_mnemonic_code(result, "VISTRF", { &i.at("VISTR"), { { 3, 2 } } });
    add_mnemonic_code(result, "VISTRBS", { &i.at("VISTR"), { { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "VISTRHS", { &i.at("VISTR"), { { 3, 1 }, { 4, 1 } } });
    add_mnemonic_code(result, "VISTRFS", { &i.at("VISTR"), { { 3, 2 }, { 4, 1 } } });
    add_mnemonic_code(result, "VSTRCB", { &i.at("VSTRC"), { { 4, 0 } } });
    add_mnemonic_code(result, "VSTRCH", { &i.at("VSTRC"), { { 4, 1 } } });
    add_mnemonic_code(result, "VSTRCF", { &i.at("VSTRC"), { { 4, 2 } } });
    add_mnemonic_code(result, "VLBRREPH", { &i.at("VLBRREP"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLBRREPF", { &i.at("VLBRREP"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLBRREPG", { &i.at("VLBRREP"), { { 2, 3 } } });
    add_mnemonic_code(result, "VFASB", { &i.at("VFA"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFADB", { &i.at("VFA"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WCDGB", { &i.at("VCFPS"), { { 2, 2 } } });
    add_mnemonic_code(result, "WFASB", { &i.at("VFA"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFADB", { &i.at("VFA"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFAXB", { &i.at("VFA"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFCSB", { &i.at("WFC"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFCDB", { &i.at("WFC"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFCXB", { &i.at("WFC"), { { 3, 4 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFKSB", { &i.at("WFK"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFKDB", { &i.at("WFK"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFKXB", { &i.at("WFK"), { { 3, 4 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFCESB", { &i.at("VFCE"), { { 3, 2 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCESBS", { &i.at("VFCE"), { { 3, 2 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFCEDB", { &i.at("VFCE"), { { 3, 3 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCEDBS", { &i.at("VFCE"), { { 3, 3 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCESB", { &i.at("VFCE"), { { 3, 2 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCESBS", { &i.at("VFCE"), { { 3, 2 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCEDB", { &i.at("VFCE"), { { 3, 3 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCEDBS", { &i.at("VFCE"), { { 3, 3 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCEXB", { &i.at("VFCE"), { { 3, 4 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCEXBS", { &i.at("VFCE"), { { 3, 4 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKESB", { &i.at("VFCE"), { { 3, 2 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKESBS", { &i.at("VFCE"), { { 3, 2 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKEDB", { &i.at("VFCE"), { { 3, 3 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKEDBS", { &i.at("VFCE"), { { 3, 3 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKESB", { &i.at("VFCE"), { { 3, 2 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKESBS", { &i.at("VFCE"), { { 3, 2 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKEDB", { &i.at("VFCE"), { { 3, 3 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKEDBS", { &i.at("VFCE"), { { 3, 3 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKEXB", { &i.at("VFCE"), { { 3, 4 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKEXBS", { &i.at("VFCE"), { { 3, 4 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "VSTRSB", { &i.at("VSTRS"), { { 4, 0 } } });
    add_mnemonic_code(result, "VSTRSH", { &i.at("VSTRS"), { { 4, 1 } } });
    add_mnemonic_code(result, "VSTRSF", { &i.at("VSTRS"), { { 4, 2 } } });
    add_mnemonic_code(result, "VSTRSZB", { &i.at("VSTRS"), { { 4, 0 }, { 5, 2 } } });
    add_mnemonic_code(result, "VFCHSB", { &i.at("VFCH"), { { 3, 2 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCHSBS", { &i.at("VFCH"), { { 3, 2 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFCHDB", { &i.at("VFCH"), { { 3, 3 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCHDBS", { &i.at("VFCH"), { { 3, 3 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHSB", { &i.at("VFCH"), { { 3, 2 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHSBS", { &i.at("VFCH"), { { 3, 2 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHDB", { &i.at("VFCH"), { { 3, 3 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHDBS", { &i.at("VFCH"), { { 3, 3 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHXB", { &i.at("VFCH"), { { 3, 4 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHXBS", { &i.at("VFCH"), { { 3, 4 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKHSB", { &i.at("VFCH"), { { 3, 2 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKHSBS", { &i.at("VFCH"), { { 3, 2 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKHDB", { &i.at("VFCH"), { { 3, 3 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKHDBS", { &i.at("VFCH"), { { 3, 3 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHSB", { &i.at("VFCH"), { { 3, 2 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHSBS", { &i.at("VFCH"), { { 3, 2 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHDB", { &i.at("VFCH"), { { 3, 3 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHDBS", { &i.at("VFCH"), { { 3, 3 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHXB", { &i.at("VFCH"), { { 3, 4 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHXBS", { &i.at("VFCH"), { { 3, 4 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFCHESB", { &i.at("VFCHE"), { { 3, 2 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCHESBS", { &i.at("VFCHE"), { { 3, 2 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFCHEDB", { &i.at("VFCHE"), { { 3, 3 }, { 4, 0 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFCHEDBS", { &i.at("VFCHE"), { { 3, 3 }, { 4, 0 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHESB", { &i.at("VFCHE"), { { 3, 2 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHESBS", { &i.at("VFCHE"), { { 3, 2 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHEDB", { &i.at("VFCHE"), { { 3, 3 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHEDBS", { &i.at("VFCHE"), { { 3, 3 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFCHEXB", { &i.at("VFCHE"), { { 3, 4 }, { 4, 8 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFCHEXBS", { &i.at("VFCHE"), { { 3, 4 }, { 4, 8 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKHESB", { &i.at("VFCHE"), { { 3, 2 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKHESBS", { &i.at("VFCHE"), { { 3, 2 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "VFKHEDB", { &i.at("VFCHE"), { { 3, 3 }, { 4, 4 }, { 5, 0 } } });
    add_mnemonic_code(result, "VFKHEDBS", { &i.at("VFCHE"), { { 3, 3 }, { 4, 4 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHESB", { &i.at("VFCHE"), { { 3, 2 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHESBS", { &i.at("VFCHE"), { { 3, 2 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHEDB", { &i.at("VFCHE"), { { 3, 3 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHEDBS", { &i.at("VFCHE"), { { 3, 3 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "WFKHEXB", { &i.at("VFCHE"), { { 3, 4 }, { 4, 12 }, { 5, 0 } } });
    add_mnemonic_code(result, "WFKHEXBS", { &i.at("VFCHE"), { { 3, 4 }, { 4, 12 }, { 5, 1 } } });
    add_mnemonic_code(result, "VCDGB", { &i.at("VCFPS"), { { 2, 3 } } });
    add_mnemonic_code(result, "VCDLG", { &i.at("VCFPL"), {} });
    add_mnemonic_code(result, "VCDLGB", { &i.at("VCFPL"), { { 2, 3 } } });
    add_mnemonic_code(result, "VCGDB", { &i.at("VCSFP"), { { 2, 3 } } });
    add_mnemonic_code(result, "VCLGDB", { &i.at("VCLGD"), { { 2, 3 } } });
    add_mnemonic_code(result, "VCLFEB", { &i.at("VCLFP"), { { 2, 0 } } });
    add_mnemonic_code(result, "VFDSB", { &i.at("VFD"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFDSB", { &i.at("VFD"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "VFDDB", { &i.at("VFD"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFDDB", { &i.at("VFD"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFDXB", { &i.at("VFD"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "VFISB", { &i.at("VFI"), { { 2, 2 } } });
    add_mnemonic_code(result, "VFIDB", { &i.at("VFI"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLDE", { &i.at("VFLL"), {} });
    add_mnemonic_code(result, "VLDEB", { &i.at("VFLL"), { { 2, 2 }, { 3, 0 } } });
    add_mnemonic_code(result, "WLDEB", { &i.at("VFLL"), { { 2, 2 }, { 3, 8 } } });
    add_mnemonic_code(result, "VFLLS", { &i.at("VFLL"), { { 2, 2 }, { 3, 0 } } });
    add_mnemonic_code(result, "WFLLS", { &i.at("VFLL"), { { 2, 2 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFLLD", { &i.at("VFLL"), { { 2, 3 }, { 3, 8 } } });
    add_mnemonic_code(result, "VLED", { &i.at("VFLR"), {} });
    add_mnemonic_code(result, "VLEDB", { &i.at("VFLR"), { { 2, 3 } } });
    add_mnemonic_code(result, "VFLRD", { &i.at("VFLR"), { { 2, 3 } } });
    add_mnemonic_code(result, "VFMAXSB", { &i.at("VFMAX"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFMAXDB", { &i.at("VFMAX"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFMAXSB", { &i.at("VFMAX"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMAXDB", { &i.at("VFMAX"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMAXXB", { &i.at("VFMAX"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "VFMINSB", { &i.at("VFMIN"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFMINDB", { &i.at("VFMIN"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFMINSB", { &i.at("VFMIN"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMINDB", { &i.at("VFMIN"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMINXB", { &i.at("VFMIN"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "VFMSB", { &i.at("VFM"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFMDB", { &i.at("VFM"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFMSB", { &i.at("VFM"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMDB", { &i.at("VFM"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFMXB", { &i.at("VFM"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "VFMASB", { &i.at("VFMA"), { { 4, 0 }, { 5, 2 } } });
    add_mnemonic_code(result, "VFMADB", { &i.at("VFMA"), { { 4, 0 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFMASB", { &i.at("VFMA"), { { 4, 8 }, { 5, 2 } } });
    add_mnemonic_code(result, "WFMADB", { &i.at("VFMA"), { { 4, 8 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFMAXB", { &i.at("VFMA"), { { 4, 8 }, { 5, 4 } } });
    add_mnemonic_code(result, "VFMSSB", { &i.at("VFMS"), { { 4, 0 }, { 5, 2 } } });
    add_mnemonic_code(result, "VFMSDB", { &i.at("VFMS"), { { 4, 0 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFMSSB", { &i.at("VFMS"), { { 4, 8 }, { 5, 2 } } });
    add_mnemonic_code(result, "WFMSDB", { &i.at("VFMS"), { { 4, 8 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFMSXB", { &i.at("VFMS"), { { 4, 8 }, { 5, 4 } } });
    add_mnemonic_code(result, "VFNMASB", { &i.at("VFNMA"), { { 4, 0 }, { 5, 2 } } });
    add_mnemonic_code(result, "VFNMADB", { &i.at("VFNMA"), { { 4, 0 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFNMASB", { &i.at("VFNMA"), { { 4, 8 }, { 5, 2 } } });
    add_mnemonic_code(result, "WFNMADB", { &i.at("VFNMA"), { { 4, 8 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFNMAXB", { &i.at("VFNMA"), { { 4, 8 }, { 5, 4 } } });
    add_mnemonic_code(result, "VFNMSSB", { &i.at("VFNMS"), { { 4, 0 }, { 5, 2 } } });
    add_mnemonic_code(result, "VFNMSDB", { &i.at("VFNMS"), { { 4, 0 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFNMSSB", { &i.at("VFNMS"), { { 4, 8 }, { 5, 2 } } });
    add_mnemonic_code(result, "WFNMSDB", { &i.at("VFNMS"), { { 4, 8 }, { 5, 3 } } });
    add_mnemonic_code(result, "WFNMSXB", { &i.at("VFNMS"), { { 4, 8 }, { 5, 4 } } });
    add_mnemonic_code(result, "VFPSOSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 0 } } });
    add_mnemonic_code(result, "WFPSOSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 8 } } });
    add_mnemonic_code(result, "VFLCSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 0 }, { 4, 0 } } });
    add_mnemonic_code(result, "VCELFB", { &i.at("VCFPL"), { { 2, 0 } } });
    add_mnemonic_code(result, "VLLEBRZH", { &i.at("VLLEBRZ"), { { 2, 1 } } });
    add_mnemonic_code(result, "VLLEBRZF", { &i.at("VLLEBRZ"), { { 2, 2 } } });
    add_mnemonic_code(result, "VLLEBRZG", { &i.at("VLLEBRZ"), { { 2, 3 } } });
    add_mnemonic_code(result, "VLLEBRZE", { &i.at("VLLEBRZ"), { { 2, 6 } } });
    add_mnemonic_code(result, "LDRV", { &i.at("VLLEBRZ"), { { 2, 3 } } });
    add_mnemonic_code(result, "LERV", { &i.at("VLLEBRZ"), { { 2, 6 } } });
    add_mnemonic_code(result, "VPKF", { &i.at("VPK"), { { 3, 2 } } });
    add_mnemonic_code(result, "VPKG", { &i.at("VPK"), { { 3, 3 } } });
    add_mnemonic_code(result, "VPKH", { &i.at("VPK"), { { 3, 1 } } });
    add_mnemonic_code(result, "WFLCSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 8 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFLNSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "WFLNSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 8 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFLPSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 0 }, { 4, 2 } } });
    add_mnemonic_code(result, "WFLPSB", { &i.at("VFPSO"), { { 2, 2 }, { 3, 8 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFPSODB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 0 } } });
    add_mnemonic_code(result, "WFPSODB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 8 } } });
    add_mnemonic_code(result, "VFLCDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 0 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFLCDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 8 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFLNDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 0 }, { 4, 1 } } });
    add_mnemonic_code(result, "WFLNDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 8 }, { 4, 1 } } });
    add_mnemonic_code(result, "VFLPDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 0 }, { 4, 2 } } });
    add_mnemonic_code(result, "WFLPDB", { &i.at("VFPSO"), { { 2, 3 }, { 3, 8 }, { 4, 2 } } });
    add_mnemonic_code(result, "WFPSOXB", { &i.at("VFPSO"), { { 2, 4 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFLCXB", { &i.at("VFPSO"), { { 2, 4 }, { 3, 8 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFLNXB", { &i.at("VFPSO"), { { 2, 4 }, { 3, 8 }, { 4, 1 } } });
    add_mnemonic_code(result, "WFLPXB", { &i.at("VFPSO"), { { 2, 4 }, { 3, 8 }, { 4, 2 } } });
    add_mnemonic_code(result, "VFSQSB", { &i.at("VFSQ"), { { 2, 2 }, { 3, 0 } } });
    add_mnemonic_code(result, "VFSQDB", { &i.at("VFSQ"), { { 2, 3 }, { 3, 0 } } });
    add_mnemonic_code(result, "WFSQSB", { &i.at("VFSQ"), { { 2, 2 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFSQDB", { &i.at("VFSQ"), { { 2, 3 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFSQXB", { &i.at("VFSQ"), { { 2, 4 }, { 3, 8 } } });
    add_mnemonic_code(result, "VFSSB", { &i.at("VFS"), { { 2, 2 }, { 3, 0 } } });
    add_mnemonic_code(result, "VFSDB", { &i.at("VFS"), { { 2, 3 }, { 3, 0 } } });
    add_mnemonic_code(result, "WFSSB", { &i.at("VFS"), { { 2, 2 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFSDB", { &i.at("VFS"), { { 2, 3 }, { 3, 8 } } });
    add_mnemonic_code(result, "WFSXB", { &i.at("VFS"), { { 2, 4 }, { 3, 8 } } });
    add_mnemonic_code(result, "VFTCISB", { &i.at("VFTCI"), { { 3, 2 }, { 4, 0 } } });
    add_mnemonic_code(result, "VFTCIDB", { &i.at("VFTCI"), { { 3, 3 }, { 4, 0 } } });
    add_mnemonic_code(result, "WFTCISB", { &i.at("VFTCI"), { { 3, 2 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFTCIDB", { &i.at("VFTCI"), { { 3, 3 }, { 4, 8 } } });
    add_mnemonic_code(result, "WFTCIXB", { &i.at("VFTCI"), { { 3, 4 }, { 4, 8 } } });
    add_mnemonic_code(result, "XHHR", { &i.at("RXSBG"), { { 2, 0 }, { 3, 31 } } });
    add_mnemonic_code(result, "XLHR", { &i.at("RXSBG"), { { 2, 32 }, { 3, 63 }, { 4, 32 } } });
    // instruction under this position contain an OR operation not marked in this list

    // in case the operand is ommited, the OR number should be assigned to the value of the ommited operand
    add_mnemonic_code(result, "VFAEBS", { &i.at("VFAE"), { { 3, 0 } } }); // operand with index 4 ORed with 1
    add_mnemonic_code(result, "VFAEHS", { &i.at("VFAE"), { { 3, 1 } } }); // operand with index 4 ORed with 1
    add_mnemonic_code(result, "VFAEFS", { &i.at("VFAE"), { { 3, 2 } } }); // operand with index 4 ORed with 1
    add_mnemonic_code(result, "VFAEZB", { &i.at("VFAE"), { { 3, 0 } } }); // operand with index 4 ORed with 2
    add_mnemonic_code(result, "VFAEZH", { &i.at("VFAE"), { { 3, 1 } } }); // operand with index 4 ORed with 2
    add_mnemonic_code(result, "VFAEZF", { &i.at("VFAE"), { { 3, 2 } } }); // operand with index 4 ORed with 2
    add_mnemonic_code(result, "VFAEZBS", { &i.at("VFAE"), { { 3, 0 } } }); // operand with index 4 ORed with 3
    add_mnemonic_code(result, "VFAEZHS", { &i.at("VFAE"), { { 3, 1 } } }); // operand with index 4 ORed with 3
    add_mnemonic_code(result, "VFAEZFS", { &i.at("VFAE"), { { 3, 2 } } }); // operand with index 4 ORed with 3
    add_mnemonic_code(result, "VSTRCBS", { &i.at("VSTRC"), { { 4, 0 } } }); // operand with index 5 ORed with 1
    add_mnemonic_code(result, "VSTRCHS", { &i.at("VSTRC"), { { 4, 1 } } }); // operand with index 5 ORed with 1
    add_mnemonic_code(result, "VSTRCFS", { &i.at("VSTRC"), { { 4, 2 } } }); // operand with index 5 ORed with 1
    add_mnemonic_code(result, "VSTRCZB", { &i.at("VSTRC"), { { 4, 0 } } }); // operand with index 5 ORed with 2
    add_mnemonic_code(result, "VSTRCZH", { &i.at("VSTRC"), { { 4, 1 } } }); // operand with index 5 ORed with 2
    add_mnemonic_code(result, "VSTRCZF", { &i.at("VSTRC"), { { 4, 2 } } }); // operand with index 5 ORed with 2
    add_mnemonic_code(result, "VSTRCZBS", { &i.at("VSTRC"), { { 4, 0 } } }); // operand with index 5 ORed with 3
    add_mnemonic_code(result, "VSTRCZHS", { &i.at("VSTRC"), { { 4, 1 } } }); // operand with index 5 ORed with 3
    add_mnemonic_code(result, "VSTRCZFS", { &i.at("VSTRC"), { { 4, 2 } } }); // operand with index 5 ORed
                                                                             // with 3 always OR
    add_mnemonic_code(result, "WFISB", { &i.at("VFI"), { { 2, 2 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WFIDB", { &i.at("VFI"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WFIXB", { &i.at("VFI"), { { 2, 4 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCDLGB", { &i.at("VCFPL"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCGDB", { &i.at("VCSFP"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCELFB", { &i.at("VCFPL"), { { 2, 2 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCLFEB", { &i.at("VCLFP"), { { 2, 2 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCEFB", { &i.at("VCFPS"), { { 2, 2 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCDGB", { &i.at("VCFPS"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCFEB", { &i.at("VCSFP"), { { 2, 2 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WCLGDB", { &i.at("VCLGD"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WLEDB", { &i.at("VFLR"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WFLRD", { &i.at("VFLR"), { { 2, 3 } } }); // operand with index 3 ORed with 8
    add_mnemonic_code(result, "WFLRX", { &i.at("VFLR"), { { 2, 4 } } }); // operand with index 3 ORed with 8

    // mnemonics not in principles
    add_mnemonic_code(result, "CIJE", { &i.at("CIJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CIJH", { &i.at("CIJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CIJL", { &i.at("CIJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CIJNE", { &i.at("CIJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CIJNH", { &i.at("CIJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CIJNL", { &i.at("CIJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGIBE", { &i.at("CGIB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGIBH", { &i.at("CGIB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGIBL", { &i.at("CGIB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGIBNE", { &i.at("CGIB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGIBNH", { &i.at("CGIB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGIBNL", { &i.at("CGIB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGIJE", { &i.at("CGIJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGIJH", { &i.at("CGIJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGIJL", { &i.at("CGIJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGIJNE", { &i.at("CGIJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGIJNH", { &i.at("CGIJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGIJNL", { &i.at("CGIJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGITE", { &i.at("CGIT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGITH", { &i.at("CGIT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGITL", { &i.at("CGIT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGITNE", { &i.at("CGIT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGITNH", { &i.at("CGIT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGITNL", { &i.at("CGIT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGRBE", { &i.at("CGRB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGRBH", { &i.at("CGRB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGRBL", { &i.at("CGRB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGRBNE", { &i.at("CGRB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGRBNH", { &i.at("CGRB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGRBNL", { &i.at("CGRB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGRJE", { &i.at("CGRJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGRJH", { &i.at("CGRJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGRJL", { &i.at("CGRJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGRJNE", { &i.at("CGRJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGRJNH", { &i.at("CGRJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGRJNL", { &i.at("CGRJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CGRTE", { &i.at("CGRT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CGRTH", { &i.at("CGRT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CGRTL", { &i.at("CGRT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CGRTNE", { &i.at("CGRT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CGRTNH", { &i.at("CGRT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CGRTNL", { &i.at("CGRT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CIBE", { &i.at("CIB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CIBH", { &i.at("CIB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CIBL", { &i.at("CIB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CIBNE", { &i.at("CIB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CIBNH", { &i.at("CIB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CIBNL", { &i.at("CIB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CITE", { &i.at("CIT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CITH", { &i.at("CIT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CITL", { &i.at("CIT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CITNE", { &i.at("CIT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CITNH", { &i.at("CIT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CITNL", { &i.at("CIT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLFITE", { &i.at("CLFIT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLFITH", { &i.at("CLFIT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLFITL", { &i.at("CLFIT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLFITNE", { &i.at("CLFIT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLFITNH", { &i.at("CLFIT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLFITNL", { &i.at("CLFIT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGIBE", { &i.at("CLGIB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGIBH", { &i.at("CLGIB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGIBL", { &i.at("CLGIB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGIBNE", { &i.at("CLGIB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGIBNH", { &i.at("CLGIB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGIBNL", { &i.at("CLGIB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGIJE", { &i.at("CLGIJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGIJH", { &i.at("CLGIJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGIJL", { &i.at("CLGIJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGIJNE", { &i.at("CLGIJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGIJNH", { &i.at("CLGIJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGIJNL", { &i.at("CLGIJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGITE", { &i.at("CLGIT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGITH", { &i.at("CLGIT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGITL", { &i.at("CLGIT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGITNE", { &i.at("CLGIT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGITNH", { &i.at("CLGIT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGITNL", { &i.at("CLGIT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGRBE", { &i.at("CLGRB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGRBH", { &i.at("CLGRB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGRBL", { &i.at("CLGRB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGRBNE", { &i.at("CLGRB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGRBNH", { &i.at("CLGRB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGRBNL", { &i.at("CLGRB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGRJE", { &i.at("CLGRJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGRJH", { &i.at("CLGRJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGRJL", { &i.at("CLGRJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGRJNE", { &i.at("CLGRJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGRJNH", { &i.at("CLGRJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGRJNL", { &i.at("CLGRJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGRTE", { &i.at("CLGRT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLGRTH", { &i.at("CLGRT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLGRTL", { &i.at("CLGRT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLGRTNE", { &i.at("CLGRT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLGRTNH", { &i.at("CLGRT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLGRTNL", { &i.at("CLGRT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLGTE", { &i.at("CLGT"), { { 1, 8 } } });
    add_mnemonic_code(result, "CLGTH", { &i.at("CLGT"), { { 1, 2 } } });
    add_mnemonic_code(result, "CLGTL", { &i.at("CLGT"), { { 1, 4 } } });
    add_mnemonic_code(result, "CLGTNE", { &i.at("CLGT"), { { 1, 6 } } });
    add_mnemonic_code(result, "CLGTNH", { &i.at("CLGT"), { { 1, 12 } } });
    add_mnemonic_code(result, "CLGTNL", { &i.at("CLGT"), { { 1, 10 } } });
    add_mnemonic_code(result, "CLIBE", { &i.at("CLIB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLIBH", { &i.at("CLIB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLIBL", { &i.at("CLIB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLIBNE", { &i.at("CLIB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLIBNH", { &i.at("CLIB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLIBNL", { &i.at("CLIB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLIJE", { &i.at("CLIJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLIJH", { &i.at("CLIJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLIJL", { &i.at("CLIJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLIJNE", { &i.at("CLIJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLIJNH", { &i.at("CLIJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLIJNL", { &i.at("CLIJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLRBE", { &i.at("CLRB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLRBH", { &i.at("CLRB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLRBL", { &i.at("CLRB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLRBNE", { &i.at("CLRB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLRBNH", { &i.at("CLRB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLRBNL", { &i.at("CLRB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLRJE", { &i.at("CLRJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLRJH", { &i.at("CLRJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLRJL", { &i.at("CLRJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLRJNE", { &i.at("CLRJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLRJNH", { &i.at("CLRJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLRJNL", { &i.at("CLRJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLRTE", { &i.at("CLRT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CLRTH", { &i.at("CLRT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CLRTL", { &i.at("CLRT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CLRTNE", { &i.at("CLRT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CLRTNH", { &i.at("CLRT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CLRTNL", { &i.at("CLRT"), { { 2, 10 } } });
    add_mnemonic_code(result, "CLTE", { &i.at("CLT"), { { 1, 8 } } });
    add_mnemonic_code(result, "CLTH", { &i.at("CLT"), { { 1, 2 } } });
    add_mnemonic_code(result, "CLTL", { &i.at("CLT"), { { 1, 4 } } });
    add_mnemonic_code(result, "CLTNE", { &i.at("CLT"), { { 1, 6 } } });
    add_mnemonic_code(result, "CLTNH", { &i.at("CLT"), { { 1, 12 } } });
    add_mnemonic_code(result, "CLTNL", { &i.at("CLT"), { { 1, 10 } } });
    add_mnemonic_code(result, "CRBE", { &i.at("CRB"), { { 2, 8 } } });
    add_mnemonic_code(result, "CRBH", { &i.at("CRB"), { { 2, 2 } } });
    add_mnemonic_code(result, "CRBL", { &i.at("CRB"), { { 2, 4 } } });
    add_mnemonic_code(result, "CRBNE", { &i.at("CRB"), { { 2, 6 } } });
    add_mnemonic_code(result, "CRBNH", { &i.at("CRB"), { { 2, 12 } } });
    add_mnemonic_code(result, "CRBNL", { &i.at("CRB"), { { 2, 10 } } });
    add_mnemonic_code(result, "CRJE", { &i.at("CRJ"), { { 2, 8 } } });
    add_mnemonic_code(result, "CRJH", { &i.at("CRJ"), { { 2, 2 } } });
    add_mnemonic_code(result, "CRJL", { &i.at("CRJ"), { { 2, 4 } } });
    add_mnemonic_code(result, "CRJNE", { &i.at("CRJ"), { { 2, 6 } } });
    add_mnemonic_code(result, "CRJNH", { &i.at("CRJ"), { { 2, 12 } } });
    add_mnemonic_code(result, "CRJNL", { &i.at("CRJ"), { { 2, 10 } } });
    add_mnemonic_code(result, "CRTE", { &i.at("CRT"), { { 2, 8 } } });
    add_mnemonic_code(result, "CRTH", { &i.at("CRT"), { { 2, 2 } } });
    add_mnemonic_code(result, "CRTL", { &i.at("CRT"), { { 2, 4 } } });
    add_mnemonic_code(result, "CRTNE", { &i.at("CRT"), { { 2, 6 } } });
    add_mnemonic_code(result, "CRTNH", { &i.at("CRT"), { { 2, 12 } } });
    add_mnemonic_code(result, "CRTNL", { &i.at("CRT"), { { 2, 10 } } });
    // operand with index 2 was omitted for the below instruction
    add_mnemonic_code(result, "NOTR", { &i.at("NORK"), { { 2, 0 } } });
    // operand with index 2 was omitted for the below instruction
    add_mnemonic_code(result, "NOTGR", { &i.at("NOGRK"), { { 2, 0 } } });
    add_mnemonic_code(result, "LOCGE", { &i.at("LOCG"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCGH", { &i.at("LOCG"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCGL", { &i.at("LOCG"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCGNE", { &i.at("LOCG"), { { 2, 6 } } });
    add_mnemonic_code(result, "LOCGNH", { &i.at("LOCG"), { { 2, 12 } } });
    add_mnemonic_code(result, "LOCGNL", { &i.at("LOCG"), { { 2, 10 } } });
    add_mnemonic_code(result, "LOCRE", { &i.at("LOCR"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCRH", { &i.at("LOCR"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCRL", { &i.at("LOCR"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCRNE", { &i.at("LOCR"), { { 2, 6 } } });
    add_mnemonic_code(result, "LOCRNH", { &i.at("LOCR"), { { 2, 12 } } });
    add_mnemonic_code(result, "LOCRNL", { &i.at("LOCR"), { { 2, 10 } } });
    add_mnemonic_code(result, "LOCGRE", { &i.at("LOCGR"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCGRH", { &i.at("LOCGR"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCGRL", { &i.at("LOCGR"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCGRNE", { &i.at("LOCGR"), { { 2, 6 } } });
    add_mnemonic_code(result, "LOCGRNH", { &i.at("LOCGR"), { { 2, 12 } } });
    add_mnemonic_code(result, "LOCGRNL", { &i.at("LOCGR"), { { 2, 10 } } });
    add_mnemonic_code(result, "LOCE", { &i.at("LOC"), { { 2, 8 } } });
    add_mnemonic_code(result, "LOCH", { &i.at("LOC"), { { 2, 2 } } });
    add_mnemonic_code(result, "LOCL", { &i.at("LOC"), { { 2, 4 } } });
    add_mnemonic_code(result, "LOCNE", { &i.at("LOC"), { { 2, 6 } } });
    add_mnemonic_code(result, "LOCNH", { &i.at("LOC"), { { 2, 12 } } });
    add_mnemonic_code(result, "LOCNL", { &i.at("LOC"), { { 2, 10 } } });
    add_mnemonic_code(result, "STOCGE", { &i.at("STOCG"), { { 2, 8 } } });
    add_mnemonic_code(result, "STOCGH", { &i.at("STOCG"), { { 2, 2 } } });
    add_mnemonic_code(result, "STOCGL", { &i.at("STOCG"), { { 2, 4 } } });
    add_mnemonic_code(result, "STOCGNE", { &i.at("STOCG"), { { 2, 6 } } });
    add_mnemonic_code(result, "STOCGNH", { &i.at("STOCG"), { { 2, 12 } } });
    add_mnemonic_code(result, "STOCGNL", { &i.at("STOCG"), { { 2, 10 } } });
    add_mnemonic_code(result, "STOCE", { &i.at("STOC"), { { 2, 8 } } });
    add_mnemonic_code(result, "STOCH", { &i.at("STOC"), { { 2, 2 } } });
    add_mnemonic_code(result, "STOCL", { &i.at("STOC"), { { 2, 4 } } });
    add_mnemonic_code(result, "STOCNE", { &i.at("STOC"), { { 2, 6 } } });
    add_mnemonic_code(result, "STOCNH", { &i.at("STOC"), { { 2, 12 } } });
    add_mnemonic_code(result, "STOCNL", { &i.at("STOC"), { { 2, 10 } } });
    // VNO V1,V2,V2        (operand with index 2 replaced with 0 )
    add_mnemonic_code(result, "VNOT", { &i.at("VNO"), { { 2, 0 } } });
    return result;
}

const std::map<std::string, machine_instruction> instruction::machine_instructions =
    instruction::get_machine_instructions();

const std::map<std::string, mnemonic_code> instruction::mnemonic_codes =
    instruction::get_mnemonic_codes(machine_instructions);
