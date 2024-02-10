cmake_minimum_required(VERSION 3.10)

# Project name - should not contain whitespace 
project(bootstrap_imgui)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()



#========== non-conan dependencies =============#
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

#========== conan imgui =============#
set(SRC_DIR_IMGUI "external/imgui")
set(SRC_IMGUIBIND ${SRC_DIR_IMGUI}/imgui_impl_opengl3.cpp ${SRC_DIR_IMGUI}/imgui_impl_sdl2.cpp)
add_library(imguibind ${SRC_IMGUIBIND})


#========== Global Configurations =============#

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)


#========== Targets Configurations ============#
# helpers library
add_library(helpers  src/helpers/Renderer.h
                     src/helpers/Renderer.cpp
                     src/helpers/HelpersOpenGl.h
                     src/helpers/HelpersOpenGl.cpp
                     src/helpers/HelpersImgui.h
                     src/helpers/HelpersImgui.cpp
)
target_include_directories(helpers PUBLIC src ${SRC_DIR_IMGUI})

# project sources
add_executable(${PROJECT_NAME}
    src/main.cpp   
)
                         
# set executable dependecies
target_link_libraries(${PROJECT_NAME} helpers imguibind)
conan_set_find_library_paths(${PROJECT_NAME})
conan_target_link_libraries(${PROJECT_NAME})
