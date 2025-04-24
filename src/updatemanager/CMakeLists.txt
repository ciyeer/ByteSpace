# 指定该库的源文件
file(GLOB ONLINEUPDATE_SRCS
    "*.cpp"
    "*.h"
)

# 创建静态库
add_library(OnlineUpdate STATIC ${ONLINEUPDATE_SRCS})

# 设置头文件搜索路径
target_include_directories(OnlineUpdate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
