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

#include "pgm_conf.h"

#include "nlohmann/json.hpp"

namespace hlasm_plugin::parser_library::config {

void to_json(nlohmann::json& j, const program_mapping& p)
{
    j = nlohmann::json { { "program", p.program }, { "pgroup", p.pgroup } };
}

void from_json(const nlohmann::json& j, program_mapping& p)
{
    j.at("program").get_to(p.program);
    j.at("pgroup").get_to(p.pgroup);
}

void to_json(nlohmann::json& j, const pgm_conf& p)
{
    j = nlohmann::json { { "pgms", p.pgms } };
    if (p.always_recognize.size())
        j["alwaysRecognize"] = p.always_recognize;
    if (p.diagnostics_suppress_limit.has_value())
        j["diagnosticsSuppressLimit"] = p.diagnostics_suppress_limit.value();
}

void from_json(const nlohmann::json& j, pgm_conf& p)
{
    j.at("pgms").get_to(p.pgms);
    if (auto it = j.find("alwaysRecognize"); it != j.end())
        it->get_to(p.always_recognize);
    if (auto it = j.find("diagnosticsSuppressLimit"); it != j.end())
        p.diagnostics_suppress_limit = it->get<unsigned>();
}

} // namespace hlasm_plugin::parser_library::config
