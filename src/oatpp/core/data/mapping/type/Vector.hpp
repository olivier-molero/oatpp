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

#ifndef oatpp_data_mapping_type_Vector_hpp
#define oatpp_data_mapping_type_Vector_hpp

#include "./Type.hpp"

#include <vector>
#include <initializer_list>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class AbstractVector {
  public:
    static const ClassId CLASS_ID;
  public:

    class AbstractPolymorphicDispatcher {
    public:
      virtual void addPolymorphicItem(const type::Void& object, const type::Void& item) const = 0;
    };

  };

  template<class T>
  class Vector;

}

template<class T, class C>
class VectorObjectWrapper : public type::ObjectWrapper<std::vector<T>, C> {
public:
  typedef std::vector<T> TemplateObjectType;
  typedef C TemplateObjectClass;
public:

  OATPP_DEFINE_OBJECT_WRAPPER_DEFAULTS(VectorObjectWrapper, TemplateObjectType, TemplateObjectClass)

  VectorObjectWrapper(std::initializer_list<T> ilist)
    : type::ObjectWrapper<TemplateObjectType, TemplateObjectClass>(std::make_shared<TemplateObjectType>(ilist))
  {}

  static VectorObjectWrapper createShared() {
    return std::make_shared<TemplateObjectType>();
  }

  VectorObjectWrapper& operator = (std::initializer_list<T> ilist) {
    this->m_ptr = std::make_shared<TemplateObjectType>(ilist);
    return *this;
  }

  T& operator[] (v_buff_usize index) const {
    return this->m_ptr->operator [] (index);
  }

  TemplateObjectType& operator*() const {
    return this->m_ptr.operator*();
  }

};

template<class T>
using Vector = VectorObjectWrapper<typename T::__Wrapper, __class::Vector<typename T::__Wrapper>>;

typedef VectorObjectWrapper<type::Void, __class::AbstractVector> AbstractVector;

namespace __class {

  template<class T>
  class Vector : public AbstractVector {
  private:

    class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
    public:

      void addPolymorphicItem(const type::Void& object, const type::Void& item) const override {
        const auto& vector = object.staticCast<type::Vector<T>>();
        const auto& vectorItem = item.staticCast<T>();
        vector->push_back(vectorItem);
      }

    };

  private:

    static type::Void creator() {
      return type::Void(std::make_shared<std::vector<T>>(), getType());
    }

    static Type createType() {
      Type type(__class::AbstractVector::CLASS_ID, nullptr, &creator, nullptr, new PolymorphicDispatcher());
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

#endif // oatpp_data_mapping_type_Vector_hpp
