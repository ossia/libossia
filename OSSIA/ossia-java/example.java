import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;
import com.sun.jna.Pointer;
import io.ossia.*;

class example
{
  public static void main(String args[])
  {
    System.out.println("Hello Ossia-Java");

    Protocol p = new OscQueryServer(1234, 5678);
    Device d = new Device(p, "processing");

    Node root = d.getRootNode();
    Node bar = root.createNode("/foo/bar");
    Parameter bar_p = bar.createParameter(Type.FLOAT_T);
    bar_p.push(1.2f);

    Device mirror_d = new Device(new OscQueryMirror("ws://127.0.0.1:5678"), "mirror");
    mirror_d.update();
    Parameter mirror_bar_p = mirror_d.getRootNode().findNode("/foo/bar").getParameter();


    System.out.println("Local value is: " + bar_p.getValue().asFloat());
    while(bar_p.getValue().asFloat() == 1.2f)
    {
      try
      {
        mirror_bar_p.push(1.4f);
        Thread.sleep(10);
      }
      catch(InterruptedException ex)
      {
        Thread.currentThread().interrupt();
      }
    }
    System.out.println("Local value is: " + bar_p.getValue().asFloat());
  }
}
