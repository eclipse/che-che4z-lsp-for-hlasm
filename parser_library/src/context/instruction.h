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

#ifndef HLASMPLUGIN_PARSERLIBRARY_CONTEXT_INSTRUCTION_H
#define HLASMPLUGIN_PARSERLIBRARY_CONTEXT_INSTRUCTION_H

#include <array>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "checking/instr_operand.h"
#include "diagnostic.h"
#include "id_storage.h"
namespace hlasm_plugin {
namespace parser_library {
namespace context {

// all mach_format types for operands of machine instructions:
// formats with length 16 are arranged in range (0,2),formats with length 32 are arranged in range(3,20),formats with
// length 48 are arranged in range (21,77)
enum class mach_format
{
    E,
    I,
    RR,
    IE,
    RRD,
    RRE,
    RRF_a,
    RRF_b,
    RRF_c,
    RRF_d,
    RRF_e,
    RI_a,
    RI_b,
    RI_c,
    RS_a,
    RS_b,
    RSI,
    RX_a,
    RX_b,
    S,
    SI,
    MII,
    RIE_a,
    RIE_b,
    RIE_c,
    RIE_d,
    RIE_e,
    RIE_f,
    RIE_g,
    RIL_a,
    RIL_b,
    RIL_c,
    RIS,
    RRS,
    RSL_a,
    RSL_b,
    RSY_a,
    RSY_b,
    RXE,
    RXF,
    RXY_a,
    RXY_b,
    SIL,
    SIY,
    SMI,
    SS_a,
    SS_b,
    SS_c,
    SS_d,
    SS_e,
    SS_f,
    SSE,
    SSF,
    VRI_a,
    VRI_b,
    VRI_c,
    VRI_d,
    VRI_e,
    VRI_f,
    VRR_a,
    VRR_b,
    VRR_c,
    VRR_d,
    VRR_e,
    VRR_f,
    VRS_a,
    VRS_b,
    VRS_c,
    VRV,
    VRX,
    VRI_g,
    VRI_h,
    VRI_i,
    VRR_g,
    VRR_h,
    VRR_i,
    VRS_d,
    VSI
};

const checking::parameter empty = { false, 0, checking::machine_operand_type::NONE };
const checking::parameter reg = { false, 4, checking::machine_operand_type::REG };
const checking::parameter dis_reg = { false, 4, checking::machine_operand_type::DIS_REG };
const checking::parameter dis_reg_r = { false, 4, checking::machine_operand_type::REG };
const checking::parameter mask = { false, 4, checking::machine_operand_type::MASK };
const checking::parameter dis_12u = { false, 12, checking::machine_operand_type::DISPLC };
const checking::parameter dis_20s = { true, 20, checking::machine_operand_type::DISPLC };
const checking::parameter base_ = { false, 4, checking::machine_operand_type::BASE };
const checking::parameter length_8 = { false, 8, checking::machine_operand_type::LENGTH };
const checking::parameter length_4 = { false, 4, checking::machine_operand_type::LENGTH };
const checking::parameter imm_4u = { false, 4, checking::machine_operand_type::IMM };
const checking::parameter imm_8s = { true, 8, checking::machine_operand_type::IMM };
const checking::parameter imm_8u = { false, 8, checking::machine_operand_type::IMM };
const checking::parameter imm_12s = { true, 12, checking::machine_operand_type::IMM };
const checking::parameter imm_16s = { true, 16, checking::machine_operand_type::IMM };
const checking::parameter imm_16u = { false, 16, checking::machine_operand_type::IMM };
const checking::parameter imm_24s = { true, 24, checking::machine_operand_type::IMM };
const checking::parameter imm_32s = { true, 32, checking::machine_operand_type::IMM };
const checking::parameter imm_32u = { false, 32, checking::machine_operand_type::IMM };
const checking::parameter vec_reg = { false, 4, checking::machine_operand_type::VEC_REG };
const checking::parameter reg_imm_12s = { true, 12, checking::machine_operand_type::REG_IMM };
const checking::parameter reg_imm_16s = { true, 16, checking::machine_operand_type::REG_IMM };
const checking::parameter reg_imm_24s = { true, 24, checking::machine_operand_type::REG_IMM };
const checking::parameter reg_imm_32s = { true, 32, checking::machine_operand_type::REG_IMM };

/*
Rules for displacement operands:
With DB formats
        - must be in format D(B), otherwise throw an error
        - parser returns this in (displacement, 0, base, true) format
With DXB Formats
        - can be either D(X,B) or D(,B) - in this case, the X is replaced with 0
        - parser returns this in (displacement, x, base, false) format
*/
const checking::machine_operand_format db_12_4_U = checking::machine_operand_format(dis_12u, empty, base_);
const checking::machine_operand_format db_20_4_S = checking::machine_operand_format(dis_20s, empty, base_);
const checking::machine_operand_format drb_12_4x4_U = checking::machine_operand_format(dis_12u, dis_reg_r, base_);
const checking::machine_operand_format dxb_12_4x4_U = checking::machine_operand_format(dis_12u, dis_reg, base_);
const checking::machine_operand_format dxb_20_4x4_S = checking::machine_operand_format(dis_20s, dis_reg, base_);
const checking::machine_operand_format dvb_12_4x4_U = checking::machine_operand_format(dis_12u, vec_reg, base_);
const checking::machine_operand_format reg_4_U = checking::machine_operand_format(reg, empty, empty);
const checking::machine_operand_format mask_4_U = checking::machine_operand_format(mask, empty, empty);
const checking::machine_operand_format imm_4_U = checking::machine_operand_format(imm_4u, empty, empty);
const checking::machine_operand_format imm_8_S = checking::machine_operand_format(imm_8s, empty, empty);
const checking::machine_operand_format imm_8_U = checking::machine_operand_format(imm_8u, empty, empty);
const checking::machine_operand_format imm_16_U = checking::machine_operand_format(imm_16u, empty, empty);
const checking::machine_operand_format imm_12_S = checking::machine_operand_format(imm_12s, empty, empty);
const checking::machine_operand_format imm_16_S = checking::machine_operand_format(imm_16s, empty, empty);
const checking::machine_operand_format imm_32_S = checking::machine_operand_format(imm_32s, empty, empty);
const checking::machine_operand_format imm_32_U = checking::machine_operand_format(imm_32u, empty, empty);
const checking::machine_operand_format vec_reg_4_U = checking::machine_operand_format(vec_reg, empty, empty);
const checking::machine_operand_format db_12_8x4L_U = checking::machine_operand_format(dis_12u, length_8, base_);
const checking::machine_operand_format db_12_4x4L_U = checking::machine_operand_format(dis_12u, length_4, base_);
const checking::machine_operand_format reg_imm_12_S = checking::machine_operand_format(reg_imm_12s, empty, empty);
const checking::machine_operand_format reg_imm_16_S = checking::machine_operand_format(reg_imm_16s, empty, empty);
const checking::machine_operand_format reg_imm_24_S = checking::machine_operand_format(reg_imm_24s, empty, empty);
const checking::machine_operand_format reg_imm_32_S = checking::machine_operand_format(reg_imm_32s, empty, empty);
// intervals dividing formats based on length
const int length_sixteen_interval = 3;
const int length_thirtytwo_interval = 21;
const int length_fortyeight_interval = 78;
// machine instruction representation for checking
class machine_instruction
{
public:
    std::string instr_name;
    mach_format format;
    std::vector<checking::machine_operand_format> operands; // what the vector of operands should look like
    size_t size_for_alloc;
    int no_optional;
    size_t page_no;

