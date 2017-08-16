using UnityEngine;
using System.Collections;
using Namespace;
using Ossia;
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

unsafe public class PresetController : MonoBehaviour {
	
	public string jsonname = "Assets/preset.json";
	public Preset p;

	Vector3 getVector(Ossia.Node node, Vector3 basevec) {
		Vector3 result = new Vector3 (basevec.x, basevec.y, basevec.z);

		string rootname = "/" + node.GetName ();
		IntPtr leafptr;
		if (BlueYetiAPI.ossia_devices_get_child(node.GetNode(), rootname + "/x", &leafptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
			IntPtr addrptr;
			if (BlueYetiAPI.ossia_devices_get_node_address(leafptr, &addrptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
				Ossia.Address addr = new Ossia.Address(addrptr);
				result.x = addr.GetValue ().GetFloat ();
			}
		}
		if (BlueYetiAPI.ossia_devices_get_child(node.GetNode(), rootname + "/y", &leafptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
			IntPtr addrptr;
			if (BlueYetiAPI.ossia_devices_get_node_address(leafptr, &addrptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
				Ossia.Address addr = new Ossia.Address(addrptr);
				result.y = addr.GetValue ().GetFloat ();
			}
		}
		if (BlueYetiAPI.ossia_devices_get_child(node.GetNode(), rootname + "/z", &leafptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
			IntPtr addrptr;
			if (BlueYetiAPI.ossia_devices_get_node_address(leafptr, &addrptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
				Ossia.Address addr = new Ossia.Address(addrptr);
				result.z = addr.GetValue ().GetFloat ();
			}
		}
		return result;	
	}

	void createCube(Ossia.Node node) {

		/// Create a cube \\\

		Debug.Log ("Creating " + node.GetName ());
		GameObject createdgo = GameObject.CreatePrimitive (PrimitiveType.Cube);
		createdgo.name = node.GetName ();
		createdgo.AddComponent<AssemblyCSharp.OssiaObject> ();

		/// Import parameters \\\

		for (int i = 0; i < node.ChildSize(); ++i) {
			Ossia.Node child = node.GetChild (i);
			switch (child.GetName ()) {
			case "position":
				createdgo.transform.position = getVector (child, createdgo.transform.position);
				break;
			case "rotation":
				createdgo.transform.rotation = Quaternion.Euler(getVector (child, createdgo.transform.eulerAngles));
				break;
			case "scale":
				createdgo.transform.localScale = getVector (child, createdgo.transform.localScale);
				break;
			default:
				break;
			}
		}

		/// Remove the node containing the parameters, now useless \\\

		GameObject controller = GameObject.Find ("OssiaController");
		var dev = controller.GetComponent<Ossia.Controller> ();
		var local_device = dev.GetDevice ();
		local_device.GetRootNode().GetChild(0).RemoveChild(node);
	}

	// Use this for initialization
	void Start () {
			 
		string jsontext = System.IO.File.ReadAllText (jsonname);
		Debug.Log (jsontext);

		p = new Preset (jsontext);

		try {
			Debug.Log("Loaded preset " + p.ToString() + " (" + p.Size() + " elements)");
		}
		catch (Exception e) {
			Debug.Log (e.Message);
		}

		try {

			GameObject controller = GameObject.Find ("OssiaController");
			if (controller == null) {
				throw new Exception("Controller not found");
			}

			var dev = controller.GetComponent<Ossia.Controller> ();
			if (dev == null) {
				throw new Exception("Device is null");
			}
				
			Ossia.Device local_device = dev.GetDevice();
			if (local_device == null) {
				throw new Exception("LocalDevice is null");
			}

			IntPtr dev_ptr = local_device.GetDevice();
			if(dev_ptr == IntPtr.Zero) {
				throw new Exception("DevPtr is null");
			}

			IntPtr res;
			BlueYetiAPI.ossia_devices_to_string(dev_ptr, &res);
			Debug.Log("Before applying: " + Marshal.PtrToStringAuto(res));

			p.ApplyToDevice(local_device, false);
			BlueYetiAPI.ossia_devices_to_string(dev_ptr, &res);
			Debug.Log("After applying: " + Marshal.PtrToStringAuto(res));

			/// Building instances as Unity objects from values imported in the device \\\

			bool endOfInstances = false;
			int currentInstance = 1;
			int numberOfInstances = 2;
			while (!endOfInstances && currentInstance <= numberOfInstances) {
				string currentIntanceKeys = "/" + local_device.GetName() + "/scene/cube." + currentInstance.ToString();
				try {
					Debug.Log("Fetching node " + currentIntanceKeys);
					IntPtr fetchednode; 
					ossia_preset_result_enum code = BlueYetiAPI.ossia_devices_get_node(local_device.GetDevice(), currentIntanceKeys, &fetchednode);
					if (code != ossia_preset_result_enum.OSSIA_PRESETS_OK) {
						Debug.Log(code);
						endOfInstances = true;
					}
					else {
						createCube(new Ossia.Node(fetchednode));
					}
				}
				catch (Exception e) {
					Debug.Log("Error occured: " + e.Message);
					endOfInstances = true;
				}
				++currentInstance;
			}
		}
		catch (Exception e) {
			Debug.Log (e.Message);
		}
	}

	// Update is called once per frame
	void Update () {

		/// Print device when Space bar is pressed \\\

		GameObject controller = GameObject.Find ("OssiaController");
		var dev = controller.GetComponent<Ossia.Controller> ();
		Ossia.Device local_device = dev.GetDevice();
		IntPtr dev_ptr = local_device.GetDevice();
		if (Input.GetKeyDown(KeyCode.Space)) {
			IntPtr strptr;
			if (BlueYetiAPI.ossia_devices_to_string (dev_ptr, &strptr) == ossia_preset_result_enum.OSSIA_PRESETS_OK) {
				Debug.Log (Marshal.PtrToStringAuto (strptr));
			} else {
				Debug.Log ("can't display device");
			}
		}
	}

	void OnApplicationQuit() {
		p.Free ();
	}
}
