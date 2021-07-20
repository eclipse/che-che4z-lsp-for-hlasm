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

#include "../common_testing.h"

using namespace hlasm_plugin::parser_library;
using namespace hlasm_plugin::parser_library::context;
using namespace hlasm_plugin::parser_library::semantics;

// tests for
// variable substitution for model statements
// concatenation of multiple substitutions
// CA instructions

TEST(var_subs, gbl_instr_only)
{
    std::string input("   gbla var");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));
}

TEST(var_subs, lcl_instr_only)
{
    std::string input("   lcla var");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));
}

TEST(var_subs, gbl_instr_more)
{
    std::string input("   gbla var,var2,var3");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it = ctx.ids().find("var");
    auto it2 = ctx.ids().find("var2");
    auto it3 = ctx.ids().find("var3");

    ASSERT_TRUE(ctx.get_var_sym(it));
    ASSERT_TRUE(ctx.get_var_sym(it2));
    ASSERT_TRUE(ctx.get_var_sym(it3));
}

TEST(var_subs, lcl_instr_more)
{
    std::string input("   lcla var,var2,var3");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it = ctx.ids().find("var");
    auto it2 = ctx.ids().find("var2");
    auto it3 = ctx.ids().find("var3");

    ASSERT_TRUE(ctx.get_var_sym(it));
    ASSERT_TRUE(ctx.get_var_sym(it2));
    ASSERT_TRUE(ctx.get_var_sym(it3));
}

TEST(var_subs, set_to_var)
{
    std::string input("&var seta 3");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));

    int tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_a();
    EXPECT_EQ(tmp, 3);
}

TEST(var_subs, set_to_var_idx)
{
    std::string input("&var(2) seta 3");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));
    std::vector<A_t> subscript1;
    subscript1.push_back(2);
    int tmp = m.get_var_sym_value(it, std::move(subscript1), {}).access_a();
    EXPECT_EQ(tmp, 3);
}

TEST(var_subs, set_to_var_idx_many)
{
    std::string input("&var(2) seta 3,4,5");
    std::string input_s1("2");
    std::string input_s2("3");
    std::string input_s3("4");
    analyzer s1(input_s1);
    analyzer s2(input_s2);
    analyzer s3(input_s3);

    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));

    int tmp;
    std::vector<A_t> subscript1;
    subscript1.push_back(2);
    tmp = m.get_var_sym_value(it, std::move(subscript1), {}).access_a();
    EXPECT_EQ(tmp, 3);
    std::vector<A_t> subscript2;
    subscript2.push_back(3);
    tmp = m.get_var_sym_value(it, std::move(subscript2), {}).access_a();
    EXPECT_EQ(tmp, 4);
    std::vector<A_t> subscript3;
    subscript3.push_back(4);
    tmp = m.get_var_sym_value(it, std::move(subscript3), {}).access_a();
    EXPECT_EQ(tmp, 5);
}

TEST(var_subs, var_sym_reset)
{
    std::string input("&var setc 'avc'   \n&var setc 'XXX'");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var");

    ASSERT_TRUE(ctx.get_var_sym(it));

    std::string tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_c();
    EXPECT_EQ(tmp, "XXX");
}

TEST(var_subs, created_set_sym)
{
    std::string input("&var setc 'avc'   \n&var2 setb 0  \n&(ab&var.cd&var2) seta 11");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("abavccd0");

    ASSERT_TRUE(ctx.get_var_sym(it));

    auto tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_a();
    EXPECT_EQ(tmp, 11);
}

TEST(var_subs, instruction_substitution_space_at_end)
{
    std::string input("&var setc 'LR '   \n &var 1,1");
    analyzer a(input);
    a.analyze();


    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)0);
}

TEST(var_subs, instruction_substitution_space_in_middle)
{
    std::string input("&var setc 'LR 1,1'   \n &var ");
    analyzer a(input);
    a.analyze();


    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)1);
}

TEST(var_concatenation, concatenated_string_dot_last)
{
    std::string input("&var setc 'avc'   \n&var2 setc '&var.'");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var2");

    ASSERT_TRUE(ctx.get_var_sym(it));

    auto tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_c();
    EXPECT_EQ(tmp, "avc");
}

TEST(var_concatenation, concatenated_string_dot)
{
    std::string input("&var setc 'avc'   \n&var2 setc '&var.-get'");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var2");

    ASSERT_TRUE(ctx.get_var_sym(it));

    auto tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_c();
    EXPECT_EQ(tmp, "avc-get");
}

