/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#ifndef oatpp_data_mapping_type_Enum_hpp
#define oatpp_data_mapping_type_Enum_hpp

#include "./Any.hpp"
#include "./Primitive.hpp"
#include "oatpp/core/data/share/MemoryLabel.hpp"

#include <type_traits>
#include <unordered_map>
#include <vector>

namespace oatpp { namespace data { namespace mapping { namespace type {

/**
 * Errors of enum interpretation.
 */
enum class EnumInterpreterError : v_int32 {

  /**
   * The interpretation was successful.
   */
  OK = 0,

  /**
   * Wrong `Interpreter` is used to interpret the variable. <br>
   * This may also occur if for example: <br>
   * `oatpp::Enum<T>` is passed to interpreter of `oatpp::Enum<T>::NotNull`.
   */
  TYPE_MISMATCH_ENUM = 1,

  /**
   * Wrong &id:oatpp::data::mapping::type::Primitive; is passed to interpreter.
   */
  TYPE_MISMATCH_ENUM_VALUE = 2,

  /**
   * Interpreter constraint is violated. <br>
   * The constraint was set to `NotNull` but interpretation to/from `nullptr` is requested.
   */
  CONSTRAINT_NOT_NULL = 3,

  /**
   * Enum entry not found.
   */
  ENTRY_NOT_FOUND = 4,

};

namespace __class {

  class AbstractEnum {
  public:
    static const ClassId CLASS_ID;
  public:

    class AbstractPolymorphicDispatcher {
    public:

      AbstractPolymorphicDispatcher(bool pNotNull)
        : notNull(pNotNull)
      {}

      const bool notNull;

      virtual type::Void toInterpretation(const type::Void& enumValue, EnumInterpreterError& error) const = 0;
      virtual type::Void fromInterpretation(const type::Void& interValue, EnumInterpreterError& error) const = 0;
      virtual type::Type* getInterpretationType() const = 0;
      virtual std::vector<type::Any> getInterpretedEnum() const = 0;

    };

  };

  template<class T, class Interpreter>
  class Enum;

}

template<typename T>
struct EnumValueInfo {
  const T value;
  const v_int32 index;
  const data::share::StringKeyLabel name;
  const data::share::StringKeyLabel description;
};

template<typename T>
struct EnumInfo {
public:
  const char* nameQualifier = nullptr;
  std::unordered_map<data::share::StringKeyLabel, EnumValueInfo<T>> byName;
  std::unordered_map<v_uint64, EnumValueInfo<T>> byValue;
  std::vector<EnumValueInfo<T>> byIndex;
};

template<class T, class Interpreter>
class EnumObjectWrapper; // FWD

template<class T>
class EnumMeta {

  template<class Type, class Interpreter>
  friend class __class::Enum;

  template<class Type, class Interpreter>
  friend class EnumObjectWrapper;

public:
  typedef T EnumType;
protected:
  static EnumInfo<T>* getInfo() {
    static EnumInfo<T> info;
    return &info;
  }
};

template<class T, bool notnull>
class EnumInterpreterAsString {
public:
  typedef String UnderlyingTypeObjectWrapper;
public:
  template <bool N>
  using InterpreterType = EnumInterpreterAsString<T, N>;
public:
  constexpr static bool notNull = notnull;
public:
  static Void toInterpretation(const Void& enumValue, EnumInterpreterError& error);
  static Void fromInterpretation(const Void& interValue, EnumInterpreterError& error);
  static Type* getInterpretationType();
};

template<class T, bool notnull>
class EnumInterpreterAsInteger {
private:
  typedef typename std::underlying_type<T>::type EnumUnderlyingType;
public:
  typedef typename ObjectWrapperByUnderlyingType<EnumUnderlyingType>::ObjectWrapper UnderlyingTypeObjectWrapper;
public:
  template <bool N>
  using InterpreterType = EnumInterpreterAsInteger<T, N>;
public:
  constexpr static bool notNull = notnull;
public:
  static Void toInterpretation(const Void& enumValue, EnumInterpreterError& error);
  static Void fromInterpretation(const Void& interValue, EnumInterpreterError& error);
  static Type* getInterpretationType();
};

template<class T, class EnumInterpreter>
class EnumObjectWrapper : public ObjectWrapper<T, __class::Enum<T, EnumInterpreter>>{
  template<class Type, class Interpreter>
  friend class EnumObjectWrapper;
public:
  typedef EnumObjectWrapper __Wrapper;
public:
  typedef typename std::underlying_type<T>::type UnderlyingEnumType;
  typedef T Z__EnumType;
  typedef __class::Enum<T, EnumInterpreter> EnumObjectClass;
  typedef EnumInterpreter Interpreter;
public:
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, EnumInterpreter::notNull>> AsString;
  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, EnumInterpreter::notNull>> AsNumber;
  typedef EnumObjectWrapper<T, typename EnumInterpreter::template InterpreterType<true>> NotNull;
public:

  EnumObjectWrapper(const std::shared_ptr<T>& ptr, const type::Type* const valueType)
    : type::ObjectWrapper<T, EnumObjectClass>(ptr, valueType)
  {}