    machine_instruction(const std::string& name,
        mach_format format,
        std::vector<checking::machine_operand_format> operands,
        int no_optional,

        size_t page_no)
        : instr_name(name)
        , format(format)
        , operands(operands)
        , size_for_alloc(get_length_by_format(format))
        , no_optional(no_optional)
        , page_no(page_no) {};
    machine_instruction(const std::string& name,
        mach_format format,
        std::vector<checking::machine_operand_format> operands,
        size_t page_no)
        : machine_instruction(name, format, operands, 0, page_no)
    {}

    bool check_nth_operand(size_t place, const checking::machine_operand* operand);

    int get_length_by_format(mach_format instruction_format) const
    {
        auto interval = (int)(instruction_format);
        if (interval < length_sixteen_interval)
        {
            return 16;
        }
        if (interval < length_thirtytwo_interval)
        {
            return 32;
        }
        if (interval < length_fortyeight_interval)
        {
            return 48;
        }
    }
    virtual bool check(const std::string& name_of_instruction,
        const std::vector<const checking::machine_operand*> operands,
        const range& stmt_range,
        const diagnostic_collector& add_diagnostic); // input vector is the vector of the actual incoming values

    // std::vector<diag_range> & get_diagnostics()
    void clear_diagnostics();
    std::vector<diagnostic_op> diagnostics;
    virtual ~machine_instruction() = default;
};

using machine_instruction_ptr = std::unique_ptr<machine_instruction>;

struct ca_instruction
{
    std::string name;
    bool operandless;
};

// representation of mnemonic codes for machine instructions
struct mnemonic_code
{
public:
    mnemonic_code(std::string instr, std::vector<std::pair<size_t, size_t>> replaced)
        : instruction(instr)
        , replaced(replaced) {};

    std::string instruction;
    // first goes place, then value
    std::vector<std::pair<size_t, size_t>> replaced;
};

// machine instruction common representation
struct assembler_instruction
{
    int min_operands;
    int max_operands; // -1 in case there is no max value
    bool has_ord_symbols;
    std::string description; // used only for hover and completion

    assembler_instruction(int min_operands, int max_operands, bool has_ord_symbols, std::string description)
        : min_operands(min_operands)
        , max_operands(max_operands)
        , has_ord_symbols(has_ord_symbols)
        , description(std::move(description)) {};
};

// static class holding string names of instructions with theirs additional info
class instruction
{
public:
    enum class instruction_array
    {
        CA,
        ASM,
        MACH,
        MNEM
    };

    static std::map<const std::string, machine_instruction_ptr> get_machine_instructions();

    static std::map<const std::string, mnemonic_code> get_mnemonic_codes();

    /*
    min_operands - minimal number of operands, non-negative integer, always defined
    max_operands - if not defined (can be infinite), value is -1, otherwise a non-negative integer
    */

    static const std::vector<ca_instruction> ca_instructions;

    static const std::map<const std::string, assembler_instruction> assembler_instructions;

    // static const std::vector<std::string> macro_processing_instructions;

    static std::map<const std::string, machine_instruction_ptr> machine_instructions;

    static const std::map<const std::string, mnemonic_code> mnemonic_codes;

    static const std::map<mach_format, const std::string> mach_format_to_string;
};


} // namespace context
} // namespace parser_library
} // namespace hlasm_plugin

#endif
