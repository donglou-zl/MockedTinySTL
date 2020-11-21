// Copyright 2020-2020 donglou. All Rights Reserved
// Author:
//    Lei Zhang <donglou_zl@163.com>
#ifndef MYTINYSTL_ALLOC_H_
#define MYTINYSTL_ALLOC_H_

#include <new>
#include <cstddef>
#include <cstdio>

// This header is responsible for allocate and
// deallocate memory, using memory pool.
//
// This file will be deprecated from V2.0.0

namespace tiny_stl
{

// Use linked list to manage memory fragmentation, to
// allocate and deallocate small mem block(no larger 
// than 4KB)
union FreeList
{
  union FreeList *next; // point to next addr
  char data[1];         // store the first addr of curr memory
};

// The range of diff memory size
enum
{
  EAlign128 = 8,
  EAlign256 = 16,
  EAlign512 = 32,
  EAlign1024 = 64,
  EAlign2048 = 128,
  EAlign4096 = 256
};

// memory size of small obj
enum
{
  ESmallObjectBytes = 4096
};

// number of free list
enum
{
  EFreeListNumber = 56
};

class alloc
{
private:
  static char* start_free;                     // start addr of mem pool
  static char* end_free;                       // end addr of mem pool
  static size_t heap_size;                     // 

  static FreeList* free_list[EFreeListNumber]; //

public:
  static void* allocate(size_t n);
  static void* deallocate(void* p, size_t n);
  static void* reallocate(void* p, size_t old_size, size_t new_size);

private:
  static size_t M_align(size_t bytes);
  static size_t M_round_up(size_t bytes);
  static size_t M_freelist_index(size_t bytes);
  static void* M_refill(size_t n);
  static void* M_chunk_alloc(size_t size, size_t &nobj);
};

// init static member
char* alloc::start_free = nullptr;
char* alloc::end_free = nullptr;
size_t alloc::heap_size = 0;

FreeList* alloc::free_list[EFreeListNumber] = {
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
  nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
  };

inline void* alloc::allocate(size_t n)
{

}

} // end of tiny_stl

#endif MYTINYSTL_ALLOC_H_