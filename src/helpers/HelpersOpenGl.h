#pragma once

#include <iostream>
#include <optional>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

namespace helpers
{

  namespace opengl
  {

    static constexpr int MAJOR_VERSION = 3;
    static constexpr int MINOR_VERSION = 3;

    static constexpr auto PROFILE = SDL_GLprofile::SDL_GL_CONTEXT_PROFILE_CORE;  // OpenGL core profile - deprecated functions are disabled
    static constexpr auto GLSL_VERSION = "#version 330 core";

    // Inits OpenGL
    std::optional<std::pair<SDL_GLContext, SDL_Window*>> Init(const int winWidth, const int winHeight);

  }

}