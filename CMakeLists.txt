cmake_minimum_required(VERSION 3.16)

# 设置项目的版本
set(BYTESPACE_VERSION 1.7)
project(ByteSpace VERSION ${BYTESPACE_VERSION} LANGUAGES CXX)

# 设置项目名称（大写）
set(PROJECT_NAME_CAPITALIZED "ByteSpace")
# 设置输出消息的等级为 NOTICE 以隐藏不必要的信息
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
# 避免重复生成 CMake 状态信息
set(CMAKE_SUPPRESS_REGENERATION ON)
# 控制 Makefile 的详细输出级别
set(CMAKE_VERBOSE_MAKEFILE FALSE)
# 设置模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 如果定义了 VERSION_SUFIX 且不为空，添加版本后缀
if (DEFINED VERSION_SUFFIX AND NOT "${VERSION_SUFFIX}" STREQUAL "")
    set(BYTESPACE_VERSION_SUFFIX "-${VERSION_SUFFIX}")
endif()

# 设置最终版本号
set(BYTESPACE_VERSION "${PROJECT_VERSION}${BYTESPACE_VERSION_SUFFIX}")

# 获取Git信息
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_HASH "unknown")
    set(GIT_BRANCH "unknown")
endif()

# 获取构建时间
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# 配置生成头文件
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/buildconfig.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/buildconfig.h"
)

# 启用 Qt 的自动化处理功能
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX /O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()

# 设置平台特定选项
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0601)
elseif(APPLE)
    add_definitions(-DMACOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
else()
    add_definitions(-DLINUX)
endif()

# 设置项目根目录路径
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 查找源文件（排除 .h.in 文件）
file(GLOB_RECURSE _source_list LIST_DIRECTORIES false
    "${PROJECT_ROOT_DIR}/src/*.cpp"
    "${PROJECT_ROOT_DIR}/src/*.h"
    "${PROJECT_ROOT_DIR}/src/*.ui"
    "${PROJECT_ROOT_DIR}/*.qrc"
)

# 组织源文件结构
source_group(TREE ${PROJECT_ROOT_DIR} FILES ${_source_list})

# 查找 Qt 模块
find_package(Qt6 REQUIRED COMPONENTS Widgets Network SerialPort Concurrent)

# 收集源文件
file(GLOB_RECURSE BYTESPACE_SRCS
    "${PROJECT_ROOT_DIR}/src/*.cpp"
    "${PROJECT_ROOT_DIR}/src/*.h"
    "${PROJECT_ROOT_DIR}/src/*.ui"
    "${PROJECT_ROOT_DIR}/*.qrc"
)

# 添加可执行目标（修复名称拼写）
qt_add_executable(ByteSpace MANUAL_FINALIZATION ${BYTESPACE_SRCS})

# 包含头文件路径
target_include_directories(ByteSpace PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 链接 Qt 库
target_link_libraries(ByteSpace PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::SerialPort
    Qt6::Concurrent
)

# 设置目标属性
set_target_properties(ByteSpace PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 安装配置
include(GNUInstallDirs)
install(TARGETS ByteSpace
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt6 最终化
qt_finalize_executable(ByteSpace)
