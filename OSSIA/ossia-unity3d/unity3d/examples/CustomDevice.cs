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

	void Start ()
	{
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
			var blu = Ossia.Node.CreateNode (root, "/foo/blu");
			var addr = blu.CreateAddress (Ossia.ossia_type.VEC3F);
		}
	}
	
	// Update is called once per frame
	void Update ()
	{
		val += 0.1f;
		var node = Ossia.Node.FindNode (local.GetRootNode (), "/foo/blu");
		node.GetAddress ().PushValue (val, val, val);
	}
}
