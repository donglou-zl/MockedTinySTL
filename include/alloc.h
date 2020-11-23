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
  static size_t heap_size;                     // extra allocated heap size

  static FreeList* free_list[EFreeListNumber]; // diff segment size list

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

// alloc memory, size = n(byte)
inline void* alloc::allocate(size_t n)
{
  FreeList* my_free_list;
  FreeList* result;

  if (n > static_cast<size_t>(ESmallObjectBytes)) {
    return std::malloc(n);
  }
  my_free_list = free_list[M_freelist_index(n)];
  result = my_free_list;
  if (nullptr == result) {
    void* r = M_refill(M_round_up(n));
    return r;
  }
  my_free_list = result->next;
  return result;
}

inline void alloc::deallocate(void* p, size_t n)
{
  if (n > static_cast<size_t>(ESmallObjectBytes)) {
    std::free(p);
    return;
  }
  FreeList* q = reinterpret_cast<FreeList*>(p);
  FreeList* my_free_list = nullptr;
  my_free_list = free_list[M_freelist_index(n)];
  q->next = my_free_list;
  my_free_list = q;
}

inline size_t alloc::M_align(size_t bytes)
{
  if (bytes <= 512) {
    return bytes <= 256 ? 
             (bytes <= 128 ? EAlign128 : EAlign256) :
             EAlign512;
  }
  return bytes <= 2048 ?
           (bytes <= 1024 ? EAlign1024 : EAlign2048) :
           EAlign4096;
}

inline size_t alloc::M_round_up(size_t bytes)
{
  return ((bytes + M_align(bytes) - 1) & (~(M_align(bytes) - 1)));
}

inline size_t alloc::M_freelist_index(size_t bytes)
{
  if (bytes <= 512) {
    return bytes <= 256 ? 
             (bytes <= 128 ? ((bytes + EAlign128 - 1) / EAlign128 - 1)
             : (15 + (bytes + EAlign256 - 129) / EAlign256))
           : (23 + (bytes + EAlign512 - 257) / EAlign512);
  }
  return bytes <= 2048 ?
           (bytes <= 1024 ? (31 + (bytes + EAlign1024 - 513) / EAlign1024)
           : (39 + (bytes + EAlign2048 - 1025) / EAlign2048)
         : (47 + (bytes + EAlign4096 - 2049) / EAlign4096);
}

void* alloc::M_refill(size_t n)
{
  size_t nblock = 10;
  char* c = M_chunk_alloc(n, nblock);
  FreeList* my_free_list = nullptr;
  FreeList* result, *cur, *next;

  if (1 == nblock) {
    return c;
  }

  my_free_list = free_list[M_freelist_index[n]];
  result = (FreeList*)c;

  my_free_list = next = (FreeList*)(c + n);
  for (size_t i = 1; ; ++i) {
    cur = next;
    next = (FreeList*)((char*)next + n);
    if (nblock - 1 == i) {
      cur->next = nullptr;
      break;
    } else {
      cur->next = next;
    }
  }
  return result;
}

char* alloc::M_chunk_alloc(size_t size, size_t &nblock)
{
  char* result = nullptr;
  size_t need_bytes = size * nblock;
  size_t pool_bytes = end_free - start_free;

  // 如果内存池剩余空间满足需求量，则返回它
  if (pool_bytes >= need_bytes) {
    result = start_free;
    start_free += need_bytes;
    return result;
  }

  // 如果内存池剩余空间不能完全满足需求量，但至少可分配一个或以上的区块，则返回它
  else if (pool_bytes >= size) {
    nblock = pool_bytes / size;
    need_bytes = size * nblock;
    result = start_free;
    start_free += need_bytes;
    return result;
  }

  else {
    if (pool_bytes > 0) {
      // 如果内存池有剩余空间，则加入到freelist中
      FreeList* my_free_list = free_list[M_freelist_index(pool_bytes)];
      ((FreeList*)start_free)->next = my_free_list;
      my_free_list = (FreeList*)start_free;
    }

    // 申请heap空间
    size_t bytes_to_get = (need_bytes << 1) + M_round_up(heap_size >> 4);
    start_free = (char*)std::malloc(bytes_to_get);

    // heap空间无法满足所需
    if (!start_free) {
      FreeList* my_free_list, *p;
      for (size_t i = size; i <= ESmallObjectBytes; i += M_align(i)) {
        my_free_list = free_list[M_freelist_index[i]];
        p = my_free_list;
        if (p) {
          my_free_list = p->next;
          start_free = (char*)p;
          end_free = start_free + i;
          return M_chunk_alloc(size, nblock);
        }
      }
      std::fprintf("out of memory");
      end_free = nullptr;
      throw std::bad_alloc();
    }
    end_free = start_free + bytes_to_get;
    heap_size += bytes_to_get;
    return M_chunk_alloc(size, nblock);
  }

}

} // end of tiny_stl

#endif MYTINYSTL_ALLOC_H_