package io.ossia;
import com.sun.jna.Structure;
import java.util.List;
import java.util.Arrays;

public class Vec2F extends Structure {
 public static class ByValue extends Vec2F implements Structure.ByValue {}
 public float x;
 public float y;

 @Override
 protected List getFieldOrder() { return Arrays.asList("x","y"); };
}