TEST(var_concatenation, concatenated_string_double_dot)
{
    std::string input("&var setc 'avc'   \n&var2 setc '&var..'");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();
    processing::context_manager m(a.hlasm_ctx());

    auto it = ctx.ids().find("var2");

    ASSERT_TRUE(ctx.get_var_sym(it));

    auto tmp = m.get_var_sym_value(it, std::vector<int> {}, {}).access_c();
    EXPECT_EQ(tmp, "avc.");
}

TEST(AGO, extended)
{
    std::string input(R"(
 AGO (2).a,.b,.c
.a anop   
&var1 setb 0
.b anop
&var2 setb 0
.c anop
&var3 setb 0
)");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it1 = ctx.ids().add("var1");
    auto it2 = ctx.ids().add("var2");
    auto it3 = ctx.ids().add("var3");

    EXPECT_FALSE(ctx.get_var_sym(it1));
    EXPECT_TRUE(ctx.get_var_sym(it2));
    EXPECT_TRUE(ctx.get_var_sym(it3));
}

TEST(AGO, extended_fail)
{
    std::string input(R"(
 AGO (8).a,.b,.c
.a anop   
&var1 setb 0
.b anop
&var2 setb 0
.c anop
&var3 setb 0
)");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it1 = ctx.ids().add("var1");
    auto it2 = ctx.ids().add("var2");
    auto it3 = ctx.ids().add("var3");

    EXPECT_TRUE(ctx.get_var_sym(it1));
    EXPECT_TRUE(ctx.get_var_sym(it2));
    EXPECT_TRUE(ctx.get_var_sym(it3));
}

TEST(AIF, extended)
{
    std::string input(R"(
 AIF (0).a,(1).b,(1).c
.a anop   
&var1 setb 0
.b anop
&var2 setb 0
.c anop
&var3 setb 0
)");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it1 = ctx.ids().add("var1");
    auto it2 = ctx.ids().add("var2");
    auto it3 = ctx.ids().add("var3");

    EXPECT_FALSE(ctx.get_var_sym(it1));
    EXPECT_TRUE(ctx.get_var_sym(it2));
    EXPECT_TRUE(ctx.get_var_sym(it3));
}

TEST(AIF, extended_fail)
{
    std::string input(R"(
 AIF (0).a,(0).b,(0).c
.a anop   
&var1 setb 0
.b anop
&var2 setb 0
.c anop
&var3 setb 0
)");
    analyzer a(input);
    a.analyze();

    auto& ctx = a.hlasm_ctx();

    auto it1 = ctx.ids().add("var1");
    auto it2 = ctx.ids().add("var2");
    auto it3 = ctx.ids().add("var3");

    EXPECT_TRUE(ctx.get_var_sym(it1));
    EXPECT_TRUE(ctx.get_var_sym(it2));
    EXPECT_TRUE(ctx.get_var_sym(it3));
}

TEST(ACTR, exceeded)
{
    std::string input(R"(
.A ANOP
 LR 1,1
 AGO .A
)");
    analyzer a(input);
    a.analyze();

    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)1);
}

TEST(ACTR, infinite_ACTR)
{
    std::string input(R"(
.A ANOP
 ACTR 1024
 LR 1,1
 AGO .A
)");
    analyzer a(input);
    a.analyze();

    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)1);
}

TEST(SET, conversions_valid)
{
    std::string input(R"(
&A SETA 1
&B SETB 0
&C SETC '2'

&A SETA &B
&A SETA &C

&C SETC '&A'
&C SETC '&B'
)");
    analyzer a(input);
    a.analyze();

    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)0);
}

TEST(SET, conversions_invalid)
{
    std::string input(R"(
&A SETA 1
&B SETB 0
&C SETC '2'

&A SETA '1'
&B SETB ('1')

&C SETC &A
&C SETC &B
)");
    analyzer a(input);
    a.analyze();

    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)6);
}

TEST(CA_instructions, undefined_relocatable)
{
    std::string input(R"(
A EQU B
L1 LR 1,1
L2 LR 1,1

&V1 SETA L2-L1
&V2 SETA A

B EQU 1
)");
    analyzer a(input);
    a.analyze();

    a.collect_diags();

    ASSERT_EQ(a.diags().size(), (size_t)3);
}

TEST(var_subs, defined_by_self_ref)
{
    std::string input("&VAR(N'&VAR+1) SETA N'&VAR+1");
    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_TRUE(a.diags().empty());

    EXPECT_EQ(get_var_vector<A_t>(a.hlasm_ctx(), "VAR"), std::vector<A_t> { 2 });
}
