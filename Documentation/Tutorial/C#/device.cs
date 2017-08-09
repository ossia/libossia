public class Example
{
   public static void Main()
   {
      var proto = new Ossia.OSCQuery(1234, 5678);
      var dev = new Ossia.Device(proto, "tatat");

      var n = Ossia.Node.CreateNode(dev.GetRootNode(), "/foo/bar");

      System.Console.WriteLine(dev.GetName());
      var a = n.CreateAddress(Ossia.ossia_type.VEC2F);

      //a.PushValue(new Ossia.Value(123));
      var v = a.GetValue().GetVec2f();
      System.Console.WriteLine("" + v.Item1 + ": " +  v.Item2);

   }
}

