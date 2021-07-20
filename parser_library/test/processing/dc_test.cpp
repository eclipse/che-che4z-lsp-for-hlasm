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

// tests for DC instruction

TEST(DC, not_defined_symbol_in_length)
{
    std::string input = R"(
A DC CL(D-C)'1'
B LR 1,1
C LR 1,1

R EQU B-A
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, non_previously_defined_length)
{
    std::string input = R"(
A DC CL(C-B)'1'
B LR 1,1
C LR 1,1

R EQU B-A
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)0);

    id_index R = a.hlasm_ctx().ids().add("R");
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(R)->value().get_abs(), 2);
}

TEST(DC, previously_defined_length)
{
    std::string input = R"(
A LR 1,1
  LR 1,1
B DC CL(B-A)'ABCD'
C LR 1,1

R EQU C-B
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)0);

    id_index R = a.hlasm_ctx().ids().add("R");
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(R)->value().get_abs(), 4);
}

TEST(DC, implicit_length)
{
    std::string input = R"(
A LR 1,1
B DC S(1,1)
C LR 1,1

R EQU C-B
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)0);

    id_index R = a.hlasm_ctx().ids().add("R");
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(R)->value().get_abs(), 4);
}

TEST(DC, implicit_length_deferred_checking)
{
    std::string input = R"(
B DC S(C,-1)
C LR 1,1

R EQU C-B
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    ASSERT_EQ(a.diags().size(), (size_t)1);
    EXPECT_EQ(a.diags()[0].code, "D022");
}

TEST(DC, simple_cycle)
{
    std::string input = R"(
A DC BL(B-A)'101'
B LR 1,1

)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, space_cycle)
{
    std::string input = R"(
A DC AL(D-C)(1,1,1)
B LR 1,1
C DC CL(B-A)'1'
D LR 1,1
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, len_attr)
{
    std::string input = R"(
X EQU -12
A DC CL(X+14)'A'
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("a"))->attributes().length(),
        (symbol_attributes::len_attr)2);

    EXPECT_EQ(a.diags().size(), (size_t)0);
}

TEST(DC, scale_attr)
{
    std::string input = R"(
X EQU 22
A DC FS(X+14)'1'
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("a"))->attributes().scale(),
        (symbol_attributes::scale_attr)36);

    EXPECT_EQ(a.diags().size(), (size_t)0);
}

TEST(DC, scale_with_unary_op)
{
    std::string input = R"(
P DC FS+12'1'
M DC FS-12'1'
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("P"))->attributes().scale(),
        (symbol_attributes::scale_attr)12);
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("M"))->attributes().scale(),
        (symbol_attributes::scale_attr)-12);

    EXPECT_EQ(a.diags().size(), (size_t)0);
}

TEST(DC, cyclic_len_non_forward)
{
    std::string input = R"(
X DC CL(A+1)'X'
A EQU L'X
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("A"))->value().get_abs(), 0);

    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, cyclic_len_forward)
{
    std::string input = R"(
A EQU L'X
X DC CL(A+1)'X'
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("A"))->value().get_abs(), 1);

    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, valid_len_ref)
{
    std::string input = R"(
A EQU L'X
X DC CL(Y)'X'
Y EQU L'A
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("A"))->value().get_abs(), 1);
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("X"))->attributes().length(),
        (symbol_attributes::len_attr)1);
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("Y"))->value().get_abs(), 1);

    EXPECT_EQ(a.diags().size(), (size_t)0);
}

TEST(DC, invalid_len_ref)
{
    std::string input = R"(
A EQU L'X
X DC CL(Y+1)'X'
Y EQU A
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("A"))->value().get_abs(), 1);
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("X"))->attributes().length(),
        (symbol_attributes::len_attr)1);
    EXPECT_EQ(a.hlasm_ctx().ord_ctx.get_symbol(a.hlasm_ctx().ids().add("Y"))->value().get_abs(), 0);

    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(DC, self_cycle)
{
    std::string input = R"(
X DC CL(L'X)'X'
)";

    analyzer a(input);
    a.analyze();
    a.collect_diags();

    EXPECT_EQ(a.diags().size(), (size_t)1);
}
