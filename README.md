# cpp-examples

## [Concurrent Queue](concurrent_queue)

A concurrent queue with the shutdown & restart ability.

[Header only library](concurrent_queue/concurrent_queue.hpp)

```c++
concurrent_queue::ConcurrentQueue<std::string> queue;

void publisher(){
  queue.push("Hello from publisher!");
  queue.shutdown();
}

void consumer(){
  std::shared_ptr<std::string> res = queue.wait_and_pop();
  std::string value;
  queue.wait_and_pop(value);
}
```