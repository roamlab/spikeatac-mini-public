// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from rh3_interfaces:msg/DxlFeedback.idl
// generated code does not contain a copyright notice

#ifndef RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__FUNCTIONS_H_
#define RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/visibility_control.h"
#include "rh3_interfaces/msg/rosidl_generator_c__visibility_control.h"

#include "rh3_interfaces/msg/detail/dxl_feedback__struct.h"

/// Initialize msg/DxlFeedback message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * rh3_interfaces__msg__DxlFeedback
 * )) before or use
 * rh3_interfaces__msg__DxlFeedback__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__init(rh3_interfaces__msg__DxlFeedback * msg);

/// Finalize msg/DxlFeedback message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
void
rh3_interfaces__msg__DxlFeedback__fini(rh3_interfaces__msg__DxlFeedback * msg);

/// Create msg/DxlFeedback message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * rh3_interfaces__msg__DxlFeedback__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
rh3_interfaces__msg__DxlFeedback *
rh3_interfaces__msg__DxlFeedback__create();

/// Destroy msg/DxlFeedback message.
/**
 * It calls
 * rh3_interfaces__msg__DxlFeedback__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
void
rh3_interfaces__msg__DxlFeedback__destroy(rh3_interfaces__msg__DxlFeedback * msg);

/// Check for msg/DxlFeedback message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__are_equal(const rh3_interfaces__msg__DxlFeedback * lhs, const rh3_interfaces__msg__DxlFeedback * rhs);

/// Copy a msg/DxlFeedback message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__copy(
  const rh3_interfaces__msg__DxlFeedback * input,
  rh3_interfaces__msg__DxlFeedback * output);

/// Initialize array of msg/DxlFeedback messages.
/**
 * It allocates the memory for the number of elements and calls
 * rh3_interfaces__msg__DxlFeedback__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__Sequence__init(rh3_interfaces__msg__DxlFeedback__Sequence * array, size_t size);

/// Finalize array of msg/DxlFeedback messages.
/**
 * It calls
 * rh3_interfaces__msg__DxlFeedback__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
void
rh3_interfaces__msg__DxlFeedback__Sequence__fini(rh3_interfaces__msg__DxlFeedback__Sequence * array);

/// Create array of msg/DxlFeedback messages.
/**
 * It allocates the memory for the array and calls
 * rh3_interfaces__msg__DxlFeedback__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
rh3_interfaces__msg__DxlFeedback__Sequence *
rh3_interfaces__msg__DxlFeedback__Sequence__create(size_t size);

/// Destroy array of msg/DxlFeedback messages.
/**
 * It calls
 * rh3_interfaces__msg__DxlFeedback__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
void
rh3_interfaces__msg__DxlFeedback__Sequence__destroy(rh3_interfaces__msg__DxlFeedback__Sequence * array);

/// Check for msg/DxlFeedback message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__Sequence__are_equal(const rh3_interfaces__msg__DxlFeedback__Sequence * lhs, const rh3_interfaces__msg__DxlFeedback__Sequence * rhs);

/// Copy an array of msg/DxlFeedback messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_rh3_interfaces
bool
rh3_interfaces__msg__DxlFeedback__Sequence__copy(
  const rh3_interfaces__msg__DxlFeedback__Sequence * input,
  rh3_interfaces__msg__DxlFeedback__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // RH3_INTERFACES__MSG__DETAIL__DXL_FEEDBACK__FUNCTIONS_H_
