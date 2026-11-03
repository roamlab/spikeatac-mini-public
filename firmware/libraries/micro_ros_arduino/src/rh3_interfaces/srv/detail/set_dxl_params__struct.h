// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from rh3_interfaces:srv/SetDxlParams.idl
// generated code does not contain a copyright notice

#ifndef RH3_INTERFACES__SRV__DETAIL__SET_DXL_PARAMS__STRUCT_H_
#define RH3_INTERFACES__SRV__DETAIL__SET_DXL_PARAMS__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'parameters'
#include "rh3_interfaces/msg/detail/dxl_params__struct.h"

/// Struct defined in srv/SetDxlParams in the package rh3_interfaces.
typedef struct rh3_interfaces__srv__SetDxlParams_Request
{
  rh3_interfaces__msg__DxlParams parameters;
} rh3_interfaces__srv__SetDxlParams_Request;

// Struct for a sequence of rh3_interfaces__srv__SetDxlParams_Request.
typedef struct rh3_interfaces__srv__SetDxlParams_Request__Sequence
{
  rh3_interfaces__srv__SetDxlParams_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rh3_interfaces__srv__SetDxlParams_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/SetDxlParams in the package rh3_interfaces.
typedef struct rh3_interfaces__srv__SetDxlParams_Response
{
  bool success;
} rh3_interfaces__srv__SetDxlParams_Response;

// Struct for a sequence of rh3_interfaces__srv__SetDxlParams_Response.
typedef struct rh3_interfaces__srv__SetDxlParams_Response__Sequence
{
  rh3_interfaces__srv__SetDxlParams_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} rh3_interfaces__srv__SetDxlParams_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // RH3_INTERFACES__SRV__DETAIL__SET_DXL_PARAMS__STRUCT_H_
