// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include "Random.hpp"
#include<thread>
#include <boost/range/algorithm/find_if.hpp>
Random r;
using namespace ossia;

std::shared_ptr<OSSIA::Node> goToRandomNode(
        const std::shared_ptr<OSSIA::Node>& root)
{
    // Get a random number between 1 and 100
    auto depth = 1 + r.getRandomUInt() % 100;

    // Try to go randomly to a node that deep and return it
    auto currentNode = root;
    for(int i = 0; i < depth; i++)
    {
        if(currentNode->children().empty())
        {
            break;
        }

        auto node_num = r.getRandomUInt() % currentNode->children().size();
        currentNode = currentNode->children()[node_num];

    }

    return currentNode;
}


class DeviceBenchmark_client : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        // declare this program "A" as Local device
        auto localProtocol = Local::create();
        auto localDevice = Device::create(localProtocol, "A");

        // declare a distant program "B" as a Minuit device
        auto minuitProtocol = Minuit::create("127.0.0.1", 6666, 9999);
        auto minuitDevice = Device::create(minuitProtocol, "B");

        bool val = minuitDevice->updateNamespace();

        std::shared_ptr<OSSIA::Address> start_addr;
        std::shared_ptr<OSSIA::Address> stop_addr;
        for(const auto& node : minuitDevice->children())
        {
            if(node->getName() == "startTick")
                start_addr = node->get_parameter();
            if(node->getName() == "stopTick")
                stop_addr = node->get_parameter();
            if(start_addr && stop_addr)
                break;
        }

        OSSIA::Float f(0);

        start_addr->push_value(new ossia::impulse{});
        // Send the "start" tick
        // Send the messages
        int iter = 1000000;
        std::chrono::steady_clock::duration total_dur;
        for(int i = 0; i < iter; i++)
        {
            // Select a new node
            auto n = goToRandomNode(minuitDevice);
            const auto& name = n->getName();
            if(name != "startTick" && name != "stopTick")
            {
                if(n->get_parameter())
                {
                    auto start = std::chrono::steady_clock::now();
                    n->get_parameter()->push_value(&f);
                    auto end = std::chrono::steady_clock::now();

                    total_dur += end - start;
                }
            }
        }

        stop_addr->push_value(new ossia::impulse{});
        std::cout << "Sending: " << iter << ": "
                  << std::chrono::duration <double, std::milli> (total_dur).count() / float(iter)
                  << "" << std::endl;

    }

};


QTEST_APPLESS_MAIN(DeviceBenchmark_client)

#include "DeviceBenchmark_client.moc"

