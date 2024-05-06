# SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
# SPDX-License-Identifier: Boost Software License - Version 1.0

find_library(LIB_PX libpx.a HINTS ${pxros_root_dir}/lib/tc162 NO_DEFAULT_PATH)

if(LIB_PX STREQUAL LIB_PX-NOTFOUND)
    message(FATAL_ERROR "Could not find PXROS installation in '${pxros_root_dir}'.")
endif()

find_library(LIB_PXN libnameserver.a HINTS ${pxros_utils_dir}/lib/tc162 NO_DEFAULT_PATH)

if(LIB_PXN STREQUAL LIB_PXN-NOTFOUND)
    message(FATAL_ERROR "Could not find PXROS utilities installation in '${pxros_utils_dir}'.")
endif()

set(PXROS_INCLUDE_DIR
    "${pxros_root_dir}/include"
    "${pxros_utils_dir}/include")

set(PXROS_LIB_PATH
    "${pxros_root_dir}/lib/tc162"
    "${pxros_root_dir}/lib/ldscripts"
    "${pxros_utils_dir}/lib/tc162")

set(PXROS_LIBS "nameserver;pxros;px;pxcon")

set_property(GLOBAL PROPERTY PXROS_INCLUDES ${PXROS_INCLUDE_DIR})
set_property(GLOBAL PROPERTY PXROS_LIBS ${PXROS_LIBS})
set_property(GLOBAL PROPERTY PXROS_LIB_PATH ${PXROS_LIB_PATH})
