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

#ifndef HLASMPLUGIN_UTILS_LIST_DIRECTORY_RC_H
#define HLASMPLUGIN_UTILS_LIST_DIRECTORY_RC_H

namespace hlasm_plugin::utils::path {
enum class list_directory_rc
{
    done,
    not_exists,
    not_a_directory,
    other_failure,
};
} // namespace hlasm_plugin::utils::path

#endif
