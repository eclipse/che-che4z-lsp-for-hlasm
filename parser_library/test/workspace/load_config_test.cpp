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
#include <fstream>
#include <iterator>

#include "gtest/gtest.h"

#include "empty_configs.h"
#include "utils/platform.h"
#include "workspaces/file_impl.h"
#include "workspaces/file_manager_impl.h"
#include "workspaces/library_local.h"
#include "workspaces/workspace.h"

using namespace hlasm_plugin::parser_library;
using namespace hlasm_plugin::parser_library::workspaces;
using hlasm_plugin::utils::platform::is_windows;

class file_proc_grps : public file_impl
{
public:
    file_proc_grps()
        : file_impl("proc_grps.json")
    {}

    file_uri uri = "test_uri";

    virtual const file_uri& get_file_name() override { return uri; }

    virtual const std::string& get_text() override { return file; }

    virtual bool update_and_get_bad() override { return false; }

    std::string file = is_windows() ?
                                    R"({
  "pgroups": [
    {
      "name": "P1",
      "libs": [
        "C:\\Users\\Desktop\\ASLib",
        "lib",
        "libs\\lib2\\",
		""
      ],
                
                "asm_options": {
                "SYSPARM": "SEVEN",
                 "PROFILE": "MAC1"
                }
    },
    {
      "name": "P2",
      "libs": [
        "C:\\Users\\Desktop\\ASLib",
        "P2lib",
        "P2libs\\libb"
      ]
    }
  ]
})"
                                    : R"({
		"pgroups": [
			{
				"name": "P1",
				"libs": [
					"/home/user/ASLib",
					"lib",
					"libs/lib2/",
			""
				],
                
                "asm_options": {
                "SYSPARM": "SEVEN",
                 "PROFILE": "MAC1"
                }
			},
			{
				"name": "P2",
				"libs": [
					"/home/user/ASLib",
					"P2lib",
					"P2libs/libb"
				]
			}
		]
	})";
};

class file_pgm_conf : public file_impl
{
public:
    file_pgm_conf()
        : file_impl("proc_grps.json")
    {}

    file_uri uri = "test_uri";

    virtual const file_uri& get_file_name() override { return uri; }

    virtual const std::string& get_text() override { return file; }

    virtual bool update_and_get_bad() override { return false; }

    std::string file = is_windows() ? R"({
  "pgms": [
    {
      "program": "pgm1",
      "pgroup": "P1"
    },
    {
      "program": "pgms\\*",
      "pgroup": "P2"
    }
  ]
})"
                                    : R"({
  "pgms": [
    {
      "program": "pgm1",
      "pgroup": "P1"
    },
    {
      "program": "pgms/*",
      "pgroup": "P2"
    }
  ]
})";
};

class file_manager_proc_grps_test : public file_manager_impl
{
public:
    file_ptr add_file(const file_uri& uri) override
    {
        if (uri.substr(uri.size() - 14) == "proc_grps.json")
            return proc_grps;
        else
            return pgm_conf;
    }

    std::shared_ptr<file_proc_grps> proc_grps = std::make_shared<file_proc_grps>();
    std::shared_ptr<file_pgm_conf> pgm_conf = std::make_shared<file_pgm_conf>();


    // Inherited via file_manager
    virtual void did_open_file(const std::string&, version_t, std::string) override {}
    virtual void did_change_file(const std::string&, version_t, const document_change*, size_t) override {}
    virtual void did_close_file(const std::string&) override {}
};

