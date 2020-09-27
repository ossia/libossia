// To use :
// * Download the latest ossia-java release
//   - put its content in a "code" folder in the sketchbook
//   e.g. ~/sketchbook/sketch_123456/code/jna.jar
//   and  ~/sketchbook/sketch_123456/code/ossia-java.jar
// * Copy this file as ~/sketchbook/sketch_123456/sketch_123456.pde

import io.ossia.*;

Protocol p = new OscQueryServer(1234, 5678);
Device d = new Device(p, "processing");

Parameter my_color;
void setup()
{
  size(100, 100);
  Node root = d.getRootNode();
  my_color = root.newChild()
                 .name("/foo/bar")
                 .type("rgba8")
                 .value(120, 20, 50, 255)
                 .description("Rectangle fill color")
                 .get()
  ;
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
