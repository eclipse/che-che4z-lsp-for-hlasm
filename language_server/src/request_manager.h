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

#ifndef HLASMPLUGIN_LANGUAGESERVER_REQUEST_MANAGER_H
#define HLASMPLUGIN_LANGUAGESERVER_REQUEST_MANAGER_H
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include "server.h"


namespace hlasm_plugin::language_server {

// Represents one LSP or DAP message received by LSP/DAP server
struct request
{
    request(json message, server* executing_server);
    json message;
    bool valid;
    server* executing_server;
};

// Holds and orders income messages(requests) from DAP and LSP.
// The requests are held in a queue.
// Runs a worker thread, that uses respectable server to execute
// requests
class request_manager
{
public:
    enum class async_policy
    {
        ASYNC,
        SYNC
    };

    explicit request_manager(std::atomic<bool>* cancel, async_policy async_pol = async_policy::ASYNC);
    void add_request(server* server, json message);
    void finish_server_requests(server* server);
    void end_worker();
    bool is_running() const;

private:
    std::atomic<bool> end_worker_;

    // request_manager uses conditional variable to put the
    // worker thread asleep when the request queue is empty
    mutable std::mutex q_mtx_;
    std::condition_variable cond_;

    // the request manager invalidates older requests on the
    // same file, when a new request to the same file comes
    std::string currently_running_file_;
    std::atomic<server*> currently_running_server_ = nullptr;

    void handle_request_(const std::atomic<bool>* end_loop);
    std::string get_request_file_(const json& r, bool* is_parsing_required = nullptr) const;

    std::deque<request> requests_;

    // cancellation token that is used to stop current parsing
    // when it was obsoleted by a new request
    std::atomic<bool>* cancel_;

    std::thread worker_;

    async_policy async_policy_;
};


} // namespace hlasm_plugin::language_server

#endif
