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

#ifndef HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_H
#define HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_H

#include <atomic>
#include <filesystem>
#include <memory>
#include <regex>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "config/pgm_conf.h"
#include "config/proc_conf.h"
#include "diagnosable_impl.h"
#include "file_manager.h"
#include "lib_config.h"
#include "library.h"
#include "message_consumer.h"
#include "processor.h"
#include "processor_group.h"


namespace hlasm_plugin::parser_library::workspaces {

using ws_uri = std::string;
using proc_grp_id = std::string;
using program_id = std::string;
using ws_highlight_info = std::unordered_map<std::string, semantics::highlighting_info>;

// represents pair program => processor group - saves
// information that a program uses certain processor group
struct program
{
    program(program_id prog_id, proc_grp_id pgroup)
        : prog_id(prog_id)
        , pgroup(pgroup)
    {}

    program_id prog_id;
    proc_grp_id pgroup;
};


// Represents a LSP workspace. It solves all dependencies between files -
// implements parse lib provider and decides which files are to be parsed
// when a particular file has been changed in the editor.
class workspace : public diagnosable_impl, public parse_lib_provider, public lsp::feature_provider
{
public:
    // Creates just a dummy workspace with no libraries - no dependencies
    // between files.
    workspace(file_manager& file_manager, const lib_config& global_config, std::atomic<bool>* cancel = nullptr);
    workspace(const ws_uri& uri,
        file_manager& file_manager,
        const lib_config& global_config,
        std::atomic<bool>* cancel = nullptr);
    workspace(const ws_uri& uri,
        const std::string& name,
        file_manager& file_manager,
        const lib_config& global_config,
        std::atomic<bool>* cancel = nullptr);

    workspace(const workspace& ws) = delete;
    workspace& operator=(const workspace&) = delete;

    workspace(workspace&& ws) = default;
    workspace& operator=(workspace&&) = delete;

    void collect_diags() const override;

    void add_proc_grp(processor_group pg);
    const processor_group& get_proc_grp(const proc_grp_id& proc_grp) const;
    const processor_group& get_proc_grp_by_program(const std::string& program) const;

    void parse_file(const std::string& file_uri);
    void refresh_libraries();
    void did_open_file(const std::string& file_uri);
    void did_close_file(const std::string& file_uri);
    void did_change_file(const std::string document_uri, const document_change* changes, size_t ch_size);
    void did_change_watched_files(const std::string& file_uri);

    location definition(const std::string& document_uri, position pos) const override;
    location_list references(const std::string& document_uri, position pos) const override;
    lsp::hover_result hover(const std::string& document_uri, position pos) const override;
    lsp::completion_list_s completion(const std::string& document_uri,
        position pos,
        char trigger_char,
        completion_trigger_kind trigger_kind) const override;

    parse_result parse_library(const std::string& library, analyzing_context ctx, const library_data data) override;
    bool has_library(const std::string& library, const std::string& program) const override;
    const asm_option& get_asm_options(const std::string& file_name) override;
    const ws_uri& uri();

    void open();
    void close();

    void set_message_consumer(message_consumer* consumer);

    processor_file_ptr get_processor_file(const std::string& filename);

protected:
    file_manager& get_file_manager();

private:
    constexpr static char FILENAME_PROC_GRPS[] = "proc_grps.json";
    constexpr static char FILENAME_PGM_CONF[] = "pgm_conf.json";
    constexpr static char HLASM_PLUGIN_FOLDER[] = ".hlasmplugin";

    std::atomic<bool>* cancel_;

    std::string name_;
    ws_uri uri_;
    file_manager& file_manager_;

    std::unordered_map<proc_grp_id, processor_group> proc_grps_;
    std::map<std::string, program> exact_pgm_conf_;
    std::vector<std::pair<program, std::regex>> regex_pgm_conf_;
    processor_group implicit_proc_grp;

    std::filesystem::path ws_path_;
    std::filesystem::path proc_grps_path_;
    std::filesystem::path pgm_conf_path_;

    bool opened_ = false;


    bool load_and_process_config();
    // Loads the pgm_conf.json and proc_grps.json from disk, adds them to file_manager_ and parses both jsons.
    // Returns false if there is any error.
    bool load_config(config::proc_conf& proc_groups, config::pgm_conf& pgm_config, file_ptr& pgm_conf_file);

    bool is_wildcard(const std::string& str);

    // files, that depend on others (e.g. open code files that use macros)
    std::set<std::string> dependants_;

    diagnostic_container config_diags_;

    void filter_and_close_dependencies_(const std::set<std::string>& dependencies, processor_file_ptr file);
    bool is_dependency_(const std::string& file_uri);

    bool program_id_match(const std::string& filename, const program_id& program) const;

    std::vector<processor_file_ptr> find_related_opencodes(const std::string& document_uri) const;
    void delete_diags(processor_file_ptr file);

    void show_message(const std::string& message);

    message_consumer* message_consumer_ = nullptr;

    // A map that holds true values for files that have diags suppressed and the user was already notified about it
    std::unordered_map<std::string, bool> diag_suppress_notified_;
    const lib_config& global_config_;
    lib_config local_config_;
    lib_config get_config();
};

} // namespace hlasm_plugin::parser_library::workspaces

#endif // !HLASMPLUGIN_PARSERLIBRARY_WORKSPACE_H
