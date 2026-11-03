// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rh3_interfaces:msg/DxlFeedback.idl
// generated code does not contain a copyright notice

#ifndef RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__STRUCT_H_
#define RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'loads'
// Member 'velocities'
// Member 'positions'
#include "std_msgs/msg/detail/int16_multi_array__struct.h"

/// Struct defined in msg/DxlFeedback in the package rh3_interfaces.
typedef struct rh3_interfaces__msg__DxlFeedback
{
  std_msgs__msg__Int16MultiArray loads;
  std_msgs__msg__Int16MultiArray velocities;
  std_msgs__msg__Int16MultiArray positions;
} rh3_interfaces__msg__DxlFeedback;

// Struct for a sequence of rh3_interfaces__msg__DxlFeedback.
typedef struct rh3_interfaces__msg__DxlFeedback__Sequence
{
  rh3_interfaces__msg__DxlFeedback * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rh3_interfaces__msg__DxlFeedback__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__STRUCT_H_
