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

#ifndef HLASMPLUGIN_PARSERLIBRARY_LIB_CONFIG_H
#define HLASMPLUGIN_PARSERLIBRARY_LIB_CONFIG_H

#include <memory>
#include <optional>

#include "nlohmann/json_fwd.hpp"
#include "parser_library_export.h"

namespace hlasm_plugin::parser_library {
namespace config {
struct pgm_conf;
}

// Encapsulates user defined settings of library and individual workspaces
class PARSER_LIBRARY_EXPORT lib_config
{
public:
    // Creates an instance of lib_config with values from input json.
    [[nodiscard]] static lib_config load_from_json(const nlohmann::json& config);
    [[nodiscard]] static lib_config load_from_json(const config::pgm_conf& config);
    // Returns a lib_config instance that is a copy of this instance, but the missing settings are replaced with the
    // parameter second. If there are more missing settings after that step, they are filled with default values
    [[nodiscard]] lib_config fill_missing_settings(const lib_config& second);

    std::optional<int64_t> diag_supress_limit;



private:
    // Returns an instance that has missing settings of this filled with not missing setting of the parameter
    [[nodiscard]] lib_config combine_two_configs(const lib_config& second) const;

    // Return instance of lib_config with default values.
    static lib_config make_default();
    const static lib_config default_config;
};

bool operator==(const lib_config& lhs, const lib_config& rhs);


} // namespace hlasm_plugin::parser_library



#endif // HLASMPLUGIN_PARSERLIBRARY_LIB_CONFIG_H
