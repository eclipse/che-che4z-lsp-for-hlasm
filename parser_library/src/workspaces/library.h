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

#ifndef HLASMPLUGIN_PARSERLIBRARY_LIBRARY_H
#define HLASMPLUGIN_PARSERLIBRARY_LIBRARY_H

#include <memory>
#include <string>

#include "diagnosable.h"

namespace hlasm_plugin::parser_library::workspaces {

class processor;

class library : public virtual diagnosable
{
public:
    virtual std::shared_ptr<processor> find_file(const std::string& file) = 0;
    virtual void refresh() = 0;

private:
};

} // namespace hlasm_plugin::parser_library::workspaces
#endif
