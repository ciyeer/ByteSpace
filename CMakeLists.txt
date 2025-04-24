cmake_minimum_required(VERSION 3.16)

# 设置项目的版本
set(TERMNITE_VERSION 1.7)
project(Termnite VERSION ${TERMNITE_VERSION} LANGUAGES CXX)

# 设置项目名称（大写）
set(PROJECT_NAME_CAPITALIZED "Termnite")
# 设置输出消息的等级为 NOTICE 以隐藏不必要的信息
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
# 避免重复生成 CMake 状态信息
set(CMAKE_SUPPRESS_REGENERATION ON)
# 控制 Makefile 的详细输出级别
set(CMAKE_VERBOSE_MAKEFILE FALSE)
# 设置模块路径
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# 如果定义了 VERSION_SUFIX 且不为空，添加版本后缀
if (DEFINED VERSION_SUFIX AND NOT "${VERSION_SUFIX}" STREQUAL "")
    set(TERMNITE_VERSION_SUFIX "-${VERSION_SUFIX}")
endif()

# 设置最终版本号
set(TERMNITE_VERSION "${PROJECT_VERSION}${TERMNITE_VERSION_SUFIX}")

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
    add_compile_options(/W4 /WX)
    add_compile_options(/O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-O2)
endif()

# 设置平台特定选项
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_WIN32_WINNT=0x0601)
elseif(APPLE)
    add_definitions(-DMACOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
else()
    add_definitions(-DLINUX)
endif()

# 设置项目目录路径
set(Termnite ${CMAKE_CURRENT_SOURCE_DIR})

# 查找源文件，包括 .cpp, .h, .ui, .qrc 文件
file(GLOB_RECURSE _source_list LIST_DIRECTORIES false
    "${Termnite}/*.cpp"
    "${Termnite}/*.h"
    "${Termnite}/*.ui"
    "${Termnite}/*.qrc"
    "${Termnite}/*.h.in"
)

# 组织源文件结构，按目录组织
source_group(TREE ${Termnite} FILES ${_source_list})

# 查找 Qt 的相关模块（Widgets, Network）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network SerialPort Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network SerialPort Concurrent)

# 递归查找源文件，包括 .cpp, .h, .ui, 和 .qrc 文件
file(GLOB_RECURSE TERMNITE_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h.in"
)

# 将 OnlineUpdate 库的源文件加入到最终构建的目标中
list(APPEND TERMNITE_SRCS ${ONLINEUPDATE_SRCS})

# 根据 Qt 的版本，使用不同的方式添加可执行文件
if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(Termnite MANUAL_FINALIZATION ${TERMNITE_SRCS}
        src/framework/settingsui/systempage.h src/framework/settingsui/systempage.cpp src/framework/settingsui/systempage.ui
        src/framework/settingsui/accountpage.h src/framework/settingsui/accountpage.cpp src/framework/settingsui/accountpage.ui
        src/framework/settingsui/appearancepage.h src/framework/settingsui/appearancepage.cpp src/framework/settingsui/appearancepage.ui
        src/framework/settingsui/helppage.h src/framework/settingsui/helppage.cpp src/framework/settingsui/helppage.ui
        src/framework/settingsui/aboutpage.h src/framework/settingsui/aboutpage.cpp src/framework/settingsui/aboutpage.ui)
else()
    add_executable(Termnite ${TERMNITE_SRCS})
endif()

# 封装自定义控件，确保控件路径正确
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 确保 buildconfig.h 文件在项目中可用
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 链接 Qt 库（Widgets, Network, SerialPort, Concurrent）
target_link_libraries(Termnite
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::Concurrent
)

# 设置目标属性，如 MacOS 和 Windows 平台的可执行文件相关选项
set_target_properties(Termnite PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 包含安装目录的标准配置
include(GNUInstallDirs)

# 安装目标（可执行文件、库、运行时文件）
install(TARGETS Termnite
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 如果是 Qt6，调用 qt_finalize_executable 完成 Qt 的最终化操作
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Termnite)
endif()
