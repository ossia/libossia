#include "ossia-java.hpp"
#include <ossia/ossia.hpp>
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
    f();
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

  ossia::value val;

  void SetInt(jni::JNIEnv& e, jni::jlong a)
  { safe_function(__func__, e, [&] { val = Int{a}; }); }
  void SetFloat(jni::JNIEnv& e, jni::jfloat a)
  { safe_function(__func__, e, [&] { val = Float{a}; }); }
  void SetChar(jni::JNIEnv& e, jni::jchar a)
  { safe_function(__func__, e, [&] { val = Char{a}; }); }
  void SetBool(jni::JNIEnv& e, jni::jboolean a)
  { safe_function(__func__, e, [&] { val = Bool{a}; }); }
  void SetString(jni::JNIEnv& e, const jni::jstring& a)
  { safe_function(__func__, e, [&] { ; }); }
  void SetTuple(jni::JNIEnv& e, const jni::jobjectArray& a)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec2f(jni::JNIEnv& e, const jfloatArray&)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec3f(jni::JNIEnv& e, const jfloatArray&)
  { safe_function(__func__, e, [&] { ; }); }
  void SetVec4f(jni::JNIEnv& e, const jfloatArray&)
  { safe_function(__func__, e, [&] { ; }); }

  jni::jint GetInt(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<Int>().value; }); }
  jni::jfloat GetFloat(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<Float>().value; }); }
  jni::jchar GetChar(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<Char>().value; }); }
  jni::jboolean GetBool(jni::JNIEnv& e)
  { return safe_function(__func__, e, [&] { return val.get<Bool>().value; }); }
  jni::String GetString(jni::JNIEnv& e) { return jni::MakeAnything(jni::ThingToMake<jni::String>{}, e, "lol"); }
  //jni::Array<jni::Object> GetTuple(jni::JNIEnv& e) { return {}; }
  //jni::Array<jni::jfloat> GetVec2f(jni::JNIEnv& e) { return {}; }
  //jni::Array<jni::jfloat> GetVec3f(jni::JNIEnv& e) { return {}; }
  //jni::Array<jni::jfloat> GetVec4f(jni::JNIEnv& e) { return {}; }
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
  /*
  using namespace ossia::java;
  jni::JNIEnv& env = jni::GetEnv(*vm);

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

  jni::RegisterNativePeer<Value>(
        env,
        jni::Class<Value>::Find(env),
        "peer",
        std::make_unique<Value, JNIEnv&>,
        "initialize",
        "finalize",
        METHOD(&Calculator::Add, "add"),
        METHOD(&Calculator::Subtract, "subtract")
        );
*/
  return jni::Unwrap(jni::jni_version_1_2);
}
