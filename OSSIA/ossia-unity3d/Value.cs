using System.Runtime;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System;

namespace Ossia
{
  public class Value
  {
    internal IntPtr ossia_value = IntPtr.Zero;

    internal protected Value(IntPtr v)
    {
      ossia_value = v;
    }

    ~Value()
    {
      Network.ossia_value_free(ossia_value);
    }

    public Value()
    { ossia_value = Network.ossia_value_create_impulse(); }
    public Value(bool v)
    { ossia_value = Network.ossia_value_create_bool(v ? 1 : 0); }
    public Value(int v)
    { ossia_value = Network.ossia_value_create_int(v); }
    public Value(float v)
    { ossia_value = Network.ossia_value_create_float(v); }
    public Value(float v, float v2)
    { ossia_value = Network.ossia_value_create_2f(v, v2); }
    public Value(float v, float v2, float v3)
    { ossia_value = Network.ossia_value_create_3f(v, v2, v3); }
    public Value(float v, float v2, float v3, float v4)
    { ossia_value = Network.ossia_value_create_4f(v, v2, v3, v4); }
    public Value(char v)
    { ossia_value = Network.ossia_value_create_char(v); }
    public Value(string v)
    { ossia_value = Network.ossia_value_create_string(v); }
    public Value(IList<float> v)
    {
      var arr = v as float[] ?? v.ToArray();
      ossia_value = Network.ossia_value_create_fn(arr, new UIntPtr((ulong)arr.Length));
    }
    public Value(IList<int> v)
    {
      var arr = v as int[] ?? v.ToArray();
      ossia_value = Network.ossia_value_create_in(arr, new UIntPtr((ulong)arr.Length));
    }
    public Value(IList<Value> v)
    {
      IntPtr[] arr = new IntPtr[v.Count()];
      int i = 0;
      foreach(Value val in v)
        arr[i++] = val.ossia_value;
      ossia_value = Network.ossia_value_create_tuple(arr, new UIntPtr((ulong)arr.Length));
    }

    public Value(object obj)
    {
      if (obj is int)
      {
        ossia_value = Network.ossia_value_create_int((int) obj);
      }
      else if (obj is bool)
      {
        ossia_value = Network.ossia_value_create_bool(((bool) obj) ? 1 : 0);
      }
      else if (obj is float)
      {
        ossia_value = Network.ossia_value_create_float((float) obj);
      }
      else if (obj is char)
      {
        ossia_value = Network.ossia_value_create_char((char) obj);
      }
      else if (obj is string)
      {
        ossia_value = Network.ossia_value_create_string((string) obj);
      }

      throw new Exception("unimplemented type");
    }

    public ossia_type GetOssiaType()
    {
      return Network.ossia_value_get_type(ossia_value);
    }

    public int GetInt()
    {
      return Network.ossia_value_to_int(ossia_value);
    }
    public bool GetBool()
    {
      return Network.ossia_value_to_bool(ossia_value);
    }
    public float GetFloat()
    {
      return Network.ossia_value_to_float(ossia_value);
    }
    public vec2f GetVec2f()
    {
      return Network.ossia_value_to_2f(ossia_value);
    }
    public vec3f GetVec3f()
    {
      return Network.ossia_value_to_3f(ossia_value);
    }
    public vec4f GetVec4f()
    {
      return Network.ossia_value_to_4f(ossia_value);
    }
    public char GetChar()
    {
      return Network.ossia_value_to_char(ossia_value);
    }

    object Get()
    {
      ossia_type t = GetOssiaType ();
      switch (t) { 
      case ossia_type.FLOAT:
        return GetFloat ();
      case ossia_type.INT:
        return GetInt ();
      case ossia_type.VEC2F:
        return GetVec2f ();
      case ossia_type.VEC3F:
        return GetVec3f ();
      case ossia_type.VEC4F:
        return GetVec4f ();
      case ossia_type.BOOL:
        return GetBool ();
      case ossia_type.STRING:
        return GetString ();
      case ossia_type.TUPLE:
        return GetTuple ();
      case ossia_type.CHAR:
        return GetChar ();
      }
      return null;
    }

