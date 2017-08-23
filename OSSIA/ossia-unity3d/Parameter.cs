using System.Collections.Generic;
using System.Runtime;
using System.Runtime.InteropServices;
using System;
using UnityEngine;

namespace Ossia
{
  public class Parameter
  {
    internal IntPtr ossia_parameter = IntPtr.Zero;

    List<ValueCallbackDelegate> callbacks;

    Network.ossia_value_callback value_cb;
    internal IntPtr ossia_callback_it = IntPtr.Zero;

    Network.ossia_parameter_callback addr_remove_callback;
    IntPtr addr_ossia_remove_callback;

    private void CleanupCallback(IntPtr addr)
    {
      if (ossia_parameter != IntPtr.Zero && ossia_parameter == addr && addr_ossia_remove_callback != IntPtr.Zero) {
        IntPtr node = Network.ossia_parameter_get_node (ossia_parameter);
        IntPtr dev = Network.ossia_node_get_device(node);
        Network.ossia_device_remove_parameter_deleting_callback (dev, addr_ossia_remove_callback);

        ossia_parameter = IntPtr.Zero;
        addr_remove_callback = null;
        ossia_callback_it = IntPtr.Zero;
      }
    }

    ~Parameter()
    {
      CleanupCallback (ossia_parameter);
    }

    internal Parameter(IntPtr address)
    {
      callbacks = new List<ValueCallbackDelegate> ();
      ossia_parameter = address;

      IntPtr node = Network.ossia_parameter_get_node (ossia_parameter);
      IntPtr dev = Network.ossia_node_get_device(node);
      addr_remove_callback = new Network.ossia_parameter_callback((IntPtr ctx, IntPtr addr) => CleanupCallback(addr));
      IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate (addr_remove_callback);
      addr_ossia_remove_callback = Network.ossia_device_add_parameter_deleting_callback(dev, intptr_delegate, (IntPtr)0);
    }

    public void SetAccessMode(ossia_access_mode m)
    { Network.ossia_parameter_set_access_mode (ossia_parameter, m); }
    public ossia_access_mode GetAccessMode()
    { return Network.ossia_parameter_get_access_mode (ossia_parameter); }

    public void SetBoundingMode(ossia_bounding_mode m)
    { Network.ossia_parameter_set_bounding_mode (ossia_parameter, m); }
    public ossia_bounding_mode GetBoundingMode()
    { return Network.ossia_parameter_get_bounding_mode (ossia_parameter); }


    public Value FetchValue()
    { return new Value (Network.ossia_parameter_fetch_value (ossia_parameter)); }
    public Value GetValue()
    { return new Value (Network.ossia_parameter_get_value (ossia_parameter)); }

    public void SetValue(Value val)
    { Network.ossia_parameter_set_value (ossia_parameter, val.ossia_value); }

    public void PushValue(Value val)
    { Network.ossia_parameter_push_value (ossia_parameter, val.ossia_value); }
    public void PushImpulse()
    { Network.ossia_parameter_push_impulse (ossia_parameter); }
    public void PushValue(int val)
    { Network.ossia_parameter_push_i (ossia_parameter, val); }
    public void PushValue(float val)
    { Network.ossia_parameter_push_f (ossia_parameter, val); }
    public void PushValue(float v1, float v2)
    { Network.ossia_parameter_push_2f (ossia_parameter, v1, v2); }
    public void PushValue(float v1, float v2, float v3)
    { Network.ossia_parameter_push_3f (ossia_parameter, v1, v2, v3); }
    public void PushValue(float v1, float v2, float v3, float v4)
    { Network.ossia_parameter_push_4f (ossia_parameter, v1, v2, v3, v4); }
    public void PushValue(char val)
    { Network.ossia_parameter_push_c (ossia_parameter, val); }
    public void PushValue(bool val)
    { Network.ossia_parameter_push_b (ossia_parameter, val ? 1 : 0); }
    public void PushValue(string val)
    { Network.ossia_parameter_push_s (ossia_parameter, val); }
    public void PushValue(float[] val)
    { Network.ossia_parameter_push_fn (ossia_parameter, val, val.Length); }
    public void PushValue(int[] val)
    { Network.ossia_parameter_push_in (ossia_parameter, val, val.Length); }


    private void DoNothing(Value v)
    {
      Debug.Log (v.ConvertString());
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
        ossia_callback_it = Network.ossia_parameter_add_callback(ossia_parameter, intptr_delegate, (IntPtr)0);
      }
      callbacks.Add (callback);
    }

    static public void CallbackWrapper(Parameter self, IntPtr value)
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
        Network.ossia_parameter_remove_callback (ossia_parameter, ossia_callback_it);
      }
    }

    public Value GetMin()
    {
      IntPtr dom = Network.ossia_parameter_get_domain(ossia_parameter);
      var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
      Network.ossia_domain_free(dom);
      return v;
    }
    public Value GetMax()
    {
      IntPtr dom = Network.ossia_parameter_get_domain(ossia_parameter);
      var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
      Network.ossia_domain_free(dom);
      return v;
    }
    public Domain GetDomain()
    {
      IntPtr dom = Network.ossia_parameter_get_domain(ossia_parameter);
      var v = new Ossia.Value(Network.ossia_domain_get_min(dom));
      var v2 = new Ossia.Value(Network.ossia_domain_get_max(dom));
      Network.ossia_domain_free(dom);
      return new Domain(v, v2);
    }
    public void SetMin(Value v)
    {
      IntPtr dom = Network.ossia_parameter_get_domain(ossia_parameter);
      Network.ossia_domain_set_min(dom, v.ossia_value);
      Network.ossia_parameter_set_domain(ossia_parameter, dom);
      Network.ossia_domain_free(dom);
    }
    public void SetMax(Value v)
    {
      IntPtr dom = Network.ossia_parameter_get_domain(ossia_parameter);
      Network.ossia_domain_set_max(dom, v.ossia_value);
      Network.ossia_parameter_set_domain(ossia_parameter, dom);
      Network.ossia_domain_free(dom);
    }
  }
}

