using System;
using System.Reflection;
using System.Collections.Generic;
using UnityEngine;

namespace Ossia
{
  //! Used to register the position and rotation transforms.
  public class OssiaTransform
  {
    public OssiaTransform(GameObject obj, Ossia.Node object_node)
    {
      {
        pos_node = object_node.AddChild ("position");
        pos_addr = pos_node.CreateParameter (Ossia.ossia_type.VEC3F);
        pos_addr.PushValue (obj.transform.position.x, obj.transform.position.y, obj.transform.position.z);
      }

      {
        rot_node = object_node.AddChild ("rotation");
        rot_addr = rot_node.CreateParameter (Ossia.ossia_type.VEC4F);
        rot_addr.PushValue (obj.transform.rotation.w, obj.transform.rotation.x, obj.transform.rotation.y, obj.transform.rotation.z);

      }
    }


    public void ReceiveUpdates(GameObject obj)
    {
      {
        var pos = obj.transform.position;

        { var val = pos_addr.GetValue ();
          if (val.GetOssiaType () == Ossia.ossia_type.VEC3F) {
            var vec3 = val.GetVec3f ();
            pos = new Vector3(vec3.f1, vec3.f2, vec3.f3);
          }
        }

        obj.transform.position = pos;
      }
      {
        var rot = obj.transform.rotation;

        { var val = pos_addr.GetValue ();
          if (val.GetOssiaType () == Ossia.ossia_type.VEC4F) {
            var vec3 = val.GetVec4f ();
            rot = new Quaternion(vec3.f1, vec3.f2, vec3.f3, vec3.f4);
          }
        }

        obj.transform.rotation = rot;
      }
    }

    public void SendUpdates(GameObject obj)
    {
      {
        var pos = obj.transform.position;
        pos_addr.PushValue (pos.x, pos.y, pos.z);
      }

      {
        var rot = obj.transform.rotation;
        rot_addr.PushValue (rot.w, rot.x, rot.y, rot.z);
      }
    }

    Ossia.Node pos_node;
    Ossia.Node rot_node;

    Ossia.Parameter pos_addr;
    Ossia.Parameter rot_addr;
  }

  //! Used to register every field that has an [Ossia.Expose] attached.
  internal class OssiaEnabledField
  {
    public OssiaEnabledComponent parent;

    public FieldInfo field;
    public string attribute;

    public Ossia.Node ossia_node;
    public Ossia.Parameter ossia_parameter;

    public OssiaEnabledField(FieldInfo f, string attr)
    {
      field = f;
      attribute = attr;
    }

    public void ReceiveUpdates(GameObject obj)
    {
        try {
          field.SetValue (parent.component, ossia_parameter.GetValue ().Get());
        }
        catch(Exception) {
        }
    }

    public void SendUpdates(GameObject obj)
    {
      ossia_parameter.PushValue (new Value(field.GetValue(parent.component)));
    }
  }


	internal class OssiaEnabledProperty
	{
		public OssiaEnabledComponent parent;

		public PropertyInfo field;
		public string attribute;

		public Ossia.Node ossia_node;
		public Ossia.Parameter ossia_parameter;

		public OssiaEnabledProperty(PropertyInfo f, string attr)
		{
			field = f;
			attribute = attr;
		}

		public void ReceiveUpdates(GameObject obj)
		{
			try {
				var value = ossia_parameter.GetValue().Get();
				var conv = Convert.ChangeType(value, field.PropertyType);
        field.SetValue (parent.component, conv, null);
			}
			catch(Exception) {
			}
		}

		public void SendUpdates(GameObject obj)
		{
			ossia_parameter.PushValue (new Value(field.GetValue(parent.component, null)));
		}
	}

  //! A component whose fields have some [Ossia.Expose] attributes
  internal class OssiaEnabledComponent
  {
    public Component component;
    public Ossia.Node component_node;

    public List<OssiaEnabledField> fields = new List<OssiaEnabledField>();
    public List<OssiaEnabledProperty> properties = new List<OssiaEnabledProperty>();

    public OssiaEnabledComponent(Component comp, Ossia.Node node)
    {
      component = comp;
      component_node = node;
    }

    public void ReceiveUpdates(GameObject obj)
    {
      foreach (var field in fields) {
        field.ReceiveUpdates (obj);
      }
      foreach (var property in properties) {
        property.ReceiveUpdates (obj);
      }
    }

    public void SendUpdates(GameObject obj)
    {
      foreach (var field in fields) {
        field.SendUpdates (obj);
      }
      foreach (var property in properties) {
        property.SendUpdates (obj);
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
      List<OssiaEnabledField> nodes = new List<OssiaEnabledField>();
      Debug.Log ("Registering component" + component.GetType().ToString());
      const BindingFlags flags = BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly;
      FieldInfo[] fields = component.GetType().GetFields(flags);

      // Find the fields that are marked for exposition
      foreach (FieldInfo field in fields) {
        if(Attribute.IsDefined(field, typeof(Ossia.Expose))) {
          var attr = (Ossia.Expose) Attribute.GetCustomAttribute(field, typeof(Ossia.Expose));
		  nodes.Add(new OssiaEnabledField(field, attr.ExposedName));
        }
      }

      if (nodes.Count > 0) {
        // Create a node for the component
        OssiaEnabledComponent ossia_c = new OssiaEnabledComponent(component, child_node.AddChild(component.GetType().ToString()));

        // Create nodes for all the fields that were exposed
        foreach (OssiaEnabledField oep in nodes) {
          oep.parent = ossia_c;
          oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute);
          oep.ossia_parameter = oep.ossia_node.CreateParameter (Ossia.Value.GetOssiaType (oep.field.FieldType));
          oep.SendUpdates (this.gameObject);
        }

        ossia_c.fields = nodes;
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

