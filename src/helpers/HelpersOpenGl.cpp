#include <optional>

#include "HelpersOpenGl.h"

namespace helpers
{

  namespace opengl
  {

    std::optional<std::pair<SDL_GLContext, SDL_Window*>> Init(const int winWidth, const int winHeight)
    {
      // # SDL Init
      if (SDL_Init(SDL_INIT_VIDEO) < 0) 
      {
        std::cerr << "There was an error initing SDL2: " << SDL_GetError() << std::endl;
        exit(1);
      }
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, PROFILE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);

      // # SDL Window creation
      SDL_Window* const window = SDL_CreateWindow("Bootstrap DearImgui SDL2 OpenGL33", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_OPENGL);
      if (window == nullptr) 
      {
        std::cerr << "There was an error creating the window: " << SDL_GetError() << std::endl;
        return std::nullopt;
      }

      // # OpenGL global context
      SDL_GLContext context_ogl = SDL_GL_CreateContext(window);
      if (context_ogl == nullptr)
      {
        std::cerr << "There was an error creating OpenGL context: " << SDL_GetError() << std::endl;
        return std::nullopt;
      }
      // # GLEW init
      if (glewInit() != GLEW_OK)
      {
        std::cout << "ERROR: Cannot init GLEW" << std::endl;
        return std::nullopt;
      }

      // select flat or smooth shading
      glShadeModel(GL_SMOOTH);

      // setup the viewport
      glViewport(0, 0, winWidth, winHeight);

      // swap syncronized with the monitor's vertical refresh
      SDL_GL_SetSwapInterval(1);

      return std::make_optional(std::make_pair(context_ogl, window));
      }
  }
}