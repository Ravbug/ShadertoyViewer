cmake_minimum_required(VERSION 3.16)

project(glew)
file(GLOB SOURCES "include/GL/*.h")
add_library(${PROJECT_NAME} STATIC ${SOURCES} "src/glew.c" "src/glewinfo.c")

target_include_directories(${PROJECT_NAME}
    PUBLIC 
    "include/"
    PRIVATE
    "include/"
)
target_compile_definitions(${PROJECT_NAME} PUBLIC "GLEW_STATIC=1")