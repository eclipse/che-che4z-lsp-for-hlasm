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

#ifndef HLASMPLUGIN_PARSERLIBRARY_LOCAL_LIBRARY_H
#define HLASMPLUGIN_PARSERLIBRARY_LOCAL_LIBRARY_H

#include <string>
#include <unordered_map>
#include <vector>

#include "diagnosable_impl.h"
#include "file_manager.h"
#include "library.h"

namespace hlasm_plugin::parser_library::workspaces {

#pragma warning(push)
#pragma warning(disable : 4250)

struct library_local_options
{
    std::vector<std::string> extensions;
    bool extensions_from_deprecated_source = false;
    bool optional_library = false;
};

// library holds absolute path to a directory and finds macro files in it
class library_local final : public library, public diagnosable_impl
{
public:
    // takes reference to file manager that provides access to the files
    // and normalised path to directory that it wraps.
    library_local(file_manager& file_manager, std::string lib_path, library_local_options options);

    library_local(const library_local&) = delete;
    library_local& operator=(const library_local&) = delete;

    library_local(library_local&&) noexcept;

    void collect_diags() const override;

    const std::string& get_lib_path() const;

    std::shared_ptr<processor> find_file(const std::string& file) override;

    void refresh() override;

    bool is_once_only() const override { return false; }

private:
    file_manager& file_manager_;

    std::string lib_path_;
    std::unordered_map<std::string, std::string> files_;
    std::vector<std::string> extensions_;
    // indicates whether load_files function was called (not whether it was succesful)
    bool files_loaded_ = false;
    bool optional_ = false;
    bool extensions_from_deprecated_source = false;

    void load_files();
};
#pragma warning(pop)

} // namespace hlasm_plugin::parser_library::workspaces
#endif
