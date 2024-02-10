#pragma once

#include <iostream>
#include <memory>

#include "HelpersOpenGl.h"
#include "HelpersImgui.h"


namespace helpers 
{

  typedef void (*FctRenderFrame_t)(void* const param);
  typedef void (*FctProcessEvents_t)(void* const param, const SDL_Event events);

  struct Context
  {
    SDL_GLContext opengl = nullptr;
    SDL_Window* mainWindow = nullptr;

    Context() = default;
    ~Context();

    inline bool isValid() const { return _isValid; }

    bool _isValid = false;

  };


  class Renderer
  {

  public:

    bool init(int winWidth, int winHeight);

    void run();

    void setCallbacks(FctRenderFrame_t pFctRenderFrame, void* pRendererParam,
                      FctProcessEvents_t pFctProcessEvents, void* pProcessEventsParam);

  private:

    std::unique_ptr<Context> initOpengl(const int winWidth, const int winHeight);
    void initImgui();

    FctRenderFrame_t _pFctRenderFrame = nullptr;
    FctProcessEvents_t _pFctProcessEvents = nullptr;

    void* _pRendererParam = nullptr;
    void* _pProcessEventsParam = nullptr;

    std::unique_ptr<Context> _pContext;
  };

   
} // helpers