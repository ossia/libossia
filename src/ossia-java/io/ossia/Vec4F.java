package io.ossia;
import com.sun.jna.Structure;
import java.util.List;
import java.util.Arrays;

public class Vec4F extends Structure {
 public Vec4F() { x = 0.f; y = 0.f; z = 0.f; w = 0.f; }
 public Vec4F(int a, int b, int c, int d) { x = (float)a; y = (float)b; z = (float) c; w = (float) d; }
 public Vec4F(float a, float b, float c, float d) { x = (float)a; y = (float)b; z = (float) c; w = (float) d; }
 public Vec4F(double a, double b, double c, float d) { x = (float)a; y = (float)b; z = (float) c; w = (float) d; }
 public Vec4F(float vals[]) { x = vals[0]; y = vals[1]; z = vals[2]; w = vals[3]; }

 public static class ByValue extends Vec4F implements Structure.ByValue {}
 public float x;
 public float y;
 public float z;
 public float w;

 @Override
 protected List getFieldOrder() { return Arrays.asList("x","y","z","w"); };
}
