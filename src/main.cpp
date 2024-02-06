// MIT LICENSE
//
// Copyright(c) 2019
// Christophe Meneboeuf <christophe@xtof.info>
// Joey De Vries < https://joeydevries.com/#contact>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.




// This code will gets you started with OpenGL Core + SDL2
// It will draw an orange squared as a test
// HEAVY borrows from: https://learnopengl.com/Getting-started/Hello-Triangle

#include <cassert>
#include <iostream>

// OpenGL
#include <glm/glm.hpp>
#include <GL/glew.h>
// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
// DearIMGUI
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "helpers/HelpersImgui.h"
#include "helpers/HelpersOpenGl.h"



static constexpr int SCREEN_WIDTH = 1600;
static constexpr int SCREEN_HEIGHT = 1200;

static constexpr int OPENGL_MAJOR_VERSION = 3;
static constexpr int OPENGL_MINOR_VERSION = 3;

static constexpr auto OPENGL_PROFILE = SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE;  // OpenGL core profile - deprecated functions are disabled
static constexpr auto GLSL_VERSION = "#version 330 core";


namespace bootstrap {

  void InitGL();
  int  SetViewport(int width, int height);

}


namespace test {

  // Required element to draw the OpenGL test rectangle
  struct Rectangle_t {
    unsigned hProgrammShader;
    unsigned hVao;
    unsigned nbIndices;
  };
  // Sets the OpenGl rectangle
  Rectangle_t SetUpRectangle();

  // Sets Dear Imgui test window
  void Imgui_TestWindow();

  // Inits the position and size of the test window
  void Imgui_TestInit();

}



int main(int argc, char *argv[])
{
  // # OpenGl init
  const auto oglInit = helpers::opengl::Init(SCREEN_WIDTH, SCREEN_HEIGHT);
  if (!oglInit.has_value())
  {
    std::cerr << "Cannot init SDL and OpenGl" << std::endl;
    return -1;
  }
  // # DearImgui init
  helpers::imgui::Init(oglInit.value(), GLSL_VERSION);

  const SDL_GLContext& openGlContext = oglInit.value().first;
  SDL_Window* const mainWindow = oglInit.value().second;
  
  // # Test elements
  const struct {
    float r = 0.2f;
    float g = 0.3f;
    float b = 0.3f;
    float a = 1.0f;
  } color_background;

  helpers::imgui::WindowRender sceneWindow{ "Scene" };
  bool success = sceneWindow.init();
  assert(success);

  // # Main loop
  SDL_Event event;
  bool quit = false;
  while(!quit)
  {

    // ## process events
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mainWindow)) {
        quit = true;
      }
    }

    // ## New frame
    // ### imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(mainWindow);
    ImGui::NewFrame();
    // ## opengl main framebuffer
    glClearColor(color_background.r, color_background.g, color_background.b, color_background.a);
    glClear(GL_COLOR_BUFFER_BIT);

    // ## Scene
    const auto square = test::SetUpRectangle();
    // ### Sends the opengl commands into the helper window 
    sceneWindow.begin();
    glUseProgram(square.hProgrammShader);
    glBindVertexArray(square.hVao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, square.nbIndices, GL_UNSIGNED_INT, 0);
    sceneWindow.end();
    // ### draw the helper window
    sceneWindow.draw();

    // ## Test imgui window
    test::Imgui_TestWindow();

    // ## imgui render
    ImGui::Render();

    // ## Display
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(mainWindow); // swap the buffers work and display buffers
  }
  

  SDL_GL_DeleteContext(openGlContext);
  SDL_DestroyWindow(mainWindow);
  SDL_Quit();

  return 0;
}



namespace bootstrap
{

  // Inits OpenGL
  // ShadeModel and Hints can be omitted / modified
  void InitGL()
  {
    // init GLEW
    if (glewInit() != GLEW_OK) {
      std::cout << "ERROR: Cannot init GLEW" << std::endl;
      exit(1);
    }

    // select flat or smooth shading
    glShadeModel(GL_SMOOTH);
  }

  // Function to reset our viewport after a window resize
  int SetViewport(const int width, const int height)
  {
    // setup the viewport
    glViewport(0, 0, width, height);
    // swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    return 1;
  }

}



namespace test
{

  // Shader sources
  static const char *Src_Shader_Vertex = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
  static const char *Src_Shader_Fragment = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

  Rectangle_t SetUpRectangle()
  {
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    const auto shader_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vertex, 1, &Src_Shader_Vertex, NULL);
    glCompileShader(shader_vertex);
    // check for shader compile errors
    int success;
    static char Info_Log[512];
    glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader_vertex, 512, NULL, Info_Log);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << Info_Log << std::endl;
    }
    // fragment shader
    const auto shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_fragment, 1, &Src_Shader_Fragment, NULL);
    glCompileShader(shader_fragment);
    // check for shader compile errors
    glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader_fragment, 512, NULL, Info_Log);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << Info_Log << std::endl;
    }
    // link shaders into the program
    const auto program_shaders = glCreateProgram();
    glAttachShader(program_shaders, shader_vertex);
    glAttachShader(program_shaders, shader_fragment);
    glLinkProgram(program_shaders);
    // check for linking errors
    glGetProgramiv(program_shaders, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(program_shaders, 512, NULL, Info_Log);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << Info_Log << std::endl;
    }
    glDeleteShader(shader_vertex);
    glDeleteShader(shader_fragment);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    // Referencing the vertices to share them between the two triangles
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(vao); // ebo will be hosted in the vao

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configure and enable vertex attribute #0 ("location" 0 in the verter shader)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    return {
      program_shaders,
      vao,
      6
    };
  }


  void Imgui_TestInit()
  {
    ImGui::SetNextWindowPos({ 25, 25 });
    ImGui::SetNextWindowSize({ SCREEN_WIDTH / 4, SCREEN_WIDTH / 6 });
  }

  void Imgui_TestWindow()
  {
    ImGui::Begin("Hello, world!");
    ImGui::Text("HELLO WORLD");
    ImGui::End();
  }
}
