
#pragma once

#include <thread>
#include <cstdint>
#include <array>
#include <map>

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#include "device_parameter.hpp"
#include "wiimote_parameter.hpp"

#define MAX_WIIMOTES_COUNT 4

struct wiimote_t;

namespace ossia {
namespace net {

    class OSSIA_EXPORT wiimote_protocol final :
        public ossia::net::protocol_base
    {

        typedef struct {
                std::map<uint16_t, device::device_parameter*> button_parameters;

                axis_parameter *wiimote_axis{};
                device::device_parameter *wiimote_gravity{};

                device::device_parameter *nunchuk_button_c{};
                device::device_parameter *nunchuk_button_z{};

                axis_parameter *nunchuk_axis{};
                device::device_parameter *nunchuk_gravity{};
                device::device_parameter *nunchuk_joystick{};

                device::device_parameter *ir_cursor;
                device::device_parameter *ir_detection;
        } wiimote_parameters;

        public:
            wiimote_protocol(const bool enable_ir);
            ~wiimote_protocol();

            void set_device(ossia::net::device_base& dev) override;

            bool pull(ossia::net::parameter_base& param) override;
            bool push(const ossia::net::parameter_base& param) override;
            bool push_raw(const ossia::net::full_parameter_data&) override;
            bool observe(ossia::net::parameter_base& param, bool enable) override;
    
            bool update(ossia::net::node_base&) override;
        
        private:
            void create_wiimote_parameters(const unsigned int wiimote_id);
            void close_event_loop();

            static void wiimote_event_loop(wiimote_protocol *self);
            static void handle_wiimote_event(
                    wiimote_protocol *self,
                    const unsigned int wiimote_id);
        
            std::array<wiimote_parameters, MAX_WIIMOTES_COUNT> m_wiimotes_parameters;

            bool m_running;
            bool m_ready;
            const bool m_enable_ir;
            std::thread m_event_thread;

            struct wiimote_t **m_wiimotes{};
            unsigned int m_wiimote_count;
            ossia::net::device_base* m_device{};
    };

}   
}
