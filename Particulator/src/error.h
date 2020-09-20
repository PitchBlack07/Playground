#pragma once
#include <stdint.h>

enum class ErrorCode : uint32_t {
	SUCCESS,
	INVALID_ARGUMENT,
	REGISTER_CLASS_FAILED,
	CREATE_WINDOW_FAILED,
};
