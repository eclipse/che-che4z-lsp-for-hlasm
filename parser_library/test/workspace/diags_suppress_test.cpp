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

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>

#include "gtest/gtest.h"

#include "lib_config.h"
#include "workspaces/file_impl.h"
#include "workspaces/file_manager_impl.h"
#include "workspaces/workspace.h"
#include "empty_configs.h"

using namespace nlohmann;
using namespace hlasm_plugin::parser_library;
using namespace hlasm_plugin::parser_library::workspaces;

std::string one_proc_grps = R"(
{
	"pgroups": [
        { "name": "P1", "libs": [] }
	]
}
)";

TEST(diags_suppress, no_suppress)
{
    json new_config = R"({"diagnosticsSuppressLimit":10})"_json;
    lib_config::load_from_json(new_config);

    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, one_proc_grps);

    std::string file_name = "a_file";

    fm.did_open_file(file_name, 0, R"(
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
)");

    workspace ws(fm);
    ws.open();
    ws.did_open_file(file_name);

    auto pfile = fm.find(file_name);
    ASSERT_TRUE(pfile);

    pfile->collect_diags();
    EXPECT_EQ(pfile->diags().size(), 6U);
}

class message_consumer_mock : public hlasm_plugin::parser_library::message_consumer
{
public:
    virtual void show_message(const std::string& message, message_type type) override
    {
        messages.push_back(std::make_pair(message, type));
    }
    std::vector<std::pair<std::string, message_type>> messages;
};
TEST(diags_suppress, do_suppress)
{
    json new_config = R"({"diagnosticsSuppressLimit":5})"_json;
    lib_config::load_from_json(new_config);

    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, one_proc_grps);

    std::string file_name = "a_file";

    fm.did_open_file(file_name, 0, R"(
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
)");

    message_consumer_mock msg_consumer;

    workspace ws(fm);
    ws.set_message_consumer(&msg_consumer);
    ws.open();
    ws.did_open_file(file_name);

    auto pfile = fm.find(file_name);
    ASSERT_TRUE(pfile);

    pfile->collect_diags();
    EXPECT_EQ(pfile->diags().size(), 0U);

    ASSERT_EQ(msg_consumer.messages.size(), 1U);
    EXPECT_EQ(msg_consumer.messages[0].first, "Diagnostics suppressed from a_file, because there is no configuration.");
    EXPECT_EQ(msg_consumer.messages[0].second, message_type::MT_INFO);
}

TEST(diags_suppress, pgm_supress_limit_changed)
{
    json new_config = R"({"diagnosticsSuppressLimit":10})"_json;
    lib_config::load_from_json(new_config);

    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, one_proc_grps);

    std::string file_name = "a_file";

    fm.did_open_file(file_name, 0, R"(
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
)");

    workspace ws(fm);
    ws.open();
    ws.did_open_file(file_name);

    auto pfile = fm.find(file_name);
    ASSERT_TRUE(pfile);

    pfile->collect_diags();
    EXPECT_EQ(pfile->diags().size(), 6U);
    pfile->diags().clear();

    std::string new_limit_str = R"("diagnosticsSuppressLimit":5,)";
    document_change ch(range({ 0, 1 }, { 0, 1 }), new_limit_str.c_str(), new_limit_str.size());

    fm.did_change_file(pgm_conf_name, 1, &ch, 1);
    ws.did_change_file(pgm_conf_name, &ch, 1);

    ws.did_change_file(file_name, &ch, 1);

    pfile = fm.find(file_name);
    ASSERT_TRUE(pfile);
    pfile->collect_diags();
    EXPECT_EQ(pfile->diags().size(), 0U);
}

TEST(diags_suppress, cancel_token)
{
    json new_config = R"({"diagnosticsSuppressLimit":5})"_json;
    lib_config::load_from_json(new_config);

    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, one_proc_grps);

    std::string file_name = "a_file";

    fm.did_open_file(file_name, 0, R"(
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
    LR 1,
)");

    std::atomic<bool> cancel = true;
    workspace ws(fm, &cancel);
    ws.open();
    ws.did_open_file(file_name);

    auto pfile = fm.find(file_name);
    ASSERT_TRUE(pfile);

    pfile->collect_diags();
    EXPECT_EQ(pfile->diags().size(), 6U);
}
