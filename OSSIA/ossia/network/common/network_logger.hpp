#pragma once
#include <functional>
#include <string>
#include <ossia_export.h>

namespace ossia
{
    class OSSIA_EXPORT network_logger
    {
        public:
            using log_callback = std::function<void(std::string)>;
            /**
             * @brief setInboundLogCallback Set log function for messages coming from outside.
             */
            void setInboundLogCallback(log_callback cb)
            { mInboundCallback = std::move(cb); }
            const log_callback& getInboundLogCallback() const
            { return mInboundCallback; }

            /**
             * @brief setOutboundLogCallback Set log function for messages going outside.
             */
            void setOutboundLogCallback(log_callback cb)
            { mOutboundCallback = std::move(cb); }
            const log_callback& getOutboundLogCallback() const
            { return mOutboundCallback; }

        private:
            log_callback mInboundCallback;
            log_callback mOutboundCallback;
    };
}
