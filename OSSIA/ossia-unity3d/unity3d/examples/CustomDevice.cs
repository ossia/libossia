using System.Collections.Generic;
using System.Runtime;
using System.Runtime.InteropServices;
using System;
using UnityEngine;
using UnityEngine.Assertions;

public class CustomDevice : MonoBehaviour
{

	// Use this for initialization
	Ossia.Device local;
	float val = 0;

	void OnApplicationQuit()
	{
		Ossia.Network.ossia_device_reset_static ();
	}

	public delegate void debug_log_delegate(string str);
	void Start ()
	{
		var callback_delegate = new debug_log_delegate ((string str) => Debug.Log("OSSIA : " + str));
		IntPtr intptr_delegate =  Marshal.GetFunctionPointerForDelegate (callback_delegate);
		Ossia.Network.ossia_set_debug_logger (intptr_delegate);

		local = new Ossia.Device (new Ossia.OSCQuery (1234, 5678), "newDevice");

		var root = local.GetRootNode ();
		Assert.AreEqual (root.ChildSize (), 0);

		// Just for tests: 
		{
			// Create a node and an address
			var bar = Ossia.Node.CreateNode (root, "/foo/bar");
			var addr = bar.CreateAddress (Ossia.ossia_type.VEC3F);
			Assert.AreEqual (root.ChildSize (), 1);

			var foo = root.GetChild (0);
			Assert.AreEqual (foo.GetName (), "foo");

			// Try removing the address
			Assert.AreNotEqual (bar.GetNode (), IntPtr.Zero);
			Assert.AreEqual (bar.GetAddress (), addr);

			foo.RemoveChild (foo.GetChild (0));
			Assert.AreEqual (bar.GetNode (), IntPtr.Zero);
			Assert.AreEqual (bar.GetAddress (), null);
		}

		{
			var str = Ossia.Node.CreateNode (root, "/my_string");
			var addr = str.CreateAddress (Ossia.ossia_type.STRING);
			addr.PushValue (Ossia.ValueFactory.createString ("some string !"));
			Debug.Log(addr.GetValue ().GetString ());
		}

		{
			var blu = Ossia.Node.CreateNode (root, "/foo/blu");
			blu.CreateAddress (Ossia.ossia_type.VEC3F);
		}
	}
	
	// Update is called once per frame
	void Update ()
	{
		if (local != null) {
			val += 0.1f;
			var node = Ossia.Node.FindNode (local.GetRootNode (), "/foo/blu");
			node.GetAddress ().PushValue (val, val, val);
		}
	}
}
