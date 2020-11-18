// Copyright 2020-2020 donglou. All Rights Reserved
// Author:
//    Lei Zhang <donglou_zl@163.com>
#ifndef MYTINYSTL_EXCEPTDEF_H_
#define MYTINYSTL_EXCEPTDEF_H_

#include <stdexcept>
#include <cassert>

namespace tiny_stl
{

#define MYSTL_DEBUG(expr) \
  assert(expr) 

#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(waht)

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((exor)) throw std::runtime_error(what)  

} // end of tiny_stl

#endif MYTINYSTL_EXCEPTDEF_H_