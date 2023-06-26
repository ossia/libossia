#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{

class node_process_raw : public looping_process<node_process_raw>
{
public:
    std::vector<ossia::token_request>& active;
    explicit node_process_raw(std::vector<ossia::token_request>& ref): active{ref} { }
    ~node_process_raw() override = default;
    void offset_impl(ossia::time_value) override { }
    void transport_impl(ossia::time_value date) override { }

    void state_impl(const ossia::token_request& req) { active.push_back(req); }

    void start() override { }
    void stop() override { }
    void pause() override { }
    void resume() override { }
    void mute_impl(bool) override { }
};

}
