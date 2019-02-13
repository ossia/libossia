#include <QApplication>
#include <QMainWindow>
#include <QFormLayout>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/detail/dataspace_list.hpp>
#include <ossia/detail/for_each.hpp>
#include <QPainter>

class ColorWidget : public QWidget
{
public:
  void setColor(QColor c)
  {
    m_color = c;
    update();
  }

  void paintEvent(QPaintEvent* e) override
  {
    QPainter p(this);
    p.fillRect(this->rect(), m_color);
  }
private:
  QColor m_color{};
};

int main(int argc, char**argv)
{
  // Set-up UI
  QApplication app{argc, argv};
  QMainWindow w;
  QWidget central;
  w.setCentralWidget(&central);
  QFormLayout lay(&central);

  // Set-up ossia
  ossia::oscquery_device dev;
  auto& root = dev.device.get_root_node();

  ossia::for_each_tagged(ossia::color_u_list{}, [&] (auto t) {
    using unit_t = typename decltype(t)::type;

    const std::string unit_text{unit_t::text()[0]};

    // Create a parameter
    ossia::net::parameter_base* param = ossia::create_parameter(root, unit_text, unit_text);

    // Create a matching widget
    auto widget = new ColorWidget;
    lay.addRow(QString::fromStdString(unit_text), widget);

    // Set-up the color updates
    param->add_callback([=] (const ossia::value& v) {
      auto& val = *v.target<typename unit_t::value_type>();
      ossia::rgba normalized{ossia::strong_value<unit_t>{val}};
      auto c = normalized.dataspace_value;
      widget->setColor(QColor::fromRgbF(c[0], c[1], c[2], c[3]));
    });
  });

  w.show();
  return app.exec();
}
