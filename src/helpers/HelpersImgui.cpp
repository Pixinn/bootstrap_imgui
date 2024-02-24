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

#include <iostream>

#include <GL/glew.h>

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

#include "HelpersImgui.h"


namespace helpers
{

  namespace imgui
  {

   

    WindowRender::~WindowRender()
    {
      if (_initialized)
      {
        glDeleteFramebuffers(1, &_frameBufferObject);
        glDeleteRenderbuffers(1, &_renderBufferObject);
        glDeleteTextures(1, &_texture);
      }
    }


    bool WindowRender::init()
    {
      if (!_initialized)
      {
        _initialized = true;

        glGenFramebuffers(1, &_frameBufferObject);
        glGenTextures(1, &_texture);
        glGenRenderbuffers(1, &_renderBufferObject); // render buffer object so OpenGl can do depth and stencil tests
      }

      return _initialized;
    }


    void WindowRender::begin()
    {

      // Get current window's size
      //ImGui::SetNextWindowSize({ 128.f, 128.f }, ImGuiCond_FirstUseEver);
      ImGui::Begin(_title.c_str());
      _size = ImGui::GetContentRegionAvail();
      ImGui::End();

      // A minimal size is required for OpenGl 
      const float x = std::max({ 1.f, _size.x });
      const float y = std::max({ 1.f, _size.y });

      glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferObject); // now all ogl commands are from/to this framebuffer
      glViewport(0, 0, int(x + 0.5f), int(y + 0.5f));

      glBindTexture(GL_TEXTURE_2D, _texture); // all the following commands are related to this _texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, int(x + 0.5f), int(y + 0.5f), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //color _texture 
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0); // unbind the _texture
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0); // attach the _texture to the binded framebuffer

      glBindRenderbuffer(GL_RENDERBUFFER, _frameBufferObject);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, int(x + 0.5f), int(y + 0.5f));
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _frameBufferObject); // Render buffer object attached to the framebuffer

      auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) // Sanity
      {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
      }

      glClearColor(0.f, 0.f, 0.f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);
    }


    void WindowRender::end()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void WindowRender::draw()
    {
      ImGui::Begin(_title.c_str());
      ImGui::Image((void*)(intptr_t)_texture, _size);
      ImGui::End();
    }
    

    Logger Logger::_Instance;

    Logger::Logger()
    {
      _bAutoScroll = true;
      clear();
    }

    void Logger::clear()
    {
      std::lock_guard<std::mutex> lock{ _mutex };
      _buf.clear();
      _lineOffsets.clear();
      _lineOffsets.push_back(0);
    }

    void Logger::logDebug(const std::string& str)
    {
      const std::string info = "[debug]    " + str + '\n';
      addLog("%s", info.c_str());
    }

    void Logger::logInfo(const std::string& str)
    {
      const std::string info = "[info]    " + str + '\n';
      addLog("%s", info.c_str());
    }

    void Logger::logWarning(const std::string& str)
    {
      const std::string warn = "[warning] " + str + '\n';
      addLog("%s", warn.c_str());
    }

    void Logger::logError(const std::string& str)
    {
      const std::string err = "[error]   " + str + '\n';
      addLog("%s", err.c_str());
    }

    void Logger::addLog(const char* fmt, ...)
    {
      std::lock_guard<std::mutex> lock{ _mutex };
      int old_size = _buf.size();
      va_list args;
      va_start(args, fmt);
      _buf.appendfv(fmt, args);
      va_end(args);
      for (int new_size = _buf.size(); old_size < new_size; old_size++)
        if (_buf[old_size] == '\n')
          _lineOffsets.push_back(old_size + 1);
    }


    void    Logger::draw(const char* title, bool* p_open)
    {
      std::unique_lock<std::mutex> lock{ _mutex };
      ImGuiTextBuffer buf{ _buf };
      lock.unlock();

      if (!ImGui::Begin(title, p_open))
      {
        ImGui::End();
        return;
      }

      // Options menu
      if (ImGui::BeginPopup("Options"))
      {
        ImGui::Checkbox("Auto-scroll", &_bAutoScroll);
        ImGui::EndPopup();
      }

      // Main window
      if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
      ImGui::SameLine();
      bool clear = ImGui::Button("Clear");
      ImGui::SameLine();
      bool copy = ImGui::Button("Copy");
      ImGui::SameLine();
      _filter.Draw("Filter", -100.0f);

      ImGui::Separator();

      if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
      {
        if (clear)
          this->clear();
        if (copy)
          ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = _buf.begin();
        const char* buf_end = _buf.end();
        if (_filter.IsActive())
        {
          // In this example we don't use the clipper when _filter is enabled.
          // This is because we don't have random access to the result of our filter.
          // A real application processing logs with ten of thousands of entries may want to store the result of
          // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
          for (int line_no = 0; line_no < _lineOffsets.Size; line_no++)
          {
            const char* line_start = buf + _lineOffsets[line_no];
            const char* line_end = (line_no + 1 < _lineOffsets.Size) ? (buf + _lineOffsets[line_no + 1] - 1) : buf_end;
            if (_filter.PassFilter(line_start, line_end))
              ImGui::TextUnformatted(line_start, line_end);
          }
        }
        else
        {
          // The simplest and easy way to display the entire buffer:
          //   ImGui::TextUnformatted(buf_begin, buf_end);
          // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
          // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
          // within the visible area.
          // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
          // on your side is recommended. Using ImGuiListClipper requires
          // - A) random access into your data
          // - B) items all being the  same height,
          // both of which we can handle since we have an array pointing to the beginning of each line of text.
          // When using the filter (in the block of code above) we don't have random access into the data to display
          // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
          // it possible (and would be recommended if you want to search through tens of thousands of entries).
          ImGuiListClipper clipper;
          clipper.Begin(_lineOffsets.Size);
          while (clipper.Step())
          {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
            {
              const char* line_start = buf + _lineOffsets[line_no];
              const char* line_end = (line_no + 1 < _lineOffsets.Size) ? (buf + _lineOffsets[line_no + 1] - 1) : buf_end;
              ImGui::TextUnformatted(line_start, line_end);
            }
          }
          clipper.End();
        }
        ImGui::PopStyleVar();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (_bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
          ImGui::SetScrollHereY(1.0f);
      }
      ImGui::EndChild();
      ImGui::End();
    }
  }
}