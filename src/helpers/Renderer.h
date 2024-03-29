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