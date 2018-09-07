package io.ossia;
import com.sun.jna.Pointer;

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
