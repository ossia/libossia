using System;
using System.Reflection;
using System.Collections.Generic;
using UnityEngine;

namespace Ossia
{
	//! Used to register the position and rotation transforms.
	//! TODO: instead use vec3f & vec4f with relevant units.
	//! This way we only need two addresses.
	public class OssiaTransform
	{
		Ossia.Node pos_node;
		Ossia.Node orient_node;

		Ossia.Node pos_x_node;
		Ossia.Node pos_y_node;
		Ossia.Node pos_z_node;

		Ossia.Address pos_x_addr;
		Ossia.Address pos_y_addr;
		Ossia.Address pos_z_addr;

		Ossia.Node rot_w_node;
		Ossia.Node rot_x_node;
		Ossia.Node rot_y_node;
		Ossia.Node rot_z_node;

		Ossia.Address rot_w_addr;
		Ossia.Address rot_x_addr;
		Ossia.Address rot_y_addr;
		Ossia.Address rot_z_addr;

		public OssiaTransform(GameObject obj, Ossia.Node object_node)
		{
			{
				pos_node = object_node.AddChild ("position");
				pos_x_node = pos_node.AddChild ("x");
				pos_y_node = pos_node.AddChild ("y");
				pos_z_node = pos_node.AddChild ("z");

				pos_x_addr = pos_x_node.CreateAddress (Ossia.ossia_type.FLOAT);
				pos_y_addr = pos_y_node.CreateAddress (Ossia.ossia_type.FLOAT);
				pos_z_addr = pos_z_node.CreateAddress (Ossia.ossia_type.FLOAT);

				pos_x_addr.PushValue (ValueFactory.createFloat (obj.transform.position.x));
				pos_y_addr.PushValue (ValueFactory.createFloat (obj.transform.position.y));
				pos_z_addr.PushValue (ValueFactory.createFloat (obj.transform.position.z));
			}

			{
				orient_node = object_node.AddChild ("rotation");
				rot_w_node = orient_node.AddChild ("w");
				rot_x_node = orient_node.AddChild ("x");
				rot_y_node = orient_node.AddChild ("y");
				rot_z_node = orient_node.AddChild ("z");

				rot_w_addr = rot_w_node.CreateAddress (Ossia.ossia_type.FLOAT);
				rot_x_addr = rot_x_node.CreateAddress (Ossia.ossia_type.FLOAT);
				rot_y_addr = rot_y_node.CreateAddress (Ossia.ossia_type.FLOAT);
				rot_z_addr = rot_z_node.CreateAddress (Ossia.ossia_type.FLOAT);

				rot_w_addr.PushValue (ValueFactory.createFloat (obj.transform.rotation.w));
				rot_x_addr.PushValue (ValueFactory.createFloat (obj.transform.position.x));
				rot_y_addr.PushValue (ValueFactory.createFloat (obj.transform.position.y));
				rot_z_addr.PushValue (ValueFactory.createFloat (obj.transform.position.z));
			}
		}


		public void ReceiveUpdates(GameObject obj)
		{
			{
				var pos = obj.transform.position;

				using (var x_val = pos_x_addr.GetValue ()) {
					if (x_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						pos.x = x_val.GetFloat ();
					}
				}

				using (var y_val = pos_y_addr.GetValue ()) {
					if (y_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						pos.y = y_val.GetFloat ();
					}
				}

				using (var z_val = pos_z_addr.GetValue ()) {
					if (z_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						pos.z = z_val.GetFloat ();
					}
				}

				obj.transform.position = pos;
			}

			{
				var rot = obj.transform.rotation;
				using (var w_val = rot_w_addr.GetValue ()) {
					if (w_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						rot.w = w_val.GetFloat ();
					}
				}

				using (var x_val = rot_x_addr.GetValue ()) {
					if (x_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						rot.x = x_val.GetFloat ();
					}
				}

				using (var y_val = rot_y_addr.GetValue ()) {
					if (y_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						rot.y = y_val.GetFloat ();
					}
				}

				using (var z_val = rot_z_addr.GetValue ()) {
					if (z_val.GetOssiaType () == Ossia.ossia_type.FLOAT) {
						rot.z = z_val.GetFloat ();
					}
				}

				obj.transform.rotation = rot;
			}

		}

		public void SendUpdates(GameObject obj)
		{
			{
				var pos = obj.transform.position;
				using (var val = ValueFactory.createFloat (pos.x)) {
					pos_x_addr.PushValue (val);
				}
				using (var val = ValueFactory.createFloat (pos.y)) {
					pos_y_addr.PushValue (val);
				}
				using (var val = ValueFactory.createFloat (pos.z)) {
					pos_z_addr.PushValue (val);
				}
			}

			{
				var rot = obj.transform.rotation;
				using (var val = ValueFactory.createFloat (rot.w)) {
					rot_w_addr.PushValue (val);
				}
				using (var val = ValueFactory.createFloat (rot.x)) {
					rot_x_addr.PushValue (val);
				}
				using (var val = ValueFactory.createFloat (rot.y)) {
					rot_y_addr.PushValue (val);
				}
				using (var val = ValueFactory.createFloat (rot.z)) {
					rot_z_addr.PushValue (val);
				}
			}
		}
	}

