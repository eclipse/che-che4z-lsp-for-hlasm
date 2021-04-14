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

#include "lib_config.h"

#include "config/pgm_conf.h"
#include "nlohmann/json.hpp"

namespace hlasm_plugin::parser_library {

const lib_config lib_config::default_config = lib_config::make_default();

lib_config lib_config::make_default()
{
    lib_config def_config;
    def_config.diag_supress_limit = 10;

    return def_config;
}

lib_config lib_config::load_from_json(const nlohmann::json& config)
{
    lib_config loaded;

    auto found = config.find("diagnosticsSuppressLimit");
    if (found != config.end())
    {
        loaded.diag_supress_limit = found->get<int64_t>();
        if (loaded.diag_supress_limit < 0)
            loaded.diag_supress_limit = 0;
    }


    return loaded;
}

lib_config lib_config::load_from_json(const config::pgm_conf& config)
{
    lib_config loaded;

    if (config.diagnostics_suppress_limit.has_value())
        loaded.diag_supress_limit = config.diagnostics_suppress_limit.value();

    return loaded;
}

lib_config lib_config::fill_missing_settings(const lib_config& second)
{
    return combine_two_configs(second).combine_two_configs(default_config);
}

lib_config lib_config::combine_two_configs(const lib_config& second) const
{
    lib_config combined(*this);
    if (!combined.diag_supress_limit.has_value())
        combined.diag_supress_limit = second.diag_supress_limit;
    return combined;
}

bool operator==(const lib_config& lhs, const lib_config& rhs)
{
    return lhs.diag_supress_limit == rhs.diag_supress_limit;
}

} // namespace hlasm_plugin::parser_library
