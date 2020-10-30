cmake_minimum_required(VERSION 3.15)

project(pollen_cli DESCRIPTION "IOTA Pollen wallet CLI")

enable_language(C)

# fetch goshimmer client library
include(FetchContent)
FetchContent_Declare(
  goshimmer-client
  GIT_REPOSITORY http://github.com/oopsmonk/goshimmer-client-c.git
  GIT_TAG e1c7e219ca420d22574a008fe678866b3faff3ac)

message(STATUS "Fetching goshimmer library")
FetchContent_MakeAvailable(goshimmer-client)

# external libs module
include(ExternalProject)

# libs in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

# external libs
include(cmake/argtable3.cmake)
include(cmake/linenoise.cmake)

# your source files
add_executable(${CMAKE_PROJECT_NAME} "src/main.c" "src/cli_cmd.c" "src/split_argv.c")

target_include_directories(
  ${CMAKE_PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}" "${CMAKE_INSTALL_PREFIX}/include"
                                "${CMAKE_INSTALL_PREFIX}/include/cjson" "${pollen_qianbao_SOURCE_DIR}")

target_link_libraries(
  ${CMAKE_PROJECT_NAME}
  PRIVATE goshimmer_client # wallet lib
          argtable3 # arg parsing
          linenoise # readline lib
          m # for linenoise
)

add_dependencies(${CMAKE_PROJECT_NAME} "ext_argtable3" "ext_linenoise")
