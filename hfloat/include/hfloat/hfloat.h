#pragma once
#ifndef HFLOAT_H
#define HFLOAT_H

#include <stdint.h>
typedef uint16_t hfloat;

#ifdef __cplusplus
extern "C" {
#endif

hfloat convert_float_to_hfloat(float f_);
float convert_hfloat_to_float(hfloat f_);

#ifdef __cplusplus
}
#endif

#endif