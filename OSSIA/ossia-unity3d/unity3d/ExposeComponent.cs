using System;
using System.Reflection;
using System.Collections.Generic;
using UnityEngine;

namespace Ossia
{
	public class ExposeComponent : MonoBehaviour
	{
		public string NodeName;
		public bool ReceiveUpdates = false;
		public bool SendUpdates = false;
		public List<Component> components;

		Ossia.Node scene_node;
		Ossia.Node child_node;
		List<OssiaEnabledComponent> ossia_components = new List<OssiaEnabledComponent>();

    void RegisterComponent(Component component, Ossia.Node node)
		{
			List<OssiaEnabledField> nodes = new List<OssiaEnabledField>();
			List<OssiaEnabledProperty> prop_nodes = new List<OssiaEnabledProperty>();

			Debug.Log ("Registering component" + component.GetType().ToString());
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
      if(nodes.Count > 0 || prop_nodes.Count > 0)
      {
        // Create a node for the component
        ossia_c = new OssiaEnabledComponent(
          component, 
          node.AddChild(component.GetType().ToString()));
        
        ossia_components.Add (ossia_c);
      }

			if (nodes.Count > 0) {
				// Create nodes for all the fields that were exposed
				foreach (OssiaEnabledField oep in nodes) {
					oep.parent = ossia_c;
					oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute);
					oep.ossia_parameter = oep.ossia_node.CreateParameter (Ossia.Value.GetOssiaType (oep.field.FieldType));
					oep.SendUpdates (this.gameObject);
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
            Debug.Log("Adding node" + oep.field.Name + " to " + node.GetName());
            try { 
              var subcomp = (Component)oep.field.GetValue (component, null);
              RegisterComponent (subcomp, subnode);
            } catch { }
          } else { 
            oep.parent = ossia_c;
            oep.ossia_node = ossia_c.component_node.AddChild (oep.attribute);
            try {
              oep.ossia_parameter = oep.ossia_node.CreateParameter (Ossia.Value.GetOssiaType (oep.field.PropertyType));
              oep.SendUpdates (this.gameObject);
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
			var obj = GameObject.Find ("OssiaController");
			if (obj != null) {
				var comp = obj.GetComponent<Ossia.Controller> ();
				if (comp != null) {
					scene_node = comp.SceneNode ();

					child_node = scene_node.AddChild(NodeName);
					foreach(var c in components)
					{
            var subnode = child_node.AddChild (c.GetType().Name);
						RegisterComponent(c, subnode);
					}

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

				foreach (var component in ossia_components) {
					component.ReceiveUpdates (this.gameObject);
				}
			}
			if (SendUpdates) {
				foreach (var component in ossia_components) {
					component.SendUpdates (this.gameObject);
				}
			}
		}
	}
}
