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

#include "dap_session_manager.h"

#include "dap_message_wrappers.h"

namespace {
std::optional<size_t> is_registration_request(std::string_view method, const nlohmann::json& msg)
{
    if (method != hlasm_plugin::language_server::dap::broadcom_tunnel_method)
        return std::nullopt;
    auto params = msg.find("params");
    if (params == msg.end())
        return std::nullopt;
    if (!params->is_number_unsigned())
        return std::nullopt;
    return params->get<size_t>();
}
std::string_view extract_method(const nlohmann::json& msg)
{
    auto method_it = msg.find("method");
    if (method_it == msg.end() || !method_it->is_string())
        return {};
    return method_it->get<std::string_view>();
}
} // namespace

namespace hlasm_plugin::language_server::dap {
void session_manager::cleanup_sessions()
{
    for (auto it = sessions.begin(); it != sessions.end();)
    {
        if (it->second->is_running())
            ++it;
        else
            it = sessions.erase(it);
    }
}
void session_manager::handle_registration_request(size_t new_id)
{
    cleanup_sessions();
    if (!sessions.size())
    {
        // currently only one debug session is supported
        auto new_session = std::make_unique<dap::session>(new_id, *cancel, *ws_mngr, *out_stream);
        sessions.try_emplace(new_session->get_session_id(), std::move(new_session));
    }
    else
    {
        /* ignore for now */
    }
}
session_manager::session_manager(
    std::atomic<bool>& c, hlasm_plugin::parser_library::workspace_manager& ws, json_sink& out)
    : cancel(&c)
    , ws_mngr(&ws)
    , out_stream(&out)
{}

// Inherited via json_sink

void session_manager::write(const nlohmann::json& msg)
{
    auto method = extract_method(msg);
    if (method.empty())
        return;
    if (auto new_id = is_registration_request(method, msg); new_id.has_value())
        handle_registration_request(new_id.value());
    else
    {
        auto it = sessions.find(method);
        if (it != sessions.end())
            it->second->write(msg);
    }
}
void session_manager::write(nlohmann::json&& msg)
{
    auto method = extract_method(msg);
    if (method.empty())
        return;
    if (auto new_id = is_registration_request(method, msg); new_id.has_value())
        handle_registration_request(new_id.value());
    else
    {
        auto it = sessions.find(method);
        if (it != sessions.end())
            it->second->write(std::move(msg));
    }
}
message_router::message_predicate session_manager::get_filtering_predicate() const
{
    return [](const nlohmann::json& msg) {
        auto method = extract_method(msg);
        return method.substr(0, dap::broadcom_tunnel_method.size()) == dap::broadcom_tunnel_method;
    };
}
} // namespace hlasm_plugin::language_server::dap
