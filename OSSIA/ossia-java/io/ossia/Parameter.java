package io.ossia;
import com.sun.jna.Pointer;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.ptr.ByReference;
import com.sun.jna.ptr.PointerByReference;
import java.lang.Object;
import java.lang.Class;

public class Parameter
{
  Parameter(Pointer p)
  {
    impl = p;
  }

  public Value getValue()
  {
    return new Value(Ossia.INSTANCE.ossia_parameter_get_value(impl));
  }
  public void setValue(Value v)
  {
    Ossia.INSTANCE.ossia_parameter_set_value(impl, v.impl);
  }

  public float getFloat()
  {
    return Ossia.INSTANCE.ossia_parameter_to_float(impl);
  }
  public int getInt()
  {
    return Ossia.INSTANCE.ossia_parameter_to_int(impl);
  }
  public byte getChar()
  {
    return Ossia.INSTANCE.ossia_parameter_to_char(impl);
  }
  public String getString()
  {
    Pointer p = Ossia.INSTANCE.ossia_parameter_to_string(impl);
    String str = p.getString(0);
    Ossia.INSTANCE.ossia_string_free(p);
    return str;
  }
  public Vec2F getVec2()
  {
    return Ossia.INSTANCE.ossia_parameter_to_2f(impl);
  }
  public Vec3F getVec3()
  {
    return Ossia.INSTANCE.ossia_parameter_to_3f(impl);
  }
  public Vec4F getVec4()
  {
    return Ossia.INSTANCE.ossia_parameter_to_4f(impl);
  }
  public boolean getBoolean()
  {
    return Ossia.INSTANCE.ossia_parameter_to_bool(impl);
  }


  public void push_impulse()
  { Ossia.INSTANCE.ossia_parameter_push_impulse(impl); }
  public void push(int v)
  { Ossia.INSTANCE.ossia_parameter_push_i(impl, v); }
  public void push(float v)
  { Ossia.INSTANCE.ossia_parameter_push_f(impl, v); }
  public void push(boolean v)
  { Ossia.INSTANCE.ossia_parameter_push_b(impl, v ? 1 : 0); }
  public void push(byte v)
  { Ossia.INSTANCE.ossia_parameter_push_c(impl, v); }
  public void push(String v)
  { Ossia.INSTANCE.ossia_parameter_push_s(impl, v); }
  public void push(float v1, float v2)
  { Ossia.INSTANCE.ossia_parameter_push_2f(impl, v1, v2); }
  public void push(float v1, float v2, float v3)
  { Ossia.INSTANCE.ossia_parameter_push_3f(impl, v1, v2, v3); }
  public void push(float v1, float v2, float v3, float v4)
  { Ossia.INSTANCE.ossia_parameter_push_4f(impl, v1, v2, v3, v4); }
  public void push(Vec2F v)
  { Ossia.INSTANCE.ossia_parameter_push_2f(impl, v.x, v.y); }
  public void push(Vec3F v)
  { Ossia.INSTANCE.ossia_parameter_push_3f(impl, v.x, v.y, v.z); }
  public void push(Vec4F v)
  { Ossia.INSTANCE.ossia_parameter_push_4f(impl, v.x, v.y, v.z, v.w); }


  public void push(int[] l)
  {
    final int sz = Native.getNativeSize(Integer.TYPE);
    final Memory p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setInt(i * sz, l[i]);
    }
    Ossia.INSTANCE.ossia_parameter_push_in(impl, p, new SizeT(l.length));
  }
  public void push(float[] l)
  {
    final int sz = Native.getNativeSize(Float.TYPE);
    final Memory p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setFloat(i * sz, l[i]);
    }
    Ossia.INSTANCE.ossia_parameter_push_fn(impl, p, new SizeT(l.length));
  }
  public void push(Value[] l)
  {
    final int sz = Native.POINTER_SIZE;
    final Memory p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setPointer(i * sz, l[i].impl);
    }
    Ossia.INSTANCE.ossia_parameter_push_list(impl, p, new SizeT(l.length));
  }

  public Domain getDomain()
  {
    return new Domain(Ossia.INSTANCE.ossia_parameter_get_domain(impl));
  }
  public void setDomain(Domain v)
  {
    Ossia.INSTANCE.ossia_parameter_set_domain(impl, v.impl);
  }

  public String getUnit()
  {
    return Ossia.INSTANCE.ossia_parameter_get_unit(impl);
  }

  public void setUnit(String u)
  {
    Ossia.INSTANCE.ossia_parameter_set_unit(impl, u);
  }

  Pointer impl;
}
