# Copyright (c) 2019 Broadcom.
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

PROJECT(JSON_fetcher)

INCLUDE(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.3.0
  GIT_SHALLOW    ON
  LOG_DOWNLOAD   ON
  GIT_PROGRESS   1
)

set(JSON_BuildTests Off)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  set(JSON_MultipleHeaders On)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


