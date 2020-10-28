cmake_minimum_required(VERSION 3.15)

project(pollen_cli DESCRIPTION "IOTA Pollen wallet CLI application")

enable_language(C)

# fetch pollen_qianbao
include(FetchContent)
FetchContent_Declare(
  pollen_qianbao
  GIT_REPOSITORY http://github.com/oopsmonk/pollen_qianbao.git
  GIT_TAG 490793e0610927ee8837a5f0e5b3f222f8b6938e)

message(STATUS "Fetching pollen library")
FetchContent_MakeAvailable(pollen_qianbao)

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
  PRIVATE qian_bao # wallet lib
          argtable3 # arg parsing
          linenoise # readline lib
          m # for linenoise
)

add_dependencies(${CMAKE_PROJECT_NAME} "ext_argtable3" "ext_linenoise")
