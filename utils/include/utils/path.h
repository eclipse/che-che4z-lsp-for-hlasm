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

#ifndef HLASMPLUGIN_UTILS_PATH_H
#define HLASMPLUGIN_UTILS_PATH_H

#include <filesystem>
#include <functional>
#include <string>

#include "list_directory_rc.h"

namespace hlasm_plugin::utils::path {

bool is_relative(const std::filesystem::path&);
bool is_absolute(const std::filesystem::path&);

std::filesystem::path absolute(const std::filesystem::path& p);
std::filesystem::path join(const std::filesystem::path& left, const std::filesystem::path& right);
std::filesystem::path lexically_normal(const std::filesystem::path& p);
std::filesystem::path lexically_relative(const std::filesystem::path& p, std::string q);
std::filesystem::path filename(const std::filesystem::path& p);
bool equal(const std::filesystem::path& left, const std::filesystem::path& right);

list_directory_rc list_directory_regular_files(
    const std::filesystem::path& d, std::function<void(const std::filesystem::path&)> h);
} // namespace hlasm_plugin::utils::path

#endif
