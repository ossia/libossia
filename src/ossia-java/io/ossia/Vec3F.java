package io.ossia;
import com.sun.jna.Structure;
import java.util.List;
import java.util.Arrays;

public class Vec3F extends Structure {
 public Vec3F() { x = 0.f; y = 0.f; z = 0.f; }
 public Vec3F(int a, int b, int c) { x = (float)a; y = (float)b; z = (float) c; }
 public Vec3F(float a, float b, float c) { x = (float)a; y = (float)b; z = (float) c; }
 public Vec3F(double a, double b, double c) { x = (float)a; y = (float)b; z = (float) c; }
 public Vec3F(float vals[]) { x = vals[0]; y = vals[1]; z = vals[2]; }

 public static class ByValue extends Vec3F implements Structure.ByValue {}
 public float x;
 public float y;
 public float z;

 @Override
 protected List getFieldOrder() { return Arrays.asList("x","y","z"); };
}
