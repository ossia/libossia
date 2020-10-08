package io.ossia;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.PointerByReference;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.Native;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.OptionalDouble;
import java.util.AbstractCollection;

public class Node
{
  Node(Pointer p)
  {
    impl = p;
  }

  // Child node management
  public Node createNode(String name)
  {
    return new Node(Ossia.INSTANCE.ossia_node_create(impl, name));
  }

  public Node findNode(String name)
  {
    Pointer p = Ossia.INSTANCE.ossia_node_find(impl, name);
    if(p != null)
      return new Node(p);
    return null;
  }

  public Node[] createPattern(String pattern)
  {
    PointerByReference data = new PointerByReference();
    SizeTByReference size = new SizeTByReference();

    Ossia.INSTANCE.ossia_node_create_pattern(impl, pattern, data, size);

    int n = (int)size.getValue().longValue();
    Node[] nodes = new Node[n];
    if(n > 0)
    {
      final int type_sz = Native.POINTER_SIZE;
      final Pointer vals = data.getValue();
      for(int i = 0; i < n; i ++)
      {
        nodes[i] = new Node(vals.getPointer(i * type_sz));
      }
    }
    Ossia.INSTANCE.ossia_node_array_free(data.getValue());
    return nodes;
  }

  public Node[] findPattern(String pattern)
  {
    PointerByReference data = new PointerByReference();
    SizeTByReference size = new SizeTByReference();

    Ossia.INSTANCE.ossia_node_find_pattern(impl, pattern, data, size);

    int n = (int)size.getValue().longValue();
    Node[] nodes = new Node[n];
    if(n > 0)
    {
      final int type_sz = Native.POINTER_SIZE;
      final Pointer vals = data.getValue();
      for(int i = 0; i < n; i ++)
      {
        nodes[i] = new Node(vals.getPointer(i * type_sz));
      }
    }
    Ossia.INSTANCE.ossia_node_array_free(data.getValue());
    return nodes;
  }

  public int childCount()
  {
    return Ossia.INSTANCE.ossia_node_child_size(impl);
  }

  public Node getChild(int i)
  {
    return new Node(Ossia.INSTANCE.ossia_node_get_child(impl, i));
  }

  // Name
  public String getName()
  {
    return Ossia.INSTANCE.ossia_node_get_name(impl);
  }

  // Parameter
  public Parameter createParameter(int t)
  {
    return new Parameter(Ossia.INSTANCE.ossia_node_create_parameter(impl, t));
  }

  public void removeParameter()
  {
    Ossia.INSTANCE.ossia_node_remove_parameter(impl);
  }

  public Parameter create(String name, String type)
  {
    return new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
  }

