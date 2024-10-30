cmake_minimum_required(VERSION 3.10)
project(Simulator)

# C/C++ version
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Platform common configuration
add_definitions(
  # LVGL
  -DLV_SCREEN_HOR_RES=240
  -DLV_SCREEN_VER_RES=240
  # FreeType
  -DFT2_BUILD_LIBRARY
  -DFT_CONFIG_MODULES_H=<External/graphics/lvgl/src/libs/freetype/ftmodule.h>
  -DFT_CONFIG_OPTIONS_H=<External/graphics/lvgl/src/libs/freetype/ftoption.h>
  # Others
  -DDATA_BROKER_LOG_LEVEL=1
  -DPAGE_LOG_LEVEL=0
  -DCONFIG_GNSS_UPDATE_PERIOD=100
  -DCONFIG_RESOURCE_PATH="./Resource/")

# Build configuration
option(ENABLE_DEBUG "Enable debug build" ON)

if(ENABLE_DEBUG)
  message(STATUS "Build type: Debug")
  add_definitions(-DDEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
  set(ENABLE_ASAN 1)
else()
  message(STATUS "Build type: Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Platform Config
if(WIN32)
  message(STATUS "Platform: windows")

  add_definitions(-DUSE_WIN32DRV=1)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT
                                                              Simulator)
elseif(UNIX)
  message(STATUS "Platform: UNIX")

  set(WARNING_FLAGS
      "-Wall -Wextra \
        -Wshadow -Wundef -Wmaybe-uninitialized -Wno-deprecated-declarations \
        -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
        -Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
        -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wclobbered -Wdeprecated  \
        -Wempty-body -Wshift-negative-value \
        -Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith -Wno-cast-function-type -Wno-inconsistent-missing-override"
  )

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS}")

  add_definitions(-DLV_USE_SDL=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1)
  find_package(SDL2 REQUIRED SDL2)
  include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS}/../
                      ${PROJECT_SOURCE_DIR})

  # Address Sanitizer
  if(ENABLE_ASAN)
    set(ASAN_FLAGS
        "-fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
    add_definitions(${ASAN_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${ASAN_FLAGS})
    message(STATUS "Address Sanitizer Enabled:" ${ASAN_FLAGS})
  endif()

endif()

# Include
set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/../..)
set(EXTERNAL_SOURCE_DIR ${PROJECT_ROOT_DIR}/External)
set(FRAMEWORKS_SOURCE_DIR ${PROJECT_ROOT_DIR}/Frameworks)
set(APP_SOURCE_DIR ${PROJECT_ROOT_DIR}/App)

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_ROOT_DIR}
  ${APP_SOURCE_DIR}
  ${EXTERNAL_SOURCE_DIR}/graphics
  ${EXTERNAL_SOURCE_DIR}/freetype/include)

# lvgl porting
file(GLOB LV_PORT_SOURCES "lv_port/*.c" "lv_port/*.cpp")

# HAL
file(GLOB HAL_SOURCES "HAL/*.cpp")

# App
file(GLOB_RECURSE APP_SOURCES ${APP_SOURCE_DIR}/*.c ${APP_SOURCE_DIR}/*.cpp)

# Frameworks
file(
  GLOB
  FRAMEWORKS_SOURCES
  ${FRAMEWORKS_SOURCE_DIR}/DataBroker/*.cpp
  ${FRAMEWORKS_SOURCE_DIR}/DeviceManager/*.cpp
  ${FRAMEWORKS_SOURCE_DIR}/PageManager/*.cpp
  ${FRAMEWORKS_SOURCE_DIR}/ResourceManager/*.cpp)

# ALL Sources
set(SOURCES
    ${LV_PORT_SOURCES}
    ${HAL_SOURCES}
    ${APP_SOURCES}
    ${FRAMEWORKS_SOURCES})

# Subdirectory
add_subdirectory(${EXTERNAL_SOURCE_DIR}/graphics/lvgl
                 ${PROJECT_BINARY_DIR}/build_lvgl)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/freetype
                 ${PROJECT_BINARY_DIR}/build_freetype)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/cJSON ${PROJECT_BINARY_DIR}/build_cjson)

# Executable
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(Simulator main.cpp ${SOURCES} ${INCLUDES})

# Link
target_link_libraries(Simulator PRIVATE lvgl lvgl::examples lvgl::demos
                                        freetype cjson ${SDL2_LIBRARIES})

# Output
set_target_properties(Simulator PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                           "${PROJECT_SOURCE_DIR}/build")

# Link Resource Path
add_custom_target(
  LinkResourcePath
  COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/build
  COMMAND ln -sf ${PROJECT_SOURCE_DIR}/../../../../Resource
          ${PROJECT_SOURCE_DIR}/build)
add_dependencies(Simulator LinkResourcePath)
