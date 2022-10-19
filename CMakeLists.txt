cmake_minimum_required(VERSION 3.16)
project(BedrockOptimizer)
set(CMAKE_CXX_STANDARD 17)
set(MOD_VERSION 0.2)
set(TEST_NUMBER 12)
set(GAME_VERSION 1.19.31.01)
option(DEV "Build as developer version" ON)
option(BETA "Build as beta version" OFF)
add_definitions(-DMOD_VERSION="${MOD_VERSION}")
add_definitions(-DMC_VERSION="${GAME_VERSION}")

if (${DEV} STREQUAL "ON")
    add_definitions(-DDEV)
    message("Build a developer version")
    set(TARGET_NAME beopt-dev)
else ()
    message("Build a Player version")
    if (${BETA} STREQUAL "OFF")
        message("Build beta version")
        set(TARGET_NAME beopt-${MOD_VERSION}-${GAME_VERSION}-beta-${TEST_NUMBER})
    else ()
        message("build stable version")
        set(TARGET_NAME beopt-${MOD_VERSION}-${GAME_VERSION})
    endif ()
endif ()

file(
        GLOB_RECURSE
        SRC_FILES_DIR
        ${PROJECT_SOURCE_DIR}/SDK/include/*.cpp
        ${PROJECT_SOURCE_DIR}/SDK/include/*.hpp
        ${PROJECT_SOURCE_DIR}/SDK/include/*.h
        src/dllmain.cpp
        src/pch.cpp
        src/Plugin.cpp
        src/OptCommand.cpp
        src/BetterBDSMod.cpp
        src/Config.cpp

        src/remotery/Remotery.c
        src/opts/RandomTickOpt.cpp
        src/opts/HopperOpt.cpp
        src/opts/ActorOpt.cpp
        src/opts/RedstoneOpt.cpp
        src/opts/EntitySystemOpt.cpp
)

include_directories(SDK/include)
include_directories(src/remotery)
#include_directories(SDK/Header/third-party)
include_directories(src/include)
link_directories(.)

add_definitions(-D"NDEBUG" -D"TEMPLATE_EXPORTS" -D"WIN32_LEAN_AND_MEAN" -D"_CRT_SECURE_NO_WARNINGS" -D"_WINDOWS"
        -D"_USRDLL" -D"_AMD64_" -D"NOMINMAX" -D"_WINDLL" -D"_UNICODE" -D"UNICODE")

add_compile_options(
        /utf-8
        /permissive- /GS /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /sdl /Zc:inline /fp:precise /errorReport:prompt /WX-
        /Zc:forScope /Gd /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:column
)

add_link_options(
        /DELAYLOAD:bedrock_server.dll
        /MANIFEST /LTCG:incremental /NXCOMPAT /DEBUG /DLL /MACHINE:X64 /OPT:REF /INCREMENTAL:NO /SUBSYSTEM:WINDOWS
        /MANIFESTUAC:NO /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO /TLBID:1
)

add_library(${TARGET_NAME} SHARED ${SRC_FILES_DIR})

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
        COMMAND cmd /c ${PROJECT_SOURCE_DIR}/prepareLib.cmd ${PROJECT_SOURCE_DIR}
        COMMENT "Preparing Library"
        )
