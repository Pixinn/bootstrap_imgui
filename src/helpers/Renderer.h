#pragma once

#include <iostream>
#include <memory>

#include "HelpersOpenGl.h"
#include "HelpersImgui.h"


namespace helpers 
{

  class Runnable
  {
  public:
    virtual int run() = 0;

    virtual void renderFrame() = 0;

    virtual void processEvent(const SDL_Event events) = 0;
  };


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

    void setRunnable(Runnable* pRunnable)
    {
      _pRunnable = pRunnable;
    }

  private:

    std::unique_ptr<Context> initOpengl(const int winWidth, const int winHeight);
    void initImgui();

    Runnable* _pRunnable = nullptr;

    std::unique_ptr<Context> _pContext;
  };

   
} // helpers