cmake_minimum_required(VERSION 3.10)

# Project name - should not contain whitespace 
project(bootstrap_imgui)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()



#========== non-conan dependencies =============#
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

#========== conan imgui =============#
set(SRC_DIR_IMGUI imgui)
set(SRC_IMGUIBIND ${SRC_DIR_IMGUI}/imgui_impl_opengl3.cpp ${SRC_DIR_IMGUI}/imgui_impl_sdl2.cpp)
add_library(imguibind ${SRC_IMGUIBIND})


#========== Global Configurations =============#

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)


#========== Targets Configurations ============#
# project sources
add_executable(${PROJECT_NAME}  src/bootstrap_imgui.cpp)
                         
# set executable dependecies
target_include_directories(${PROJECT_NAME} PRIVATE src imgui)
target_link_libraries(${PROJECT_NAME} imguibind)
conan_set_find_library_paths(${PROJECT_NAME})
conan_target_link_libraries(${PROJECT_NAME})
