package io.ossia;
import com.sun.jna.Pointer;

public class Device implements AutoCloseable
{
  public Device(Protocol p, String n)
  {
    if(p != null && p.impl != null)
      impl = Ossia.INSTANCE.ossia_device_create(p.impl, n);
  }

  @Override
  public void close() {
    if(impl != null)
      Ossia.INSTANCE.ossia_device_free(impl);
  }

  public boolean update()
  {
    return Ossia.INSTANCE.ossia_device_update_namespace(impl) != 0;
  }

  public Node getRootNode() {
    return new Node(Ossia.INSTANCE.ossia_device_get_root_node(impl));
  }

  Pointer impl;
}
