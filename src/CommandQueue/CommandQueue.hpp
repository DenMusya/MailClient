#ifndef COMMANDQUEUE
#define COMMANDQUEUE

#include <condition_variable>
#include <mutex>
#include <queue>

namespace mailclient::net {

template <typename T>
class CommandQueue {
 public:
  void push(const T& cmd) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(cmd);
    cv_.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return !queue_.empty(); });
    T cmd = std::move(queue_.front());
    queue_.pop();
    return cmd;
  }

 private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cv_;
};

}  // namespace mailclient::net

#endif
