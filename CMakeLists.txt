cmake_minimum_required(VERSION 3.10)

# Project name - should not contain whitespace 
project(bootstrap_imgui)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()



#========== non-conan dependencies =============#
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

#========== conan imgui =============#
set(SRC_DIR_IMGUI imgui)
set(SRC_IMGUI ${SRC_DIR_IMGUI}/imgui_impl_opengl3.cpp ${SRC_DIR_IMGUI}/imgui_impl_sdl.cpp)


#========== Global Configurations =============#

set(CMAKE_CXX_STANDARD 14)     
set(CMAKE_VERBOSE_MAKEFILE ON)


#========== Targets Configurations ============#
# sources
add_executable(${PROJECT_NAME}  src/bootstrap_imgui.cpp
                                ${SRC_IMGUI})
                               
# set executable dependecies
target_include_directories(${PROJECT_NAME} PRIVATE src imgui)
conan_set_find_library_paths(${PROJECT_NAME})
conan_target_link_libraries(${PROJECT_NAME})


#============ IDE ============#
source_group("imgui"  FILES ${SRC_IMGUI})