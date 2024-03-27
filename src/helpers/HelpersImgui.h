//MIT License
//
//Copyright(c) 2024
//Christophe Meneboeuf <christophe@xtof.info>
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

#pragma once

#include <mutex>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

#include <imgui.h>
#include <ImGuiColorTextEdit/TextEditor.h>


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

      /// @brief Constrains the widow to the provided aspect ratio (x:y)
      /// @details Set to 0 to deactivate
      void setAspectRatio(const float aspectRatio)
      {
        _aspectRatio = aspectRatio;
      }

    private:
      
      /// @brief Calback to constrain aspect ratio
      static void AspectRatio(ImGuiSizeCallbackData* data);


      bool _initialized = false;
      float _aspectRatio = 0.f;      

      GLuint _frameBufferObject = 0;
      GLuint _renderBufferObject = 0;
      GLuint _texture = 0;

      const std::string _title;
      ImVec2            _size = { 640.f, 480.f };

    };

    class WindowShader
    {
    public:

      WindowShader(const std::string& windowTitle);

      bool init(const GLuint hShader);

      /// @brief returns true if a new source code is available
      bool draw();

      inline const char* getSourceCode() const { return _src; }

      void setSourceCode(const GLuint shader);

    private:
      ImGuiColorTextEdit::TextEditor _editor;

      static constexpr int MAX_SIZE_SRC = 10 * 1024;

      const std::string _title;
      char _src[MAX_SIZE_SRC];
      GLuint _hShader = 0;

    };
    
    class Logger
    {

    public:
      Logger();

    static Logger& GetInstance() {
        return _Instance;
      }

      void clear();      

      void logDebug(const std::string& str);

      void logInfo(const std::string& str);

      void logWarning(const std::string& str);

      void logError(const std::string& str);

      void draw();

    private:

      void addLog(const char* fmt, ...) IM_FMTARGS(2);

      static Logger _Instance;

      ImGuiTextBuffer     _buf;
      ImGuiTextFilter     _filter;
      ImVector<int>       _lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
      bool                _bAutoScroll;  // Keep scrolling if already at the bottom.
      const std::string   _title{ "Logger" };

      std::mutex _mutex;
    };

  }

}