import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;

class example
{
  public static void main(String args[])
  {
    System.out.println("Hello Ossia-Java");

    Pointer proto = Ossia.INSTANCE.ossia_protocol_oscquery_server_create(1234, 5678);
    Pointer device = Ossia.INSTANCE.ossia_device_create(proto, "processing");

    Pointer root = Ossia.INSTANCE.ossia_device_get_root_node(device);
    Pointer bar = Ossia.INSTANCE.ossia_node_create(root, "/foo/bar");
    Pointer bar_p = Ossia.INSTANCE.ossia_node_create_parameter(root, Type.FLOAT_T);

    Ossia.INSTANCE.ossia_parameter_push_f(bar_p, 1.2f);

    while(Ossia.INSTANCE.ossia_value_to_float(Ossia.INSTANCE.ossia_parameter_get_value(bar_p)) == 1.2f)
    {
      try
      {
        Thread.sleep(10);
      }
      catch(InterruptedException ex)
      {
        Thread.currentThread().interrupt();
      }
    }

    Ossia.INSTANCE.ossia_device_free(device);
    Ossia.INSTANCE.ossia_protocol_free(proto);
  }
}
