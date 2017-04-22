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


class DeviceBenchmark_Nsec_client : public QObject
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
                start_addr = node->get_address();
            if(node->getName() == "stopTick")
                stop_addr = node->get_address();
            if(start_addr && stop_addr)
                break;
        }

        OSSIA::Float f(0);

        bool a = false;
        bool b = false;
        std::thread checktime([&] () {
            a = true;
            auto t0 = std::chrono::steady_clock::now();
            while(true)
            {
                if(std::chrono::steady_clock::now() - t0 > std::chrono::seconds(10))
                {
                    b = true;
                    break;
                }
            }
        });

        while(!a);

        int num_sent = 0;
        start_addr->push_value(new ossia::impulse{});
        while(!b)
        {
            // Select a new node
            auto n = goToRandomNode(minuitDevice);
            const auto& name = n->getName();
            if(name != "startTick" && name != "stopTick")
            {
                if(n->get_address())
                {
                    n->get_address()->push_value(&f);
                }
            }

            num_sent ++;
        }
        stop_addr->push_value(new ossia::impulse{});

        std::cerr << "Sent " << num_sent << " values in 10 seconds" << std::endl;


    }

};


QTEST_APPLESS_MAIN(DeviceBenchmark_Nsec_client)

#include "DeviceBenchmark_Nsec_client.moc"

