# --- Detect if the plugin is build out of tree or not ---
if(CMAKE_PROJECT_NAME STREQUAL "obs-studio")
  set(BUILD_OUT_OF_TREE OFF)
else()
  set(BUILD_OUT_OF_TREE ON)
  cmake_minimum_required(VERSION 3.28...3.30)
endif()
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)

project(${_name} VERSION ${_version})

include(compilerconfig)
include(defaults)
include(helpers)

add_library(${PROJECT_NAME} MODULE)

target_link_libraries(${PROJECT_NAME} PRIVATE OBS::libobs)

if(BUILD_OUT_OF_TREE)
  find_package(libobs REQUIRED)
  find_package(obs-frontend-api REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE OBS::obs-frontend-api)
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE OBS::frontend-api)
endif()

find_package(Qt6 COMPONENTS Svg Xml Widgets Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Svg Qt::Xml Qt6::Core Qt6::Widgets)
target_compile_options(${PROJECT_NAME}
                       PRIVATE $<$<C_COMPILER_ID:Clang,AppleClang>:-Wno-quoted-include-in-framework-header -Wno-comma>)
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

if(OS_WINDOWS)
  configure_file(cmake/windows/resources/installer-Windows.iss.in
                 "${CMAKE_CURRENT_BINARY_DIR}/installer-Windows.generated.iss")
endif()

target_sources(
  ${PROJECT_NAME}
  PRIVATE src/plugin-main.cpp
          src/obs-utils.cpp
          src/obs-utils.hpp
          src/svg-source.cpp
          src/svg-source.hpp
          src/svg-utils.cpp
          src/svg-utils.hpp)

if(BUILD_OUT_OF_TREE)
  set_target_properties_plugin(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${_name})
else()
  set_target_properties_obs(${PROJECT_NAME} PROPERTIES FOLDER "plugins/finitesingularity" PREFIX "")
endif()
