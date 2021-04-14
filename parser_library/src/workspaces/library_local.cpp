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

#include "library_local.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <locale>
#include <regex>

#include "utils/path.h"
#include "utils/platform.h"
#include "wildcard.h"

namespace hlasm_plugin::parser_library::workspaces {

namespace {
void adjust_extensions_vector(std::vector<std::string>& extensions, bool extensions_from_deprecated_source)
{
    bool contains_empty = false;
    for (auto& ext : extensions)
    {
        if (ext.empty())
            contains_empty = true;
        else
        {
            if (ext[0] != '.')
                ext.insert(0, 1, '.');
        }
    }
    // from longest to shortest, then lexicographically
    std::sort(extensions.begin(), extensions.end(), [](const std::string& l, const std::string& r) {
        if (l.size() > r.size())
            return true;
        if (l.size() < r.size())
            return false;
        return l < r;
    });
    if (extensions_from_deprecated_source && !contains_empty)
        extensions.emplace_back(); // alwaysRecognize always implied accepting files without an extension
    extensions.erase(std::unique(extensions.begin(), extensions.end()), extensions.end());
}
} // namespace

library_local::library_local(file_manager& file_manager, std::string lib_path, library_local_options options)
    : file_manager_(file_manager)
    , lib_path_(std::move(lib_path))
    , extensions_(std::move(options.extensions))
    , optional_(options.optional_library)
    , extensions_from_deprecated_source(options.extensions_from_deprecated_source)
{
    if (extensions_.size())
        adjust_extensions_vector(extensions_, extensions_from_deprecated_source);
}

library_local::library_local(library_local&& l) noexcept
    : file_manager_(l.file_manager_)
    , lib_path_(std::move(l.lib_path_))
    , files_(std::move(l.files_))
    , extensions_(std::move(l.extensions_))
    , files_loaded_(l.files_loaded_)
    , optional_(l.optional_)
    , extensions_from_deprecated_source(l.extensions_from_deprecated_source)
{}

void library_local::collect_diags() const
{
    // does not have any diagnosable children
}

void library_local::refresh()
{
    files_.clear();
    load_files();
}

const std::string& library_local::get_lib_path() const { return lib_path_; }

std::shared_ptr<processor> library_local::find_file(const std::string& file_name)
{
    if (!files_loaded_)
        load_files();

    if (auto found = files_.find(file_name); found != files_.end())
        return file_manager_.add_processor_file(utils::path::join(lib_path_, found->second).string());
    else
        return nullptr;
}

void library_local::load_files()
{
    auto [files_list, rc] = file_manager_.list_directory_files(lib_path_);
    files_.clear();
    diags().clear();

    switch (rc)
    {
        case hlasm_plugin::utils::path::list_directory_rc::done:
            break;
        case hlasm_plugin::utils::path::list_directory_rc::not_exists:
            if (!optional_)
                add_diagnostic(diagnostic_s::error_L0002(lib_path_));
            break;
        case hlasm_plugin::utils::path::list_directory_rc::not_a_directory:
            add_diagnostic(diagnostic_s::error_L0002(lib_path_));
            break;
        case hlasm_plugin::utils::path::list_directory_rc::other_failure:
            add_diagnostic(diagnostic_s::error_L0001(lib_path_));
            break;
    }

    bool extension_removed = false;
    for (const auto& file : files_list)
    {
        if (extensions_.empty())
        {
            files_[context::to_upper_copy(file.first)] = file.second;
            continue;
        }

        for (const auto& extension : extensions_)
        {
            std::string_view filename(file.first);

            if (filename.size() <= extension.size())
                continue;

            if (filename.substr(filename.size() - extension.size()) != extension)
                continue;
            filename.remove_suffix(extension.size());

            const auto [_, inserted] = files_.try_emplace(context::to_upper_copy(std::string(filename)), file.second);
            // TODO: the stored value is a full path, yet we try to interpret it as a relative one later on
            if (!inserted)
                add_diagnostic(diagnostic_s::warning_L0004(lib_path_, context::to_upper_copy(std::string(filename))));

            if (extension.size())
                extension_removed = true;
            break;
        }
    }
    if (extension_removed && extensions_from_deprecated_source)
        add_diagnostic(diagnostic_s::warning_L0003(lib_path_));

    files_loaded_ = true;
}

} // namespace hlasm_plugin::parser_library::workspaces
