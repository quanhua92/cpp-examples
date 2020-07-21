#include <glog/logging.h>
#include <thread>
#include "pipeline_module.hpp"
#include "concurrent_queue.hpp"

using MIMO = modular_pipeline::MIMOPipelineModule<std::string, std::string>;
using OutputSharedPtr = MIMO::OutputSharedPtr;

class ExampleMIMOPipelineModule : public MIMO {
protected:
  PIO::InputUniquePtr prepareInputPayload() override {
    return std::make_unique<std::string>("a string from prepareInputPayload");
  }
  PIO::OutputSharedPtr spinOnce(PIO::InputUniquePtr input) override{
    std::string output_string = "[Output] = " + *input.get();
    return std::make_shared<std::string>(output_string);
  }

public:
  ExampleMIMOPipelineModule(const std::string module_id, bool sequential_mode)
  :MIMO(module_id, sequential_mode) {
  }
};

ExampleMIMOPipelineModule pipeline_module("ExampleMIMOPipelineModule", false);

using SIMO = modular_pipeline::SIMOPipelineModule<std::string, std::string>;
using SIMOQueue = concurrent_queue::ConcurrentQueue<SIMO::InputUniquePtr>;
using SIMOQueueSharedPtr = std::shared_ptr<SIMOQueue>;
SIMOQueueSharedPtr simo_input_queue = std::make_shared<SIMOQueue>();

class ExampleSIMOPipelineModule : public SIMO {
public:
  ExampleSIMOPipelineModule(SIMOQueueSharedPtr input_queue, const std::string module_id, bool sequential_mode)
      :SIMO(input_queue, module_id, sequential_mode) {
  }

protected:
  PIO::OutputSharedPtr spinOnce(PIO::InputUniquePtr input){
    std::string output_string = "[Output] = " + *input.get();
    return std::make_shared<std::string>(output_string);
  }
};

ExampleSIMOPipelineModule simo_pipeline_module(simo_input_queue, "ExampleSIMOPipelineModule", false);


void my_callback(const OutputSharedPtr &output){
  LOG(INFO) << "CB_1 receives: " << *output.get();
}

void my_second_callback(const OutputSharedPtr &output){
  LOG(INFO) << "CB_2 receives: " << *output.get();
}

void worker(){
  pipeline_module.spin();
}

void simo_worker(){
  simo_pipeline_module.spin();
}

int main(int argc, char* argv[]){
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);

#if 0
  pipeline_module.registerOutputCallback(my_callback);
  pipeline_module.registerOutputCallback(my_second_callback);

  std::thread t1(worker);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  pipeline_module.shutdown();
  t1.join();
#endif

#if 1
  simo_input_queue->push(std::make_unique<std::string>("Message 01"));
  simo_input_queue->push(std::make_unique<std::string>("Message 02"));
  simo_input_queue->push(std::make_unique<std::string>("Message 03"));

  simo_pipeline_module.registerOutputCallback(my_callback);
  simo_pipeline_module.registerOutputCallback(my_second_callback);

  std::thread t1(simo_worker);
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  simo_pipeline_module.shutdown();
  t1.join();
#endif



  return 1;
}