#pragma once
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace oscquery
{
namespace detail
{

// We're in an array
struct to_json_domain
{
    rapidjson::Writer<rapidjson::StringBuffer>& writer;
    void operator()()
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::net::domain_base<impulse> & dom)
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::net::domain_base<int32_t> & dom)
    {
        if(dom.min) writer.Int(*dom.min); else writer.Null();
        if(dom.max) writer.Int(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Int(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::net::domain_base<float> & dom)
    {
        if(dom.min) writer.Double(*dom.min); else writer.Null();
        if(dom.max) writer.Double(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Double(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<char> & dom)
    {
        if(dom.min) writeChar(writer, *dom.min); else writer.Null();
        if(dom.max) writeChar(writer, *dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writeChar(writer, val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<bool> & dom)
    {
        if(dom.min) writer.Bool(*dom.min); else writer.Null();
        if(dom.max) writer.Bool(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Bool(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<std::string> & dom)
    {
        writer.Null();
        writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
                writer.String(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::net::domain_base<std::vector<ossia::value>> & dom)
    {
        if(dom.min)
        {
            writer.StartArray();
            for(auto& val : *dom.min)
                val.apply(value_to_json{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            writer.StartArray();
            for(auto& val : *dom.max)
                val.apply(value_to_json{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
            {
                writer.StartArray();
                for(auto& sub : val)
                    sub.apply(value_to_json{writer});
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    template<std::size_t N>
    void operator()(const ossia::net::domain_base<std::array<float, N>> & dom)
    {
        if(dom.min)
        {
            auto& vec = *dom.min;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            auto& vec = *dom.max;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& vec : dom.values)
            {
                writer.StartArray();
                for(std::size_t i = 0; i < N; i++)
                    writer.Double(vec[i]);
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::net::domain_base<ossia::value> & dom)
    {
        if(dom.min)
        {
            dom.min->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            dom.max->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val: dom.values)
            {
                val.apply(value_to_json{writer});
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
};

}
}
}
