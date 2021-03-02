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

#ifndef HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_MANAGER_H
#define HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_MANAGER_H

// This file specifies the interface of parser library.
// The pimpl (pointer to implementation) idiom is used to hide its implementation.
// It implements LSP requests and notifications and is used by the language server.

#include <atomic>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "lib_config.h"
#include "message_consumer.h"
#include "parser_library_export.h"
#include "protocol.h"

namespace hlasm_plugin {
namespace parser_library {
namespace workspaces {
class workspace;
}
using ws_id = workspaces::workspace*;
namespace debugging {
struct create_debugger_result;
}

// Interface that can be implemented to be able to get list of
// diagnostics from workspace manager whenever a file is parsed
// Passes list of all diagnostics that are in all currently opened files.
class PARSER_LIBRARY_EXPORT diagnostics_consumer
{
public:
    virtual void consume_diagnostics(diagnostic_list diagnostics) = 0;
    virtual ~diagnostics_consumer() {};
};

// Interface that can be implemented to be able to get performance metrics
//(time that parsing took, number of parsed lines, etc)
class PARSER_LIBRARY_EXPORT performance_metrics_consumer
{
public:
    virtual void consume_performance_metrics(const performance_metrics& metrics) = 0;
};

// The main class that encapsulates all functionality of parser library.
// All the methods are C++ version of LSP and DAP methods.
class PARSER_LIBRARY_EXPORT workspace_manager
{
    class impl;

public:
    workspace_manager(std::atomic<bool>* cancel = nullptr);

    workspace_manager(const workspace_manager&) = delete;
    workspace_manager& operator=(const workspace_manager&) = delete;

    workspace_manager(workspace_manager&&) noexcept;
    workspace_manager& operator=(workspace_manager&&) noexcept;

    virtual ~workspace_manager();


    virtual size_t get_workspaces(ws_id* workspaces, size_t max_size);
    virtual size_t get_workspaces_count();

    virtual void add_workspace(const char* name, const char* uri);
    virtual void remove_workspace(const char* uri);

    virtual void did_open_file(const char* document_uri, version_t version, const char* text, size_t text_size);
    virtual void did_change_file(
        const char* document_uri, version_t version, const document_change* changes, size_t ch_size);
    virtual void did_close_file(const char* document_uri);
    virtual void did_change_watched_files(const char** paths, size_t size);

    virtual position_uri definition(const char* document_uri, const position pos);
    virtual position_uris references(const char* document_uri, const position pos);
    virtual string_array hover(const char* document_uri, const position pos);
    virtual completion_list completion(
        const char* document_uri, const position pos, const char trigger_char, int trigger_kind);
    virtual const std::vector<token_info>& semantic_tokens(const char* document_uri);

    virtual void configuration_changed(const lib_config& new_config);

    // implementation of observer pattern - register consumer. Unregistering not implemented (yet).
    virtual void register_diagnostics_consumer(diagnostics_consumer* consumer);
    virtual void register_performance_metrics_consumer(performance_metrics_consumer* consumer);
    virtual void set_message_consumer(message_consumer* consumer);

    virtual debugging::create_debugger_result create_debugger(const char* file_name);

private:
    impl* impl_;
};
} // namespace parser_library
} // namespace hlasm_plugin
#endif // !HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_MANAGER_H
