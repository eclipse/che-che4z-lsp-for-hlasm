# Copyright (c) 2021 Broadcom.
# The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
#
# This program and the accompanying materials are made
# available under the terms of the Eclipse Public License 2.0
# which is available at https://www.eclipse.org/legal/epl-2.0/
#
# SPDX-License-Identifier: EPL-2.0
#
# Contributors:
#   Broadcom, Inc. - initial API and implementation

target_sources(parser_library PRIVATE
	file.h
	file_impl.cpp
	file_impl.h
	file_manager.h
	file_manager_impl.cpp
	file_manager_impl.h
	library.h
	library_local.cpp
	library_local.h
	parse_lib_provider.cpp
	parse_lib_provider.h
	processor.h
	processor_file_impl.cpp
	processor_file_impl.h
	processor_group.h
	wildcard.cpp
	wildcard.h
	workspace.cpp
	workspace.h
)

