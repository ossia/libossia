// To use :
// * Download http://repo1.maven.org/maven2/net/java/dev/jna/jna/4.5.1/jna-4.5.1.jar
//   - rename it to jna.jar
//   - put it in a "code" folder in the sketchbook
//   e.g. ~/sketchbook/sketch_123456/code/jna.jar
// * Then do the same with the ossia-java release for your operating system:
//   e.g. ~/sketchbook/sketch_123456/code/ossia-java.jar

import io.ossia.*;

Protocol p = new OscQueryServer(1234, 5678);
Device d = new Device(p, "processing");

Parameter my_color;
void setup()
{
  size(100, 100);
  Node root = d.getRootNode();
  Node bar = root.createNode("/foo/bar");
  bar.setDescription("Rectangle fill color");
  my_color = bar.createParameter(Type.VEC3F_T);
  my_color.setUnit("color.rgba8");
  my_color.push(120f, 20f, 50f, 255f);
}

void stop()
{
  d.close();
  p.close();
}

void draw()
{
  Vec4F col = my_color.getValue().asVec4();
  fill(col.x, col.y, col.z);
  rect(20, 20, 60, 60);
}
