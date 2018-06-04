// To use :
// download http://repo1.maven.org/maven2/net/java/dev/jna/jna/4.5.1/jna-4.5.1.jar
// put it in a "code" folder in the sketchbook
// and rename it to jna.jar (maybe not necessary ?)
// e.g. ~/sketchbook/sketch_123456/code/jna.jar

// Then change the path to libossia.so/ossia.dll/libossia.dylib

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Platform;

public static interface OssiaType
{
  public static final int FLOAT_T = 0;
  public static final int INT_T = 1;
  public static final int VEC2F_T = 2;
  public static final int VEC3F_T = 3;
  public static final int VEC4F_T = 4;
  public static final int IMPULSE_T = 5;
  public static final int BOOL_T = 6;
  public static final int STRING_T = 7;
  public static final int LIST_T = 8;
  public static final int CHAR_T = 9;
}

 public class Vec2F implements Structure.ByValue {
  public float x;
  public float y;
}
 public class Vec3F implements Structure.ByValue {
  public float x;
  public float y;
  public float z;
}
 public class Vec4F implements Structure.ByValue {
  public float x;
  public float y;
  public float z;
  public float w;
}
public class SizeT extends IntegerType {
  public SizeT() { this(0); }
  public SizeT(long value) { super(Native.SIZE_T_SIZE, value, true); }
}

interface CLibrary extends Library
{
  CLibrary INSTANCE = (CLibrary) Native.loadLibrary("/tmp/libossia-processing/OSSIA/libossia.so", CLibrary.class);

  Pointer ossia_protocol_oscquery_server_create(int osc_port, int ws_port);
  void ossia_protocol_free(Pointer protocol);

  Pointer ossia_device_create(Pointer protocol, String name);
  void ossia_device_free(Pointer device);

  Pointer ossia_device_get_root_node(Pointer device);

  Pointer ossia_node_create(Pointer root, String name);

  Pointer ossia_node_create_parameter(Pointer root, int type);
  void ossia_parameter_push_impulse(Pointer param);
  void ossia_parameter_push_i(Pointer param, int v);
  void ossia_parameter_push_b(Pointer param, boolean v);
  void ossia_parameter_push_f(Pointer param, float v);
  void ossia_parameter_push_2f(Pointer param, float v1, float v2);
  void ossia_parameter_push_3f(Pointer param, float v1, float v2, float v3);
  void ossia_parameter_push_4f(Pointer param, float v1, float v2, float v3, float v4);
  void ossia_parameter_push_s(Pointer param, String s);
  void ossia_parameter_push_in(Pointer param, int[] s, SizeT n);
  void ossia_parameter_push_fn(Pointer param, float[] s, SizeT n);

  Pointer ossia_parameter_set_value(Pointer param, Pointer value);
  Pointer ossia_parameter_push_value(Pointer param, Pointer value);

  Pointer ossia_parameter_get_value(Pointer param);
  Pointer ossia_parameter_fetch_value(Pointer param);

  int ossia_value_to_int(Pointer val);
  boolean ossia_value_to_bool(Pointer val);
  float ossia_value_to_float(Pointer val);
  Vec2F ossia_value_to_2f(Pointer val);
  Vec3F ossia_value_to_3f(Pointer val);
  Vec4F ossia_value_to_4f(Pointer val);
  String ossia_value_to_string(Pointer val); // todo how to free ?
}


Pointer proto = CLibrary.INSTANCE.ossia_protocol_oscquery_server_create(1234, 5678);
Pointer device = CLibrary.INSTANCE.ossia_device_create(proto, "processing");
void setup()
{
  Pointer root = CLibrary.INSTANCE.ossia_device_get_root_node(device);
  Pointer bar = CLibrary.INSTANCE.ossia_node_create(root, "/foo/bar");
  Pointer bar_p = CLibrary.INSTANCE.ossia_node_create_parameter(root, OssiaType.FLOAT_T);

  CLibrary.INSTANCE.ossia_parameter_push_f(bar_p, 1.234);
}

void stop()
{
  CLibrary.INSTANCE.ossia_device_free(device);
  CLibrary.INSTANCE.ossia_protocol_free(proto);
}

void render()
{
}
