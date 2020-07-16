#include <iostream>
#include <string>
#include "concurrent_queue.hpp"

concurrent_queue::ConcurrentQueue<std::string> queue;

void publisher(){
  std::cout << "Publisher started!" << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  queue.push("Hello from publisher!");
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Publisher calls queue.shutdown()!" << std::endl;
  queue.shutdown();
  std::cout << "Publisher finished!" << std::endl;
}

void consumer(){
  std::cout << "Consumer started!" << std::endl;
  std::cout << "Consumer check queue size = " << queue.size() << std::endl;
  std::cout << "Consumer check queue empty = " << queue.empty() << std::endl;
  std::shared_ptr<std::string> res_1 = queue.try_pop();
  if(res_1 == nullptr){
    std::cout << "Consumer try_pop: res_1 == nullptr" << std::endl;
  } else {
    std::cout << "Consumer try_pop: " << *res_1 << std::endl;
  }

  std::shared_ptr<std::string> res_2 = queue.wait_and_pop();
  std::cout << "Consumer wait_and_pop: " << *res_2 << std::endl;
  std::string value;
  queue.wait_and_pop(value);
  std::cout << "Consumer calls queue.isShutdown(): " << queue.isShutdown() << std::endl;
  std::cout << "Consumer finished!" << std::endl;
}

int main(){
  std::thread t1(publisher);
  std::thread t2(consumer);

  t1.join();
  t2.join();

  return 1;
}
