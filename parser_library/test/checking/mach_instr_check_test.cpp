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

#include "gtest/gtest.h"

#include "checking/instruction_checker.h"

using namespace hlasm_plugin::parser_library;
using namespace hlasm_plugin::parser_library::checking;

class machine_instr_test : public testing::Test
{
public:
    machine_instr_test()
    {
        test_balr_true.push_back(&op_val_14);
        test_balr_true.push_back(&op_val_15);

        test_lr_true.push_back(&op_val_0);
        test_lr_true.push_back(&op_val_2);

        test_br_true.push_back(&op_val_11);

        test_mvcl_true.push_back(&op_val_4);
        test_mvcl_true.push_back(&op_val_0);

        test_xr_true.push_back(&op_val_15);
        test_xr_true.push_back(&op_val_15);

        test_clc_true.push_back(&length_one);
        test_clc_true.push_back(&op_val_2);

        test_bal_valid.push_back(&op_val_2);
        test_bal_valid.push_back(&valid_op);

        test_a_invalid.push_back(&op_val_4);
        test_a_invalid.push_back(&invalid_op);
    }

protected:
    one_operand op_val_0 = one_operand(0);
    one_operand op_val_1 = one_operand(1);
    one_operand op_val_2 = one_operand(2);
    one_operand op_val_3 = one_operand(3);
    one_operand op_val_4 = one_operand(4);
    one_operand op_val_11 = one_operand(11);
    one_operand op_val_12 = one_operand(12);
    one_operand op_val_13 = one_operand(13);
    one_operand op_val_14 = one_operand(14);
    one_operand op_val_15 = one_operand(15);
    address_operand length_one = address_operand(address_state::UNRES, 1, 2, 4);
    address_operand valid_op = address_operand(address_state::RES_VALID, 1, 1, 111);
    address_operand invalid_op = address_operand(address_state::RES_INVALID, 1, 1, 1);

    diagnostic_collector collector;

    std::vector<const checking::machine_operand*> test_no_operand_true =
        std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_balr_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_lr_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_br_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_mvcl_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_xr_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_clc_true = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_bal_valid = std::vector<const checking::machine_operand*>();
    std::vector<const checking::machine_operand*> test_a_invalid = std::vector<const checking::machine_operand*>();
};

TEST_F(machine_instr_test, checker_test)
{
    std::string balr_name = "BALR";
    std::string bal_name = "BAL";
    std::string lr_name = "LR";
    std::string mvcl_name = "MVCL";
    std::string xr_name = "XR";
    std::string clc_name = "CLC";
    std::string a_name = "A";

    auto balr_true = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(balr_name))
                         ->get()
                         ->check(balr_name, test_balr_true, range(), collector);
    EXPECT_TRUE(balr_true);
    auto lr_true = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(lr_name))
                       ->get()
                       ->check(lr_name, test_lr_true, range(), collector);
    EXPECT_TRUE(lr_true);
    auto mvcl_true = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(mvcl_name))
                         ->get()
                         ->check(mvcl_name, test_mvcl_true, range(), collector);
    EXPECT_TRUE(mvcl_true);
    auto xr_true = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(xr_name))
                       ->get()
                       ->check(xr_name, test_xr_true, range(), collector);
    EXPECT_TRUE(xr_true);
    auto clc_true_one = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(clc_name))
                            ->get()
                            ->check(clc_name, test_clc_true, range(), collector);
    EXPECT_TRUE(clc_true_one);
    auto bal_valid = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(bal_name))
                         ->get()
                         ->check(bal_name, test_bal_valid, range(), collector);
    EXPECT_TRUE(bal_valid);
    auto a_invalid = (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(a_name))
                         ->get()
                         ->check(a_name, test_a_invalid, range(), collector);
    EXPECT_FALSE(a_invalid);

    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(balr_name))
        ->get()
        ->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(lr_name))->get()->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(mvcl_name))
        ->get()
        ->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(xr_name))->get()->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(clc_name))
        ->get()
        ->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(bal_name))
        ->get()
        ->clear_diagnostics();
    (&hlasm_plugin::parser_library::context::instruction::machine_instructions.at(a_name))->get()->clear_diagnostics();
}
