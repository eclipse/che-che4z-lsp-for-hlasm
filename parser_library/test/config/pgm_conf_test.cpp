/*
 * Copyright (c) 2021 Broadcom.
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

#include "config/pgm_conf.h"
#include "nlohmann/json.hpp"

using namespace hlasm_plugin::parser_library::config;

TEST(pgm_conf, full_content_read)
{
    const auto config = R"({
"pgms":[
  {
    "program": "P1",
    "pgroup": "G1"
  },
  {
    "program": "P2",
    "pgroup": "G2"
  }
],
"alwaysRecognize":[
  "*.mac",
  "*.mac2",
  "*.mac3"
],
"diagnosticsSuppressLimit": 123
})"_json.get<pgm_conf>();

    ASSERT_EQ(config.pgms.size(), 2);
    EXPECT_EQ(config.pgms[0].pgroup, "G1");
    EXPECT_EQ(config.pgms[0].program, "P1");
    EXPECT_EQ(config.pgms[1].pgroup, "G2");
    EXPECT_EQ(config.pgms[1].program, "P2");

    ASSERT_EQ(config.always_recognize.size(), 3);
    EXPECT_EQ(config.always_recognize[0], "*.mac");
    EXPECT_EQ(config.always_recognize[1], "*.mac2");
    EXPECT_EQ(config.always_recognize[2], "*.mac3");

    EXPECT_EQ(config.diagnostics_suppress_limit, 123u);
}

TEST(pgm_conf, full_content_write)
{
    const pgm_conf config = { { { "P1", "G1" }, { "P2", "G2" } }, { "*.mac1", "*.mac2", "*.mac3" }, 321 };
    const auto expected =
        R"({"pgms":[{"program":"P1","pgroup":"G1"},{"program":"P2","pgroup":"G2"}],"alwaysRecognize":["*.mac1","*.mac2","*.mac3"],"diagnosticsSuppressLimit":321})"_json;

    EXPECT_EQ(nlohmann::json(config), expected);
}

TEST(pgm_conf, minimal_content_read)
{
    const auto config = R"({"pgms":[]})"_json.get<pgm_conf>();
    EXPECT_EQ(config.pgms.size(), 0);
    EXPECT_EQ(config.always_recognize.size(), 0);
    EXPECT_FALSE(config.diagnostics_suppress_limit.has_value());
}

TEST(pgm_conf, minimal_content_write)
{
    const auto config_json = nlohmann::json(pgm_conf {});
    const auto expected = R"({"pgms":[]})"_json;

    EXPECT_EQ(config_json, expected);
}

TEST(pgm_conf, invalid)
{
    const auto cases = {
        R"({})"_json,
        R"({"pgms":[{}]})"_json,
        R"({"pgms":[{"program":""}]})"_json,
        R"({"pgms":[{"pgroup":""}]})"_json,
    };

    for (const auto& input : cases)
        EXPECT_THROW(input.get<pgm_conf>(), nlohmann::json::exception);
}
