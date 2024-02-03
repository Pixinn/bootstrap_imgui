#include <iostream>

#include "HelpersImgui.h"

namespace helpers
{


    WindowRender::~WindowRender()
    {
      if(_initialized)
      {
        glDeleteFramebuffers(1, &_frameBufferObject);
        glDeleteRenderbuffers(1, &_renderBufferObject);
        glDeleteTextures(1, &_texture);
      }
    }


    bool WindowRender::init()
    {
      if(!_initialized)
      {
        _initialized = true;

        glGenFramebuffers(1, &_frameBufferObject);
        glGenTextures(1, &_texture);
        glGenRenderbuffers(1, &_renderBufferObject); // render buffer object so OpenGl can do depth and stencil tests
      }

      return _initialized;
    }


    void WindowRender::bind()
    {

      // Get current window's size
      //ImGui::SetNextWindowSize({ 128.f, 128.f }, ImGuiCond_FirstUseEver);
      ImGui::Begin(_title.c_str());
      _size = ImGui::GetContentRegionAvail();
      ImGui::End();      
      
      // A minimal size is required for OpenGl 
      const float x = std::max({1.f, _size.x});
      const float y = std::max({1.f, _size.y });
     
      glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferObject); // now all ogl commands are from/to this framebuffer
      glViewport(0, 0, int(x + 0.5f), int(y + 0.5f));
     
      glGenTextures(1, &_texture);
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


    void WindowRender::unbind()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void WindowRender::draw()
    {
      ImGui::Begin(_title.c_str());
      ImGui::Image((void*)(intptr_t)_texture, _size);
      ImGui::End();
    }

}