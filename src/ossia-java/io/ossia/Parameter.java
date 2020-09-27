package io.ossia;
import com.sun.jna.Pointer;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.ptr.ByReference;
import com.sun.jna.ptr.PointerByReference;
import java.lang.Object;
import java.lang.Class;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.OptionalDouble;

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


  public void push(Value v)
  { Ossia.INSTANCE.ossia_parameter_push_value(impl, v.impl); }
  public void push_impulse()
  { Ossia.INSTANCE.ossia_parameter_push_impulse(impl); }
  public void push(int v)
  { Ossia.INSTANCE.ossia_parameter_push_i(impl, v); }
  public void push(float v)
  { Ossia.INSTANCE.ossia_parameter_push_f(impl, v); }
  public void push(double v)
  { Ossia.INSTANCE.ossia_parameter_push_f(impl, (float)v); }
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
    Ossia.INSTANCE.ossia_parameter_push_in(impl, l, new SizeT(l.length));
  }
  public void push(float[] l)
  {
    Ossia.INSTANCE.ossia_parameter_push_fn(impl, l, new SizeT(l.length));
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
  public void setDomain(int min, int max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(float min, float max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(double min, double max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(Vec2F min, Vec2F max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(Vec3F min, Vec3F max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(Vec4F min, Vec4F max)
  {
    setDomain(new Domain(min, max));
  }
  public void setDomain(int[] v)
  {
    setDomain(new Domain(v));
  }
  public void setDomain(float[] v)
  {
    setDomain(new Domain(v));
  }
  public void setDomain(String[] v)
  {
    setDomain(new Domain(v));
  }
  public void setDomain(Value[] v)
  {
    setDomain(new Domain(v));
  }

  public String getUnit()
  {
    return Ossia.INSTANCE.ossia_parameter_get_unit(impl);
  }
  public void setUnit(String u)
  {
    Ossia.INSTANCE.ossia_parameter_set_unit(impl, u);
  }

  public int getAccessMode()
  {
    return Ossia.INSTANCE.ossia_parameter_get_access_mode(impl);
  }
  public void setAccessMode(int u)
  {
    Ossia.INSTANCE.ossia_parameter_set_access_mode(impl, u);
  }

  public int getBoundingMode()
  {
    return Ossia.INSTANCE.ossia_parameter_get_bounding_mode(impl);
  }
  public void setBoundingMode(int u)
  {
    Ossia.INSTANCE.ossia_parameter_set_bounding_mode(impl, u);
  }

  public boolean getMuted()
  {
    return Ossia.INSTANCE.ossia_parameter_get_muted(impl) != 0 ? true : false;
  }
  public void setMuted(boolean u)
  {
    Ossia.INSTANCE.ossia_parameter_set_muted(impl, u ? 1 : 0);
  }

  public boolean getDisabled()
  {
    return Ossia.INSTANCE.ossia_parameter_get_disabled(impl) != 0 ? true : false;
  }
  public void setDisabled(boolean u)
  {
    Ossia.INSTANCE.ossia_parameter_set_disabled(impl, u ? 1 : 0);
  }

  public boolean getCritical()
  {
    return Ossia.INSTANCE.ossia_parameter_get_critical(impl) != 0 ? true : false;
  }
  public void setCritical(boolean u)
  {
    Ossia.INSTANCE.ossia_parameter_set_critical(impl, u ? 1 : 0);
  }

  public boolean getRepetitionFilter()
  {
    return Ossia.INSTANCE.ossia_parameter_get_repetition_filter(impl) != 0 ? true : false;
  }
  public void setRepetitionFilter(boolean u)
  {
    Ossia.INSTANCE.ossia_parameter_set_repetition_filter(impl, u ? 1 : 0);
  }

  public Node getNode()
  {
    return new Node(Ossia.INSTANCE.ossia_parameter_get_node(impl));
  }

  public String getDescription()
  {
    return getNode().getDescription();
  }
  public void setDescription(String v)
  {
    getNode().setDescription(v);
  }

  public String getExtendedType()
  {
    return getNode().getExtendedType();
  }
  public void setExtendedType(String v)
  {
    getNode().setExtendedType(v);
  }

  public boolean getHidden()
  {
    return getNode().getHidden();
  }
  public void setHidden(boolean v)
  {
    getNode().setHidden(v);
  }

  public OptionalInt getRefreshRate()
  {
    return getNode().getRefreshRate();
  }
  public void setRefreshRate(OptionalInt u)
  {
    getNode().setRefreshRate(u);
  }
  public void setRefreshRate(int u)
  {
    getNode().setRefreshRate(u);
  }
  public void unsetRefreshRate()
  {
    getNode().unsetRefreshRate();
  }

  public OptionalDouble getPriority()
  {
    return getNode().getPriority();
  }
  public void setPriority(OptionalDouble u)
  {
    getNode().setPriority(u);
  }
  public void setPriority(double u)
  {
    getNode().setPriority(u);
  }
  public void unsetPriority()
  {
    getNode().unsetPriority();
  }


  public OptionalDouble getValueStepSize()
  {
    return getNode().getValueStepSize();
  }
  public void setValueStepSize(OptionalDouble u)
  {
    getNode().setValueStepSize(u);
  }
  public void setValueStepSize(double u)
  {
    getNode().setValueStepSize(u);
  }
  public void unsetValueStepSize()
  {
    getNode().unsetValueStepSize();
  }


  public Optional<Value> getDefaultValue()
  {
    return getNode().getDefaultValue();
  }
  public void setDefaultValue(Value u)
  {
    getNode().setDefaultValue(u);
  }

  Pointer impl;
}
