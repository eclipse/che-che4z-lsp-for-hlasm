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

#ifndef HLASMPARSER_PARSERLIBRARY_CONFIG_PGM_CONF_H
#define HLASMPARSER_PARSERLIBRARY_CONFIG_PGM_CONF_H

#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

namespace hlasm_plugin::parser_library::config {

struct program_mapping
{
    std::string program;
    std::string pgroup;
};
void to_json(nlohmann::json& j, const program_mapping& p);
void from_json(const nlohmann::json& j, program_mapping& p);

struct pgm_conf
{
    std::vector<program_mapping> pgms;
    std::vector<std::string> always_recognize;
    std::optional<unsigned> diagnostics_suppress_limit;
};
void to_json(nlohmann::json& j, const pgm_conf& p);
void from_json(const nlohmann::json& j, pgm_conf& p);

} // namespace hlasm_plugin::parser_library::config

#endif // HLASMPARSER_PARSERLIBRARY_CONFIG_PGM_CONF_H
