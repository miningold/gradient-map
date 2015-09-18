#pragma once

#include <mutex>
#include <condition_variable>

namespace knight {

class Semaphore {
 public:
  Semaphore(int count = 0) : count_{count} { }

  void Notify() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      count_++;
    }
    condition_.notify_one();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this]{ return count_ > 0; });
    count_--;
  }

  bool TryWait() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count_ > 0) {
      count_--;
      return true;
    } else {
      return false;
    }
  }

 private:
  std::mutex mutex_;
  std::condition_variable condition_;
  int count_;
};

} // namespace knight