	//! Used to register every field that has an [Ossia.Expose] attached.
	internal class OssiaEnabledParameter
	{
		public OssiaEnabledComponent parent;

		public FieldInfo field;
		public Ossia.Expose attribute;

		public Ossia.Node ossia_node;
		public Ossia.Address ossia_address;

		public OssiaEnabledParameter(FieldInfo f, Ossia.Expose attr)
		{
			field = f;
			attribute = attr;
		}

		public void ReceiveUpdates(GameObject obj)
		{
			using (var val = ossia_address.GetValue ()) {
				try {
					field.SetValue (parent, val.ToObject());
				}
				catch(Exception) {
				}
			}
		}

		public void SendUpdates(GameObject obj)
		{
			using (var val = ValueFactory.createFromObject (field.GetValue(parent.component))) {
				ossia_address.PushValue (val);
			}
		}
	}

	//! A component whose fields have some [Ossia.Expose] attributes
	internal class OssiaEnabledComponent
	{
		public MonoBehaviour component;
		public Ossia.Node component_node;

		public List<OssiaEnabledParameter> parameters;

		public OssiaEnabledComponent(MonoBehaviour comp, Ossia.Node node)
		{
			component = comp;
			component_node = node;
		}

		public void ReceiveUpdates(GameObject obj)
		{
			foreach (var parameter in parameters) {
				parameter.ReceiveUpdates (obj);
			}
		}

		public void SendUpdates(GameObject obj)
		{
			foreach (var parameter in parameters) {
				parameter.SendUpdates (obj);
			}
		}
	}
		
	//! Expose an object through ossia.
	//! Currently this shows only the transform and the fields with [Ossia.Expose].
	public class Object : MonoBehaviour
	{
		public bool ReceiveUpdates = false;
		public bool SendUpdates = false;

		Ossia.Node scene_node;
		Ossia.Node child_node;

		OssiaTransform ossia_transform;

		List<OssiaEnabledComponent> ossia_components = new List<OssiaEnabledComponent>();

		void RegisterComponent(MonoBehaviour component)
		{
			List<OssiaEnabledParameter> nodes = new List<OssiaEnabledParameter>();
			Debug.Log ("Registering component" + component.GetType().ToString());
			const BindingFlags flags = BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly;
			FieldInfo[] fields = component.GetType().GetFields(flags);

			// Find the fields that are marked for exposition
			foreach (FieldInfo field in fields) {
				if(Attribute.IsDefined(field, typeof(Ossia.Expose))) {
					var attr = (Ossia.Expose) Attribute.GetCustomAttribute(field, typeof(Ossia.Expose));
					nodes.Add(new OssiaEnabledParameter(field, attr));
				}
			}

			if (nodes.Count > 0) {
				// Create a node for the component
				OssiaEnabledComponent ossia_c = new OssiaEnabledComponent(component, child_node.AddChild(component.GetType().ToString()));

				// Create nodes for all the fields that were exposed
				foreach (OssiaEnabledParameter oep in nodes) {
					oep.parent = ossia_c;
					oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute.ExposedName);
					oep.ossia_address = oep.ossia_node.CreateAddress (Ossia.Value.TypeToOssia2 (oep.field.FieldType));
					oep.SendUpdates (this.gameObject);
				}

				ossia_c.parameters = nodes;
				ossia_components.Add (ossia_c);
			}
		}

		void RegisterObject(GameObject obj)
		{
			child_node = scene_node.AddChild(obj.name);
			ossia_transform = new OssiaTransform (obj, child_node);

			// For each component, we check the public fields.
			// If these fields have the Ossia.Expose attribute,
			// then we create node structures for them.
			MonoBehaviour[] comps = obj.GetComponents <MonoBehaviour>();
			foreach (MonoBehaviour component in comps) {
				RegisterComponent (component);
			}
		}

		public void Start()
		{
			var obj = GameObject.Find ("OssiaController");
			if (obj != null) {
				var comp = obj.GetComponent<Ossia.Controller> ();
				if (comp != null) {
					scene_node = comp.SceneNode ();
					RegisterObject (this.gameObject);

					if (ReceiveUpdates) {
						child_node.SetValueUpdating (true);
					}
				}
			} else {
				Debug.Log("OssiaController was not found.");
			}
		}

		public void Update()
		{
			if (ReceiveUpdates) {
				if (!child_node.GetValueUpdating ()) {
					child_node.SetValueUpdating (true);
				}

				ossia_transform.ReceiveUpdates (this.gameObject);
				foreach (var component in ossia_components) {
					component.ReceiveUpdates (this.gameObject);
				}
			}
			if (SendUpdates) {
				ossia_transform.SendUpdates (this.gameObject);
				foreach (var component in ossia_components) {
					component.SendUpdates (this.gameObject);
				}
			}
		}
	}
}

