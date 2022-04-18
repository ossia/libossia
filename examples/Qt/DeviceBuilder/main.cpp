// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include <QLabel>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

using namespace ossia;

int main(int argc, char** argv)
{
  // Set-up UI
  QApplication app{argc, argv};
  QMainWindow w;
  QWidget central;
  w.setCentralWidget(&central);
  QGridLayout lay(&central);

  using namespace ossia::net;
  // Create a device which will listen on the websocket port 5678 and osc port 1234
  generic_device device{
    std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678),
    "my_device"};

  auto add_btn = new QPushButton("Add Parameter");
  auto rem_btn = new QPushButton("Remove Parameter");
  auto table = new QTableWidget(0,2);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);

  QObject::connect(add_btn, &QPushButton::clicked, [&](){
    auto& node = create_node(device, "parameter");
    auto param = node.create_parameter(val_type::FLOAT);
    table->insertRow(table->rowCount());
    int row = table->rowCount() - 1;
    auto label = QString::fromStdString(node.get_name());
    auto spin = new QDoubleSpinBox;
    auto mdl = table->model();
    mdl->setData(mdl->index(row,0), QVariant::fromValue<node_base*>(&node), Qt::UserRole);
    table->item(row,0)->setText(label);
    table->setCellWidget(row, 1, spin);

    QObject::connect(spin, qOverload<double>(&QDoubleSpinBox::valueChanged),
      [param](double v){
        param->push_value(static_cast<float>(v));
      });

    param->add_callback([spin](const ossia::value& v){
      spin->blockSignals(true);
      spin->setValue(v.get<float>());
      spin->blockSignals(false);
    });
  });

  QObject::connect(rem_btn, &QPushButton::clicked, [&](){
    auto items = table->selectedItems();
    for(auto i : items)
    {
      auto name = i->text().toStdString();
      device.remove_child(name);
      table->removeRow(i->row());
    }
  });

  QObject::connect(table, &QTableWidget::itemChanged,
    [&](QTableWidgetItem *item){
    if(item->column() == 0)
    {
      auto name = item->text().toStdString();
      auto node = item->data(Qt::UserRole).value<node_base*>();
      node->set_name(name);
    }
  });

  lay.addWidget(add_btn, 0, 0);
  lay.addWidget(rem_btn, 0, 1);
  lay.addWidget(table, 1, 0, 2, 10);

  w.show();
  return app.exec();
}

// declare custom type to use in QVariant
Q_DECLARE_METATYPE(ossia::net::node_base*)
