# SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
# SPDX-License-Identifier: Boost Software License - Version 1.0
cmake_minimum_required(VERSION 3.19.1)

project(os)

file( GLOB_RECURSE SRCS *.c *.h)

add_library(${PROJECT_NAME} STATIC ${SRCS})
# target_include_directories(${PROJECT_NAME} PUBLIC ../tspi)