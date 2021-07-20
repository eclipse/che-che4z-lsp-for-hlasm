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

#include "analyzer.h"

void expect_no_errors(const std::string& text)
{
    using namespace hlasm_plugin::parser_library;
    std::string input = (text);
    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)0);
}

void expect_errors(const std::string& text)
{
    using namespace hlasm_plugin::parser_library;
    std::string input = (text);
    analyzer a(input);
    a.analyze();
    a.collect_diags();
    EXPECT_GT(a.diags().size(), (size_t)0);
}

TEST(data_definition_grammar, modifiers)
{
    expect_no_errors(R"( DC 10FDP(123)L(2*3)S(2*4)E(-12*2)'2.25'
 DC 10FDP(123)L2S(2*4)E(-12*2)'2.25'
 DC 10FDP(123)L(2*3)S6E(-12*2)'2.25'
 DC 10FDP(123)L.(2*3)S6E(-12*2)'2.25'
 DC 10FDP(123)L(2*3)S(2*4)E12'2.25'
 DC 10FDP(123)L(2*3)S(2*4)E-12'2.25'
 DC 10FDP(123)L(2*3)S6E0'2.25'
 DC 10FDP(123)L.(2*3)S6E0'2.25'
 DC 10FDP(123)L3S(2*4)E12'2.25'
 DC 10FDP(123)L1S30E(-12*2)'2.25'
 DC 10FDP(123)L1S-30E(-12*2)'2.25'
 DC 10FDP(123)L.1S30E(-12*2)'2.25'
 DC 10FDP(123)L1S30E40'2.25'
 DC 10FDP(123)L1S-30E-40'2.25'
 DC 10FDP(123)L.1S30E40'2.25'
 DC 10FDP(123)L.1S-30E-40'2.25'

 DC (1*8)FDP(123)L2S(2*4)E(-12*2)'2.25'
 DC (1*8)FDP(123)L(2*3)S6E(-12*2)'2.25'
 DC (1*8)FDP(123)L(2*3)S(2*4)E12'2.25'
 DC (1*8)FDP(123)L(2*3)S6E0'2.25'
 DC (1*8)FDP(123)L3S(2*4)E12'2.25'
 DC (1*8)FDP(123)L1S30E(-12*2)'2.25'
 DC (1*8)FDP(123)L1S30E40'2.25'

 DC 10FDL(2*3)S(2*4)E(-12*2)'2.25'
 DC 10FDL2S(2*4)E(-12*2)'2.25'
 DC 10FDL(2*3)S6E(-12*2)'2.25'
 DC 10FDL(2*3)S(2*4)E12'2.25'
 DC 10FDL(2*3)S6E0'2.25'
 DC 10FDL3S(2*4)E12'2.25'
 DC 10FDL1S30E(-12*2)'2.25'
 DC 10FDL1S30E40'2.25'

 DC (1*8)FDL(2*3)S(2*4)E(-12*2)'2.25'
 DC (1*8)FDL2S(2*4)E(-12*2)'2.25'
 DC (1*8)FDL(2*3)S6E(-12*2)'2.25'
 DC (1*8)FDL(2*3)S(2*4)E12'2.25'
 DC (1*8)FDL(2*3)S6E0'2.25'
 DC (1*8)FDL3S(2*4)E12'2.25'
 DC (1*8)FDL1S30E(-12*2)'2.25'
 DC (1*8)FDL1S30E40'2.25'
 DC 13FL.(13)'2.25')");

    expect_errors(" DC 10FDP(123)L(2*3)S(2*4)E(-12*2)(34)'2.25'");
    expect_errors(" DC 10FDP(123)(1)L(2*3)S(2*4)E(-12*2)'2.25'");
    expect_errors(" DC (1*8)FDL1S(1+2)(3+1)E40'2.25'");
    expect_errors(" DC %");
}

