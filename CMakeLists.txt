cmake_minimum_required(VERSION 3.23)

# Project name - should not contain whitespace 
project(bootstrap_imgui)


#========== conan depedencies =============
find_package(SDL2 REQUIRED)
find_package(glew REQUIRED)
find_package(glm  REQUIRED)
find_package(imgui REQUIRED)


#========== non-conan dependencies =============#
find_package(OpenGL REQUIRED COMPONENTS OpenGL)


#========== Global Configurations =============#

set(CMAKE_CXX_STANDARD 17)     
set(CMAKE_VERBOSE_MAKEFILE ON)


#========== imgui =============#
set(SRC_DIR_IMGUI "external/imgui")
set(SRC_IMGUIBIND ${SRC_DIR_IMGUI}/imgui_impl_opengl3.cpp ${SRC_DIR_IMGUI}/imgui_impl_sdl2.cpp)
add_library(imgui-bind ${SRC_IMGUIBIND})
#target_include_directories(imgui-bind PRIVATE ${imgui_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(imgui-bind PUBLIC SDL2::SDL2main imgui::imgui)

set(SRC_DIR_IMGUICOLORTEXTEDIT "external/ImGuiColorTextEdit")
set(SRC_IMGUICOLORTEXTEDIT ${SRC_DIR_IMGUICOLORTEXTEDIT}/TextEditor.h ${SRC_DIR_IMGUICOLORTEXTEDIT}/TextEditor.cpp)
add_library(imgui-colortextedit ${SRC_IMGUICOLORTEXTEDIT})
# target_include_directories(imgui-colortextedit PRIVATE ${imgui_INCLUDE_DIRS})
target_link_libraries(imgui-colortextedit PUBLIC imgui-bind)



#========== Targets Configurations ============#


# helpers library
add_library(helpers  src/helpers/TDequeConcurrent.h
                     src/helpers/Renderer.h
                     src/helpers/Renderer.cpp
                     src/helpers/Logger.h
                     src/helpers/Logger.cpp
                     src/helpers/HelpersOpenGl.h
                     src/helpers/HelpersOpenGl.cpp
                     src/helpers/HelpersImgui.h
                     src/helpers/HelpersImgui.cpp
)
target_include_directories(helpers 
	PUBLIC   src external
)
target_link_libraries(helpers PUBLIC imgui-bind imgui-colortextedit SDL2::SDL2main GLEW::GLEW glm::glm imgui::imgui)

# project sources
add_executable(${PROJECT_NAME}
    src/example/main.cpp   
)

                         
# set executable dependecies
target_link_libraries(${PROJECT_NAME} helpers imgui-bind imgui-colortextedit)

# compilation definitions
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(${PROJECT_NAME} PUBLIC "IS_WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL  "Darwin")
  target_compile_definitions(${PROJECT_NAME} PUBLIC "IS_MACOS")
endif()

# install resources
# install(DIRECTORY "src/example/shaders" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_custom_command(TARGET  ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
                           ${CMAKE_SOURCE_DIR}/src/example/assets/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
add_custom_command(TARGET  ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
                           ${CMAKE_SOURCE_DIR}/src/example/shaders/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders                
)
