using System.Runtime;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System;
using UnityEngine;
namespace Ossia
{
	public class ValueFactory
	{
		static public Value createString(string v)
		{
			return new Value(Network.ossia_value_create_string(v));
		}
		static public Value createInt(int v)
		{
			return new Value(Network.ossia_value_create_int(v));
		}
		static public Value createFloat(float v)
		{
			return new Value(Network.ossia_value_create_float(v));
		}
		static public Value createBool(bool v)
		{
			return new Value(Network.ossia_value_create_bool(v ? 1 : 0));
		}
		static public Value createChar(char v)
		{
			return new Value(Network.ossia_value_create_char(v));
		}

		static public Value createFromObject(object obj)
		{
			if (obj is int)
			{
				return createInt ((int)obj);
			}
			else if (obj is bool)
			{
				return createBool ((bool)obj);
			}
			else if (obj is float)
			{
				return createFloat ((float)obj);
			}
			else if (obj is char)
			{
				return createChar ((char)obj);
			}
			else if (obj is string)
			{
				return createString ((string)obj);
			}

			throw new Exception("unimplemented type");
		}
	}
	public class Value : IDisposable
	{
		internal IntPtr ossia_value = IntPtr.Zero;
		bool disposed = false;

		internal protected Value(IntPtr v)
		{
			ossia_value = v;
		}

		public Value()
		{
			ossia_value = Network.ossia_value_create_impulse();
		}
		public Value(bool v)
		{
			ossia_value = Network.ossia_value_create_bool(v ? 1 : 0);
		}
		public Value(int v)
		{
			ossia_value = Network.ossia_value_create_int(v);
		}
		public Value(float v)
		{
			ossia_value = Network.ossia_value_create_float(v);
		}
		public Value(float v, float v2)
		{
			ossia_value = Network.ossia_value_create_2f(v, v2);
		}
		public Value(float v, float v2, float v3)
		{
			ossia_value = Network.ossia_value_create_3f(v, v2, v3);
		}
		public Value(float v, float v2, float v3, float v4)
		{
			ossia_value = Network.ossia_value_create_4f(v, v2, v3, v4);
		}
		public Value(char v)
		{
			ossia_value = Network.ossia_value_create_char(v);
		}
		public Value(string v)
		{
			ossia_value = Network.ossia_value_create_string(v);
		}

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		protected virtual void Dispose(bool disposing)
		{
			if (disposed)
				return;

			if (disposing) {
				//Free(); TODO memleak
			}

			disposed = true;
		}

		public void Free()
		{
			Network.ossia_value_free (ossia_value);
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
		public unsafe string GetString()
		{
			IntPtr str = IntPtr.Zero;
			UIntPtr sz = UIntPtr.Zero;
			Network.ossia_value_to_byte_array(ossia_value, out str, out sz);
			string s = Marshal.PtrToStringAnsi (str, (int) sz);
			Network.ossia_string_free (str);
			return s;
		}

		public IntPtr GetValue() {
			return ossia_value;
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
