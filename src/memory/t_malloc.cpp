/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "memory/t_malloc.h"
#include <cstdlib>
#include <cstring>

#ifdef PADDLE_MOBILE_FPGA

#include "fpga/api.h"

#endif

namespace paddle_mobile {
namespace memory {
const int MALLOC_ALIGN = 64;

#ifdef PADDLE_MOBILE_FPGA
namespace fpga = paddle_mobile::fpga;

void Copy(void *dst, const void *src, size_t num) {
  std::memcpy(dst, src, num);
}

void *Alloc(size_t size) { return fpga::fpga_malloc(size); }

void Free(void *ptr) {
  if (ptr) {
    fpga::fpga_free(ptr);
  }
}

#else
void Copy(void *dst, const void *src, size_t num) {
  std::memcpy(dst, src, num);
}

void *Alloc(size_t size) {
  size_t offset = sizeof(void *) + MALLOC_ALIGN - 1;
  char *p = static_cast<char *>(malloc(offset + size));
  if (!p) {
    return nullptr;
  }
  void *r = reinterpret_cast<void *>(reinterpret_cast<size_t>(p + offset) &
                                     (~(MALLOC_ALIGN - 1)));
  static_cast<void **>(r)[-1] = p;
  return r;
}

void Free(void *ptr) {
  if (ptr) {
    free(static_cast<void **>(ptr)[-1]);
  }
}

#endif

}  // namespace memory
}  // namespace paddle_mobile
