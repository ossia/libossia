#include <ossia-qt/qml_context.hpp>
#include <ossia/network/common/debug.hpp>
namespace ossia
{
namespace qt
{

qml_context::~qml_context()
{

}

void qml_context::savePreset(qml_device* dev, const QUrl& file)
{
  fmt::MemoryWriter w;

//  ossia::net::debug_recursively(w, dev->device().getRootNode());

//  std::cerr << w.str() << std::endl;
  if(file.isLocalFile())
  {
    auto preset = ossia::devices::make_preset(dev->device());
    {
      QFile f(file.toLocalFile());
      f.open(QIODevice::WriteOnly);

      auto str = ossia::presets::write_json(preset);
      f.write(str.data(), str.size());
    }
  }
}

void qml_context::loadPreset(qml_device* dev, const QUrl& file)
{
  if(file.isLocalFile())
  {
    auto kv = ossia::presets::read_json(QFile(file.toLocalFile()).readAll().toStdString());
    ossia::devices::apply_preset(dev->device(), kv);
  }
}

}
}
