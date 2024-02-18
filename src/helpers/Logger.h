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


#include <cassert>
#include <string>
#include <functional>
#include <thread>

#include <rlutil/Rlutil.h>

#include "TDequeConcurrent.h"

namespace helpers
{
        

    class Logger
    {

    public:
        enum eLevel {
            DEBUG,
            INFO,
            WARN,
            ERR
        };

        struct Log_t {
          std::string msg;
          eLevel      level;
        };

    public:

    static Logger* GetInstance()
    {
      if(_PInstance == nullptr)
      {
        _PInstance = new Logger();
        _PInstance->init();
      }
      return _PInstance;
    }

    ~Logger();

    bool log(const Log_t& log);
    inline bool error(const std::string& message)
    {
      return log({ message, ERR });
    }
    inline bool warning(const std::string& message)
    {
      return log({ message, WARN });
    }
    inline bool debug(const std::string& message)
    {
      return log({ message, DEBUG });
    }
    inline bool info(const std::string& message)
    {
      return log({ message, INFO });
    }

    private:

      Logger() = default;

      bool init();

          
      std::function<void(const Log_t&)> _print ///< Initialized to a default printer to stdout
      {
        [this](const Logger::Log_t& log) -> void 
        {
          rlutil::saveDefaultColor();
          std::unique_lock<std::mutex> lock{ this->_mutex };
          switch (log.level)
          {
          case helpers::Logger::eLevel::DEBUG:                
            std::cout << "DEBUG: ";
            break;
          case helpers::Logger::eLevel::INFO:
            rlutil::setColor(rlutil::GREEN);
            std::cout << "INFO: ";
            break;
          case helpers::Logger::eLevel::WARN:
            rlutil::setColor(rlutil::YELLOW);
            std::cout << "WARNING: ";
            break;
          case helpers::Logger::eLevel::ERR:
            rlutil::setColor(rlutil::RED);
            std::cout << "ERROR: ";
            break;
          default:
            assert(false);
            break;
          }
          std::cout << log.msg << '\n';
          rlutil::resetColor();
        }
      };

    private:

      static Logger* _PInstance;

      std::thread* _pThread = nullptr;
      std::mutex   _mutex;
      TDequeConcurrent<Log_t> _logs;
      std::atomic_bool _stopRequired;

    public:
            
        /// @brief Sets a custom printing function
        void setPrinter(decltype(_print) printer)
        {
          _print = printer;
        }

    };

} // namespace helpers 


