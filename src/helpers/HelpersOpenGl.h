#pragma once

#include <iostream>
#include <memory>

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

  } // opengl
   
} // helpers