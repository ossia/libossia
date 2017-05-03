#include <QtTest>
#include <ossia/ossia.hpp>
#include <iostream>
#include "Random.hpp"
#include <thread>
#include <atomic>
#include <boost/range/algorithm/find_if.hpp>
Random r;
using namespace ossia;

std::atomic<int> num_received{0};
OSSIA::Node* createNodeFromPath(
        const std::vector<std::string> &path,
        OSSIA::Device *dev)
{
    using namespace ossia;
    // Find the relevant node to add in the device
    OSSIA::Node* node = dev;
    for(int i = 0; i < path.size(); i++)
    {
        const auto& children = node->children();
        auto it = boost::range::find_if(
                    children,
                    [&] (const std::shared_ptr<OSSIA::Node>& ossia_node) {
            return ossia_node->getName() == path[i];
        });

        if(it != children.end())
        {
            node = it->get();
        }
        else
        {
            // We have to start adding sub-nodes from here.
            for(int k = i; k < path.size(); k++)
            {
                auto newNodeIt = node->emplace(node->children().begin(), path[k]);
                node = newNodeIt->get();
                auto addr = node->create_address(Type::FLOAT);
                //dev->get_protocol()->observeAddressValue(addr, true);
                addr->addCallback([&] (const OSSIA::Value* val) {
                    ++num_received;
                });
            }

            break;
        }
    }

    return node;
}

class DeviceBenchmark_Nsec_server : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
        auto localProtocol = Local::create();
        auto localDevice = Device::create(localProtocol, "B");

        auto addrlist = r.getRandomAddresses(100);
        for(const auto& addr : addrlist)
        {
            createNodeFromPath(addr, localDevice.get());
        }

        // Add two nodes for start and stop of the timer.

        std::chrono::steady_clock::time_point start_time;
        std::chrono::steady_clock::time_point stop_time;

        bool b = false;

        auto st = localDevice->emplace(localDevice->children().end(), "startTick");
        auto st_addr = (*st)->create_address(OSSIA::Type::IMPULSE);
        st_addr->addCallback([&] (const OSSIA::Value* val) {
            start_time = std::chrono::steady_clock::now();
            std::cerr << "START received" << std::endl;
        });
        auto et = localDevice->emplace(localDevice->children().end(), "stopTick");
        auto et_addr = (*et)->create_address(OSSIA::Type::IMPULSE);
        et_addr->addCallback([&] (const OSSIA::Value* val) {
            stop_time = std::chrono::steady_clock::now();
            b = true;
            std::cerr << "STOP received" << std::endl;
        });

        auto minuitProtocol = Minuit::create("127.0.0.1", 9999, 6666);
        auto minuitDevice = Device::create(minuitProtocol, "A");

        while(!b);

        auto t0 = std::chrono::steady_clock::now();
        while(true)
        {
            if(std::chrono::steady_clock::now() - t0 > std::chrono::seconds(10))
            {
                break;
            }
        }


        std::cerr << "Received " << num_received<< " values in 10 seconds" << std::endl;
    }

};


QTEST_APPLESS_MAIN(DeviceBenchmark_Nsec_server)

#include "DeviceBenchmark_Nsec_server.moc"

