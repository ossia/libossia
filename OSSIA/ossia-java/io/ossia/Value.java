package io.ossia;
import com.sun.jna.Pointer;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.ptr.ByReference;
import com.sun.jna.ptr.PointerByReference;
import java.util.ArrayList;
import java.lang.Object;
import java.lang.Class;

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
  public void set(int[] l)
  {
    final int sz = Native.getNativeSize(Integer.TYPE);
    final Pointer p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setInt(i * sz, l[i]);
    }
    close();
    impl = Ossia.INSTANCE.ossia_value_create_in(p, new SizeT(l.length));
  }
  public void set(float[] l)
  {
    final int sz = Native.getNativeSize(Float.TYPE);
    final Pointer p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setFloat(i * sz, l[i]);
    }
    close();
    impl = Ossia.INSTANCE.ossia_value_create_fn(p, new SizeT(l.length));
  }
  public void set(Value[] l)
  {
    final int sz = Pointer.SIZE;
    final Pointer p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setPointer(i * sz, l[i].impl);
    }
    close();
    impl = Ossia.INSTANCE.ossia_value_create_list(p, new SizeT(l.length));
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
  public boolean asBoolean()
  {
    return Ossia.INSTANCE.ossia_value_convert_bool(impl);
  }
  public int[] asInts()
  {
    int[] list = new int[0];

    if(getType() != Type.LIST_T)
    {
      return list;
    }

    final PointerByReference ptr = new PointerByReference();
    final SizeTByReference sz = new SizeTByReference();

    Ossia.INSTANCE.ossia_value_to_in(impl, ptr, sz);

    int n = sz.getValue().intValue();
    list = new int[n];

    int type_sz = Native.getNativeSize(Integer.TYPE);

    if (n > 0) {
      final Pointer vals = ptr.getValue();

      for (int i = 0; i < n; i++) {
        list[i] = vals.getInt(i * type_sz);
      }

      Ossia.INSTANCE.ossia_value_free_fn(vals);
    }

    return list;
  }

  public float[] asFloats()
  {
    float[] list = new float[0];

    if(getType() != Type.LIST_T)
    {
      return list;
    }

    final PointerByReference ptr = new PointerByReference();
    final SizeTByReference sz = new SizeTByReference();

    Ossia.INSTANCE.ossia_value_to_fn(impl, ptr, sz);

    final int n = sz.getValue().intValue();
    list = new float[n];

    final int type_sz = Native.getNativeSize(Float.TYPE);

    if (n > 0) {
      final Pointer vals = ptr.getValue();

      for (int i = 0; i < n; i++) {
        list[i] = vals.getFloat(i * type_sz);
      }

      Ossia.INSTANCE.ossia_value_free_in(vals);
    }

    return list;
  }

  public Value[] asList()
  {
    Value[] list = new Value[0];

    if(getType() != Type.LIST_T)
    {
      return list;
    }

    final PointerByReference ptr = new PointerByReference();
    final SizeTByReference sz = new SizeTByReference();

    Ossia.INSTANCE.ossia_value_to_list(impl, ptr, sz);

    final int n = sz.getValue().intValue();
    list = new Value[n];

    final int type_sz = Pointer.SIZE;

    if (n > 0) {
      final Pointer vals = ptr.getValue();

      for (int i = 0; i < n; i++) {
        list[i] = new Value(vals.getPointer(i * type_sz));
      }

      Ossia.INSTANCE.ossia_value_free_list(vals);
    }

    return list;
  }

  public int getType()
  {
    return Ossia.INSTANCE.ossia_value_get_type(impl);
  }

  public boolean isImpulse()
  {
    return getType() == Type.IMPULSE_T;
  }
  public boolean isFloat()
  {
    return getType() == Type.FLOAT_T;
  }
  public boolean isInt()
  {
    return getType() == Type.INT_T;
  }
  public boolean isChar()
  {
    return getType() == Type.CHAR_T;
  }
  public boolean isVec2()
  {
    return getType() == Type.VEC2F_T;
  }
  public boolean isVec3()
  {
    return getType() == Type.VEC3F_T;
  }
  public boolean isVec4()
  {
    return getType() == Type.VEC4F_T;
  }
  public boolean isBoolean()
  {
    return getType() == Type.BOOL_T;
  }
  public boolean isList()
  {
    return getType() == Type.LIST_T;
  }


  @Override
  public void close()
  {
    Ossia.INSTANCE.ossia_value_free(impl);
  }

  Pointer impl;
}
