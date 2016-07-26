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

#include <ossia/network/v2/base/device.hpp>
#include <ossia/network/v2/generic/generic_node.hpp>

namespace impl
{
class BasicDevice final :
        public OSSIA::v2::Device,
        public BasicNode
{
    private:
    public:
        BasicDevice() = delete;
        BasicDevice(const BasicDevice&) = delete;
        BasicDevice(BasicDevice&&) = delete;
        BasicDevice& operator=(const BasicDevice&) = delete;
        BasicDevice& operator=(BasicDevice&&) = delete;

        BasicDevice(std::unique_ptr<OSSIA::v2::Protocol> protocol,
                    std::string name);

        ~BasicDevice();
};
}
