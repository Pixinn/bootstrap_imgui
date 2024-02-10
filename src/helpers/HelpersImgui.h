#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

#include <imgui.h>

namespace helpers
{

  namespace imgui
  {
    
    /// @brief an Imgui Window diplaying a rendered scene
    class WindowRender
    {
    public:

      WindowRender(const std::string& windowTitle)
        : _title(windowTitle)
      {}

      ~WindowRender();

      bool init();

      /// @brief The window will "capture" the following OpenGl rendering command until the call of end()
      void begin();

      /// @brief Stops "capturing" the OpenGl commands
      void end();

      /// @brief Draws the window
      void draw();

    private:

      bool _initialized = false;

      GLuint _frameBufferObject = 0;
      GLuint _renderBufferObject = 0;
      GLuint _texture = 0;

      const std::string _title;
      ImVec2            _size = { 640.f, 480.f };

    };

  }

}