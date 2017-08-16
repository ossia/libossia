using System.Collections.Generic;
using System.Runtime;
using System.Runtime.InteropServices;
using System;

namespace Ossia
{
	public class Address
	{
		internal IntPtr ossia_address = IntPtr.Zero;

		List<ValueCallbackDelegate> callbacks;

		Network.ossia_value_callback value_cb;
		internal IntPtr ossia_callback_it = IntPtr.Zero;

		Network.ossia_address_callback addr_remove_callback;
		IntPtr addr_ossia_remove_callback;

		private void CleanupCallback(IntPtr addr)
		{
			if (ossia_address != IntPtr.Zero && ossia_address == addr && addr_ossia_remove_callback != IntPtr.Zero) {
				IntPtr node = Network.ossia_address_get_node (ossia_address);
				IntPtr dev = Network.ossia_node_get_device(node);
				Network.ossia_device_remove_address_deleting_callback (dev, addr_ossia_remove_callback);

				ossia_address = IntPtr.Zero;
				addr_remove_callback = null;
				ossia_callback_it = IntPtr.Zero;
			}
		}

		~Address()
		{
			CleanupCallback (ossia_address);
		}

		internal Address(IntPtr address)
		{
			callbacks = new List<ValueCallbackDelegate> ();
			ossia_address = address;

			IntPtr node = Network.ossia_address_get_node (ossia_address);
			IntPtr dev = Network.ossia_node_get_device(node);
			addr_remove_callback = new Network.ossia_address_callback((IntPtr ctx, IntPtr addr) => CleanupCallback(addr));
			IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate (addr_remove_callback);
			addr_ossia_remove_callback = Network.ossia_device_add_address_deleting_callback(dev, intptr_delegate, (IntPtr)0);
		}

		public void SetAccessMode(ossia_access_mode m)
		{ Network.ossia_address_set_access_mode (ossia_address, m); }
		public ossia_access_mode GetAccessMode()
		{ return Network.ossia_address_get_access_mode (ossia_address); }

		public void SetBoundingMode(ossia_bounding_mode m)
		{ Network.ossia_address_set_bounding_mode (ossia_address, m); }
		public ossia_bounding_mode GetBoundingMode()
		{ return Network.ossia_address_get_bounding_mode (ossia_address); }


        public Value FetchValue()
        { return new Value (Network.ossia_address_fetch_value (ossia_address)); }
		public Value GetValue()
       { return new Value (Network.ossia_address_get_value (ossia_address)); }

		public void SetValue(Value val)
		{ Network.ossia_address_set_value (ossia_address, val.ossia_value); }

		public void PushValue(Value val)
		{ Network.ossia_address_push_value (ossia_address, val.ossia_value); }
		public void PushImpulse()
		{ Network.ossia_address_push_impulse (ossia_address); }
		public void PushValue(int val)
		{ Network.ossia_address_push_i (ossia_address, val); }
		public void PushValue(float val)
		{ Network.ossia_address_push_f (ossia_address, val); }
		public void PushValue(float v1, float v2)
		{ Network.ossia_address_push_2f (ossia_address, v1, v2); }
		public void PushValue(float v1, float v2, float v3)
		{ Network.ossia_address_push_3f (ossia_address, v1, v2, v3); }
		public void PushValue(float v1, float v2, float v3, float v4)
		{ Network.ossia_address_push_4f (ossia_address, v1, v2, v3, v4); }
		public void PushValue(char val)
		{ Network.ossia_address_push_c (ossia_address, val); }
		public void PushValue(bool val)
		{ Network.ossia_address_push_b (ossia_address, val ? 1 : 0); }
		public void PushValue(string val)
		{ Network.ossia_address_push_s (ossia_address, val); }


		private void DoNothing(Value v)
		{
		}


		public void SetValueUpdating(bool b)
		{
			if (b) {
				AddCallback (DoNothing);
			} else {
				RemoveCallback (DoNothing);
			}
		}

		public void AddCallback(ValueCallbackDelegate callback)
		{
			if (callbacks.Count == 0) {
				// We initialize the callback structure.
			    value_cb = new Network.ossia_value_callback ((IntPtr ctx, IntPtr p) => CallbackWrapper(this, p));
				IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate (value_cb);
				ossia_callback_it = Network.ossia_address_add_callback(ossia_address, intptr_delegate, (IntPtr)0);
			}
			callbacks.Add (callback);
		}

		static public void CallbackWrapper(Address self, IntPtr value)
		{
			Ossia.Value val = new Ossia.Value (value);
			foreach(var cb in self.callbacks)
			{
				cb (val);
			}
		}

		public void RemoveCallback(ValueCallbackDelegate c)
		{
			callbacks.RemoveAll(x => x == c);
			if (callbacks.Count == 0) {
				Network.ossia_address_remove_callback (ossia_address, ossia_callback_it);
			}
		}

		public Value GetMin()
		{
			IntPtr dom = Network.ossia_address_get_domain(ossia_address);
			var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
			Network.ossia_domain_free(dom);
			return v;
		}
		public Value GetMax()
		{
			IntPtr dom = Network.ossia_address_get_domain(ossia_address);
			var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
			Network.ossia_domain_free(dom);
			return v;
		}
		public Domain GetDomain()
		{
			IntPtr dom = Network.ossia_address_get_domain(ossia_address);
			var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
			var v2 = new Ossia.Value(Network.ossia_domain_get_max(dom));
			Network.ossia_domain_free(dom);
			return new Domain(v, v2);
		}
		public void SetMin(Value v)
		{
			IntPtr dom = Network.ossia_address_get_domain(ossia_address);
			Network.ossia_domain_set_min(dom, v.ossia_value);
			Network.ossia_address_set_domain(ossia_address, dom);
			Network.ossia_domain_free(dom);
		}
		public void SetMax(Value v)
		{
			IntPtr dom = Network.ossia_address_get_domain(ossia_address);
			Network.ossia_domain_set_max(dom, v.ossia_value);
			Network.ossia_address_set_domain(ossia_address, dom);
			Network.ossia_domain_free(dom);
		}
	}
}

