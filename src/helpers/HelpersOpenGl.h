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

#include <iostream>
#include <memory>
#include <unordered_map>
#include <filesystem>

#include <GL/glew.h>
#include <SDL2/SDL.h>

namespace helpers 
{

  namespace opengl
  {

    static constexpr int MAJOR_VERSION = 3;
    static constexpr int MINOR_VERSION = 3;

    static constexpr auto PROFILE = SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE;  // OpenGL core profile - deprecated functions are disabled
    static constexpr auto GLSL_VERSION = "#version 330 core";

    class ILogger
    {
    public:
      virtual void logDebug(const std::string& str) = 0;
      virtual void logInfo(const std::string& str) = 0;
      virtual void logWarning(const std::string& str) = 0;
      virtual void logError(const std::string& str) = 0;
    };

    class LoggerDefault : public ILogger
    {

      virtual void logDebug(const std::string& str) override
      {
        std::cout << "[debug]: " << str << std::endl;
      }

      virtual void logInfo(const std::string& str) override
      {
        std::cout << "[logInfo]: " << str << std::endl;
      }

      virtual void logWarning(const std::string& str) override
      {
        std::cout << "[warning]: " << str << std::endl;
      }

      virtual void logError(const std::string& str) override
      {
        std::cout << "[error]: " << str << std::endl;
      }

    };

    class Texture
    {
    public:

      Texture() = default;
      ~Texture();

      /// @brief Loads initializes an RGBA texture
      /// @return false if an error occured
      bool init(const std::filesystem::path& path);

      const std::string& error() const { return _lastError; }

      inline int width() const { return _width;  }
      inline int height() const { return _height; }
      inline int handle() const { return _handle; }

      bool isInit() const { return _isInit; }

    private:
      int _width = 0;
      int _height = 0;
      GLuint   _handle = 0u;
      bool   _isInit = false;
      std::string _lastError;
    };



    class FactoryTexture
    {
    public:

      /// @brief Instanciates a RGBA texture
      /// @param path Path of the RGBA texture
      static std::shared_ptr<Texture> Create(const std::filesystem::path& path);

    };


    /// @brief Wrapper around a shader
  /// @details Useful to free the resources with it's destructor
    class Shader
    {
    public:

      static constexpr int NO_TYPE = -1;

      Shader() = default;
      Shader(Shader&& rhs) = default;

      inline ~Shader()
      {
        glDeleteShader(_handle);
      }

      Shader operator=(const Shader& rhs) = delete;

      /// @brief Loads and compiles the shader
      /// @param path Path of the shader's source code
      /// @param type Type of the shader: **GL_VERTEX_SHADER** or **GL_FRAGMENT_SHADER**
      bool init(const std::filesystem::path& path, const int type);

      /// @brief Compiles the shader from a textual source code
      /// @param source Source code
      /// @param type Type of the shader: **GL_VERTEX_SHADER** or **GL_FRAGMENT_SHADER**
      bool compile(const std::string& source, const int type);

      /// @brief returns the handle to the compiled shader program
      inline GLuint handle() const
      {
        return _handle;
      }

    private:
      GLuint _handle;
      int _type = NO_TYPE;
    };

    class FactoryShader
    {
    public:

      /// @brief Instanciates either a vertex or a fragment shader
      /// @param path Path of the shader's source code
      /// @param type Type of the shader: **GL_VERTEX_SHADER** or **GL_FRAGMENT_SHADER**
      static std::shared_ptr<Shader> Create(const std::filesystem::path& path, const int type);

    };


    class Program
    {
    public:
      Program(std::shared_ptr<Shader> pFragShader, std::shared_ptr<Shader> pVertShader, ILogger* pLogger = nullptr);
      ~Program();

      bool init();
      bool build();

      inline GLuint handle() const
      {
        return _handle;
      }

      /// @brief Reurns a shared pointer to one of the shader
      /// @param type Type of the shader: **GL_VERTEX_SHADER** or **GL_FRAGMENT_SHADER**
      std::shared_ptr<Shader> shader(const int type);

    private:
      GLuint _handle = 0;
      std::shared_ptr<Shader> _pFragShader;
      std::shared_ptr<Shader> _pVertShader;
      ILogger* _pLogger;
      LoggerDefault _loggerDefault;
    };


    class FactoryProgram
    {
    public:

      /// @brief Instanciates an OpenGL program
      static std::shared_ptr<Program> Create(std::shared_ptr<Shader> pFragShader, std::shared_ptr<Shader> pVertShader);

    };


  } // opengl
   
} // helpers