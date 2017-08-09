using UnityEngine;
using System.Runtime;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System;

namespace Ossia {
	public class Node : IDisposable
	{
		internal IntPtr ossia_node = IntPtr.Zero;
		Address ossia_address = null;
		bool disposed = false;
		bool updating = false;
		List<Node> children = new List<Node>();

		public Node(IntPtr n)
		{
			ossia_node = n;
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
				Free();
			}

			disposed = true;
		}

		public string GetName()
		{
			IntPtr nameptr = Network.ossia_node_get_name (ossia_node);
			if (nameptr == IntPtr.Zero)
				return "ENONAME";
			string name = Marshal.PtrToStringAnsi (nameptr);
			Network.ossia_string_free(nameptr);
			return name;
		}


		public Node AddChild (string name)
		{
			var cld = Network.ossia_node_add_child (ossia_node, name);
			if (cld != IntPtr.Zero) {
				var node = new Node (cld);
				children.Add (node);
				return node;
			}
			return null;
		}

		public void RemoveChild(Node child)
		{
			if (child != null) {
				Network.ossia_node_remove_child (ossia_node, child.ossia_node);
				children.Remove (child);
				child.Free ();
			}
		}

		public void Free()
		{
			//Network.ossia_node_free (ossia_node);
		}

		public int ChildSize()
		{
			return children.Count;
		}

		public Node GetChild(int child)
		{
			return children[child];
		}

		public Address CreateAddress(ossia_type type)
		{
			if(ossia_address == null)
				ossia_address = new Address (Network.ossia_node_create_address (ossia_node, type));
			return ossia_address;
		}

		public void RemoveAddress()
		{
			Network.ossia_node_remove_address (ossia_node, ossia_address.ossia_address);
			ossia_address = null;
		}

		public IntPtr GetNode() {
			return ossia_node;
		}

		public Ossia.Address GetAddress() {
			Debug.LogError (GetName());
			Debug.LogError (ossia_address);
			return ossia_address;
		}

		public bool GetValueUpdating()
		{
			return updating;
		}
		public void SetValueUpdating(bool b)
		{
			if(ossia_address != null)
			{
				ossia_address.SetValueUpdating (b);
			}

			for (int i = 0; i < ChildSize (); i++) {
				Node child = GetChild (i);
				child.SetValueUpdating (b);
			}

			updating = b;
		}

		public static Node CreateNode(Node root, string s)
		{
			IntPtr p = ossia_node_create(root.ossia_node, s);
			return new Node(p);
		}
		public static Node Find(Node root, string s)
		{
			IntPtr p = ossia_node_find(root.ossia_node, s);
			return new Node(p);
		}

	}
}
