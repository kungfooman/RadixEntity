#ifndef PROPERTY_HPP
#define PROPERTY_HPP

#include <functional>

#include "op_type_traits"

class Entity;


struct EntityIoValueType {
  uint32_t namespc;
  uint32_t type;
};

class PropertyBase {
protected:
  EntityIoValueType valueType;

  const char *const m_name;
  Entity *m_container;

  PropertyBase(const char *name, Entity *container);
};

enum PropertyAccess {
  R = 1,
  W = 2,
  RW = 3,
};

template<typename T, PropertyAccess A>
class Property : public PropertyBase {
public:
  // TODO: type-erased callbacks

  using ReadRawFuncPtr = std::remove_reference_t<T>&&(*)(Entity&);
  using ReadStdFuncPtr = std::function<std::remove_reference_t<T>&&(Entity&)>;

  // Could be changed to a T&& new value param
  using WriteRawFuncPtr = void(*)(Entity&, const T &newval);
  using WriteStdFuncPtr = std::function<void(Entity&, const T &newval)>;

  using OnChangeRawFuncPtr = void(*)(Entity&, const T &newval);
  using OnChangeStdFuncPtr = std::function<void(Entity&, const T &newval)>;

protected:
  T *m_valuePtr;
  OnChangeStdFuncPtr m_onChange;

  template<typename V>
  using ei = std::enable_if_t<V::value>;

public:
  Property(const char *name, Entity *container, T *valuePtr, OnChangeStdFuncPtr onChange = {}) :
    PropertyBase(name, container) {
    m_valuePtr = valuePtr;
    m_onChange = onChange;
  }

  operator const T&() const {
    static_assert(A | PropertyAccess::R, "Property has no read access");
    return *m_valuePtr;
  }

  template<typename C, typename = ei<can_op_eq<const T&, C>>>
  op_eq_type<const T&, C> operator==(C o) const {
    return *m_valuePtr == o;
  }
  // TODO: other operators

  template<typename C, typename = ei<std::is_convertible<C, T>>>
  const T& operator=(const C &v) {
    static_assert(A | PropertyAccess::W, "Property has no write access");
    //if (m_onChange) {
    //  m_onChange(*container, TODO);
    //}
    *m_valuePtr = v;
    return *m_valuePtr;
  }
  template<typename C, typename = ei<std::is_convertible<C, T>>>
  const T& operator=(T &&v) {
    static_assert(A | PropertyAccess::W, "Property has no write access");
    *m_valuePtr = std::move(v);
    return *m_valuePtr;
  }
};

// TODO: synthetic properties (user get/set method)

#endif /* PROPERTY_H */