cmake_minimum_required(VERSION 2.8)

project(spark_demo)

add_definitions(-DGL_GLEXT_PROTOTYPES)


add_compile_options("-lGL")

message("SYSTEM NAME : ${CMAKE_SYSTEM_NAME}")
message("SYSTEM VERSION : ${CMAKE_SYSTEM_VERSION}")
message("SYSTEM PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")

AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Core" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Emitters" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Modifiers" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Renderers" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Extensions/Zones" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/RenderingAPIs/OpenGLES" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src/Window" spark_demo_app_SRC)
AUX_SOURCE_DIRECTORY("${PROJECT_SOURCE_DIR}/src" spark_demo_app_SRC)
list(APPEND spark_demo_app_SRC ${PROJECT_SOURCE_DIR}/demos/src/SnowDemo.cpp)

include_directories ("${PROJECT_SOURCE_DIR}/target/qnx7/usr/include")
include_directories ("${PROJECT_SOURCE_DIR}/include")
include_directories ("${PROJECT_SOURCE_DIR}/third_party/glm/include")
include_directories ("${PROJECT_SOURCE_DIR}/third_party/glad/include")
include_directories ("${PROJECT_SOURCE_DIR}/src")

find_library(GLEW GLEW REQUIRED)

ADD_EXECUTABLE(spark_demo_app ${spark_demo_app_SRC})

target_link_libraries(spark_demo_app /usr/lib/x86_64-linux-gnu/libGLEW.so)
target_link_libraries(spark_demo_app /usr/lib/x86_64-linux-gnu/libGLESv2.so)
target_link_libraries(spark_demo_app /usr/lib/x86_64-linux-gnu/libglfw.so)
# target_link_libraries(spark_demo_app ${PROJECT_SOURCE_DIR}/target/qnx7/aarch64le/libGLESv2.so)
# target_link_libraries(spark_demo_app ${PROJECT_SOURCE_DIR}/target/qnx7/aarch64le/libm.so)
# target_link_libraries(spark_demo_app ${PROJECT_SOURCE_DIR}/target/qnx7/aarch64le/libstdc++.so)
# target_link_libraries(spark_demo_app ${PROJECT_SOURCE_DIR}/target/qnx7/aarch64le/libc.so)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/res
				DESTINATION ${PROJECT_SOURCE_DIR}/build/Linux/x86_64/Debug/spark_demo_app)
