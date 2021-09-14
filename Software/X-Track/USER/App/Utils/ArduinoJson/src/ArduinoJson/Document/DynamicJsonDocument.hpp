// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

#include <ArduinoJson/Document/BasicJsonDocument.hpp>

//#include <stdlib.h>  // malloc, free
#include "lvgl/lvgl.h"

namespace ARDUINOJSON_NAMESPACE {

struct DefaultAllocator {
  void* allocate(size_t size) {
    return lv_mem_alloc(size);
  }

  void deallocate(void* ptr) {
      lv_mem_free(ptr);
  }

  void* reallocate(void* ptr, size_t new_size) {
    return lv_mem_realloc(ptr, new_size);
  }
};

typedef BasicJsonDocument<DefaultAllocator> DynamicJsonDocument;

}  // namespace ARDUINOJSON_NAMESPACE
