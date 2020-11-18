// Copyright 2020-2020 donglou. All Rights Reserved
// Author:
//    Lei Zhang <donglou_zl@163.com>
#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// This header file is for extracting type info.

namespace tiny_stl
{

// helper struct
template <class T, T v>
struct m_integral_constant
{
  static constexpr T value = v;
}

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;

typedef m_bool_constant<true> m_true_type;
typedef m_bool_constant<false> m_false_type;

/***************************************************/
// type traits

// is pair

// --- forward declaration begin
template <class T1, class T2>
struct pair;
// --- forward declaration end

template <class T>
struct is_pair : tiny_stl::m_false_type {};

template <class T1, class T2>
struct is_pair<tiny_stl::pair<T1, T2>> : tiny_stl::m_true_type {};

} // end of tiny_stl

#endif MYTINYSTL_TYPE_TRAITS_H_