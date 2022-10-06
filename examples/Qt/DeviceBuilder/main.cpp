// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <QApplication>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTreeWidget>
#include <QWidget>

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
  auto add_node_btn = new QPushButton("Add Node (without parameter)");
  auto rem_btn = new QPushButton("Remove Node");
  auto tree = new QTreeWidget;
  tree->setColumnCount(2);
  tree->setSelectionBehavior(QAbstractItemView::SelectRows);

  auto add_node = [&](bool with_param = false) {
    QTreeWidgetItem* item{};
    node_base* node{};

    std::string name("node");
    if(with_param)
      name = "parameter";

    auto selection = tree->selectedItems();
    if(selection.empty())
    {
      item = new QTreeWidgetItem(tree);
      node = &create_node(device, name);
    }
    else
    {
      auto parent = selection.first();
      item = new QTreeWidgetItem(parent);
      parent->setExpanded(true);

      auto parent_node = parent->data(0, Qt::UserRole).value<node_base*>();
      if(parent_node)
        node = &create_node(*parent_node, name);
      else
        qDebug() << "Selected item has no node_base* attached.";
    }

    ossia::net::parameter_base* param{};

    if(with_param)
      param = node->create_parameter(val_type::FLOAT);

    auto label = QString::fromStdString(node->get_name());

    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(0, label);
    item->setData(0, Qt::UserRole, QVariant::fromValue<node_base*>(node));
    tree->addTopLevelItem(item);

    if(with_param)
    {
      auto spin = new QDoubleSpinBox;
      spin->setMinimum(std::numeric_limits<double>::lowest());
      spin->setMaximum(std::numeric_limits<double>::max());
      tree->setItemWidget(item, 1, spin);

      QObject::connect(
          spin, qOverload<double>(&QDoubleSpinBox::valueChanged),
          [param](double v) { param->push_value(static_cast<float>(v)); });

      param->add_callback([spin](const ossia::value& v) {
        spin->blockSignals(true);
        spin->setValue(v.get<float>());
        spin->blockSignals(false);
      });
    }
  };

  QObject::connect(add_node_btn, &QPushButton::clicked, add_node);
  QObject::connect(add_btn, &QPushButton::clicked, [&]() { add_node(true); });

  QObject::connect(rem_btn, &QPushButton::clicked, [&]() {
    auto items = tree->selectedItems();
    for(auto i : items)
    {
      auto name = i->text(0).toStdString();
      device.remove_child(name);
      auto parent = i->parent();
      if(parent)
        parent->removeChild(i);
      delete i;
    }
  });

  QObject::connect(tree, &QTreeWidget::itemChanged, [&](QTreeWidgetItem* item) {
    auto name = item->text(0).toStdString();
    auto node = item->data(0, Qt::UserRole).value<node_base*>();
    if(node && node->get_name() != name)
      node->set_name(name);
  });

  lay.addWidget(add_node_btn, 0, 0);
  lay.addWidget(add_btn, 0, 1);
  lay.addWidget(rem_btn, 0, 2);
  lay.addWidget(tree, 1, 0, 2, 10);

  w.show();
  return app.exec();
}

// declare custom type to use in QVariant
Q_DECLARE_METATYPE(ossia::net::node_base*)
