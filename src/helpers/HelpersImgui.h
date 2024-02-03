#pragma once

#include <GL/glew.h>
#include <imgui.h>

namespace helpers
{


    // / @brief an Imgui Window diplaying a rendered scene
    class WindowRender
    {
    public:

      WindowRender(const std::string& title)
        : _title(title)
      {}

      ~WindowRender();

      bool init();

      /// @brief Takes the next OpenGl rendering command
      void bind();
      
      /// @brief Releases the rendering command handler
      void unbind();
      
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