  public Parameter create(String name, String type, float min, float max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(min, max));
    return p;
  }

  public Parameter create(String name, String type, double min, double max)
  {
    return create(name, type, (float) min, (float) max);
  }

  public Parameter create(String name, String type, int min, int max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(min, max));
    return p;
  }

  public Parameter create(String name, String type, Vec2F min, Vec2F max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(min, max));
    return p;
  }

  public Parameter create(String name, String type, Vec3F min, Vec3F max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(min, max));
    return p;
  }

  public Parameter create(String name, String type, Vec4F min, Vec4F max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(min, max));
    return p;
  }

  public Parameter create(String name, String type, float[] min, float[] max)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    if(min.length == 2 && max.length == 2)
      p.setDomain(new Domain(new Vec2F(min), new Vec2F(max)));
    else if(min.length == 3 && max.length == 3)
      p.setDomain(new Domain(new Vec3F(min), new Vec3F(max)));
    else if(min.length == 4 && max.length == 4)
      p.setDomain(new Domain(new Vec4F(min), new Vec4F(max)));
    else
      p.setDomain(new Domain(new Value(min), new Value(max)));
    return p;
  }

  public Parameter create(String name, String type, String[] values)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(values));
    return p;
  }

  public Parameter create(String name, String type, int[] values)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(values));
    return p;
  }

  public Parameter create(String name, String type, float[] values)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(values));
    return p;
  }

  public Parameter create(String name, String type, Value[] values)
  {
    Parameter p = new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
    p.setDomain(new Domain(values));
    return p;
  }

  public ParameterBuilder newChild() { return new ParameterBuilder(this); }
  public ParameterBuilder newChild(String name) { return new ParameterBuilder(this).name(name); }
  public ParameterBuilder newChild(String name, String type) { return new ParameterBuilder(this).name(name).type(type); }


  public Parameter getParameter()
  {
    Pointer p = Ossia.INSTANCE.ossia_node_get_parameter(impl);
    if(p != null)
      return new Parameter(p);
    return null;
  }

  // Description
  public String getDescription()
  {
    return Ossia.INSTANCE.ossia_node_get_description(impl);
  }
  public void setDescription(String u)
  {
    Ossia.INSTANCE.ossia_node_set_description(impl, u);
  }

  // Extended type
  public String getExtendedType()
  {
    return Ossia.INSTANCE.ossia_node_get_extended_type(impl);
  }
  public void setExtendedType(String u)
  {
    Ossia.INSTANCE.ossia_node_set_extended_type(impl, u);
  }

  // Hidden
  public boolean getHidden()
  {
    return Ossia.INSTANCE.ossia_node_get_hidden(impl) == 0 ? false : true;
  }
  public void setHidden(boolean u)
  {
    Ossia.INSTANCE.ossia_node_set_hidden(impl, u ? 1 : 0);
  }

  // Refresh rate
  public OptionalInt getRefreshRate()
  {
    IntByReference ref = new IntByReference(0);
    int res = Ossia.INSTANCE.ossia_node_get_refresh_rate(impl, ref);
    if(ref.getValue() != 0)
    {
      return OptionalInt.of(res);
    }
    else
    {
      return OptionalInt.empty();
    }
  }

  public void setRefreshRate(OptionalInt u)
  {
    if(u.isPresent())
      Ossia.INSTANCE.ossia_node_set_refresh_rate(impl, u.getAsInt());
    else
      Ossia.INSTANCE.ossia_node_unset_refresh_rate(impl);
  }

  public void setRefreshRate(int u)
  {
    Ossia.INSTANCE.ossia_node_set_refresh_rate(impl, u);
  }

  public void unsetRefreshRate()
  {
    Ossia.INSTANCE.ossia_node_unset_refresh_rate(impl);
  }

  // Priority
  public OptionalDouble getPriority()
  {
    IntByReference ref = new IntByReference(0);
    float res = Ossia.INSTANCE.ossia_node_get_priority(impl, ref);
    if(ref.getValue() != 0)
    {
      return OptionalDouble.of(res);
    }
    else
    {
      return OptionalDouble.empty();
    }
  }
  public void setPriority(OptionalDouble u)
  {
    if(u.isPresent())
      Ossia.INSTANCE.ossia_node_set_priority(impl, (float)u.getAsDouble());
    else
      Ossia.INSTANCE.ossia_node_unset_priority(impl);
  }
  public void setPriority(double u)
  {
    Ossia.INSTANCE.ossia_node_set_priority(impl, (float)u);
  }
  public void unsetPriority()
  {
    Ossia.INSTANCE.ossia_node_unset_priority(impl);
  }

  // Step size
  public OptionalDouble getValueStepSize()
  {
    IntByReference ref = new IntByReference(0);
    double res = Ossia.INSTANCE.ossia_node_get_value_step_size(impl, ref);
    if(ref.getValue() != 0)
    {
      return OptionalDouble.of(res);
    }
    else
    {
      return OptionalDouble.empty();
    }
  }
  public void setValueStepSize(OptionalDouble u)
  {
    if(u.isPresent())
      Ossia.INSTANCE.ossia_node_set_value_step_size(impl, u.getAsDouble());
    else
      Ossia.INSTANCE.ossia_node_unset_value_step_size(impl);
  }
  public void setValueStepSize(double u)
  {
    Ossia.INSTANCE.ossia_node_set_value_step_size(impl, u);
  }
  public void unsetValueStepSize()
  {
    Ossia.INSTANCE.ossia_node_unset_value_step_size(impl);
  }

  // Instance bounds
  public Optional<InstanceBounds> getInstanceBounds()
  {
    IntByReference min = new IntByReference(0);
    IntByReference max = new IntByReference(0);
    IntByReference ok = new IntByReference(0);
    Ossia.INSTANCE.ossia_node_get_instance_bounds(impl, min, max, ok);
    if(ok.getValue() != 0)
    {
      return Optional.of(new InstanceBounds(min.getValue(), max.getValue()));
    }
    else
    {
      return Optional.empty();
    }
  }

  public void setInstanceBounds(Optional<InstanceBounds> u)
  {
    if(u.isPresent())
    {
      InstanceBounds inst = u.get();
      Ossia.INSTANCE.ossia_node_set_instance_bounds(impl, inst.min, inst.max);
    }
    else
    {
      Ossia.INSTANCE.ossia_node_unset_instance_bounds(impl);
    }
  }

  public void setInstanceBounds(int min, int max)
  {
    Ossia.INSTANCE.ossia_node_set_instance_bounds(impl, min, max);
  }
  public void unsetInstanceBounds()
  {
    Ossia.INSTANCE.ossia_node_unset_instance_bounds(impl);
  }

  // Default value
  public Optional<Value> getDefaultValue()
  {
    Pointer p = Ossia.INSTANCE.ossia_node_get_default_value(impl);
    if(p != Pointer.NULL)
    {
      return Optional.empty();
    }
    else
    {
      return Optional.of(new Value(p));
    }
  }

  public void setDefaultValue(Value u)
  {
    Ossia.INSTANCE.ossia_node_set_default_value(impl, u.impl);
  }

  Pointer impl;
}
