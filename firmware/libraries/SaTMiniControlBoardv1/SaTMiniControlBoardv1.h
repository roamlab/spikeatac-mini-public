/********************************************************************************************************/
// SaTMiniControlBoardv1 - Controller library for SpikeATac Mini Control Board v1 (physical board v1)
//
// This library handles all micro-ROS communication including:
//   - Node initialization and executor management
//   - Publishers for sensor feedback (capacitive, accel, pvdf)
//
// The actual sensor reading is delegated to SpikeatacMiniController, which is ROS-agnostic.
// This separation allows SpikeatacMiniController to be used independently for testing
// without requiring a ROS agent connection.
/********************************************************************************************************/
#ifndef SaTMiniControlBoardv1_h
#define SaTMiniControlBoardv1_h

#include <micro_ros_arduino.h>
#include <SPI.h>
#include <stdio.h>
#include <unistd.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/u_int16_multi_array.h>
#include <std_msgs/msg/int32_multi_array.h>
#include <std_msgs/msg/multi_array_dimension.h>
#include <micro_ros_utilities/type_utilities.h>
#include <SpikeatacMiniController.h>


/***********************************************************************************************/
/******************************** AUX STRUCTS, DEFINITIONS **************************************/
/***********************************************************************************************/

// Macros to handle micro-ROS return codes
// void error_loop(uint16_t freq = 10);
// #define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop(5);}}
// #define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}



/**
  Enum with all possible states in the controller internal state machine.
*/
enum State{DECIDE_ACTION, PUBLISH_FEEDBACK, PUBLISH_ACCEL_PVDF};



/***********************************************************************************************/
/********************************  SaTMiniControlBoardv1 CLASS  ********************************/
/***********************************************************************************************/

/**
  The SaTMiniControlBoardv1 class is meant to handle the SpikeATac Mini sensor sampling process
  and all ROS communication. Debugging happens using ROS logging.
*/
class SaTMiniControlBoardv1
{
private:
  static SaTMiniControlBoardv1* instance_; //Singleton instance

  //Controller related
  State current_state_;
  uint8_t num_spikeatacs_;
  SpikeatacMiniController* spikeatac_controller_ = NULL;
  uint8_t total_num_capacitive_signals_;
  uint8_t total_num_accel_signals_;
  uint8_t total_num_pvdf_signals_;

  //Service Flags
  volatile bool service_feedback_ = false;
  volatile bool service_accel_pvdf_ = false;

  //Array of configs
  SpikeatacMiniConfig* spikeatac_cfg_ = NULL;

  //Feedback timer related
  IntervalTimer fb_ticker_;
  uint32_t fb_tick_us_;
  IntervalTimer spikeatac_accel_pvdf_ticker_;
  uint32_t spikeatac_accel_pvdf_tick_us_;

  //Micro-ROS related
  rcl_allocator_t allocator_;
  rclc_support_t support_;
  rcl_node_t node_;
  const char * node_name_ = "satmini_control_node";
  rclc_executor_t executor_;
  // Publishers
  rcl_publisher_t capacitive_fb_publisher_;
  std_msgs__msg__UInt16MultiArray capacitive_fb_msg_;
  rcl_publisher_t accel_fb_publisher_;
  std_msgs__msg__Int32MultiArray accel_fb_msg_;
  rcl_publisher_t pvdf_fb_publisher_;
  std_msgs__msg__Int32MultiArray pvdf_fb_msg_;

public:

  /***********************************************************************************************/
  /*************************** CONSTRUCTOR, DESTRUCTOR AND INIT **********************************/
  /***********************************************************************************************/

  /**
    SaTMiniControlBoardv1 constructor
    @param fb_tick_us determines the period in microseconds to trigger a feedback reading
    @param spikeatac_accel_pvdf_tick_us_
    @param num_spikeatacs sets the number of spikeatacs (length of the spikeatac_config array)
    @param spikeatac_config pointer to array of SpikeatacMiniConfig structs with configuration of each spikeatac
  */
  SaTMiniControlBoardv1(uint32_t fb_tick_us, uint32_t spikeatac_accel_pvdf_tick_us,
                uint8_t num_spikeatacs, SpikeatacMiniConfig* spikeatac_config);

