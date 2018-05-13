#pragma once
#include <ossia-qt/score/qml_autom.hpp>
#include <wobjectdefs.h>
#include <ossia/editor/mapper/mapper.hpp>

namespace ossia
{
namespace qt
{
class qml_mapping : public qml_process
{
    W_OBJECT(qml_mapping)

  public:
    qml_mapping(QQuickItem* parent = nullptr);
    ~qml_mapping() override;

    void setup() override { }
    std::shared_ptr<ossia::time_process> process() const override;
  private:
    void reset_impl() override;
    /* TODO
    std::shared_ptr<ossia::mapper> m_impl;
    */
};
}
}
