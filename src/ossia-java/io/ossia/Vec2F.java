package io.ossia;
import com.sun.jna.Structure;
import java.util.List;
import java.util.Arrays;

public class Vec2F extends Structure {

 public Vec2F() { x = 0.f; y = 0.f; }
 public Vec2F(int a, int b) { x = (float)a; y = (float)b; }
 public Vec2F(float a, float b) { x = (float)a; y = (float)b; }
 public Vec2F(double a, double b) { x = (float)a; y = (float)b; }
 public Vec2F(float vals[]) { x = vals[0]; y = vals[1]; }

 public static class ByValue extends Vec2F implements Structure.ByValue {}
 public float x;
 public float y;

 @Override
 protected List getFieldOrder() { return Arrays.asList("x","y"); };
}
