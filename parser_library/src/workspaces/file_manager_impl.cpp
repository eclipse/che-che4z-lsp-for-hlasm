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

#include "file_manager_impl.h"

#include <map>

#include "processor_file_impl.h"
#include "utils/path.h"
#include "utils/platform.h"

namespace hlasm_plugin::parser_library::workspaces {

void file_manager_impl::collect_diags() const
{
    for (auto& it : files_)
    {
        collect_diags_from_child(*it.second);
    }
}

file_ptr file_manager_impl::add_file(const file_uri& uri)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.emplace(uri, std::make_shared<file_impl>(uri));
    return ret.first->second;
}

processor_file_ptr file_manager_impl::change_into_processor_file_if_not_already_(std::shared_ptr<file_impl>& to_change)
{
    auto processor = std::dynamic_pointer_cast<processor_file>(to_change);
    if (processor)
        return processor;
    else
    {
        auto proc_file = std::make_shared<processor_file_impl>(std::move(*to_change), cancel_);
        to_change = proc_file;
        return proc_file;
    }
}

processor_file_ptr file_manager_impl::add_processor_file(const file_uri& uri)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.find(uri);
    if (ret == files_.end())
    {
        auto ptr = std::make_shared<processor_file_impl>(uri, cancel_);
        files_.emplace(uri, ptr);
        return ptr;
    }
    else
        return change_into_processor_file_if_not_already_(ret->second);
}

processor_file_ptr file_manager_impl::get_processor_file(const file_uri& uri)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.find(uri);
    if (ret == files_.end())
    {
        return std::make_shared<processor_file_impl>(uri);
    }
    else
        return change_into_processor_file_if_not_already_(ret->second);
}

void file_manager_impl::remove_file(const file_uri& document_uri)
{
    std::lock_guard guard(files_mutex);
    auto file = files_.find(document_uri);
    if (file == files_.end())
        return;

    // close the file internally
    files_.erase(document_uri);
}

file_ptr file_manager_impl::find(const std::string& key)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.find(key);
    if (ret == files_.end())
        return nullptr;

    return ret->second;
}

processor_file_ptr file_manager_impl::find_processor_file(const std::string& key)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.find(key);
    if (ret == files_.end())
        return nullptr;

    return change_into_processor_file_if_not_already_(ret->second);
}


std::vector<processor_file*> file_manager_impl::list_updated_files()
{
    std::vector<processor_file*> list;
    for (auto& item : files_)
    {
        auto p = dynamic_cast<processor_file*>(item.second.get());
        if (p && p->parse_info_updated())
            list.push_back(p);
    }
    return list;
}

list_directory_result file_manager_impl::list_directory_files(const std::string& path)
{
    std::filesystem::path lib_p(path);
    list_directory_result result;

    result.second = utils::path::list_directory_regular_files(lib_p, [&result](const std::filesystem::path& f) {
        result.first[utils::path::filename(f).string()] = utils::path::absolute(f).string();
    });
    return result;
}

void file_manager_impl::prepare_file_for_change_(std::shared_ptr<file_impl>& file)
{
    if (file.unique())
        return;
    // another shared ptr to this file exists, we need to create a copy
    auto proc_file = std::dynamic_pointer_cast<processor_file>(file);
    if (proc_file)
        file = std::make_shared<processor_file_impl>(*file, cancel_);
    else
        file = std::make_shared<file_impl>(*file);
}

void file_manager_impl::did_open_file(const std::string& document_uri, version_t version, std::string text)
{
    std::lock_guard guard(files_mutex);
    auto ret = files_.emplace(document_uri, std::make_shared<file_impl>(document_uri));
    prepare_file_for_change_(ret.first->second);
    ret.first->second->did_open(std::move(text), version);
}

void file_manager_impl::did_change_file(
    const std::string& document_uri, version_t, const document_change* changes, size_t ch_size)
{
    // the version is the version after the changes -> I dont see how is that useful
    // should we just overwrite the version??
    // on the other hand, the spec clearly specifies that each change increments version by one.

    std::lock_guard guard(files_mutex);

    auto file = files_.find(document_uri);
    if (file == files_.end())
        return; // if the file does not exist, no action is taken

    prepare_file_for_change_(file->second);

    for (size_t i = 0; i < ch_size; ++i)
    {
        std::string text_s(changes[i].text, changes[i].text_length);
        if (changes[i].whole)
            file->second->did_change(std::move(text_s));
        else
            file->second->did_change(changes[i].change_range, std::move(text_s));
    }
}

void file_manager_impl::did_close_file(const std::string& document_uri)
{
    std::lock_guard guard(files_mutex);
    auto file = files_.find(document_uri);
    if (file == files_.end())
        return;

    prepare_file_for_change_(file->second);
    // close the file externally
    file->second->did_close();

    // if the file does not exist, no action is taken
}

bool file_manager_impl::file_exists(const std::string& file_name)
{
    std::error_code ec;
    return std::filesystem::exists(file_name, ec);
    // TODO use error code??
}

bool file_manager_impl::lib_file_exists(const std::string& lib_path, const std::string& file_name)
{
    return std::filesystem::exists(utils::path::join(lib_path, file_name));
}

} // namespace hlasm_plugin::parser_library::workspaces