    public int[] GetIntArray()
    {
      IntPtr data = IntPtr.Zero;
      UIntPtr sz = UIntPtr.Zero;
      Network.ossia_value_to_in(ossia_value, out data, out sz);

      int[] v = new int[(int) sz];
      Marshal.Copy (data, v, 0, (int)sz);

      Network.ossia_value_free_in (data);
      return v;
    }

    public float[] GetFloatArray()
    {
      IntPtr data = IntPtr.Zero;
      UIntPtr sz = UIntPtr.Zero;
      Network.ossia_value_to_fn(ossia_value, out data, out sz);

      float[] v = new float[(int) sz];
      Marshal.Copy (data, v, 0, (int)sz);

      Network.ossia_value_free_fn (data);
      return v;
    }
    public unsafe Value[] GetTuple()
    {
      IntPtr data = IntPtr.Zero;
      UIntPtr sz = UIntPtr.Zero;
      Network.ossia_value_to_fn(ossia_value, out data, out sz);

      Value[] v = new Value[(int) sz];
      IntPtr* ptr = (IntPtr*)data.ToPointer ();
      for (int i = 0; i < (int)sz; i++) {
        IntPtr p = *ptr;
        v [i] = new Value (p);
        ptr = ptr + Marshal.SizeOf (sz);
      }

      Network.ossia_value_free_fn (data);
      return v;
    }

    public string GetString()
    {
      IntPtr str = IntPtr.Zero;
      UIntPtr sz = UIntPtr.Zero;
      Network.ossia_value_to_byte_array(ossia_value, out str, out sz);

      string s = Marshal.PtrToStringAnsi (str, (int) sz);

      Network.ossia_string_free (str);
      return s;
    }

    static public ossia_type ObjectToOssiaType(object obj)
    {
      if (obj is int)
      {
        return ossia_type.INT;
      }
      else if (obj is bool)
      {
        return ossia_type.BOOL;
      }
      else if (obj is float)
      {
        return ossia_type.FLOAT;
      }
      else if (obj is char)
      {
        return ossia_type.CHAR;
      }
      else if (obj is string)
      {
        return ossia_type.STRING;
      }

      throw new Exception("unimplemented type");
    }

    static public ossia_type TypeToOssia<T>(T obj)
    {
      if (obj is int)
      {
        return ossia_type.INT;
      }
      else if (obj is bool)
      {
        return ossia_type.BOOL;
      }
      else if (obj is float)
      {
        return ossia_type.FLOAT;
      }
      else if (obj is char)
      {
        return ossia_type.CHAR;
      }
      else if (obj is string)
      {
        return ossia_type.STRING;
      }

      throw new Exception("unimplemented type" + obj.GetType());
    }


    static public ossia_type TypeToOssia2(Type obj)
    {
      if (obj == typeof(System.Int32))
      {
        return ossia_type.INT;
      }
      else if (obj == typeof(System.Boolean))
      {
        return ossia_type.BOOL;
      }
      else if (obj == typeof(System.Single))
      {
        return ossia_type.FLOAT;
      }
      else if (obj == typeof(System.Char))
      {
        return ossia_type.CHAR;
      }
      else if (obj == typeof(System.String))
      {
        return ossia_type.STRING;
      }

      throw new Exception("unimplemented type" + obj.GetType());
    }

    public object ToObject()
    {
      switch (GetOssiaType ()) {
      case ossia_type.INT:
        return GetInt ();
      case ossia_type.FLOAT:
        return GetFloat ();
      case ossia_type.BOOL:
        return GetBool ();
      case ossia_type.CHAR:
        return GetChar ();
      case ossia_type.STRING:
        return GetString ();
      default:
        throw new Exception("unimplemented type");
      }
    }

  }
}
