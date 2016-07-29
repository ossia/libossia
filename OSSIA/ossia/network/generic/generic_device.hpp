/*!
 * \file JamomaDevice.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <ossia/network/base/device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <boost/utility/string_ref.hpp>
namespace impl
{
class OSSIA_EXPORT BasicDevice final :
        public ossia::net::Device,
        public BasicNode
{
    private:
    public:
        BasicDevice() = delete;
        BasicDevice(const BasicDevice&) = delete;
        BasicDevice(BasicDevice&&) = delete;
        BasicDevice& operator=(const BasicDevice&) = delete;
        BasicDevice& operator=(BasicDevice&&) = delete;

        BasicDevice(std::unique_ptr<ossia::net::Protocol> protocol,
                    std::string name);

        const ossia::net::Node& getRootNode() const override { return *this; }
        ossia::net::Node& getRootNode() override { return *this; }

        ~BasicDevice();
};

// address : format /a/b/c
OSSIA_EXPORT BasicNode* find_node(
        BasicDevice& dev,
        boost::string_ref address);
OSSIA_EXPORT BasicNode& find_or_create_node(
    BasicDevice& dev,
    boost::string_ref address);
}
