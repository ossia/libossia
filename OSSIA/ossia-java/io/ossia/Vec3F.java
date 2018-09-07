package io.ossia;
import com.sun.jna.Structure;
import java.util.List;
import java.util.Arrays;

public class Vec3F extends Structure {
 public static class ByValue extends Vec3F implements Structure.ByValue {}
 public float x;
 public float y;
 public float z;

 @Override
 protected List getFieldOrder() { return Arrays.asList("x","y","z"); };
}