TEST(data_definition_grammar, modifiers_lower_case)
{
    expect_no_errors(R"(
 dc 10fdp(123)l(2*3)s(2*4)e(-12*2)'2.25'
 dc 10fdp(123)l2s(2*4)e(-12*2)'2.25'
 dc 10fdp(123)l(2*3)s6e(-12*2)'2.25'
 dc 10fdp(123)l.(2*3)s6e(-12*2)'2.25'
 dc 10fdp(123)l(2*3)s(2*4)e12'2.25'
 dc 10fdp(123)l(2*3)s(2*4)e-12'2.25'
 dc 10fdp(123)l(2*3)s6e0'2.25'
 dc 10fdp(123)l.(2*3)s6e0'2.25'
 dc 10fdp(123)l3s(2*4)e12'2.25'
 dc 10fdp(123)l1s30e(-12*2)'2.25'
 dc 10fdp(123)l1s-30e(-12*2)'2.25'
 dc 10fdp(123)l.1s30e(-12*2)'2.25'
 dc 10fdp(123)l1s30e40'2.25'
 dc 10fdp(123)l1s-30e-40'2.25'
 dc 10fdp(123)l.1s30e40'2.25'
 dc 10fdp(123)l.1s-30e-40'2.25'

 dc (1*8)fdp(123)l2s(2*4)e(-12*2)'2.25'
 dc (1*8)fdp(123)l(2*3)s6e(-12*2)'2.25'
 dc (1*8)fdp(123)l(2*3)s(2*4)e12'2.25'
 dc (1*8)fdp(123)l(2*3)s6e0'2.25'
 dc (1*8)fdp(123)l3s(2*4)e12'2.25'
 dc (1*8)fdp(123)l1s30e(-12*2)'2.25'
 dc (1*8)fdp(123)l1s30e40'2.25'

 dc 10fdl(2*3)s(2*4)e(-12*2)'2.25'
 dc 10fdl2s(2*4)e(-12*2)'2.25'
 dc 10fdl(2*3)s6e(-12*2)'2.25'
 dc 10fdl(2*3)s(2*4)e12'2.25'
 dc 10fdl(2*3)s6e0'2.25'
 dc 10fdl3s(2*4)e12'2.25'
 dc 10fdl1s30e(-12*2)'2.25'
 dc 10fdl1s30e40'2.25'

 dc (1*8)fdl(2*3)s(2*4)e(-12*2)'2.25'
 dc (1*8)fdl2s(2*4)e(-12*2)'2.25'
 dc (1*8)fdl(2*3)s6e(-12*2)'2.25'
 dc (1*8)fdl(2*3)s(2*4)e12'2.25'
 dc (1*8)fdl(2*3)s6e0'2.25'
 dc (1*8)fdl3s(2*4)e12'2.25'
 dc (1*8)fdl1s30e(-12*2)'2.25'
 dc (1*8)fdl1s30e40'2.25'
 dc 13fl.(13)'2.25'
)");
}

TEST(data_definition_grammar, address_nominal)
{
    expect_no_errors(" DC (1*8)S(512(12))");
    expect_no_errors(" DC 8S(512(12))");
    expect_no_errors(" DC S(512(12))");
    expect_no_errors(" DC SP(13)(512(12))");
    expect_no_errors(" DC SP(13)L2(512(12))");
    expect_no_errors(" DC SP(13)L(2)(512(12))");
    expect_no_errors(" DC S(512(12),418(0))");
    expect_no_errors(
        R"(  USING A,5
     DC S(512(12),418(0),A_field)
A       DSECT
A_field DS F)");
    expect_no_errors(" DC S(512(0))");
    expect_no_errors("A DC S(*-A+4(0))");

    expect_errors(" DC S(512())");
    expect_errors(" DC S(512(0)");
    expect_errors(" DC SP(13)L(13)(512(12,13))");
    expect_errors(" DC A(512(12)");
}

TEST(data_definition_grammar, expression_nominal)
{
    expect_no_errors("A DC A(*-A,*+4)");
    expect_no_errors("A DC A(A+32)");
    expect_no_errors("A DC AL4(A+32)");
    expect_no_errors("A DC AL(4)(A+32)");
    expect_no_errors("A DC 10AL(4)(A+32)");
    expect_no_errors("A DC (1+9)A(*-A,*+4)");
}

TEST(data_definition_grammar, no_nominal)
{
    expect_no_errors("A DC 0C");
    expect_no_errors("A DC 0CL10");
    expect_no_errors("A DC 0CL(1+10)");
}

TEST(data_definition, duplication_factor)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "13C'A'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    auto dup_f = parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs();
    EXPECT_EQ(dup_f, 13);
}

TEST(data_definition, duplication_factor_expr)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "(13*2)C'A'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    auto dup_f = parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs();
    EXPECT_EQ(dup_f, 26);
}

TEST(data_definition, duplication_factor_out_of_range)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "1231312123123123123C'A'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_GT(parsed.diags().size(), (size_t)0);

    auto dup_f = parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs();
    EXPECT_EQ(dup_f, 1);
}

TEST(data_definition, duplication_factor_invalid_number)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "-C'A'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_GT(parsed.diags().size(), (size_t)0);

    auto dup_f = parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs();
    EXPECT_EQ(dup_f, 1);
}

