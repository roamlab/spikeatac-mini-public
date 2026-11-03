#include <SaTMiniControlBoardv1.h>

// Global definition of error_loop used to handle micro-ROS return codes
// void error_loop(uint16_t freq){
//   uint16_t period = (int)(1000/freq);
//   while(1){
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     delay(period);
//   }
// }

/***********************************************************************************************/
/************ CONSTRUCTOR, DESTRUCTOR, INIT AND RETURN CODE HANDLING ***************************/
/***********************************************************************************************/


SaTMiniControlBoardv1* SaTMiniControlBoardv1::instance_ = nullptr;


SaTMiniControlBoardv1::SaTMiniControlBoardv1(uint32_t fb_tick_us, uint32_t spikeatac_accel_pvdf_tick_us,
                             uint8_t num_spikeatacs, SpikeatacMiniConfig* spikeatac_config){
  instance_ = this;

  fb_tick_us_ = fb_tick_us;
  spikeatac_accel_pvdf_tick_us_ = spikeatac_accel_pvdf_tick_us;
  num_spikeatacs_ = num_spikeatacs;
  spikeatac_cfg_ = spikeatac_config;

  // Sum up all sensors to come up with the total num of capacitive signals
  total_num_capacitive_signals_ = 0;
  if (spikeatac_cfg_ != NULL){
    for(uint8_t i=0; i < num_spikeatacs; i++){
      total_num_capacitive_signals_ += spikeatac_cfg_[i].num_cdc_signals;
    }
  }
  // Sum up all sensors to come up with the total num of accel signals
  total_num_accel_signals_ = 0;
  if (spikeatac_cfg_ != NULL){
    for(uint8_t i=0; i < num_spikeatacs; i++){
      total_num_accel_signals_ += spikeatac_cfg_[i].num_accel_signals;
    }
  }

  // Sum up all sensors to come up with the total num of pvdf signals
  total_num_pvdf_signals_ = 0;
  if (spikeatac_cfg_ != NULL){
    for(uint8_t i=0; i < num_spikeatacs; i++){
      total_num_pvdf_signals_ += spikeatac_cfg_[i].num_adc_signals;
    }
  }
  
}


SaTMiniControlBoardv1::~SaTMiniControlBoardv1(){

  if(spikeatac_cfg_ != NULL){
    if (spikeatac_cfg_[0].accel_on) {
      free(accel_fb_msg_.layout.dim.data);
      free(accel_fb_msg_.data.data);
      RCCHECK(rcl_publisher_fini(&accel_fb_publisher_, &node_));
    }
    if (spikeatac_cfg_[0].cdc_on) {
      free(capacitive_fb_msg_.layout.dim.data);
      free(capacitive_fb_msg_.data.data);
      RCCHECK(rcl_publisher_fini(&capacitive_fb_publisher_, &node_));
    }
    if (spikeatac_cfg_[0].adc_on) {
      free(pvdf_fb_msg_.layout.dim.data);
      free(pvdf_fb_msg_.data.data);
      RCCHECK(rcl_publisher_fini(&pvdf_fb_publisher_, &node_));
    }
    delete spikeatac_cfg_;
    delete spikeatac_controller_;
  }
  // Destroy the node
  RCCHECK(rcl_node_fini(&node_));  
}


bool SaTMiniControlBoardv1::initialize(){

  //Set digital input/outputs
  pinMode(LED_BUILTIN, OUTPUT);                  //Teensy board LED
  //Turn ON LED on Teensy until rosserial connection is established.
  teensyLedOn();
  
  /*** Microros Setup ***/
  set_microros_transports(); 
  
  allocator_ = rcl_get_default_allocator();

  // Create init_options -- if no agent is running, RCL_RET_ERROR is returned. Try until agent connection established
  // note that rclc_support_init also seems to wait for ~10 seconds internally, so doing something inside of the while loop (e.g. blinking an LED) is unpredictable.
  while (rclc_support_init(&support_, 0, NULL, &allocator_) != RCL_RET_OK)
  {
    delay(100);
  }
  
  // Create node
  RCCHECK(rclc_node_init_default(&node_, "satmini_controller_node", "", &support_));
  // Init executor
  executor_ = rclc_executor_get_zero_initialized_executor();
  // total number of handles = #subscriptions + #timers + #Services = 1 + 1 + 1
  RCCHECK(rclc_executor_init(&executor_, &support_.context, 3, &allocator_));

  // Only do this if we are operating with Spikeatac sensors
  if(spikeatac_cfg_ != NULL){

    // check that configs have all true or all false for accel_on and cdc_on

    // allocate memory for spikeatac controllers
    spikeatac_controller_ = new SpikeatacMiniController[num_spikeatacs_];
    for (uint8_t i = 0; i < num_spikeatacs_; i++) {
      if (!spikeatac_controller_[i].start(&spikeatac_cfg_[i])){
        error_loop(SPIKEATAC_ERROR);
      }
    }

    if (spikeatac_cfg_[0].cdc_on) {
      if(!initCapacitiveMultiArrayMsg(&capacitive_fb_msg_)){
        error_loop(SPIKEATAC_ERROR);
      }
      // create capacitive publisher
      rclc_publisher_init_best_effort(
          &capacitive_fb_publisher_,
          &node_,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt16MultiArray),
          "satmini_capacitive_feedback");
    }

    if (spikeatac_cfg_[0].accel_on) {
      if(!initAccelMultiArrayMsg(&accel_fb_msg_)){
        error_loop(SPIKEATAC_ERROR);
      }
      // create accel publisher
      rclc_publisher_init_best_effort(
          &accel_fb_publisher_,
          &node_,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
          "satmini_accel_feedback");
    }

    if (spikeatac_cfg_[0].adc_on) {
      if(!initPvdfMultiArrayMsg(&pvdf_fb_msg_)){
        error_loop(SPIKEATAC_ERROR);
      }
      // create pvdf publisher
      rclc_publisher_init_best_effort(
          &pvdf_fb_publisher_,
          &node_,
          ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
          "satmini_pvdf_feedback");
    }


  }

  //Other initializations
  current_state_ = DECIDE_ACTION;
  fb_ticker_.begin(isrFeedback, fb_tick_us_);
  spikeatac_accel_pvdf_ticker_.begin(isrAccelPvdf, spikeatac_accel_pvdf_tick_us_);
  teensyLedOff();
  return true;
}


