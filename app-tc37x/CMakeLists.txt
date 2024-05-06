cmake_minimum_required(VERSION 3.19.1)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/tricore-toolchain.cmake)
project(tc37x_pxros_bsp_example LANGUAGES CXX C ASM )

# xtask configuration done via cargo.
#
# Paths must be normalized to avoid issues with backslashes on Windows.
cmake_path(SET pxros_root_dir   NORMALIZE $ENV{PXROS_ROOT_PATH})
cmake_path(SET pxros_utils_dir  NORMALIZE $ENV{PXROS_UTILS})
cmake_path(SET illd_base        NORMALIZE $ENV{ILLD_BASE})
cmake_path(SET illd_precompiled NORMALIZE $ENV{ILLD_PRECOMPILED})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_C_FLAGS_RELEASE "-Os")
set(CMAKE_C_FLAGS_DEBUG "-O2 -g")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Os -g")

# the used architecure
set(ARCH tc162)

# compiler options for all targets
add_compile_options(
    -march=${ARCH}
    -ffunction-sections
    -fno-exceptions
    -fno-unwind-tables
    -gdwarf-3
    )

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(PXROS)
get_property(px_includes GLOBAL PROPERTY PXROS_INCLUDES)
get_property(px_libs GLOBAL PROPERTY PXROS_LIBS)
get_property(px_lib_paths GLOBAL PROPERTY PXROS_LIB_PATH)

link_directories(ld ${px_lib_paths})

set(BOARD LITEKIT_TC375_V2_X)
set(UC_NB_CORES 3)

set(LDSCRIPT_DIRECTORIES
    ${px_lib_paths}
    ${CMAKE_SOURCE_DIR}/ld
    ${CMAKE_SOURCE_DIR}/pxros/config
    ${CMAKE_SOURCE_DIR}/pxros/tasks/InitTask
    ${CMAKE_SOURCE_DIR}/pxros/tasks/LedClientCpp
)
foreach(dir ${LDSCRIPT_DIRECTORIES})
    list(APPEND LDINCL -I${dir})
endforeach()

file ( GLOB LDSCRIPT ${CMAKE_SOURCE_DIR}/ld/tc37x_pxros_bsp_example.ld.tmpl)
add_custom_target(
    tc37x_pxros_bsp_example.ld ALL
    COMMAND ${CMAKE_C_COMPILER} -E -P -C -DUC_NB_CORES=${UC_NB_CORES} -march=${ARCH} "-I$<JOIN:${LDSCRIPT_DIRECTORIES},;-I>" -x c ${LDSCRIPT} -o tc37x_pxros_bsp_example.ld
    SOURCES ${LDSCRIPT}
    COMMAND_EXPAND_LISTS
)

