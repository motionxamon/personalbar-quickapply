cmake_minimum_required(VERSION 3.24)

project(PersonalBarQuickApply LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AE_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ae_sdk" CACHE PATH "Adobe After Effects SDK root")

if(NOT EXISTS "${AE_SDK_ROOT}/Examples/Headers/AEConfig.h")
    file(GLOB AE_SDK_CANDIDATES "${AE_SDK_ROOT}/*")
    foreach(candidate ${AE_SDK_CANDIDATES})
        if(EXISTS "${candidate}/Examples/Headers/AEConfig.h")
            set(AE_SDK_ROOT "${candidate}" CACHE PATH "Adobe After Effects SDK root" FORCE)
            break()
        endif()
    endforeach()
endif()

if(NOT EXISTS "${AE_SDK_ROOT}/Examples/Headers/AEConfig.h")
    message(FATAL_ERROR "Could not find AE SDK. Expected Examples/Headers/AEConfig.h under AE_SDK_ROOT=${AE_SDK_ROOT}")
endif()

set(PIPL_TOOL "${AE_SDK_ROOT}/Examples/Resources/PiPLtool.exe")
set(PIPL_INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/QuickApplyPlus_PiPL.r")
set(PIPL_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/QuickApplyPlus_PiPL_temp.rc")

add_custom_command(
    OUTPUT "${PIPL_OUTPUT}"
    COMMAND "${PIPL_TOOL}" QuickApplyPlus_PiPL.r QuickApplyPlus_PiPL_temp.rc
    DEPENDS "${PIPL_INPUT}" "${PIPL_TOOL}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src"
    COMMENT "Generating PiPL resource"
)

add_custom_target(GeneratePersonalBarPiPL DEPENDS "${PIPL_OUTPUT}")

add_library(PersonalBarQuickApply MODULE
    src/QuickApplyPlus.cpp
    src/QuickApplyPopupWin.cpp
    src/QuickApplyPopupWin.h
    src/QuickApplyPlus.rc
    "${AE_SDK_ROOT}/Examples/Util/AEGP_SuiteHandler.cpp"
    "${AE_SDK_ROOT}/Examples/Util/MissingSuiteError.cpp"
)

add_dependencies(PersonalBarQuickApply GeneratePersonalBarPiPL)

set_target_properties(PersonalBarQuickApply PROPERTIES
    PREFIX ""
    OUTPUT_NAME "PersonalBarQuickApply"
    SUFFIX ".aex"
)

target_compile_definitions(PersonalBarQuickApply PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

target_include_directories(PersonalBarQuickApply PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${AE_SDK_ROOT}/Examples/Headers"
    "${AE_SDK_ROOT}/Examples/Headers/SP"
    "${AE_SDK_ROOT}/Examples/Util"
    "${AE_SDK_ROOT}/Examples/Resources"
)

target_link_libraries(PersonalBarQuickApply PRIVATE
    user32
    gdi32
    comctl32
)
