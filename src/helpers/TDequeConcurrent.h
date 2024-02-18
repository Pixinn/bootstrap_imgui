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

#include <deque>
#include <mutex>
#include <condition_variable>


  namespace helpers {

  //! @brief A templated *thread-safe* collection based on dequeue
  //!
  //! @details    pop_front() waits for the notification of a filling method if the collection is empty.
  //!             The various "emplace" operations are factorized by using the generic "addData_protected".
  //!             This generic asks for a concrete operation to use, which can be passed as a lambda.
  //!             **There is no control of flow**: this can lead to memory allocation errors!
  template< typename T >
  class TDequeConcurrent {

  public:
      //! \brief Emplaces a new instance of T in front of the deque
      template<typename... Args>
      void emplace_front(Args&&... args)
      {
          addData_protected([&] {
              _collection.emplace_front(std::forward<Args>(args)...);
          });
      }

      //! \brief Emplaces a new instance of T at the back of the deque
      template<typename... Args>
      void emplace_back(Args&&... args)
      {
          addData_protected([&] {
              _collection.emplace_back(std::forward<Args>(args)...);
          });
      }

      //! \brief Clears the deque
      void clear(void)
      {
          std::lock_guard<std::mutex> lock(_mutex);
          _collection.clear();
      }

      //! \brief Returns the front element and removes it from the collection
      //!
      //!        No exception is ever returned as we garanty that the deque is not empty
      //!        before trying to return data.
      T pop_front(void) noexcept
      {
          std::unique_lock<std::mutex> lock{ _mutex };
          while (_collection.empty()) {
              _condNewData.wait(lock);
          }
          auto elem = std::move(_collection.front());
          _collection.pop_front();
          return elem;
      }



  private:

      //! \brief Protects the deque, calls the provided function and notifies the presence of new data
      //! \param The concrete operation to be used. It MUST be an operation which will add data to the deque,
      //!        as it will notify that new data are available!
      template<class F>
      void addData_protected(F&& fct)
      {
          std::unique_lock<std::mutex> lock{ _mutex };
          fct();
          lock.unlock();
          _condNewData.notify_one();
      }

      std::deque<T> _collection;              ///< Concrete, not thread safe, storage.
      std::mutex    _mutex;                   ///< Mutex protecting the concrete storage
      std::condition_variable _condNewData;   ///< Condition used to notify that new data are available.
  };

}


