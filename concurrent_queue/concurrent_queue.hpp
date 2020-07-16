#include <queue>
#include <mutex>
#include <memory>
#include <atomic>

namespace concurrent_queue{

template<typename T>
class ConcurrentQueue {
public:
  typedef std::queue<std::shared_ptr<T>> DataQueue;
  explicit ConcurrentQueue():
    mutex_(), data_queue_(), data_cond_(), shutdown_(false) {
  };
  ~ConcurrentQueue() = default;
  ConcurrentQueue(const ConcurrentQueue<T>&) = delete;
  void operator=(const ConcurrentQueue<T>&) = delete;

  // waits until it can push
  bool push(T new_value){
    if(shutdown_) return false;
    std::shared_ptr<T> data(std::make_shared<T>(std::move(new_value)));
    std::lock_guard<std::mutex> lk(mutex_);
    data_queue_.push(data);
    data_cond_.notify_one();
    return true;
  }

  // wait until data is available in the queue and return the value
  std::shared_ptr<T> wait_and_pop(){
    std::unique_lock<std::mutex> lk(mutex_);
    data_cond_.wait(lk, [this]{return !data_queue_.empty() || shutdown_;});
    if(shutdown_) return std::shared_ptr<T>();
    std::shared_ptr<T> res = data_queue_.front();
    data_queue_.pop();
    return res;
  }

  // wait until data is available in the queue and assign the value to the `value` parameter
  bool wait_and_pop(T& value){
    std::unique_lock<std::mutex> lk(mutex_);
    data_cond_.wait(lk, [this]{return !data_queue_.empty() || shutdown_;});
    if(shutdown_) return false;
    value = std::move(*data_queue_.front());
    data_queue_.pop();
    return true;
  }

  // pop without waiting for data availability. If empty queue the return nullptr.
  // the caller has to implement the wait functionality explicitly.
  // if need wait for data, use `wait_and_pop()`
  std::shared_ptr<T> try_pop(){
    std::lock_guard<std::mutex> lk(mutex_);
    if(data_queue_.empty() || shutdown_)
      return std::shared_ptr<T>();
    std::shared_ptr<T> res = data_queue_.front();
    data_queue_.pop();
    return res;
  }

  // check if the queue is empty
  bool empty() const {
    std::lock_guard<std::mutex> lk(mutex_);
    return data_queue_.empty();
  }

  // return the size of the queue
  size_t size() const {
    std::lock_guard<std::mutex> lk(mutex_);
    return data_queue_.size();
  }

  // shutdown the queue and notify all waiting threads
  void shutdown(){
    std::unique_lock<std::mutex> lk(mutex_);
    shutdown_ = true;
    lk.unlock();
    data_cond_.notify_all();
  }

  // restart the queue
  void restart(){
    std::unique_lock<std::mutex> lk(mutex_);
    shutdown_ = false;
    lk.unlock();
    data_cond_.notify_all();
  }

  // check if the queue is shutdown
  bool isShutdown() const{
    return shutdown_;
  }

private:
  mutable std::mutex mutex_;
  DataQueue data_queue_;
  std::condition_variable data_cond_;
  std::atomic_bool shutdown_;
};

}
