cmake_minimum_required(VERSION 3.21)
project(MacroGD VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable fmt's broken consteval path with the Android NDK Clang 21 compiler.
# Newer fmt releases honor this definition; older bundled releases overwrite it,
# so patch their cached base.h after Geode SDK has fetched the dependency.
add_compile_definitions(FMT_USE_CONSTEVAL=0)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

CPMAddPackage("gh:maxnut/GDReplayFormat#d8d2f0b1193a46cdf47b413dc832616dd052b6a9")

set(_fmt_cache "$ENV{CPM_SOURCE_CACHE}")
if(NOT _fmt_cache)
    message(FATAL_ERROR "CPM_SOURCE_CACHE is not set; cannot apply the Android fmt compatibility patch.")
endif()

file(GLOB _fmt_base_headers "${_fmt_cache}/fmt/*/include/fmt/base.h")
if(NOT _fmt_base_headers)
    message(FATAL_ERROR "Could not find fmt/base.h in CPM_SOURCE_CACHE: ${_fmt_cache}")
endif()

foreach(_fmt_header IN LISTS _fmt_base_headers)
    file(READ "${_fmt_header}" _fmt_contents)
    set(_fmt_original "${_fmt_contents}")
    string(REPLACE
        "#  define FMT_USE_CONSTEVAL 1"
        "#  define FMT_USE_CONSTEVAL 0"
        _fmt_contents
        "${_fmt_contents}"
    )
    if(NOT _fmt_contents STREQUAL _fmt_original)
        file(WRITE "${_fmt_header}" "${_fmt_contents}")
        message(STATUS "MacroGD: disabled fmt consteval in ${_fmt_header}")
    endif()
endforeach()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} libGDR)
setup_geode_mod(${PROJECT_NAME})
