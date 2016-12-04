#include "ossia-java.hpp"
#include <ossia/editor/value/value.hpp>

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

namespace ossia
{
namespace java
{
// See http://stackoverflow.com/a/12014833/1495627
template<typename Fun>
auto safe_function(std::string name, jni::JNIEnv& env, Fun f)
    -> decltype(f())
{
  try
  {
    return f();
  }
  catch(const std::exception& e)
  {
    name += ": ";
    name += e.what();
    if (auto newExcCls = env.FindClass("java/lang/Error"))
      env.ThrowNew(newExcCls, name.c_str());

    return decltype(f())();
  }
  catch(...)
  {
    if (auto newExcCls = env.FindClass("java/lang/Error"))
      env.ThrowNew(newExcCls, name.c_str());

    return decltype(f())();
  }
}

class Value
{
public:

  static constexpr auto Name() { return "Value"; }
  Value(JNIEnv&) { }
  ~Value() { }

  using ValueArray = jni::Array<jni::Object<Value>>;
  using FloatArray = jni::Array<jni::jfloat>;

  ossia::value val;

  void SetInt(jni::JNIEnv& e, jni::jlong a)
  { safe_function(__func__, e, [&] { val = int{a}; }); }
  void SetFloat(jni::JNIEnv& e, jni::jfloat a)
  { safe_function(__func__, e, [&] { val = float{a}; }); }
  void SetChar(jni::JNIEnv& e, jni::jchar a)
  { safe_function(__func__, e, [&] { val = char{a}; }); }
  void SetBool(jni::JNIEnv& e, jni::jboolean a)
  { safe_function(__func__, e, [&] { val = bool{a}; }); }
  void SetString(jni::JNIEnv& e, jni::String a)
  { safe_function(__func__, e, [&] { ; }); }
  void SetTuple(jni::JNIEnv& e, ValueArray a)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec2f(jni::JNIEnv& e, FloatArray)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec3f(jni::JNIEnv& e, FloatArray)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec4f(jni::JNIEnv& e, FloatArray)
  { safe_function(__func__, e, [&] { ; }); }

  jni::jint GetInt(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<int>(); }); }
  jni::jfloat GetFloat(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<float>(); }); }
  jni::jchar GetChar(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<char>(); }); }
  jni::jboolean GetBool(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<bool>(); }); }
  jni::String GetString(jni::JNIEnv& e)
  {
    return safe_function(__func__, e, [&] {
      return jni::MakeAnything(jni::ThingToMake<jni::String>{}, e, "lol");
    });
  }
  /*
  jarray<jobject> GetTuple(jni::JNIEnv& e) {
    return safe_function(__func__, e, [&] {
      return jni::NewObjectArray(e, 1234, jni::FindClass(e, "ossia/Value"));
    });
  }

  FloatArray GetVec2f(jni::JNIEnv& e) { return {}; }
  FloatArray GetVec3f(jni::JNIEnv& e) { return {}; }
  FloatArray GetVec4f(jni::JNIEnv& e) { return {}; }*/
  jni::jlong GetType(jni::JNIEnv& e) { return {}; }
};

}
}
struct Calculator
{


  jni::jlong Add(jni::JNIEnv&, jni::jlong a, jni::jlong b) { return a + b; }
  jni::jlong Subtract(jni::JNIEnv&, jni::jlong a, jni::jlong b) { return a - b; }
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*)
{
  using namespace ossia::java;
  jni::JNIEnv& env = jni::GetEnv(*vm);


  jni::RegisterNativePeer<Value>(
        env,
        jni::Class<Value>::Find(env),
        "peer",
        std::make_unique<Value, JNIEnv&>,
        "initialize",
        "finalize",
        METHOD(&Value::SetInt, "setInt"),
        METHOD(&Value::SetFloat, "setFloat"),
        METHOD(&Value::SetBool, "setBool"),
        METHOD(&Value::SetChar, "setChar"),
        METHOD(&Value::SetString, "setString"),
        METHOD(&Value::SetTuple, "setTuple"),
        /*
        METHOD(&Value::SetVec2f, "setVec2f"),
        METHOD(&Value::SetVec3f, "setVec3f"),
        METHOD(&Value::SetVec4f, "setVec4f"),
        */

        METHOD(&Value::GetInt, "getInt"),
        METHOD(&Value::GetFloat, "getFloat"),
        METHOD(&Value::GetBool, "getBool"),
        METHOD(&Value::GetChar, "getChar"),
        METHOD(&Value::GetString, "getString"),

        METHOD(&Value::GetType, "getType")
        );
  return jni::Unwrap(jni::jni_version_1_2);
}
