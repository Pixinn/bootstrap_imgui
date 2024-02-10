#pragma once

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include "Renderer.h"

namespace helpers 
{


  Context::~Context()
  {
    SDL_GL_DeleteContext(opengl);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
  }



  bool Renderer::init(int winWidth, int winHeight)
  {
    // # OpenGl init
    _pContext = std::move(initOpengl(winWidth, winHeight));
    if (!_pContext->isValid())
    {
      std::cerr << "Cannot init SDL and OpenGl" << std::endl;
      return false;
    }

    // # DearImgui init
    initImgui();

    return true;
  }



  void Renderer::run()
  {
    SDL_Event event;
    bool quit = false;
    while (!quit)
    {

      // ## process events
      while (SDL_PollEvent(&event))
      {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
          quit = true;
        }
        if (event.type == SDL_WINDOWEVENT
          && event.window.event == SDL_WINDOWEVENT_CLOSE
          && event.window.windowID == SDL_GetWindowID(_pContext->mainWindow))
        {
          quit = true;
        }
      }

      // ## New frame
      // ### imgui
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplSDL2_NewFrame(_pContext->mainWindow);
      ImGui::NewFrame();

      // # RUN CUSTOM RENDER CODE
      _pFctRenderFrame(_pRendererParam);

      // # imgui render
      ImGui::Render();

      // # Display
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      SDL_GL_SwapWindow(_pContext->mainWindow); // swap the buffers work and display buffers
    }
  }


  void Renderer::setCallbacks(FctRenderFrame_t pFctRenderFrame, void* pRendererParam,
    FctProcessEvents_t pFctProcessEvents, void* pProcessEventsParam)
  {
    _pFctRenderFrame = pFctRenderFrame;
    _pRendererParam = pRendererParam;
    _pFctProcessEvents = pFctProcessEvents;
    _pProcessEventsParam = pProcessEventsParam;
  }


  std::unique_ptr<Context> Renderer::initOpengl(const int winWidth, const int winHeight)
  {

    std::unique_ptr<Context> pContext = std::make_unique<Context>();

    // # SDL Init
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      std::cerr << "There was an error initing SDL2: " << SDL_GetError() << std::endl;
      return pContext;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, opengl::PROFILE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opengl::MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opengl::MINOR_VERSION);

    // # SDL Window creation
    pContext->mainWindow = SDL_CreateWindow("Bootstrap DearImgui SDL2 OpenGL33", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_OPENGL);
    if (pContext->mainWindow == nullptr)
    {
      std::cerr << "There was an error creating the window: " << SDL_GetError() << std::endl;
      return pContext;
    }

    // # OpenGL global pContext
    pContext->opengl = SDL_GL_CreateContext(pContext->mainWindow);
    if (pContext->opengl == nullptr)
    {
      std::cerr << "There was an error creating OpenGL pContext: " << SDL_GetError() << std::endl;
      return pContext;
    }
    // # GLEW init
    if (glewInit() != GLEW_OK)
    {
      std::cout << "ERROR: Cannot init GLEW" << std::endl;
      return pContext;
    }

    // select flat or smooth shading
    glShadeModel(GL_SMOOTH);

    // setup the viewport
    glViewport(0, 0, winWidth, winHeight);

    // swap syncronized with the monitor's vertical refresh
    SDL_GL_SetSwapInterval(1);

    pContext->_isValid = true;
    return pContext;
  }



  void Renderer::initImgui()
  {
    // # Imgui Init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(_pContext->mainWindow, _pContext->opengl);
    ImGui_ImplOpenGL3_Init(opengl::GLSL_VERSION);
  }
   
} // helpers
