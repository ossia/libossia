#include "qml_impulse.hpp"
namespace ossia
{
namespace qt
{
/*
qml_impulse::qml_impulse(QObject *parent):
    QObject{parent}
{

}

qml_impulse::~qml_impulse()
{

}

QString qml_impulse::node() const
{
    return m_node;
}

void qml_impulse::setNode(QString node)
{
    if(!publisher_singleton)
        return;

    if (m_node == node)
        return;

    m_node = node;

    if(m_ossia_node)
    {
        auto par = m_ossia_node->getParent();
        auto& cld = par->children();
        auto it = std::find(cld.begin(), cld.end(), m_ossia_node);
        if(it != cld.end())
            par->erase(it);
    }

    m_ossia_node = getOrCreateNode(
                publisher_singleton->_localDevice,
                node.split('/'));

    if(auto addr = m_ossia_node->getAddress())
    {
        m_address = addr;
    }
    else
    {
        m_address = m_ossia_node->createAddress(OSSIA::Value::Type::IMPULSE);
        m_address->addCallback([=] (const OSSIA::Value* val) {
            emit impulse();
        });
    }

    emit nodeChanged(node);
}*/
}
}
