#pragma once
#include <future>
#include <functional>
#include "EventHandler.h"

namespace utils
{

  /*!
    \brief Controller for binding asynchronous processes.

    Any function bound to Run function of this class will be executed asynchronously,
    and checked for finishing. Class can automatically notify user about finishing
    using PollEvents function of EventHandler super class.
  */
  class ConcurrentBinding : public EventHandler
  {
  public:
    //! Execute provided function asynchronously from a caller thread.
    void Run(const std::function<bool(void)> &func)
    {
      result_ = std::async(
          std::launch::async,
          std::bind(&ConcurrentBinding::ParallelFunction, this, func)
        );
    }

    //! Check if asynchronous function is still running.
    bool IsRunning() const
    {
      if (result_.valid())
      {
        using namespace std::chrono_literals;
        const auto status = result_.wait_for(0ms);
        return status != std::future_status::ready;
      }
      return false;
    }

    //! Set callback which will be called via PollEvents after asynchronous function is finished.
    void SetCallbackOnFinish(const std::function<void(bool)> &func)
    {
      callback_on_finish_ = func;
    }

  private:
    std::function<void(bool)> callback_on_finish_;
    std::future<bool> result_;

    bool ParallelFunction(const std::function<bool(void)> &func)
    {
      const auto result = func();
      RegisterEvent("Finished");
      return result;
    }

    virtual void ProcessEvent(const Event *e) override
    {
      if (e->id == "Finished" && callback_on_finish_)
      {
        callback_on_finish_(result_.get());
      }
    }
  };

}