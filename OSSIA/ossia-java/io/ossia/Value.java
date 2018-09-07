package io.ossia;
import com.sun.jna.Pointer;

public class Value implements AutoCloseable
{
  Value(Pointer p)
  {
    impl = p;
  }

  public Value()
  {
    impl = Ossia.INSTANCE.ossia_value_create_impulse();
  }

  public void set_impulse()
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_impulse();
  }
  public void set(float v)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_float(v);
  }
  public void set(int v)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_int(v);
  }
  public void set(boolean v)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_bool(v ? 1 : 0);
  }
  public void set(byte v)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_char(v);
  }
  public void set(String v)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_string(v);
  }
  public void set(float v1, float v2)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_2f(v1, v2);
  }
  public void set(float v1, float v2, float v3)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_3f(v1, v2, v3);
  }
  public void set(float v1, float v2, float v3, float v4)
  {
    close();
    impl = Ossia.INSTANCE.ossia_value_create_4f(v1, v2, v3, v4);
  }

  public float asFloat()
  {
    return Ossia.INSTANCE.ossia_value_convert_float(impl);
  }
  public int asInt()
  {
    return Ossia.INSTANCE.ossia_value_convert_int(impl);
  }
  public byte asChar()
  {
    return Ossia.INSTANCE.ossia_value_convert_char(impl);
  }
  public Vec2F asVec2()
  {
    return Ossia.INSTANCE.ossia_value_convert_2f(impl);
  }
  public Vec3F asVec3()
  {
    return Ossia.INSTANCE.ossia_value_convert_3f(impl);
  }
  public Vec4F asVec4()
  {
    return Ossia.INSTANCE.ossia_value_convert_4f(impl);
  }
  public boolean asBool()
  {
    return Ossia.INSTANCE.ossia_value_convert_bool(impl);
  }
  public boolean isImpulse()
  {
    return Ossia.INSTANCE.ossia_value_get_type(impl) == Type.IMPULSE_T;
  }

  @Override
  public void close()
  {
    Ossia.INSTANCE.ossia_value_free(impl);
  }

  Pointer impl;
}
