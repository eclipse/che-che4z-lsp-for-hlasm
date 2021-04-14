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

#ifndef HLASMPLUGIN_HLASMLANGUAGESERVER_SERVER_STREAMS_H
#define HLASMPLUGIN_HLASMLANGUAGESERVER_SERVER_STREAMS_H

#include <memory>

#include "json_channel.h"

namespace hlasm_plugin::language_server {
class server_streams
{
public:
    virtual ~server_streams() = default;
    virtual json_sink& get_response_stream() & = 0;
    virtual json_source& get_request_stream() & = 0;

    static std::unique_ptr<server_streams> create(int argc, char** argv);
};
} // namespace hlasm_plugin::language_server

#endif // !HLASMPLUGIN_HLASMLANGUAGESERVER_SERVER_STREAMS_H
