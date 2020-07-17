#include <glog/logging.h>
#include <thread>
#include "pipeline_module.hpp"

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

  void shutdownQueues() override {}

public:
  ExampleMIMOPipelineModule(const std::string module_id, bool sequential_mode)
  :MIMO(module_id, sequential_mode) {
  }
};

ExampleMIMOPipelineModule pipeline_module("ExampleMIMOPipelineModule", false);

void my_callback(const OutputSharedPtr &output){
  LOG(INFO) << "CB_1 receives: " << *output.get();
}

void my_second_callback(const OutputSharedPtr &output){
  LOG(INFO) << "CB_2 receives: " << *output.get();
}

void worker(){
  pipeline_module.spin();
}

int main(int argc, char* argv[]){
  google::InstallFailureSignalHandler();
  google::InitGoogleLogging(argv[0]);

  pipeline_module.registerOutputCallback(my_callback);
  pipeline_module.registerOutputCallback(my_second_callback);

  std::thread t1(worker);

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  pipeline_module.shutdown();

  t1.join();
  return 1;
}