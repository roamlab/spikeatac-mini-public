// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rh3_interfaces:msg/DxlParams.idl
// generated code does not contain a copyright notice

#ifndef RH3_INTERFACES__MSG__DETAIL__DXL_PARAMS__STRUCT_H_
#define RH3_INTERFACES__MSG__DETAIL__DXL_PARAMS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'chain_label'
#include "rosidl_runtime_c/string.h"

/// Struct defined in msg/DxlParams in the package rh3_interfaces.
typedef struct rh3_interfaces__msg__DxlParams
{
  bool apply_to_id;
  bool apply_to_chain;
  bool apply_all;
  uint8_t id;
  rosidl_runtime_c__String chain_label;
  int16_t min_pos;
  int16_t max_pos;
  int16_t profile_v;
  int16_t profile_a;
  int16_t p_gain;
  int16_t i_gain;
  int16_t d_gain;
} rh3_interfaces__msg__DxlParams;

// Struct for a sequence of rh3_interfaces__msg__DxlParams.
typedef struct rh3_interfaces__msg__DxlParams__Sequence
{
  rh3_interfaces__msg__DxlParams * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rh3_interfaces__msg__DxlParams__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // RH3_INTERFACES__MSG__DETAIL__DXL_PARAMS__STRUCT_H_
