using UnityEngine;
using System.Runtime;
using System.Runtime.InteropServices;
using System;
using System.Collections;
using Ossia;

namespace Ossia
{
  public class Controller : MonoBehaviour
  {
    public string appName = "Unity";

    bool set = false;

    Ossia.Local local_protocol = null;
    Ossia.Device local_device = null;
    Ossia.OSCQuery oscq_protocol = null;

    Ossia.Node scene_node;
    Ossia.Network main;

    public delegate void debug_log_delegate (string str);

    debug_log_delegate callback_delegate = null;

    static void DebugLogCallback (string str)
    {
      Debug.Log ("OSSIA : " + str);
    }

    public static Controller Get()
    {
      GameObject controller = GameObject.Find ("OssiaController");
      if (controller == null) {
        throw new Exception ("Controller GameObject not found");
      }
      var dev = controller.GetComponent<Ossia.Controller> ();
      if (dev == null) {
        throw new Exception ("Controller component not found");
      }
      return dev;
    }

    void Awake ()
    {
      Debug.Log ("OSSIA: Starting");
      if (!set) {
        set = true;

        // Setup the log so that the errors in the C API are shown in the
        // Unity3D console
        callback_delegate = new debug_log_delegate (DebugLogCallback);

        // Convert callback_delegate into a function pointer that can be
        // used in unmanaged code.
        IntPtr intptr_delegate = 
          Marshal.GetFunctionPointerForDelegate (callback_delegate);

        // Call the API passing along the function pointer.
        Ossia.Network.ossia_set_debug_logger (intptr_delegate);

        local_protocol = new Ossia.Local ();
        local_device = new Ossia.Device (local_protocol, "unity");
        scene_node = local_device.GetRootNode ().AddChild ("scene");

        oscq_protocol = new Ossia.OSCQuery (1234, 5678);
        local_protocol.ExposeTo (oscq_protocol);
      }
    }

    public Ossia.Node SceneNode ()
    {
      return scene_node; 
    }

    void OnApplicationQuit ()
    {
      Debug.Log ("OSSIA: Quitting");
      Network.ossia_device_reset_static ();
    }

    public Ossia.Device GetDevice ()
    {
      return local_device;
    }
  }
}
