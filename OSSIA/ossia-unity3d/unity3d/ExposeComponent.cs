using System;
using System.Reflection;
using System.Collections.Generic;
using UnityEngine;

namespace Ossia
{
  public class ExposeComponent : MonoBehaviour
	{
    public UpdateMode mode = UpdateMode.ReceiveUpdates;
		public string NodeName;
		public List<Component> components;

		Ossia.Node scene_node;

    public Ossia.Node child_node;
    List<OssiaEnabledComponent> ossia_components = new List<OssiaEnabledComponent>();

    public void ReceiveUpdates()
    {
      if (child_node == null) {
        Start ();
      }

      if (!child_node.GetValueUpdating ()) 
        child_node.SetValueUpdating (true);

      foreach (var component in ossia_components) {
        component.ReceiveUpdates ();
      }
    }

    public void SendUpdates()
    {
      if (child_node == null) {
        Start ();
      }

      if(child_node.GetValueUpdating())
        child_node.SetValueUpdating (false);

      foreach (var component in ossia_components) {
        component.SendUpdates ();
      }
    }

    void RegisterComponent(Component component, Ossia.Node node)
		{
			List<OssiaEnabledField> nodes = new List<OssiaEnabledField>();
			List<OssiaEnabledProperty> prop_nodes = new List<OssiaEnabledProperty>();

			const BindingFlags flags = BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly;
			FieldInfo[] fields = component.GetType().GetFields(flags);

			// Find the fields that are marked for exposition
			foreach (FieldInfo field in fields) {
				nodes.Add(new OssiaEnabledField(field, field.Name));
      }
      // Same for components
      foreach (PropertyInfo prop in component.GetType().GetProperties ()) {
        prop_nodes.Add(new OssiaEnabledProperty(prop, prop.Name));
      }

      OssiaEnabledComponent ossia_c;
      if (nodes.Count > 0 || prop_nodes.Count > 0) {
        // Create a node for the component
        ossia_c = new OssiaEnabledComponent (
          component, 
          node.AddChild (component.GetType ().ToString ()));
        
        ossia_components.Add (ossia_c);
      } else {
        return;
      }

			if (nodes.Count > 0) {
				// Create nodes for all the fields that were exposed
				foreach (OssiaEnabledField oep in nodes) {
					oep.parent = ossia_c;
					oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute);
					oep.ossia_parameter = oep.ossia_node.CreateParameter (Ossia.Value.GetOssiaType (oep.field.FieldType));
					oep.SendUpdates ();
				}

        ossia_c.fields = nodes;
			}

			if (prop_nodes.Count > 0) {
				// Create nodes for all the fields that were exposed
				foreach (OssiaEnabledProperty oep in prop_nodes) {
          if(oep.field.PropertyType == typeof(UnityEngine.Transform)) { 
            // TODO
          } else if(oep.field.PropertyType == typeof(UnityEngine.Component)) { 
            var subnode = node.AddChild (oep.field.Name);
            try { 
              var subcomp = (Component)oep.field.GetValue (component, null);
              RegisterComponent (subcomp, subnode);
            } catch { }
          } else { 
            oep.parent = ossia_c;
            oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute);
            try {
              oep.ossia_parameter = oep.ossia_node.CreateParameter (Ossia.Value.GetOssiaType (oep.field.PropertyType));
              oep.SendUpdates ();
            } catch { 
              Debug.Log("parameter not ok" + oep.field.Name + " => " + oep.field.PropertyType);
              ossia_c.component_node.RemoveChild(oep.ossia_node);
            }
          }
				}

				ossia_c.properties = prop_nodes;
			}
		}

		public void Start()
    {
      if (child_node != null) {
        Debug.Log("Object already registered.");        
        return;
      }

      var comp = Controller.Get ();
      scene_node = comp.SceneNode ();

      child_node = scene_node.AddChild(NodeName);
      foreach(var c in components)
      {
        var subnode = child_node.AddChild (c.GetType().Name);
        RegisterComponent(c, subnode);
      }

      child_node.SetValueUpdating (mode == UpdateMode.ReceiveUpdates);
		}

		public void Update()
    {
      if (child_node == null) {
        Debug.Log("Object not registered.");  
        Start ();
      }

      if (mode == UpdateMode.ReceiveUpdates) {
        ReceiveUpdates ();
			}
      else if (mode == UpdateMode.SendUpdates) {
        SendUpdates ();
			}
		}
	}
}
