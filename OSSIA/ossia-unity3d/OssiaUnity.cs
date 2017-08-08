using UnityEngine;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System;

namespace Ossia
{
	public enum ossia_type
	{
		FLOAT,
		INT,
		VEC2F,
		VEC3F,
		VEC4F,
		IMPULSE,
		BOOL,
		STRING,
		TUPLE,
		CHAR
	}

	public enum ossia_access_mode
	{
		BI, 
		GET, 
		SET
	}

	public enum ossia_bounding_mode
	{
		FREE,
		CLIP,
		WRAP,
		FOLD,
		LOW,
		HIGH
	}

	public delegate void ValueCallbackDelegate(Ossia.Value t);


	[System.AttributeUsage(System.AttributeTargets.All)]
	public class Expose : System.Attribute
	{
		public string ExposedName;

		public Expose(string name)
		{
			this.ExposedName = name;
		}
	}

}