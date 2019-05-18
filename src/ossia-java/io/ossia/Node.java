package io.ossia;
import com.sun.jna.Pointer;

public class Node
{
  Node(Pointer p)
  {
    impl = p;
  }

  public Node createNode(String s)
  {
    return new Node(Ossia.INSTANCE.ossia_node_create(impl, s));
  }

  public Node findNode(String s)
  {
    Pointer p = Ossia.INSTANCE.ossia_node_find(impl, s);
    if(p != null)
      return new Node(p);
    return null;
  }

  public int childCount()
  {
    return Ossia.INSTANCE.ossia_node_child_size(impl);
  }

  public Node getChild(int i)
  {
    return new Node(Ossia.INSTANCE.ossia_node_get_child(impl, i));
  }

  public Parameter createParameter(int t)
  {
    return new Parameter(Ossia.INSTANCE.ossia_node_create_parameter(impl, t));
  }

  public Parameter create(String name, String type)
  {
    return new Parameter(Ossia.INSTANCE.ossia_create_parameter(impl, name, type));
  }

  public Parameter getParameter()
  {
    Pointer p = Ossia.INSTANCE.ossia_node_get_parameter(impl);
    if(p != null)
      return new Parameter(p);
    return null;
  }

  public String getDescription()
  {
    return Ossia.INSTANCE.ossia_node_get_description(impl);
  }
  public void setDescription(String u)
  {
    Ossia.INSTANCE.ossia_node_set_description(impl, u);
  }

  public String getExtendedType()
  {
    return Ossia.INSTANCE.ossia_node_get_extended_type(impl);
  }
  public void setExtendedType(String u)
  {
    Ossia.INSTANCE.ossia_node_set_extended_type(impl, u);
  }

  Pointer impl;
}
