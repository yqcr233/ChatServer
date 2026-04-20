cmake_minimum_required(VERSION 3.0)
project(Chat)

# 配置编译选项
set(CMAKE_CXX_CFLAGS ${CMAKE_CXX_CFLAGS} -g)

# 配置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件查找路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/rsa)
include_directories(${PROJECT_SOURCE_DIR}/include/server/base64)
include_directories(${PROJECT_SOURCE_DIR}/include/server/aesgcm)
include_directories(${PROJECT_SOURCE_DIR}/include/server/web)
include_directories(${PROJECT_SOURCE_DIR}/thirdpart)
# set(MUDUO_INCLUDE_DIR "/home/fjf/chatpro_res/build/release-install-cpp11/include")
# include_directories(${MUDUO_INCLUDE_DIR})

# # 配置库文件查找路径
# set(MUDUO_LIB_DIR "/home/fjf/chatpro_res/build/release-install-cpp11/lib")
# link_directories(${MUDUO_LIB_DIR})

# 加载子目录
add_subdirectory(src)