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
      }
    }
    System.out.println("Local value is: " + bar_p.getValue().asFloat());

    root.create("float", "float", -5., 5.);
    root.create("int", "int", -127, 127);
    root.create("2D", "vec2", new Vec2F(-1.f, -1.f), new Vec2F(1.f, 1.f));
    root.create("3D", "vec3", new Vec3F(-1.f, -1.f, -1.f), new Vec3F(1.f, 1.f, 1.f));
    root.create("4D", "vec4", new Vec4F(-1.f, -1.f, -1.f, -1.f), new Vec4F(1.f, 1.f, 1.f, 1.f));

    root.create("string_array_domain", "string", new String[]{"foo", "bar", "baz"});
    root.create("int_array_domain", "int", new int[]{1,3,5,7,9});
    root.create("float_array_domain", "float", new float[]{2.6f, 67.5f});

    {
      Parameter p1 = root.create("float_array_minmaxdomain", "floatlist");
      p1.push(new float[]{2.6f, 67.5f, 1000.f, -4000.f});
    }

    {
      Parameter p2 = root.create("int_array_minmaxdomain", "intlist");
      p2.push(new int[]{6, 67, 1000, -4000});
    }

    {
      Parameter list = root.create("list", "list");
      list.push(new Value[]{new Value(132), new Value(10.456), new Value("foo bar")});
    }

    // Best API:
    Parameter built = root.newChild()
        .name("/boo/bar/baz")
        .type("float")
        .domain(-5.f, 5.f)
        .description("Sweet summer child")
        .unit("m/s")
        .bounding(BoundingMode.CLIP)
        .value(1.4)
        .get();

    System.out.println("The example will now run for 100 seconds. Go check it on http://127.0.0.1:5678");
    try
    {
      Thread.sleep(100000);
    }
    catch(InterruptedException ex)
    {
      Thread.currentThread().interrupt();
    }
  }
}