file ( GLOB_RECURSE ILLDSRCS 
    ${illd_base}/iLLD/*.c 
    ${illd_base}/Service/*.c 
    ${illd_base}/iLLD/*.c 
    ${illd_base}/Infra/Platform/*.c 
    ${illd_base}/Infra/Sfr/*.c
)
file ( GLOB_RECURSE SRCS pxros/*.c pxros/*.cpp src/*.c)
file ( GLOB_RECURSE LIBSRCS bsp/*c crt0/*.S machine/*.c)
file ( GLOB_RECURSE LIBOS libos/*.c)

# Only when building the iLLD from sources
if (DEFINED ENV{BUILD_ILLD})
    add_library(illd STATIC ${ILLDSRCS})
    
    target_include_directories(illd PRIVATE
        illd/illd_cfg
        ${illd_base}/iLLD/TC37A/Tricore
        ${illd_base}/Infra/Platform
        ${illd_base}/Service/CpuGeneric
        ${illd_base}/Infra/Sfr/TC37A/_Reg
        ${illd_base}/iLLD/TC37A/Tricore/Gtm/Atom/PwmHl
        ${illd_base}/iLLD/TC37A/Tricore/Gtm/Tom/PwmHl
    )

    target_compile_definitions(illd PUBLIC
        -DNO_WARN_MACHINE_INTRINSICS
        -D__TC37XX__
        -DTC37XB
        -D${BOARD}
        -DBSP_ISR_SUPPORT_DISABLED
        -DUC_NB_CORES=${UC_NB_CORES}
    )
endif()

add_library(bsp STATIC ${LIBSRCS} )
target_include_directories(bsp PRIVATE
    bsp bsp/uc bsp/uc/uc_tc37 bsp/uc/uc_tc37/TC37xB_RegCHeaders/_Reg
    crt0
    machine
    .
    )
target_compile_definitions(bsp PUBLIC
    -DNO_WARN_MACHINE_INTRINSICS
    -D__TC37XX__
    -DTC37XB
    -D${BOARD}
    -DBSP_ISR_SUPPORT_DISABLED
    -DUC_NB_CORES=${UC_NB_CORES}
)

add_library(os STATIC ${LIBOS})
target_include_directories(os PRIVATE
    libos
    .
)
target_compile_definitions(os PUBLIC
    -DNO_WARN_MACHINE_INTRINSICS
    -D__TC37XX__
    -DTC37XB
    -D${BOARD}
    -DBSP_ISR_SUPPORT_DISABLED
    -DUC_NB_CORES=${UC_NB_CORES}
)

add_library(app STATIC ${SRCS} )
target_include_directories(app PRIVATE
    bsp bsp/uc bsp/uc/uc_tc37 bsp/uc/uc_tc37/TC37xB_RegCHeaders/_Reg
    crt0
    machine
    .
    illd/illd_cfg
    pxros/tasks/GETHDriver/GETHDriver/include
    pxros/tasks/GETHDriver/GETHDriver/lib
    pxros/tasks/GETHDriver/pxnet/include
    pxros/tasks/GETHDriver/pxnet/DriverTask/include
    ${illd_base}/iLLD/TC37A/Tricore
    ${illd_base}/Infra/Platform
    ${illd_base}/Service/CpuGeneric
    ${illd_base}/Infra/Sfr/TC37A/_Reg
    ${px_includes})

target_compile_definitions(app PUBLIC
    -DNO_WARN_MACHINE_INTRINSICS
    -D__TC37XX__
    -DTC37XB
    -D${BOARD}
    -DBSP_ISR_SUPPORT_DISABLED
    -DUC_NB_CORES=${UC_NB_CORES}
)
add_dependencies(app tc37x_pxros_bsp_example.ld)

file(WRITE null.c "")
add_executable(${PROJECT_NAME}.elf null.c )
target_link_libraries(${PROJECT_NAME}.elf PUBLIC app bsp os illd)

# If BUILD_ILLD is not set we use the precompiled librarty
if (NOT DEFINED ENV{BUILD_ILLD})
    target_link_directories(${PROJECT_NAME}.elf BEFORE PUBLIC ${illd_precompiled})
endif()

# set the build type of the rust application values: debug/release
set(RUST_BUILD_TYPE debug)

# This picks the linked rust library for the executor folder and configure
# all linker stuff
target_link_directories(${PROJECT_NAME}.elf BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/ld PUBLIC ${px_lib_paths})
target_link_options(${PROJECT_NAME}.elf BEFORE PUBLIC -e _crt0_reset -march=${ARCH} -fno-exceptions -Wl,-T,${CMAKE_SOURCE_DIR}/ld/tc37xB_memory.ld,--gc-sections,-Map,${PROJECT_NAME}.map,-n)

# Specify --whole-archive to tell the linker to include _all_ content in the output file
#
# This is needed for automatic deployment to work
target_link_libraries(${PROJECT_NAME}.elf PUBLIC -Wl,--whole-archive rust_tasks -Wl,--no-whole-archive nameserver pxhndcall pxros px pxcon)

add_dependencies(${PROJECT_NAME}.elf tc37x_pxros_bsp_example.ld)