  EnumObjectWrapper() {}

  EnumObjectWrapper(std::nullptr_t) {}

  EnumObjectWrapper(const std::shared_ptr<T>& ptr)
    : type::ObjectWrapper<T, EnumObjectClass>(ptr)
  {}

  EnumObjectWrapper(std::shared_ptr<T>&& ptr)
    : type::ObjectWrapper<T, EnumObjectClass>(std::forward<std::shared_ptr<T>>(ptr))
  {}

  template<class OtherInter>
  EnumObjectWrapper(const EnumObjectWrapper<T, OtherInter>& other)
    : type::ObjectWrapper<T, EnumObjectClass>(other.getPtr())
  {}

  template<class OtherInter>
  EnumObjectWrapper(EnumObjectWrapper<T, OtherInter>&& other)
    : type::ObjectWrapper<T, EnumObjectClass>(std::move(other.getPtr()))
  {}

  inline EnumObjectWrapper& operator = (std::nullptr_t) {
    this->m_ptr.reset();
    return *this;
  }

  template<class OtherInter>
  inline EnumObjectWrapper& operator = (const EnumObjectWrapper<T, OtherInter>& other) {
    this->m_ptr = other.m_ptr;
    return *this;
  }

  template<class OtherInter>
  inline EnumObjectWrapper& operator = (EnumObjectWrapper<T, OtherInter>&& other) {
    this->m_ptr = std::forward<std::shared_ptr<T>>(other.m_ptr);
    return *this;
  }

public:

  EnumObjectWrapper(T value)
    : type::ObjectWrapper<T, EnumObjectClass>(std::make_shared<T>(value))
  {}

  EnumObjectWrapper& operator = (T value) {
    this->m_ptr = std::make_shared<T>(value);
    return *this;
  }