bool SaTMiniControlBoardv1::initAccelMultiArrayMsg(std_msgs__msg__Int32MultiArray * msg){
  msg->layout.dim.data = (std_msgs__msg__MultiArrayDimension*) malloc(num_spikeatacs_ *
                          sizeof(std_msgs__msg__MultiArrayDimension));
  if(msg->layout.dim.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->layout.dim.size = num_spikeatacs_;
  msg->layout.dim.capacity = num_spikeatacs_;
  msg->layout.data_offset = 0;
  uint8_t stride_count = 0;
  for(uint8_t i=0; i < num_spikeatacs_; i++){
    msg->layout.dim.data[i].label.data = &spikeatac_cfg_[i].label[0];
    msg->layout.dim.data[i].label.size = spikeatac_cfg_[i].label.length();    
    msg->layout.dim.data[i].label.capacity = spikeatac_cfg_[i].label.length();
    msg->layout.dim.data[i].size = spikeatac_cfg_[i].num_accel_signals;
    msg->layout.dim.data[i].stride =  stride_count;
    stride_count += msg->layout.dim.data[i].size;
  }
  msg->data.data = (int32_t*)malloc(total_num_accel_signals_ * sizeof(int32_t));
  if(msg->data.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->data.size = total_num_accel_signals_;
  msg->data.capacity=  total_num_accel_signals_;
  return true;
}


bool SaTMiniControlBoardv1::initCapacitiveMultiArrayMsg(std_msgs__msg__UInt16MultiArray * msg) {
  msg->layout.dim.data = (std_msgs__msg__MultiArrayDimension*) malloc(num_spikeatacs_ *
                          sizeof(std_msgs__msg__MultiArrayDimension));
  if(msg->layout.dim.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->layout.dim.size = num_spikeatacs_;
  msg->layout.dim.capacity = num_spikeatacs_;
  msg->layout.data_offset = 0;
  uint8_t stride_count = 0;
  for(uint8_t i=0; i < num_spikeatacs_; i++){
    msg->layout.dim.data[i].label.data = &spikeatac_cfg_[i].label[0];
    msg->layout.dim.data[i].label.size = spikeatac_cfg_[i].label.length();    
    msg->layout.dim.data[i].label.capacity = spikeatac_cfg_[i].label.length();
    msg->layout.dim.data[i].size = spikeatac_cfg_[i].num_cdc_signals;
    msg->layout.dim.data[i].stride =  stride_count;
    stride_count += msg->layout.dim.data[i].size;
  }
  msg->data.data = (uint16_t*)malloc(total_num_capacitive_signals_ * sizeof(uint16_t));
  if(msg->data.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->data.size = total_num_capacitive_signals_;
  msg->data.capacity = total_num_capacitive_signals_;
  return true;
}


bool SaTMiniControlBoardv1::initPvdfMultiArrayMsg(std_msgs__msg__Int32MultiArray * msg){
  msg->layout.dim.data = (std_msgs__msg__MultiArrayDimension*) malloc(num_spikeatacs_ *
                          sizeof(std_msgs__msg__MultiArrayDimension));
  if(msg->layout.dim.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->layout.dim.size = num_spikeatacs_;
  msg->layout.dim.capacity = num_spikeatacs_;
  msg->layout.data_offset = 0;
  uint8_t stride_count = 0;
  for(uint8_t i=0; i < num_spikeatacs_; i++){
    msg->layout.dim.data[i].label.data = &spikeatac_cfg_[i].label[0];
    msg->layout.dim.data[i].label.size = spikeatac_cfg_[i].label.length();    
    msg->layout.dim.data[i].label.capacity = spikeatac_cfg_[i].label.length();
    msg->layout.dim.data[i].size = spikeatac_cfg_[i].num_adc_signals;
    msg->layout.dim.data[i].stride =  stride_count;
    stride_count += msg->layout.dim.data[i].size;
  }
  msg->data.data = (int32_t*)malloc(total_num_pvdf_signals_ * sizeof(int32_t));
  if(msg->data.data==NULL){
    error_loop(SPIKEATAC_ERROR);
    return false;
  }
  msg->data.size = total_num_pvdf_signals_;
  msg->data.capacity = total_num_pvdf_signals_;
  return true;
}


/***********************************************************************************************/
/*************************** MAIN FUNCTIONALITY AND GETTERS ************************************/
/***********************************************************************************************/

void SaTMiniControlBoardv1::spinStateMachine(){
  switch(current_state_){

    case DECIDE_ACTION:
      // disable interrupts when accessing shared service flags
      bool service_feedback_copy;
      bool service_accel_pvdf_copy;
      noInterrupts();
      service_feedback_copy =  service_feedback_;
      service_accel_pvdf_copy = service_accel_pvdf_;
      interrupts();

      if(service_feedback_copy){
        current_state_ = PUBLISH_FEEDBACK;
        break;
      }
      else if(service_accel_pvdf_copy){
        current_state_ = PUBLISH_ACCEL_PVDF;
        break;
      }
      else{
        rclc_executor_spin_some(&executor_, RCL_MS_TO_NS(1));
        break;
      }


    case PUBLISH_FEEDBACK:
      if(spikeatac_cfg_ != NULL) {
        if (spikeatac_cfg_[0].cdc_on) {
          readCapacitiveFeedback();
          RCSOFTCHECK(rcl_publish(&capacitive_fb_publisher_, &capacitive_fb_msg_, NULL));
        }
      }
      noInterrupts();
      service_feedback_ = false;
      interrupts();
      current_state_ = DECIDE_ACTION;
      rclc_executor_spin_some(&executor_, RCL_MS_TO_NS(1));
      break;

    
    case PUBLISH_ACCEL_PVDF:
      if(spikeatac_cfg_ != NULL){
        if (spikeatac_cfg_[0].accel_on) {
          readAccelFeedback();
          RCSOFTCHECK(rcl_publish(&accel_fb_publisher_, &accel_fb_msg_, NULL));
        }
        if (spikeatac_cfg_[0].adc_on) {
          readPvdfFeedback();
          RCSOFTCHECK(rcl_publish(&pvdf_fb_publisher_, &pvdf_fb_msg_, NULL));
        }
      }
      noInterrupts();
      service_accel_pvdf_ = false;
      interrupts();
      current_state_ = DECIDE_ACTION;
      rclc_executor_spin_some(&executor_, RCL_MS_TO_NS(1));
      break;
  }
}


void SaTMiniControlBoardv1::isrFeedback() {
  instance_->setServiceFbFlag();
}

void SaTMiniControlBoardv1::isrAccelPvdf() {
  instance_->setServiceAccelPvdfFlag();
}


void SaTMiniControlBoardv1::setServiceFbFlag(){
  service_feedback_ = true;
}


void SaTMiniControlBoardv1::setServiceAccelPvdfFlag(){
  service_accel_pvdf_ = true;
}



void SaTMiniControlBoardv1::readAccelFeedback() {
  //Get readings from all connected spikeatac accelerometers and load them into msg
  uint16_t stride_count = 0;
  for(uint8_t i=0; i < num_spikeatacs_; i++){
    spikeatac_controller_[i].readAccel(&accel_fb_msg_.data.data[stride_count]);
    stride_count += spikeatac_cfg_[i].num_accel_signals;
  }
}


void SaTMiniControlBoardv1::readCapacitiveFeedback() {
  uint16_t stride_count = 0;
  for (uint8_t i=0; i< num_spikeatacs_; i++) {
    // Get readings from all connected spikeatac capacitive sensors and load them into msg
    spikeatac_controller_[i].readCapacitive(&capacitive_fb_msg_.data.data[stride_count]);
    stride_count += spikeatac_cfg_[i].num_cdc_signals;
  }
}


void SaTMiniControlBoardv1::readPvdfFeedback() {
  uint16_t stride_count = 0;
  for (uint8_t i=0; i< num_spikeatacs_; i++) {
    // Get readings from all connected spikeatac pvdf sensors and load them into msg
    spikeatac_controller_[i].readPvdf(&pvdf_fb_msg_.data.data[stride_count]);
    stride_count += spikeatac_cfg_[i].num_adc_signals;
  }
}



void SaTMiniControlBoardv1::teensyLedOn(){ 
  digitalWriteFast(LED_BUILTIN, HIGH);   // turn the LED on
}


void SaTMiniControlBoardv1::teensyLedOff(){
  digitalWriteFast(LED_BUILTIN, LOW);   // turn the LED off
}
