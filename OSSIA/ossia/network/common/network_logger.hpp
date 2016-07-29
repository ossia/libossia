#pragma once
#include <functional>
#include <string>
#include <ossia_export.h>

namespace ossia
{
    class OSSIA_EXPORT NetworkLogger
    {
        public:
            using LogCallback = std::function<void(std::string)>;
            /**
             * @brief setInboundLogCallback Set log function for messages coming from outside.
             */
            void setInboundLogCallback(LogCallback cb)
            { mInboundCallback = std::move(cb); }
            const LogCallback& getInboundLogCallback() const
            { return mInboundCallback; }

            /**
             * @brief setOutboundLogCallback Set log function for messages going outside.
             */
            void setOutboundLogCallback(LogCallback cb)
            { mOutboundCallback = std::move(cb); }
            const LogCallback& getOutboundLogCallback() const
            { return mOutboundCallback; }

        private:
            LogCallback mInboundCallback;
            LogCallback mOutboundCallback;
    };
}
