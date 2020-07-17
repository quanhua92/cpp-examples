#ifndef MODULAR_PIPELINE_PIPELINE_MODULE_HPP
#define MODULAR_PIPELINE_PIPELINE_MODULE_HPP
#include <atomic>
#include <memory>

namespace modular_pipeline {

/**
 * This is an abstract class of a PipeLine module. Template on expected input and expected output payloads.
 * This class handles spinning the module by getting and sending from/to the input/output.
 * If is_sequential is False, the spin() method will run once and must be called by the caller when needed.
 */
template <typename Input, typename Output>
class PipelineModule {
public:
  using InputUniquePtr = std::unique_ptr<Input>;
  using OutputSharedPtr = std::shared_ptr<Output>;

  PipelineModule(const std::string &module_id, const bool &sequential_mode)
  : shutdown_(false), module_id_(module_id), sequential_mode_(sequential_mode) {};
  virtual ~PipelineModule() {
    LOG(INFO) << logPrefix() << "destructor called!";
  }

  /**
   * The spin() function will be called by a thread. Run until the module is shutdown.
   */
  void spin(){
    while(!shutdown_){
      InputUniquePtr input = prepareInputPayload();
      if(input){
        OutputSharedPtr output = spinOnce(std::move(input));
        if(output){
          if(sendOutputPayload(output)){
            VLOG(2) << logPrefix() << "sent output!";
          } else {
            LOG(WARNING) << logPrefix() << "send output failed!";
          }
        } else {
          LOG_IF(WARNING, VLOG_IS_ON(1))
          << logPrefix() << "No output payload";
          // TODO: call failure callbacks to notify other modules
        }
      } else {
        LOG_IF(WARNING, VLOG_IS_ON(1))
          << logPrefix() << "No input payload";
      }

      // Break the while loop if we are in the sequential mode
      if(sequential_mode_){
        return;
      }
    }
  }

  /**
   * Stop the module
   */
  virtual void shutdown(){
    LOG_IF(WARNING, shutdown_) << logPrefix() << "shutdown() is already requested before.";
    LOG(INFO) << logPrefix() << "Shutdown all queues";
    shutdownQueues();
    shutdown_ = true;
    LOG(INFO) << logPrefix() << "shutdown finished!";
  }

  /**
   * Return True if the module is processing data. False if waiting for input.
   */
  inline bool isWorking() const{
    return is_working_;
  }
protected:
  /**
   * abstract function to prepare input payload which will be sent to spinOnce method
   */
  virtual InputUniquePtr prepareInputPayload() = 0;

  /**
   * abstract function to handle the input pointer and return the output pointer
   * @param input: an input payload unique ptr created by prepareInputPayload
   * @return the output payload of the pipeline. Return a nullptr if don't want to send to output modules
   */
  virtual OutputSharedPtr spinOnce(InputUniquePtr input) = 0;

  /** abstract function to send the output of spinOnce to output consumers
   * One can implement this to call a set of callbacks from different modules.
   */
  virtual bool sendOutputPayload(OutputSharedPtr output) = 0;

  /**
   * abstract function to shutdown all the input and output queues if needed.
   */
  virtual void shutdownQueues() = 0;

  virtual inline std::string logPrefix(){
    return "Module [" + module_id_  + "]: ";
  }
private:
  std::atomic_bool is_working_;
  std::atomic_bool shutdown_;
  std::string module_id_;
  bool sequential_mode_;
};

/**
 * Multiple Input Multiple Output (MIMO) pipeline module
 * This is still an abstract class, user needs to implement:
 * - prepareInputPayload()
 * - spinOnce()
 * - shutdownQueues()
 * By default, the multiple outputs are handled using callbacks
 * We don't know how the user wants with multiple inputs.
 * Note:
 * - One can receive multiple inputs via callbacks. Then, create an input payload in getInputPayload() method.
 * - One can override the sendOutputPayload() to send data to multiple queues and callbacks.
 */
template <typename Input, typename Output>
class MIMOPipelineModule : public PipelineModule<Input, Output> {
public:
  using PIO = PipelineModule<Input, Output>;
  using OutputCallback = std::function<void(const typename PIO::OutputSharedPtr & output)>;

  MIMOPipelineModule(const std::string &module_id, const bool &sequential_mode)
    : PipelineModule(module_id, sequential_mode), output_callbacks_(){}

  /**
   * Register a output callback to receive the output payload.
   * This will be called in sendOutputPayload
   */
   virtual void registerOutputCallback(const typename OutputCallback& callback){
     CHECK(callback) << "callback can't be nullptr";
     output_callbacks_.push_back(callback);
   }
protected:

  /**
   * This function will call a set of output callbacks to send the output payload
   * @param output : the output payload created by spinOnce()
   * @return False if can not send the output payload
   */
  bool sendOutputPayload(typename PIO::OutputSharedPtr output) override {
    for(OutputCallback &cb : output_callbacks_){
      cb(output);
    }
    return true;
  }

private:
  std::vector<OutputCallback> output_callbacks_;
};

}
#endif //MODULAR_PIPELINE_PIPELINE_MODULE_HPP
