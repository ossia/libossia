package io.ossia;
import com.sun.jna.Pointer;

public class Domain implements AutoCloseable
{
  Domain(Pointer p)
  {
    impl = p;
  }

  public Domain(int min, int max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_int(min);
    Pointer b = Ossia.INSTANCE.ossia_value_create_int(max);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(float min, float max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_float(min);
    Pointer b = Ossia.INSTANCE.ossia_value_create_float(max);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(Vec2F min, Vec2F max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_2f(min.x, min.y);
    Pointer b = Ossia.INSTANCE.ossia_value_create_2f(max.x, max.y);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(Vec3F min, Vec3F max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_3f(min.x, min.y, min.z);
    Pointer b = Ossia.INSTANCE.ossia_value_create_3f(max.x, max.y, min.z);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(Vec4F min, Vec4F max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_4f(min.x, min.y, min.z, min.w);
    Pointer b = Ossia.INSTANCE.ossia_value_create_4f(max.x, max.y, min.z, min.w);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }


  @Override
  public void close() {
    if(impl != null)
      Ossia.INSTANCE.ossia_device_free(impl);
  }

  Value getMin()
  {
    return new Value(Ossia.INSTANCE.ossia_domain_get_min(impl));
  }
  Value getMax()
  {
    return new Value(Ossia.INSTANCE.ossia_domain_get_max(impl));
  }

  void setMin(Value v)
  {
    Ossia.INSTANCE.ossia_domain_set_min(impl, v.impl);
  }
  void setMax(Value v)
  {
    Ossia.INSTANCE.ossia_domain_set_max(impl, v.impl);
  }
  Pointer impl;
}