  ~SaTMiniControlBoardv1();

  /**
    Defines hardware connections for the Teensy pins, starts the spikeatac sensors
    and sets up all ROS communication

    @return boolean to indicate if initialization was a success or not.
  */
  bool initialize();


  /**
    Dynamically allocates memory for the Int32MultiArray messages related to
    the data from the spikeatac accelerometers in a flattened manner:
    data = [s0aX, s0aY, s0aZ, ..., s3aX, s3aY, s3aZ]
    where s0 is sensor '0' and aX is the acceleration in the X axis on that sensor
    We use Int32 because the accel data can be +/- and is in mm/s^2, so int32 gives us ~6g range

    @return boolean to indicate if initialization was a success or not.
  */
  bool initAccelMultiArrayMsg(std_msgs__msg__Int32MultiArray * msg);


  /**
    Dynamically allocates memory for the Int32MultiArray messages related to
    the data from the spikeatac capacitive sensors in a flattened manner:
    data = [s0c0, s0c1, ..., s3c5, s3c6]
    where s0 is sensor '0' and c0 is the capacitive signal '0' on that sensor
    We use Int32 because the capacitive data is in raw counts which can be above int16 range

    @return boolean to indicate if initialization was a success or not.
  */
  bool initCapacitiveMultiArrayMsg(std_msgs__msg__UInt16MultiArray * msg);


  /**
    Dynamically allocates memory for the Int16MultiArray messages related to
    the data from the spikeatac pvdf sensors in a flattened manner:
    data = [s0p0, s0p1, ..., s3p5, s3p6]
    where s0 is sensor '0' and p0 is the pvdf signal '0' on that sensor

    @return boolean to indicate if initialization was a success or not.
  */
  bool initPvdfMultiArrayMsg(std_msgs__msg__Int32MultiArray * msg);


  /***********************************************************************************************/
  /*********************** MAIN FUNCTIONALITY, SETTERS AND GETTERS *******************************/
  /***********************************************************************************************/

  /**
    Entry into the state machine that publishes the capacitive feedback and the
    accel/pvdf feedback at their requested rates.
    This method should be called inside an endless loop after the controller has been initialized.
  */
  void spinStateMachine();


  /**
    Interrupt wrapper static function definition which will call the class member setServiceFlag function
    See https://isocpp.org/wiki/faq/pointers-to-members#memfnptr-vs-fnptr
  */
  static void isrFeedback();


  /**
    Interrupt wrapper static function definition which will call the class member setServiceFlag function
    See https://isocpp.org/wiki/faq/pointers-to-members#memfnptr-vs-fnptr
  */
  static void isrAccelPvdf();


  /**
    Sets a flag to indicate that we need to publish the capacitive feedback.
    This function is called inside the interrupt service routine when
    the feedback timer expires.
  */
  void setServiceFbFlag();

  /**
    Sets a flag to indicate that we need to publish the accelerometer feedback.
    This function is called inside the interrupt service routine when
    the accelerometer timer expires.
  */
  void setServiceAccelPvdfFlag();


  /**
  * Reads data from all spikeatac accelerometers connected and stores it
  * in the accel_fb_msg_ member variable.
  */
  void readAccelFeedback();


  /**
  * Reads data from all spikeatac capacitive sensors connected and stores it
  * in the capacitive_fb_msg_ member variable.
  */
  void readCapacitiveFeedback();


  /**
  * Reads data from all spikeatac PVDF sensors connected and stores it
  * in the pvdf_fb_msg_ member variable.
  */
 void readPvdfFeedback();


  /**
    Turns the Teensy integrated LED on for debugging purposes.
  */
  void teensyLedOn();


  /**
    Turns the Teensy integrated LED on for debugging purposes.
  */
  void teensyLedOff();


};


#endif  // SaTMiniControlBoardv1_h
