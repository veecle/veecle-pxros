# SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
# SPDX-License-Identifier: Boost Software License - Version 1.0
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR tricore)

set(target tricore)

if (CMAKE_HOST_WIN32)
    set (EXE .exe)
    set(HIGHTEC_BASE_PATH C:/HighTec/toolchains/tricore/v8.1.2/bin)
endif()
if (CMAKE_HOST_UNIX)
    set(HIGHTEC_BASE_PATH /opt/toolchains/tricore/v8.1.2/bin)
endif()

set(CMAKE_C_COMPILER ${HIGHTEC_BASE_PATH}/clang${EXE})
set(CMAKE_C_FLAGS -march=tc162)
set(CMAKE_C_COMPILER_TARGET ${target})
set(CMAKE_CXX_COMPILER ${HIGHTEC_BASE_PATH}/clang${EXE})
set(CMAKE_CXX_FLAGS -march=tc162)
set(CMAKE_CXX_COMPILER_TARGET ${target})
set(CMAKE_ASM_COMPILER ${HIGHTEC_BASE_PATH}/clang${EXE})
set(CMAKE_ASM_FLAGS -march=tc162)

set(OBJCOPY ${HIGHTEC_BASE_PATH}/llvm-objcopy${EXE})
set(AR ${HIGHTEC_BASE_PATH}/llvm-ar.exe)
set(HEX_CAT srec_cat)
