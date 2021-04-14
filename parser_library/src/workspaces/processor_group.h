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

#ifndef HLASMPLUGIN_PARSERLIBRARY_PROCESSOR_GROUP_H
#define HLASMPLUGIN_PARSERLIBRARY_PROCESSOR_GROUP_H

#include "config/proc_conf.h"
#include "diagnosable_impl.h"
#include "library.h"

namespace hlasm_plugin::parser_library::workspaces {

// Represents a named set of libraries (processor_group)
class processor_group : public diagnosable_impl
{
public:
    processor_group(const std::string& name, const config::assembler_options& asm_options)
        : name_(name)
        , asm_optns { asm_options.sysparm, asm_options.profile }
    {}

    void collect_diags() const override
    {
        for (auto&& lib : libs_)
        {
            collect_diags_from_child(*lib);
        }
    }

    void add_library(std::unique_ptr<library> library) { libs_.push_back(std::move(library)); }

    const std::string& name() const { return name_; }

    const std::vector<std::unique_ptr<library>>& libraries() const { return libs_; }

    const asm_option& asm_options() const { return asm_optns; }


private:
    std::vector<std::unique_ptr<library>> libs_;
    std::string name_;
    asm_option asm_optns;
};
} // namespace hlasm_plugin::parser_library::workspaces
#endif // !HLASMPLUGIN_PARSERLIBRARY_PROCESSOR_GROUP_H
