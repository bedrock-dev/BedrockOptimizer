cmake_minimum_required(VERSION 3.16)
project(BetterBDS)
set(CMAKE_CXX_STANDARD 17)

set(TARGET_NAME BedrockOptimizer)
file(
        GLOB_RECURSE
        SRC_FILES_DIR
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.cpp
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.hpp
        ${PROJECT_SOURCE_DIR}/SDK/Header/*.h
        src/dllmain.cpp
        src/pch.cpp
        src/Plugin.cpp
        src/OptCommand.cpp
        src/BetterBDSMod.cpp
        src/Config.cpp

        src/remotery/Remotery.c
        src/opts/HopperOpt.cpp
        src/opts/ActorOpt.cpp
        src/opts/RedstoneOpt.cpp
        src/opts/EntitySystemOpt.cpp
)

include_directories(SDK/Header)
include_directories(src/remotery)
include_directories(SDK/Header/third-party)
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
