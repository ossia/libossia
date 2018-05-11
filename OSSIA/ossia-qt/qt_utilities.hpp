/*
#pragma once
#include <wobjectdefs.h>
#include <QString>
#include <QObject>

#include <cstdarg>


template<typename... Args> constexpr auto make_overload(Args...)
{
  return qOverload<Args...>;
}

#define O_SIGNAL(ret, NAME, ...) \
  ret NAME(__VA_ARGS__) \
{ \
    using w_SignalType = decltype(make_overload(__VA_ARGS__)(&W_ThisType::NAME)); \
    return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__, 0)); \
} \
static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
    decltype(w_SignalState(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size; \
friend constexpr auto w_SignalState(w_internal::w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> w_counter, W_ThisType **w_this) \
    W_RETURN(w_internal::binary::tree_append(w_SignalState(w_counter.prev(), w_this), \
        w_internal::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__))))) \
static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}


#define O_SIGNAL2(ret, NAME, ...) \
  ret NAME(__VA_ARGS__) \
{ \
    using w_SignalType = decltype(make_overload(__VA_ARGS__)(&W_ThisType::NAME)); \
    return w_internal::SignalImplementation<w_SignalType, W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__)>{this}(W_OVERLOAD_REMOVE(__VA_ARGS__, 0)); \
} \
static constexpr int W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) = \
    decltype(w_SignalState(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size; \
friend constexpr auto w_SignalState(w_internal::w_number<W_MACRO_CONCAT(w_signalIndex_##NAME,__LINE__) + 1> w_counter, W_ThisType **w_this) \
    W_RETURN(w_internal::binary::tree_append(w_SignalState(w_counter.prev(), w_this), \
        w_internal::makeMetaSignalInfo( \
            W_OVERLOAD_RESOLVE(__VA_ARGS__)(&W_ThisType::NAME), #NAME, \
            W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)(), \
            W_PARAM_TOSTRING(W_OVERLOAD_TYPES(__VA_ARGS__)), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(__VA_ARGS__))))) \
static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(NAME, __VA_ARGS__)) {}

class  blah : public QObject
{
  W_OBJECT(blah)
  O_SIGNAL(void, appNameChanged, (QString)(appName), int(whatever));
  O_SIGNAL2(void, appNameChanged, QString, appName, int, whatever);

  void foo(QString(bar), int(bah))
  {
    using w_SignalType = decltype(make_overload(QString(bar), int(bah))(&W_ThisType::foo));
    return w_internal::SignalImplementation<w_SignalType, w_signalIndex_foo_39>{this}(QString(bar), int(bah), 0);

  }
  static constexpr int w_signalIndex_foo_39 = \
      decltype(w_SignalState(w_internal::w_number<>{}, static_cast<W_ThisType**>(nullptr)))::size; \
  friend constexpr auto w_SignalState(w_internal::w_number<w_signalIndex_foo_39 + 1> w_counter, W_ThisType **w_this) \
      W_RETURN(w_internal::binary::tree_append(w_SignalState(w_counter.prev(), w_this), \
          w_internal::makeMetaSignalInfo( \
              W_OVERLOAD_RESOLVE(QString(bar))(&W_ThisType::foo), "foo", \
              W_INTEGRAL_CONSTANT_HELPER(foo, QString(bar))(), \
              W_PARAM_TOSTRING(W_OVERLOAD_TYPES(QString(bar))), W_PARAM_TOSTRING(W_OVERLOAD_REMOVE(QString(bar)))))) \
  static inline void w_GetAccessSpecifierHelper(W_INTEGRAL_CONSTANT_HELPER(foo, QString(bar))) {}

};

*/
