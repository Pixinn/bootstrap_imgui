//MIT License
//
//Copyright(c) 2024
//Christophe Meneboeuf <christophe@xtof.info>
//Joey De Vries < https://joeydevries.com/#contact>
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.





// This code will gets you started with OpenGL Core + SDL2
// It will draw an orange squared as a test
// HEAVY borrows from: https://learnopengl.com/Getting-started/Hello-Triangle

#ifdef IS_WINDOWS
  #include <Windows.h>
#elif IS_MACOS
  #include <mach-o/dyld.h>
#else
  #include <unistd.h>
#endif

#include <cassert>
#include <iostream>
#include <filesystem>

// OpenGL
#include <glm/glm.hpp>
#include <GL/glew.h>
// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
// DearIMGUI
#include <imgui.h>

#include "helpers/HelpersImgui.h"
#include "helpers/HelpersOpenGl.h"
#include "helpers/Logger.h"
#include "helpers/Renderer.h"


char PATH_EXECUTABLE[1024];

static constexpr int SCREEN_WIDTH = 1600;
static constexpr int SCREEN_HEIGHT = 900;

static constexpr int OPENGL_MAJOR_VERSION = 3;
static constexpr int OPENGL_MINOR_VERSION = 3;

static constexpr auto OPENGL_PROFILE = SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE;  // OpenGL core profile - deprecated functions are disabled
static constexpr auto GLSL_VERSION = "#version 330 core";



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

}


static void RenderFrame(void)
{

}

class Main : public helpers::Runnable
{

public:

  Main()
    : _logger{ helpers::imgui::Logger::GetInstance() }
  { 
uint32_t len = sizeof(PATH_EXECUTABLE);
#ifdef IS_WINDOWS
    GetModuleFileName(nullptr, PATH_EXECUTABLE, len);
#elif IS_MACOS
  const bool success = (_NSGetExecutablePath(PATH_EXECUTABLE, &len) == 0);
  if(!success) {
    std::cout << "ERROR executable path is too long" << std::endl;
    exit(-1);
  }
#else
  int bytes = MIN(readlink("/proc/self/exe", PATH_EXECUTABLE, len), len - 1);
  if (bytes >= 0)
    pBuf[bytes] = '\0';
#endif
  }

  int run() override
  {
    // # Init rendering environment
    if (!_renderer.init(SCREEN_WIDTH, SCREEN_HEIGHT))
    {
      std::cout << "Cannot init SDL and OpenGl" << std::endl;
      return -1;
    }
    _renderer.setRunnable(this);

    // # Init attributes
    bool success = _sceneWindow.init();
    assert(success);
    // ## Pluging the imgui logger into the general logger
    helpers::Logger::GetInstance()->setPrinter(
      [this](const helpers::Logger::Log_t& log)
      {
        static std::mutex MutexLogger;
        std::lock_guard<std::mutex> lock{ MutexLogger };
        switch (log.level)
        {
        case helpers::Logger::eLevel::DEBUG:
          _logger.logInfo(log.msg);
          break;
        case helpers::Logger::eLevel::INFO:
          _logger.logInfo(log.msg);
          break;
        case helpers::Logger::eLevel::WARN:
          _logger.logWarning(log.msg);
          break;
        case helpers::Logger::eLevel::ERR:
          _logger.logError(log.msg);
          break;
        default:
          assert(false);
          break;
        }
      }
    );
    helpers::Logger::GetInstance()->debug("Test debug");
    helpers::Logger::GetInstance()->info("Test info");
    helpers::Logger::GetInstance()->warning("Test warning");
    helpers::Logger::GetInstance()->error("Test error");
    // # Example geometries
    _square = test::SetUpRectangle();



    // # Enter main loop
    _renderer.run();

    return 0;
  }


private:

  // # Test elements
  const struct {
    float r = 0.2f;
    float g = 0.3f;
    float b = 0.3f;
    float a = 1.0f;
  } _colorBackground;

  helpers::imgui::WindowRender _sceneWindow{ "Scene" };
  test::Rectangle_t _square;

  helpers::imgui::Logger& _logger;

  // # Renderer
  helpers::Renderer _renderer;

private:

  virtual void renderFrame() override
  {
    // ## opengl main framebuffer
    glClearColor(_colorBackground.r, _colorBackground.g, _colorBackground.b, _colorBackground.a);
    glClear(GL_COLOR_BUFFER_BIT);

    // ## Scene
    // ### Sends the opengl commands into the helper window 
    _sceneWindow.begin();
    glUseProgram(_square.hProgrammShader);
    glBindVertexArray(_square.hVao); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, _square.nbIndices, GL_UNSIGNED_INT, 0);
    _sceneWindow.end();
    // ### draw the helper window
    _sceneWindow.draw();

    // ## Logger
    _logger.draw("Logger");

    // ## Test imgui window
    test::Imgui_TestWindow();
  }

  virtual void processEvent(const SDL_Event events) override
  {
    (void)(events);
  }

};




int main(int argc, char *argv[])
{
  Main runnable;
  return runnable.run();
}





namespace test
{


  Rectangle_t SetUpRectangle()
  {
    std::filesystem::path pathExe{ PATH_EXECUTABLE };
    const auto pathShaderVertex = pathExe.parent_path() / "shaders" / "square.vert";
    const auto pathShaderFragment = pathExe.parent_path() / "shaders" / "square.frag";
    const auto pShaderVertex = helpers::opengl::FactoryShader::Create(pathShaderVertex.string(), GL_VERTEX_SHADER);
    const auto pShaderFragment = helpers::opengl::FactoryShader::Create(pathShaderFragment.string(), GL_FRAGMENT_SHADER);
    // link shaders into the program
    const auto program_shaders = glCreateProgram();
    glAttachShader(program_shaders, pShaderVertex->handle());
    glAttachShader(program_shaders, pShaderFragment->handle());
    glLinkProgram(program_shaders);
    // check for linking errors
    int success;
    static char Info_Log[512];
    glGetProgramiv(program_shaders, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(program_shaders, 512, NULL, Info_Log);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << Info_Log << std::endl;
    }
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


  void Imgui_TestWindow()
  {
    ImGui::Begin("Hello, world!");
    ImGui::Text("HELLO WORLD");
    ImGui::End();
  }
}
