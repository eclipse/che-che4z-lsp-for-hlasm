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

#include <sstream>
#include <string>

#include "antlr4-runtime.h"
#include "gtest/gtest.h"

#include "hlasmparser.h"
#include "lexing/input_source.h"
#include "lexing/lexer.h"
#include "lexing/token_stream.h"

using namespace hlasm_plugin::parser_library;

// tests lexer class:
// continuation statements, rewinding, token creation

using parser = parsing::hlasmparser;

TEST(lexer_test, rntest)
{
    std::string out = R"(ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
EOLLN
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
EOLLN
EOF
)";

    semantics::source_info_processor src_proc(false);
    lexing::input_source input("TEST TEST \r\n TEST1 TEST2");
    lexing::lexer l(&input, &src_proc);
    lexing::token_stream tokens(&l);
    parser parser(&tokens);

    tokens.fill();

    std::stringstream token_stream;
    for (auto token : tokens.getTokens())
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
    auto token_string = token_stream.str();

    ASSERT_EQ(token_string, out);
}

TEST(lexer_test, new_line_in_ignored)
{
    semantics::source_info_processor src_proc(false);
    // test case, when a newline is in the first 15 ignored characters after continuation
    lexing::input_source input(
        R"(NAME1 OP1      OPERAND1,OPERAND2,OPERAND3   This is the normal         X
        
label lr 1,1)");
    lexing::lexer l(&input, &src_proc);
    lexing::token_stream tokens(&l);
    parser parser(&tokens);

    tokens.fill();

    std::stringstream token_stream;
    size_t eolln_count = 0;
    for (auto token : tokens.getTokens())
    {
        if (parser.getVocabulary().getSymbolicName(token->getType()) == "EOLLN")
            ++eolln_count;
    }

    EXPECT_EQ(eolln_count, (size_t)2);
}

TEST(lexer_test, unlimited_line)
{
    std::string in =
        R"(LABEL INSTR    2,1,                                                                   THIS SHOULD NOT BE IGNORED
      INSTR    2    REMARK
      INSTR    2,1, TOTO JE REMARK)";
    std::string out = R"(ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
NUM
COMMA
NUM
COMMA
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
EOLLN
IGNORED
SPACE
ORDSYMBOL
SPACE
NUM
SPACE
ORDSYMBOL
EOLLN
IGNORED
SPACE
ORDSYMBOL
SPACE
NUM
COMMA
NUM
COMMA
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
EOLLN
EOF
)";

    semantics::source_info_processor src_proc(false);
    lexing::input_source input(in);
    lexing::lexer l(&input, &src_proc);
    lexing::token_stream tokens(&l);
    parser parser(&tokens);
    l.set_unlimited_line(true);

    tokens.fill();

    std::stringstream token_stream;
    for (auto token : tokens.getTokens())
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
    auto token_string = token_stream.str();

    ASSERT_EQ(token_string, out);
}

TEST(lexer_test, rewind_input)
{
    std::string in =
        R"(    REWIND1
REWIND2
    REWIND3)";
    std::string out = R"(SPACE
ORDSYMBOL
SPACE
ORDSYMBOL
EOLLN
ORDSYMBOL
ORDSYMBOL
EOLLN
ORDSYMBOL
EOLLN
SPACE
ORDSYMBOL
ORDSYMBOL
EOLLN
SPACE
ORDSYMBOL
EOLLN
EOF
)";
    lexing::input_source input(in);
    semantics::source_info_processor src_proc(false);
    lexing::lexer l(&input, &src_proc);
    lexing::token_stream tokens(&l);
    parser parser(&tokens);

    std::stringstream token_stream;
    lexing::token_ptr token;
    do
    {
        token = l.nextToken();
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
        if (token->getText() == "REWIND1")
        {
            l.rewind_input({ 0, 0 });
            break;
        }
    } while (token->getType() != antlr4::Token::EOF);

    do
    {
        token = l.nextToken();
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
        if (token->getText() == "REWIND2")
        {
            l.rewind_input({ 0, 4 });
            break;
        }
    } while (token->getType() != antlr4::Token::EOF);

    do
    {
        token = l.nextToken();
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
        if (token->getText() == "REWIND3")
        {
            l.rewind_input({ 1, 17 });
            break;
        }
    } while (token->getType() != antlr4::Token::EOF);

    do
    {
        token = l.nextToken();
        token_stream << parser.getVocabulary().getSymbolicName(token->getType()) << std::endl;
    } while (token->getType() != antlr4::Token::EOF);

    auto token_string = token_stream.str();

    ASSERT_EQ(token_string, out);
}

TEST(lexer_test, special_spaces)
{
    std::string in = "A\v\f\t LR";
    lexing::input_source input(in);
    semantics::source_info_processor src_proc(false);
    lexing::lexer l(&input, &src_proc);

    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::IDENTIFIER);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::SPACE);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::ORDSYMBOL);
}

TEST(lexer_test, attribute_in_continuation)
{
    std::string in =
        R"(       LR                                                           1,Lx
               'SYMBOL
)";

    semantics::source_info_processor src_proc(false);
    lexing::input_source input(in);
    lexing::lexer l(&input, &src_proc);

    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::SPACE);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::ORDSYMBOL);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::SPACE);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::NUM);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::COMMA);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::ORDSYMBOL);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::CONTINUATION);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::IGNORED);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::IGNORED);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::ATTR);
    ASSERT_EQ(l.nextToken()->getType(), lexing::lexer::ORDSYMBOL);
}
