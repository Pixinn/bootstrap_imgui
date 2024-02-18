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

#include <mutex>

#include "Logger.h"


namespace helpers
{

    Logger* Logger::_PInstance = nullptr;


    Logger::~Logger()
    {
        if(_pThread != nullptr)
        {
          _stopRequired.store(true);
            _pThread->join();
            delete _pThread;
        }
    }

    bool Logger::log(const Log_t& log)
    {
      if (_pThread == nullptr) {
        return false;
      }
      
      _logs.emplace_back(log);
      return true;
    }

    bool Logger::init()
    {        
        if(_pThread == nullptr)
        {
            _pThread = new std::thread(
                [this]()
                {
                    while(!this->_stopRequired.load())
                    {
                        const auto log = _logs.pop_front();
                        _print(log);
                    }
                }
            );
            return _pThread != nullptr;  
        }
        _stopRequired = false;
        return false;
    }

} // helpers
