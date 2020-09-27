package io.ossia;
import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.StringArray;
import java.util.AbstractCollection;

public class Domain implements AutoCloseable
{
  Domain(Pointer p)
  {
    impl = p;
  }

  public Domain(Value min, Value max)
  {
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(min.impl, max.impl);
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
  public Domain(double min, double max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_float((float)min);
    Pointer b = Ossia.INSTANCE.ossia_value_create_float((float)max);
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
    Pointer b = Ossia.INSTANCE.ossia_value_create_3f(max.x, max.y, max.z);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(Vec4F min, Vec4F max)
  {
    Pointer a = Ossia.INSTANCE.ossia_value_create_4f(min.x, min.y, min.z, min.w);
    Pointer b = Ossia.INSTANCE.ossia_value_create_4f(max.x, max.y, max.z, max.w);
    impl = Ossia.INSTANCE.ossia_domain_make_min_max(a, b);
    Ossia.INSTANCE.ossia_value_free(b);
    Ossia.INSTANCE.ossia_value_free(a);
  }
  public Domain(String[] set)
  {
    impl = Ossia.INSTANCE.ossia_domain_make_string_set(set, new SizeT(set.length));
  }
  public Domain(int[] set)
  {
    impl = Ossia.INSTANCE.ossia_domain_make_int_set(set, new SizeT(set.length));
  }
  public Domain(float[] set)
  {
    impl = Ossia.INSTANCE.ossia_domain_make_float_set(set, new SizeT(set.length));
  }
  public Domain(Value[] l)
  {
    final int sz = Native.POINTER_SIZE;
    final Memory p = new Memory(l.length * sz);

    for (int i = 0; i < l.length; i++) {
      p.setPointer(i * sz, l[i].impl);
    }
    impl = Ossia.INSTANCE.ossia_domain_make_value_set(p, new SizeT(l.length));
  }


  @Override
  public void close() {
    if(impl != null)
      Ossia.INSTANCE.ossia_domain_free(impl);
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
