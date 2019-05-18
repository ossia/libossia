import io.ossia.*;

class test
{
  public static void main(String args[])
  {
    Value v = new Value();
    v.set(1.f, 5.f, 10.f);
    Vec3F vec = v.asVec3();
    System.out.println(vec.x + ", " + vec.y + ", " + vec.z);
  }
}