TEST(workspace, load_config_synthetic)
{
    file_manager_proc_grps_test file_manager;
    lib_config config;
    workspace ws("test_proc_grps_uri", "test_proc_grps_name", file_manager, config);

    ws.open();

    auto& pg = ws.get_proc_grp("P1");
    EXPECT_EQ("P1", pg.name());
    auto expected = []() -> std::array<std::string, 4> {
        if (is_windows())
            return { "C:\\Users\\Desktop\\ASLib\\",
                "test_proc_grps_uri\\lib\\",
                "test_proc_grps_uri\\libs\\lib2\\",
                "test_proc_grps_uri\\" };
        else
            return {
                "/home/user/ASLib/", "test_proc_grps_uri/lib/", "test_proc_grps_uri/libs/lib2/", "test_proc_grps_uri/"
            };
    }();

    EXPECT_EQ(std::size(expected), pg.libraries().size());
    for (size_t i = 0; i < std::min(std::size(expected), pg.libraries().size()); ++i)
    {
        library_local* libl = dynamic_cast<library_local*>(pg.libraries()[i].get());
        ASSERT_NE(libl, nullptr);
        EXPECT_EQ(expected[i], libl->get_lib_path());
    }

    auto& pg2 = ws.get_proc_grp("P2");
    EXPECT_EQ("P2", pg2.name());

    auto expected2 = []() -> std::array<std::string, 3> {
        if (is_windows())
            return {
                "C:\\Users\\Desktop\\ASLib\\", "test_proc_grps_uri\\P2lib\\", "test_proc_grps_uri\\P2libs\\libb\\"
            };
        else
            return { "/home/user/ASLib/", "test_proc_grps_uri/P2lib/", "test_proc_grps_uri/P2libs/libb/" };
    }();

    EXPECT_EQ(std::size(expected2), pg2.libraries().size());
    for (size_t i = 0; i < std::min(std::size(expected2), pg2.libraries().size()); ++i)
    {
        library_local* libl = dynamic_cast<library_local*>(pg2.libraries()[i].get());
        ASSERT_NE(libl, nullptr);
        EXPECT_EQ(expected2[i], libl->get_lib_path());
    }


    // test of pgm_conf and workspace::get_proc_grp_by_program
    auto& pg3 = is_windows() ? ws.get_proc_grp_by_program("test_proc_grps_uri\\pgm1")
                             : ws.get_proc_grp_by_program("test_proc_grps_uri/pgm1");

    EXPECT_EQ(pg3.libraries().size(), std::size(expected));
    for (size_t i = 0; i < std::min(std::size(expected), pg3.libraries().size()); ++i)
    {
        library_local* libl = dynamic_cast<library_local*>(pg3.libraries()[i].get());
        ASSERT_NE(libl, nullptr);
        EXPECT_EQ(expected[i], libl->get_lib_path());
    }


    auto& pg4 = is_windows() ? ws.get_proc_grp_by_program("test_proc_grps_uri\\pgms\\anything")
                             : ws.get_proc_grp_by_program("test_proc_grps_uri/pgms/anything");

    EXPECT_EQ(pg4.libraries().size(), std::size(expected2));
    for (size_t i = 0; i < std::min(std::size(expected2), pg4.libraries().size()); ++i)
    {
        library_local* libl = dynamic_cast<library_local*>(pg4.libraries()[i].get());
        ASSERT_NE(libl, nullptr);
        EXPECT_EQ(expected2[i], libl->get_lib_path());
    }
    // test of asm_options
    const auto& asm_options = ws.get_asm_options(is_windows() ? "test_proc_grps_uri\\pgm1" : "test_proc_grps_uri/pgm1");

    EXPECT_EQ("SEVEN", asm_options.sysparm);
    EXPECT_EQ("MAC1", asm_options.profile);
}


TEST(workspace, pgm_conf_malformed)
{
    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, R"({ "pgms": [})");
    fm.did_open_file(proc_grps_name, 0, empty_proc_grps);

    lib_config config;
    workspace ws(fm, config);
    ws.open();

    ws.collect_diags();
    ASSERT_EQ(ws.diags().size(), 1U);
    EXPECT_EQ(ws.diags()[0].code, "W0003");
}

TEST(workspace, proc_grps_malformed)
{
    file_manager_impl fm;

    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, R"({ "pgroups" []})");

    lib_config config;
    workspace ws(fm, config);
    ws.open();

    ws.collect_diags();
    ASSERT_EQ(ws.diags().size(), 1U);
    EXPECT_EQ(ws.diags()[0].code, "W0002");
}

TEST(workspace, pgm_conf_missing)
{
    file_manager_impl fm;
    fm.did_open_file(proc_grps_name, 0, empty_proc_grps);

    lib_config config;
    workspace ws(fm, config);
    ws.open();

    ws.collect_diags();
    ASSERT_EQ(ws.diags().size(), 0U);
}

TEST(workspace, proc_grps_missing)
{
    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);

    lib_config config;
    workspace ws(fm, config);
    ws.open();

    ws.collect_diags();
    ASSERT_EQ(ws.diags().size(), 0U);
}
TEST(workspace, asm_options_invalid)
{
    std::string proc_file = R"({
  "pgroups": [
    {
      "name": "P1",
      "libs": [ "lib" ],    
      "asm_options": {
        "SYSPARM" : 42
   
        }
    }
  ]
})";
    file_manager_impl fm;
    fm.did_open_file(pgm_conf_name, 0, empty_pgm_conf);
    fm.did_open_file(proc_grps_name, 0, proc_file);

    lib_config config;
    workspace ws(fm, config);
    ws.open();

    ws.collect_diags();
    ASSERT_EQ(ws.diags().size(), 1U);
    EXPECT_EQ(ws.diags()[0].code, "W0002");
}