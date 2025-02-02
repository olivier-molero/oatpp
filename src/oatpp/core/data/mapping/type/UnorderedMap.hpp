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

#ifndef oatpp_data_mapping_type_UnorderedMap_hpp
#define oatpp_data_mapping_type_UnorderedMap_hpp

#include "./Type.hpp"

#include <unordered_map>
#include <initializer_list>
#include <utility>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class AbstractUnorderedMap {
  public:
    static const ClassId CLASS_ID;
  public:

    class AbstractPolymorphicDispatcher {
    public:
      virtual void addPolymorphicItem(const type::Void& object, const type::Void& key, const type::Void& value) const = 0;
    };

  };

  template<class Key, class Value>
  class UnorderedMap;

}

template<class Key, class Value, class C>
class UnorderedMapObjectWrapper : public type::ObjectWrapper<std::unordered_map<Key, Value>, C> {
public:
  typedef std::unordered_map<Key, Value> TemplateObjectType;
  typedef C TemplateObjectClass;
public:

OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(UnorderedMapObjectWrapper, TemplateObjectType, TemplateObjectClass)

  UnorderedMapObjectWrapper(std::initializer_list<std::pair<const Key, Value>> ilist)
    : type::ObjectWrapper<TemplateObjectType, TemplateObjectClass>(std::make_shared<TemplateObjectType>(ilist))
  {}

  static UnorderedMapObjectWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  UnorderedMapObjectWrapper& operator = (std::initializer_list<std::pair<const Key, Value>> ilist) {
    this->m_ptr = std::make_shared<TemplateObjectType>(ilist);
    return *this;
  }

  Value& operator[] (const Key& key) const {
    return this->m_ptr->operator [] (key);
  }

  TemplateObjectType& operator*() const {
    return this->m_ptr.operator*();
  }

};

template<class Key, class Value>
using UnorderedMap = UnorderedMapObjectWrapper<
  typename Key::__Wrapper,
  typename Value::__Wrapper,
  __class::UnorderedMap<
    typename Key::__Wrapper,
    typename Value::__Wrapper
  >
>;

namespace __class {

  template<class Key, class Value>
  class UnorderedMap : public AbstractUnorderedMap {
  private:

    class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
    public:

      void addPolymorphicItem(const type::Void& object, const type::Void& key, const type::Void& value) const override {
        const auto& map = object.staticCast<type::UnorderedMap<Key, Value>>();
        const auto& k = key.staticCast<Key>();
        const auto& v = value.staticCast<Value>();
        map[k] = v;
      }

    };

  private:

    static type::Void creator() {
      return type::Void(std::make_shared<std::unordered_map<Key, Value>>(), getType());
    }

    static Type createType() {
      Type type(__class::AbstractUnorderedMap::CLASS_ID, nullptr, &creator, nullptr, new PolymorphicDispatcher());
      type.params.push_back(Key::Class::getType());
      type.params.push_back(Value::Class::getType());
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

#endif // oatpp_data_mapping_type_UnorderedMap_hpp