  T operator*() const {
    return this->m_ptr.operator*();
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (TP){
    return this->m_ptr.get() == nullptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (TP){
    return this->m_ptr.get() != nullptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline bool operator == (TP value) const {
    if(!this->m_ptr) return false;
    return *this->m_ptr == value;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline bool operator != (TP value) const {
    if(!this->m_ptr) return true;
    return *this->m_ptr != value;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<typename TP::Z__EnumType, Z__EnumType>::value, void>::type
  >
  inline bool operator == (const TP &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<typename TP::Z__EnumType, Z__EnumType>::value, void>::type
  >
  inline bool operator != (const TP &other) const {
    return !operator == (other);
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline operator TP() const {
    return *this->m_ptr;
  }

public:

  static const EnumValueInfo<T>& getEntryByName(const String& name) {
    auto it = EnumMeta<T>::getInfo()->byName.find(name);
    if(it != EnumMeta<T>::getInfo()->byName.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByName()]: Error. Entry not found.");
  }

  static const EnumValueInfo<T>& getEntryByValue(T value) {
    auto it = EnumMeta<T>::getInfo()->byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::getInfo()->byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByValue()]: Error. Entry not found.");
  }

  static const EnumValueInfo<T>& getEntryByUnderlyingValue(UnderlyingEnumType value) {
    auto it = EnumMeta<T>::getInfo()->byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::getInfo()->byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByUnderlyingValue()]: Error. Entry not found.");
  }

  static const EnumValueInfo<T>& getEntryByIndex(v_int32 index) {
    if(index >= 0 && index < EnumMeta<T>::getInfo()->byIndex.size()) {
      return EnumMeta<T>::getInfo()->byIndex[index];
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByIndex()]: Error. Entry not found.");
  }

  static const std::vector<EnumValueInfo<T>>& getEntries() {
    return EnumMeta<T>::getInfo()->byIndex;
  }

};

template <class T>
using Enum = EnumObjectWrapper<T, EnumInterpreterAsString<T, false>>;

template<class T, bool notnull>
Void EnumInterpreterAsString<T, notnull>::toInterpretation(const Void& enumValue, EnumInterpreterError& error) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, notnull>> EnumOW;

  if(enumValue.valueType != EnumOW::Class::getType()) {
    error = EnumInterpreterError::TYPE_MISMATCH_ENUM;
    return Void(nullptr, String::Class::getType());
  }

  if(!enumValue) {
    if(notnull) {
      error = EnumInterpreterError::CONSTRAINT_NOT_NULL;
      return Void(nullptr, String::Class::getType());
    }
    return Void(nullptr, String::Class::getType());
  }

  const auto& ow = enumValue.staticCast<EnumOW>();
  const auto& entry = EnumOW::getEntryByValue(*ow);
  return entry.name.toString();
}

template<class T, bool notnull>
Void EnumInterpreterAsString<T, notnull>::fromInterpretation(const Void& interValue, EnumInterpreterError& error) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, notnull>> EnumOW;

  if(interValue.valueType != String::Class::getType()) {
    error = EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE;
    return Void(nullptr, EnumOW::Class::getType());
  }

  if(!interValue) {
    if(notnull) {
      error = EnumInterpreterError::CONSTRAINT_NOT_NULL;
      return Void(nullptr, EnumOW::Class::getType());
    }
    return Void(nullptr, EnumOW::Class::getType());
  }

  try {
    const auto &entry = EnumOW::getEntryByName(interValue.staticCast<String>());
    return EnumOW(entry.value);
  } catch (const std::runtime_error& e) { // TODO - add a specific error for this.
    error = EnumInterpreterError::ENTRY_NOT_FOUND;
  }
  return Void(nullptr, EnumOW::Class::getType());
}

template<class T, bool notnull>
Type* EnumInterpreterAsString<T, notnull>::getInterpretationType() {
  return String::Class::getType();
}

template<class T, bool notnull>
Void EnumInterpreterAsInteger<T, notnull>::toInterpretation(const Void& enumValue, EnumInterpreterError& error) {

  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, notnull>> EnumOW;
  typedef typename std::underlying_type<T>::type EnumUT;
  typedef typename ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper UTOW;

  if(enumValue.valueType != EnumOW::Class::getType()) {
    error = EnumInterpreterError::TYPE_MISMATCH_ENUM;
    return Void(nullptr, UTOW::Class::getType());
  }

  if(!enumValue) {
    if(notnull) {
      error = EnumInterpreterError::CONSTRAINT_NOT_NULL;
      return Void(nullptr, UTOW::Class::getType());
    }
    return Void(nullptr, UTOW::Class::getType());
  }

  const auto& ow = enumValue.staticCast<EnumOW>();
  return UTOW(static_cast<EnumUT>(*ow));

}

template<class T, bool notnull>
Void EnumInterpreterAsInteger<T, notnull>::fromInterpretation(const Void& interValue, EnumInterpreterError& error) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, notnull>> EnumOW;

  typedef typename std::underlying_type<T>::type EnumUT;
  typedef typename ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper OW;

  if(interValue.valueType != OW::Class::getType()) {
    error = EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE;
    return Void(nullptr, EnumOW::Class::getType());
  }

  if(!interValue) {
    if(notnull) {
      error = EnumInterpreterError::CONSTRAINT_NOT_NULL;
      return Void(nullptr, EnumOW::Class::getType());
    }
    return Void(nullptr, EnumOW::Class::getType());
  }

  try{
    const auto& entry = EnumOW::getEntryByUnderlyingValue(
      interValue.staticCast<OW>()
    );
    return EnumOW(entry.value);
  } catch (const std::runtime_error& e) { // TODO - add a specific error for this.
    error = EnumInterpreterError::ENTRY_NOT_FOUND;
  }
  return Void(nullptr, EnumOW::Class::getType());
}

template<class T, bool notnull>
Type* EnumInterpreterAsInteger<T, notnull>::getInterpretationType() {
  typedef typename std::underlying_type<T>::type EnumUT;
  return ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper::Class::getType();
}

namespace __class {

  template<class T, class Interpreter>
  class Enum : public AbstractEnum {
  private:

    class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
    public:
      PolymorphicDispatcher()
        : AbstractPolymorphicDispatcher(Interpreter::notNull)
      {}

      type::Void toInterpretation(const type::Void& enumValue, EnumInterpreterError& error) const override {
        return Interpreter::toInterpretation(enumValue, error);
      }

      type::Void fromInterpretation(const type::Void& interValue, EnumInterpreterError& error) const override {
        return Interpreter::fromInterpretation(interValue, error);
      }

      type::Type* getInterpretationType() const override {
        return Interpreter::getInterpretationType();
      }

      std::vector<type::Any> getInterpretedEnum() const override {

        typedef type::EnumObjectWrapper<T, Interpreter> EnumOW;

        std::vector<type::Any> result({});

        for(const auto& e : EnumOW::getEntries()) {
          type::EnumInterpreterError error = type::EnumInterpreterError::OK;
          result.push_back(type::Any(toInterpretation(EnumOW(e.value), error)));
          if(error != type::EnumInterpreterError::OK) {
            throw std::runtime_error("[oatpp::data::mapping::type::__class::Enum<T, Interpreter>::getInterpretedEnum()]: Unknown error.");
          }
        }

        return result;

      }

    };

  private:

    static type::Void creator() {
      return type::Void(std::make_shared<T>(), getType());
    }

    static Type createType() {
      Type type(__class::AbstractEnum::CLASS_ID, type::EnumMeta<T>::getInfo()->nameQualifier, &creator, nullptr, new PolymorphicDispatcher());
      return type;
    }

  public:

    static Type* getType() {
      static Type type = createType();
      return &type;
    }

  };

}

}}}}

namespace std {

  template<class T, class I>
  struct hash <oatpp::data::mapping::type::EnumObjectWrapper<T, I>> {

    typedef oatpp::data::mapping::type::EnumObjectWrapper<T, I> argument_type;
    typedef v_uint64 result_type;

    result_type operator()(argument_type const &e) const noexcept {
      if (e.get() == nullptr) return 0;
      return static_cast<v_uint64>(*e);
    }

  };

}

#endif // oatpp_data_mapping_type_Enum_hpp
