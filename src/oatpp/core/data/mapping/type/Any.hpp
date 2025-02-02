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

#ifndef oatpp_data_mapping_type_Any_hpp
#define oatpp_data_mapping_type_Any_hpp

#include "./Type.hpp"

#include "oatpp/core/collection/LinkedList.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Countable.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class Any {
  public:
    static const ClassId CLASS_ID;

    static Type *getType() {
      static Type type(CLASS_ID, nullptr);
      return &type;
    }

  };

}

class AnyHandle : public base::Countable {
public:

  AnyHandle(const std::shared_ptr<void>& objPtr, const Type* const objType)
    : ptr(objPtr)
    , type(objType)
  {}

  std::shared_ptr<void> ptr;
  const Type* const type;

};

class Any : public ObjectWrapper<AnyHandle, __class::Any>{
public:
  typedef Any __Wrapper;
public:

  Any();

  Any(std::nullptr_t);

  Any(const Any& other);
  Any(Any&& other);

  Any(const std::shared_ptr<base::Countable>& ptr, const Type* const type);

  template<class T, class C>
  Any(const ObjectWrapper<T, C>& polymorph)
    : ObjectWrapper(std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.valueType), __class::Any::getType())
  {}

  template<class T, class C>
  void store(const ObjectWrapper<T, C>& polymorph) {
    m_ptr = std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.valueType);
  }

  const Type* getStoredType() const;

  template<class WrapperType>
  typename WrapperType::__Wrapper retrieve() const {

    if(m_ptr) {

      if(m_ptr->type != WrapperType::__Wrapper::Class::getType()) {
        throw std::runtime_error("[oatpp::data::mapping::type::Any::retrieve()]: Error. The value type doesn't match.");
      }

      return typename WrapperType::__Wrapper(std::static_pointer_cast<typename WrapperType::__Wrapper::ObjectType>(m_ptr->ptr), m_ptr->type);

    }

    return nullptr;

  }

  Any& operator=(std::nullptr_t);

  Any& operator=(const Any& other);
  Any& operator=(Any&& other);

  template<class T, class C>
  Any& operator=(const ObjectWrapper<T, C>& polymorph) {
    m_ptr = std::make_shared<AnyHandle>(polymorph.getPtr(), polymorph.valueType);
    return *this;
  }

  bool operator == (std::nullptr_t) const;
  bool operator != (std::nullptr_t) const;

  bool operator == (const Any& other) const;
  bool operator != (const Any& other) const;

};

}}}}

#endif //oatpp_data_mapping_type_Any_hpp
