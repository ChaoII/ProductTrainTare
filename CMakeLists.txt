cmake_minimum_required(VERSION 3.10)
project(traintare-api CXX)

# add_dependence 支持
include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_definitions(-DNOMINMAX)
set(DEPENDS "")
add_executable(${PROJECT_NAME} main.cc)
#drogon
find_package(Drogon CONFIG REQUIRED)
list(APPEND DEPENDS Drogon::Drogon)

#fastdeploy
set(FASTDEPLOY_DIR CACHE PATH "")
include(${FASTDEPLOY_DIR}/FastDeploy.cmake)
include_directories(${FASTDEPLOY_INCS})
list(APPEND DEPENDS ${FASTDEPLOY_LIBS})


# opencv fastdeploy 中已经包含opencv
#if (WIN32)
#    Option(OpenCV_DIR CACHE PATH "")
#    if(NOT OpenCV_DIR)
#        message(FATAL "must specific opencv dir")
#    endif ()
#endif ()

#find_package(OpenCV CONFIG REQUIRED)
#list(APPEND DEPENDS ${OpenCV_LIBS})

if (APPLE)
    message(apple)
    set(OPENSSL_ROOT_DIR "/usr/local/Cellar/openssl@1.1/1.1.1o/")
endif ()

# jwt-cpp
include_directories("${CMAKE_SOURCE_DIR}/3rdpart/jwt-cpp/include")
#find_path(TWT_INCLUDE_DIRS "jwt-cpp/base.h")
find_package(OpenSSL REQUIRED)
list(APPEND DEPENDS OpenSSL::SSL OpenSSL::Crypto)

#workspace
include_directories("${CMAKE_SOURCE_DIR}/3rdpart/workspace/include")

target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDS})

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)
aux_source_directory(custom CUSTOM_SRC)
aux_source_directory(task TASK_SRC)


target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/models)
target_sources(${PROJECT_NAME}
        PRIVATE ${SRC_DIR} ${CTL_SRC} ${FILTER_SRC} ${PLUGIN_SRC}
        ${MODEL_SRC} ${CUSTOM_SRC} ${TASK_SRC})


target_compile_options(${PROJECT_NAME} PRIVATE "/utf-8")
# 复制文件
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.json ${CMAKE_BINARY_DIR}
)
