// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

#include <ArduinoJson/Polyfills/assert.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

namespace ARDUINOJSON_NAMESPACE {

template <typename TAdaptedString>
inline bool slotSetKey(VariantSlot* var, TAdaptedString key, MemoryPool* pool) {
  if (!var)
    return false;
  return slotSetKey(var, key, pool, typename TAdaptedString::storage_policy());
}

template <typename TAdaptedString>
inline bool slotSetKey(VariantSlot* var, TAdaptedString key, MemoryPool* pool,
                       storage_policies::decide_at_runtime) {
  if (key.isStatic()) {
    return slotSetKey(var, key, pool, storage_policies::store_by_address());
  } else {
    return slotSetKey(var, key, pool, storage_policies::store_by_copy());
  }
}

template <typename TAdaptedString>
inline bool slotSetKey(VariantSlot* var, TAdaptedString key, MemoryPool*,
                       storage_policies::store_by_address) {
  ARDUINOJSON_ASSERT(var);
  var->setKey(key.data(), storage_policies::store_by_address());
  return true;
}

template <typename TAdaptedString>
inline bool slotSetKey(VariantSlot* var, TAdaptedString key, MemoryPool* pool,
                       storage_policies::store_by_copy) {
  const char* dup = pool->saveString(key);
  if (!dup)
    return false;
  ARDUINOJSON_ASSERT(var);
  var->setKey(dup, storage_policies::store_by_copy());
  return true;
}

inline size_t slotSize(const VariantSlot* var) {
  size_t n = 0;
  while (var) {
    n++;
    var = var->next();
  }
  return n;
}

inline VariantData* slotData(VariantSlot* slot) {
  return reinterpret_cast<VariantData*>(slot);
}
}  // namespace ARDUINOJSON_NAMESPACE
