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


#include <cassert>
#include <fstream>
#include <filesystem>

#include <imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Logger.h"
#include "HelpersOpenGl.h"

namespace helpers
{

  namespace opengl
  {

    std::vector<std::string> GetErrors()
    {
      std::vector<std::string> errors;
      GLenum err;
      while ((err = glGetError()) != GL_NO_ERROR) {
        std::string errorString;
        switch (err) {
        case GL_INVALID_ENUM:
          errorString = "GL_INVALID_ENUM";
          break;
        case GL_INVALID_VALUE:
          errorString = "GL_INVALID_VALUE";
          break;
        case GL_INVALID_OPERATION:
          errorString = "GL_INVALID_OPERATION";
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
          break;
        case GL_OUT_OF_MEMORY:
          errorString = "GL_OUT_OF_MEMORY";
          break;
        default:
          errorString = "Unknown Error";
          break;

        }
        errors.push_back(errorString);

      }
      return errors;
    }
    

    Texture::~Texture()
    {
      glDeleteTextures(1, &_handle);
    }

    bool Texture::init(const std::filesystem::path& path)
    {
      // load image
      stbi_set_flip_vertically_on_load(1);
      unsigned char* imageData = stbi_load(path.string().c_str(), &_width, &_height, NULL, 4);
      if (imageData == nullptr) {
        stbi_image_free(imageData);
        _errors.push_back("Cannot load image " + path.string());
        return false;
      }

      // prepare texture
      glGenTextures(1, &_handle);
      glBindTexture(GL_TEXTURE_2D, _handle);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // load texture
      glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
      
      // free resources
      stbi_image_free(imageData);

      const int result = glGetError();
      _errors = GetErrors();
      if (!_errors.empty()) {
        return false;
      }

      _isInit = true;
      return true;
    }


    std::shared_ptr<Texture> FactoryTexture::Create(const std::filesystem::path& path)
    {
      auto texture = std::make_shared<Texture>();
      if (!texture->init(path))
      {
        const auto& errors = texture->errors();
        if (!errors.empty())
        {
          std::string errMsg = "Cannot create texture " + path.string() + ':';
          for (const auto& error : texture->errors()) {
            errMsg += "\n\t" + error;
          }
          Logger::GetInstance()->error(errMsg);
        }
      }
      return texture;
    }


    bool Shader::init(const std::filesystem::path& path, const int type)
    {
      const auto logger = helpers::Logger::GetInstance();

      if (_type != NO_TYPE)
      {
        logger->error("Shader is already initialized");
        return false;
      }

      // read source file
      std::ifstream streamInVertex{ path };
      if (!streamInVertex.good()) {
        logger->error("Cannot open file " + path.string());
        return false;
      }
      std::string strSrc{ std::istreambuf_iterator<char>{streamInVertex}, {} };

      // create resource
      _handle = glCreateShader(type);
      if (_handle == 0)
      {
        logger->error("An error occured while creating a the shader");
        return false;
      }   

      // init
      if (compile(strSrc, type))
      {
        _type = type;
        return true;
      }
      else {
        return false;
      }
    }
     

    bool Shader::compile(const std::string& source, const int type)
    {

      assert(_handle != 0);

      const auto src = source.c_str();
      const auto logger = helpers::Logger::GetInstance();

      glShaderSource(_handle, 1, &src, NULL);
      glCompileShader(_handle);

      // check compilation errors
      int success;
      char info_log[512];
      glGetShaderiv(_handle, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(_handle, 512, NULL, info_log);
        logger->error("Shader compilation failed: " + std::string{ info_log });
        return false;
      }

      return true;    
    }


    std::shared_ptr<Shader> FactoryShader::Create(const std::filesystem::path& path, const int type)
    {
      auto shader = std::make_shared<Shader>();
      shader->init(path, type);
      return shader;
    }




    Program::Program(std::shared_ptr<Shader> pFragShader, std::shared_ptr<Shader> pVertShader, ILogger* pLogger)
      : _pFragShader{pFragShader}
      , _pVertShader{pVertShader}
      , _pLogger{&_loggerDefault}
    {
      if (pLogger != nullptr)
        _pLogger = pLogger;
    }


    Program::~Program()
    {
      if (_handle != 0)
        glDeleteProgram(_handle);
    }

    bool Program::init()
    {
      _handle = glCreateProgram();
      if(_handle != 0)
      {
        glAttachShader(_handle, _pFragShader->handle());
        glAttachShader(_handle, _pVertShader->handle());
      }
      return _handle != 0;
    }


    bool Program::build()
    {
      // link shaders into the program
      glLinkProgram(_handle);
      // check for linking errors
      int success;
      static char Info_Log[512];
      glGetProgramiv(_handle, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(_handle, 512, NULL, Info_Log);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << Info_Log << std::endl;
      }

      return true;
    }

    std::shared_ptr<Shader> Program::shader(const int type)
    {
      switch (type)
      {
      case GL_VERTEX_SHADER:
        return _pVertShader;
        break;
      case GL_FRAGMENT_SHADER:
        return _pFragShader;
        break;
      default:
        _pLogger->logError("Type must be GL_VERTEX_SHADER or GL_FRAGMENT_SHADER");
        _pLogger->logInfo("Returning type GL_FRAGMENT_SHADER by default");
        return _pFragShader; // Or something invalid?
        break;
      }
    }


    std::shared_ptr<Program> FactoryProgram::Create(std::shared_ptr<Shader> pFragShader, std::shared_ptr<Shader> pVertShader)
    {
      auto pProgram = std::make_shared<Program>(pFragShader, pVertShader);
      pProgram->init();
      return pProgram;
    }

} // opengl
} // helpers