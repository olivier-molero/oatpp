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

#ifndef oatpp_data_mapping_type_UnorderedSet_hpp
#define oatpp_data_mapping_type_UnorderedSet_hpp

#include "./Type.hpp"

#include <unordered_set>
#include <initializer_list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

class AbstractUnorderedSet {
public:
  static const ClassId CLASS_ID;
public:

  class AbstractPolymorphicDispatcher {
  public:
    virtual void addPolymorphicItem(const type::Void& object, const type::Void& item) const = 0;
  };

};

template<class T>
class UnorderedSet;

}

template<class T, class C>
class UnorderedSetObjectWrapper : public type::ObjectWrapper<std::unordered_set<T>, C> {
public:
  typedef std::unordered_set<T> TemplateObjectType;
  typedef C TemplateObjectClass;
public:

OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(UnorderedSetObjectWrapper, TemplateObjectType, TemplateObjectClass)

  UnorderedSetObjectWrapper(std::initializer_list<T> ilist)
    : type::ObjectWrapper<TemplateObjectType, TemplateObjectClass>(std::make_shared<TemplateObjectType>(ilist))
  {}

  static UnorderedSetObjectWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  UnorderedSetObjectWrapper& operator = (std::initializer_list<T> ilist) {
    this->m_ptr = std::make_shared<TemplateObjectType>(ilist);
    return *this;
  }

  bool operator[] (const T& key) const {
    if(this->m_ptr) {
      auto it = this->m_ptr->find(key);
      return it != this->m_ptr->end();
    }
    return false;
  }

  TemplateObjectType& operator*() const {
    return this->m_ptr.operator*();
  }

};

template<class T>
using UnorderedSet = UnorderedSetObjectWrapper<typename T::__Wrapper, __class::UnorderedSet<typename T::__Wrapper>>;

typedef UnorderedSetObjectWrapper<type::Void, __class::AbstractUnorderedSet> AbstractUnorderedSet;

namespace __class {

template<class T>
class UnorderedSet : public AbstractUnorderedSet {
private:

  class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
  public:

    void addPolymorphicItem(const type::Void& object, const type::Void& item) const override {
      const auto& set = object.staticCast<type::UnorderedSet<T>>();
      const auto& setItem = item.staticCast<T>();
      set->insert(setItem);
    }

  };

private:

  static type::Void creator() {
    return type::Void(std::make_shared<std::unordered_set<T>>(), getType());
  }

  static Type createType() {
    Type type(__class::AbstractUnorderedSet::CLASS_ID, nullptr, &creator, nullptr, new PolymorphicDispatcher());
    type.params.push_back(T::Class::getType());
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

#endif // oatpp_data_mapping_type_UnorderedSet_hpp
