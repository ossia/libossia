using System;
using System.Reflection;
using System.Collections.Generic;
using UnityEngine;

namespace Ossia
{
	//! TODO implement this so that all the public fields of 
	//! the given MonoBehaviour are exposed through ossia
	public class ExposeComponent : MonoBehaviour
	{
		public MonoBehaviour component;
	}
}