// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License

#pragma once

#include <ArduinoJson/Namespace.hpp>

namespace ARDUINOJSON_NAMESPACE {

class Filter {
 public:
  explicit Filter(VariantConstRef v) : _variant(v) {}

  bool allow() const {
    return _variant;
  }

  bool allowArray() const {
    return _variant == true || _variant.is<ArrayConstRef>();
  }

  bool allowObject() const {
    return _variant == true || _variant.is<ObjectConstRef>();
  }

  bool allowValue() const {
    return _variant == true;
  }

  template <typename TKey>
  Filter operator[](const TKey& key) const {
    if (_variant == true)  // "true" means "allow recursively"
      return *this;
    VariantConstRef member = _variant[key];
    return Filter(member.isNull() ? _variant["*"] : member);
  }

 private:
  VariantConstRef _variant;
};

struct AllowAllFilter {
  bool allow() const {
    return true;
  }

  bool allowArray() const {
    return true;
  }

  bool allowObject() const {
    return true;
  }

  bool allowValue() const {
    return true;
  }

  template <typename TKey>
  AllowAllFilter operator[](const TKey&) const {
    return AllowAllFilter();
  }
};

}  // namespace ARDUINOJSON_NAMESPACE
