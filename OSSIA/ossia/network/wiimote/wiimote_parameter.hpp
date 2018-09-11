
#pragma once

#include <cstdint>

#include "device_parameter.hpp"

struct wiimote_t;

namespace ossia {
namespace net {

    class rumble_parameter : public device::device_parameter {

        public:
            rumble_parameter(
                ossia::net::node_base& node,
                struct wiimote_t *wiimote);

        protected:
            void device_update_value() override;

        private:
            struct wiimote_t *m_wiimote;
    };

    class led_parameter : public device::device_parameter {

        public:
            led_parameter(
                ossia::net::node_base& node,
                struct wiimote_t *wiimote,
                const uint8_t led);

            ~led_parameter();

        protected:
            void device_update_value() override;

        private:
            struct wiimote_t *m_wiimote;
            const uint8_t m_led;
            static std::map<wiimote_t*, uint8_t> m_led_mask;
    };


    class axis_parameter : public device::device_parameter {

        public:
            axis_parameter(
                ossia::net::node_base& node,
                struct wiimote_t *wiimote);

        protected:
            void device_update_value() override {}
            void on_first_callback_added() override;
            void on_removing_last_callback() override;

        private:
            struct wiimote_t *m_wiimote;
    };

}
}