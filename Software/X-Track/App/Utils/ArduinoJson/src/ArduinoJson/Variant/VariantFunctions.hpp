// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

#include <ArduinoJson/Polyfills/attributes.hpp>
#include <ArduinoJson/Variant/VariantData.hpp>

namespace ARDUINOJSON_NAMESPACE {

template <typename TVisitor>
inline typename TVisitor::result_type variantAccept(const VariantData *var,
                                                    TVisitor &visitor) {
  if (var != 0)
    return var->accept(visitor);
  else
    return visitor.visitNull();
}

inline const CollectionData *variantAsArray(const VariantData *var) {
  return var != 0 ? var->asArray() : 0;
}

inline const CollectionData *variantAsObject(const VariantData *var) {
  return var != 0 ? var->asObject() : 0;
}

inline CollectionData *variantAsObject(VariantData *var) {
  return var != 0 ? var->asObject() : 0;
}

inline bool variantCopyFrom(VariantData *dst, const VariantData *src,
                            MemoryPool *pool) {
  if (!dst)
    return false;
  if (!src) {
    dst->setNull();
    return true;
  }
  return dst->copyFrom(*src, pool);
}

inline int variantCompare(const VariantData *a, const VariantData *b);

inline void variantSetNull(VariantData *var) {
  if (!var)
    return;
  var->setNull();
}

template <typename TAdaptedString>
inline bool variantSetString(VariantData *var, TAdaptedString value,
                             MemoryPool *pool) {
  if (!var)
    return false;
  return var->setString(value, pool);
}

inline size_t variantSize(const VariantData *var) {
  return var != 0 ? var->size() : 0;
}

inline CollectionData *variantToArray(VariantData *var) {
  if (!var)
    return 0;
  return &var->toArray();
}

inline CollectionData *variantToObject(VariantData *var) {
  if (!var)
    return 0;
  return &var->toObject();
}

inline NO_INLINE VariantData *variantAddElement(VariantData *var,
                                                MemoryPool *pool) {
  return var != 0 ? var->addElement(pool) : 0;
}

inline NO_INLINE VariantData *variantGetOrAddElement(VariantData *var,
                                                     size_t index,
                                                     MemoryPool *pool) {
  return var != 0 ? var->getOrAddElement(index, pool) : 0;
}

template <typename TChar>
NO_INLINE VariantData *variantGetOrAddMember(VariantData *var, TChar *key,
                                             MemoryPool *pool) {
  return var != 0 ? var->getOrAddMember(adaptString(key), pool) : 0;
}

template <typename TString>
NO_INLINE VariantData *variantGetOrAddMember(VariantData *var,
                                             const TString &key,
                                             MemoryPool *pool) {
  return var != 0 ? var->getOrAddMember(adaptString(key), pool) : 0;
}

inline bool variantIsNull(const VariantData *var) {
  return var == 0 || var->isNull();
}

}  // namespace ARDUINOJSON_NAMESPACE
