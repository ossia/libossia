package io.ossia;
import java.util.Optional;
import java.util.OptionalInt;
import java.util.OptionalDouble;
import java.util.function.Consumer;
import java.util.AbstractCollection;
import java.util.List;
import java.util.ArrayList;

public class ParameterBuilder
{
  Node parent;
  String name;
  String type;
  List<Consumer<Parameter>> funcs = new ArrayList<>();
  public ParameterBuilder(Node p)
  {
    parent = p;
  }

  public ParameterBuilder name(String s)
  {
    name = s;
    return this;
  }

  public ParameterBuilder type(String s)
  {
    type = s;
    return this;
  }

  public ParameterBuilder description(String s)
  {
    funcs.add((Parameter p) -> p.setDescription(s));
    return this;
  }

  public ParameterBuilder domain(Domain s)
  {
    funcs.add((Parameter p) -> p.setDomain(s));
    return this;
  }
  public ParameterBuilder domain(int min, int max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(float min, float max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(double min, double max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(Vec2F min, Vec2F max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(Vec3F min, Vec3F max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(Vec4F min, Vec4F max)
  {
  funcs.add((Parameter p) -> p.setDomain(min, max));
  return this;
  }
  public ParameterBuilder domain(int[] v)
  {
  funcs.add((Parameter p) -> p.setDomain(v));
  return this;
  }
  public ParameterBuilder domain(float[] v)
  {
  funcs.add((Parameter p) -> p.setDomain(v));
  return this;
  }
  public ParameterBuilder domain(String[] v)
  {
  funcs.add((Parameter p) -> p.setDomain(v));
  return this;
  }
  public ParameterBuilder domain(Value[] v)
  {
  funcs.add((Parameter p) -> p.setDomain(v));
  return this;
  }

  public ParameterBuilder unit(String v)
  {
    funcs.add((Parameter p) -> p.setUnit(v));
    return this;
  }
  public ParameterBuilder access(int v)
  {
    funcs.add((Parameter p) -> p.setAccessMode(v));
    return this;
  }
  public ParameterBuilder bounding(int v)
  {
    funcs.add((Parameter p) -> p.setBoundingMode(v));
    return this;
  }
  public ParameterBuilder critical(boolean b)
  {
    funcs.add((Parameter p) -> p.setCritical(true));
    return this;
  }
  public ParameterBuilder repetitionFilter(boolean b)
  {
    funcs.add((Parameter p) -> p.setRepetitionFilter(true));
    return this;
  }
  public ParameterBuilder refreshRate(int b)
  {
    funcs.add((Parameter p) -> p.setRefreshRate(b));
    return this;
  }
  public ParameterBuilder priority(double b)
  {
    funcs.add((Parameter p) -> p.setPriority(b));
    return this;
  }
  public ParameterBuilder valueStepSize(double b)
  {
    funcs.add((Parameter p) -> p.setValueStepSize(b));
    return this;
  }
  public ParameterBuilder defaultValue(Value b)
  {
    funcs.add((Parameter p) -> p.setDefaultValue(b));
    return this;
  }

  public ParameterBuilder value(Value v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value()
  { funcs.add((Parameter p) -> p.push_impulse()); return this; }
  public ParameterBuilder value(int v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(float v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(double v)
  { funcs.add((Parameter p) -> p.push((float)v)); return this; }
  public ParameterBuilder value(boolean v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(byte v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(String v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(float v1, float v2)
  { funcs.add((Parameter p) -> p.push(v1, v2)); return this; }
  public ParameterBuilder value(float v1, float v2, float v3)
  { funcs.add((Parameter p) -> p.push(v1, v2, v3)); return this; }
  public ParameterBuilder value(float v1, float v2, float v3, float v4)
  { funcs.add((Parameter p) -> p.push(v1, v2, v3, v4)); return this; }
  public ParameterBuilder value(Vec2F v)
  { funcs.add((Parameter p) -> p.push(v.x, v.y)); return this; }
  public ParameterBuilder value(Vec3F v)
  { funcs.add((Parameter p) -> p.push(v.x, v.y, v.z)); return this; }
  public ParameterBuilder value(Vec4F v)
  { funcs.add((Parameter p) -> p.push(v.x, v.y, v.z, v.w)); return this; }
  public ParameterBuilder value(int[] v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(float[] v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public ParameterBuilder value(Value[] v)
  { funcs.add((Parameter p) -> p.push(v)); return this; }
  public Parameter get()
  {
    if(name.isEmpty())
      return null;
    if(type.isEmpty())
      return null;

    Parameter p = parent.create(name, type);
    for(Consumer func : funcs)
    {
      func.accept(p);
    }
    return p;
  }
}