TEST(data_definition, all_fields)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "(1*8)FDP(123)L2S(2*4)E(-12*2)'2.25'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    auto dup_f = parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs();
    EXPECT_EQ(dup_f, 8);

    EXPECT_EQ(parsed.program_type->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 123);
    EXPECT_EQ(parsed.length->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 2);
    EXPECT_EQ(parsed.length_type, expressions::data_definition::length_type::BYTE);
    EXPECT_EQ(parsed.scale->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 8);
    EXPECT_EQ(parsed.exponent->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), -24);
    ASSERT_NE(parsed.nominal_value->access_string(), nullptr);
    EXPECT_EQ(parsed.nominal_value->access_string()->value, "2.25");
}

TEST(data_definition, no_nominal)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "0FDL2";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    EXPECT_EQ(parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 0);
    EXPECT_EQ(parsed.program_type, nullptr);
    EXPECT_EQ(parsed.length->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 2);
    EXPECT_EQ(parsed.length_type, expressions::data_definition::length_type::BYTE);
    EXPECT_EQ(parsed.scale, nullptr);
    EXPECT_EQ(parsed.exponent, nullptr);
    ASSERT_EQ(parsed.nominal_value, nullptr);
}

TEST(data_definition, no_nominal_expr)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "0FDL(2+2)";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    EXPECT_EQ(parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 0);
    EXPECT_EQ(parsed.program_type, nullptr);
    EXPECT_EQ(parsed.length->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 4);
    EXPECT_EQ(parsed.length_type, expressions::data_definition::length_type::BYTE);
    EXPECT_EQ(parsed.scale, nullptr);
    EXPECT_EQ(parsed.exponent, nullptr);
    ASSERT_EQ(parsed.nominal_value, nullptr);
}

TEST(data_definition, bit_length)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "(1*8)FDP(123)L.2S-8E(-12*2)'2.25'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_EQ(parsed.diags().size(), (size_t)0);

    EXPECT_EQ(parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 8);

    EXPECT_EQ(parsed.program_type->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 123);
    EXPECT_EQ(parsed.length->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 2);
    EXPECT_EQ(parsed.length_type, expressions::data_definition::length_type::BIT);
    EXPECT_EQ(parsed.scale->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), -8);
    EXPECT_EQ(parsed.exponent->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), -24);
    ASSERT_NE(parsed.nominal_value->access_string(), nullptr);
    EXPECT_EQ(parsed.nominal_value->access_string()->value, "2.25");
}

TEST(data_definition, unexpected_dot)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "(1*8)FDL.2S.-8E(-12*2)'2.25'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_GT(parsed.diags().size(), (size_t)0);

    EXPECT_EQ(parsed.dupl_factor->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 8);

    EXPECT_EQ(parsed.program_type, nullptr);
    EXPECT_EQ(parsed.length->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), 2);
    EXPECT_EQ(parsed.length_type, expressions::data_definition::length_type::BIT);
    EXPECT_EQ(parsed.scale->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), -8);
    EXPECT_EQ(parsed.exponent->evaluate(a.hlasm_ctx().ord_ctx).get_abs(), -24);
    ASSERT_NE(parsed.nominal_value->access_string(), nullptr);
    EXPECT_EQ(parsed.nominal_value->access_string()->value, "2.25");
}

TEST(data_definition, unexpected_minus)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "(1*8)FDL.2S.-E(-12*2)'2.25'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_GT(parsed.diags().size(), (size_t)0);
}

TEST(data_definition, wrong_modifier_order)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = "1HL-12P(123)S1'1.25'";
    analyzer a(input);
    auto res = a.parser().data_def();

    auto parsed = std::move(res->value);
    EXPECT_GT(parsed.diags().size(), (size_t)0);
}

TEST(data_definition, B_wrong_nominal_value)
{
    using namespace hlasm_plugin::parser_library;

    std::string input = " DC B'12'";
    analyzer a(input);
    a.analyze();

    a.collect_diags();
    EXPECT_EQ(a.diags().size(), (size_t)1);
}

TEST(data_definition, trim_labels)
{
    using namespace hlasm_plugin::parser_library;
    std::string input = R"(
&L SETC 'LABEL '
&L EQU  0
)";

    analyzer a(input);
    a.analyze();

    a.collect_diags();
    ASSERT_EQ(a.diags().size(), 0);
}

TEST(data_definition, externals)
{
    expect_no_errors(" EXTRN E1,E2\n DC A(E1,E2)");
    expect_no_errors(" WXTRN W1,W2\n DC A(W1,W2)");
}
