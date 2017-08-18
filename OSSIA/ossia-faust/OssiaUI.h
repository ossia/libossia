/************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; If not, see <http://www.gnu.org/licenses/>.

    EXCEPTION : As a special exception, you may create a larger work
    that contains this FAUST architecture section and distribute
    that work under terms of your choice, so long as this FAUST
    architecture section is not modified.


 ************************************************************************
 ************************************************************************/

#ifndef FAUST_OSSIAUI_H
#define FAUST_OSSIAUI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include "faust/gui/UI.h"
#include "faust/gui/PathBuilder.h"
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/domain/domain.hpp>

class OssiaUI final : public PathBuilder, public UI
{
    ossia::net::generic_device m_dev;
    ossia::net::node_base* m_curNode{};

    std::vector<std::pair<ossia::net::parameter_base*, FAUSTFLOAT*>> m_values;
    std::atomic_bool m_running;

public:
    OssiaUI(uint16_t osc_port, uint16_t ws_port)
      : m_dev{std::make_unique<ossia::oscquery::oscquery_server_protocol>(osc_port, ws_port), "Faust"}
      , m_curNode{&m_dev.get_root_node()}
    {
    }

    ~OssiaUI()
    {
    }

    void run(int ms)
    {
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            for(auto p : m_values)
                p.first->push_value(*p.second);
        }
    }

private:
    void openTabBox(const char* label) override
    {
        m_curNode = m_curNode->create_child(label);
    }
    void openHorizontalBox(const char* label) override
    {
        m_curNode = m_curNode->create_child(label);
    }
    void openVerticalBox(const char* label) override
    {
        m_curNode = m_curNode->create_child(label);
    }
    void closeBox() override
    {
        m_curNode = m_curNode->get_parent();
    }


    // -- active widgets
    void addButton(const char* label, FAUSTFLOAT* zone) override
    {
        auto n = m_curNode->create_child(label);
        auto a = n->create_address(ossia::val_type::BOOL);
        a->add_callback([zone] (const ossia::value& val) {
            *zone = val.get<bool>() ? 1.0 : 0.0;
        });
    }

    void addCheckButton(const char* label, FAUSTFLOAT* zone) override
    {
        auto n = m_curNode->create_child(label);
        auto a = n->create_address(ossia::val_type::BOOL);
        a->add_callback([zone] (const ossia::value& val) {
            *zone = val.get<bool>() ? 1.0 : 0.0;
        });
    }

    void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
    {
        auto n = m_curNode->create_child(label);
        auto a = n->create_address(ossia::val_type::FLOAT);
        ossia::net::set_default_value(*n, init);
        ossia::net::set_domain(*n, ossia::make_domain(min, max));
        ossia::net::set_value_step_size(*n, step);
        a->add_callback([zone] (const ossia::value& val) {
            *zone = val.get<float>();
        });
    }

    void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
    {
        addVerticalSlider(label, zone, init, min, max, step);
    }

    void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override
    {
        addVerticalSlider(label, zone, init, min, max, step);
    }


    // -- passive widgets
    void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
    {
        auto n = m_curNode->create_child(label);
        auto a = n->create_address(ossia::val_type::FLOAT);
        ossia::net::set_domain(*n, ossia::make_domain(min, max));
        ossia::net::set_access_mode(*n, ossia::access_mode::GET);

        m_values.push_back({a, zone});
    }
    void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override
    {
        auto n = m_curNode->create_child(label);
        auto a = n->create_address(ossia::val_type::FLOAT);
        ossia::net::set_domain(*n, ossia::make_domain(min, max));
        ossia::net::set_access_mode(*n, ossia::access_mode::GET);

        m_values.push_back({a, zone});
    }

    // -- metadata declarations
    void declare(FAUSTFLOAT* zone, const char* key, const char* val) override
    {
        std::cout << "declare key : " << key << " val : " << val << std::endl;
    }

};

#endif
