#include <QtTest>
#include <ossia/ossia.hpp>
#include <ossia/network/http/http.hpp>

#include <ossia/network/websocket-generic-client/ws_generic_client.hpp>
#include <iostream>

using namespace ossia;

class DeviceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    /*! test life cycle and accessors functions */
    void test_basic()
    {
    {
      ossia::net::generic_device local_device{
        std::make_unique<ossia::net::local_protocol>(), "test" };
    }
    {
      ossia::net::generic_device osc_device{
        std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997), "test_osc" };
    }
    {
      ossia::net::generic_device minuit_device{
        std::make_unique<ossia::net::minuit_protocol>("i-score", "127.0.0.1", 13579, 13580), "test_minuit" };
    }
    }

    void test_comm_osc()
    {
      int N = 10;
      auto proto = std::make_unique<ossia::net::local_protocol>();
      auto proto_p = proto.get();
      ossia::net::generic_device local_device{
        std::move(proto), "test" };

      std::vector<ossia::net::address_base*> local_addr, remote_addr;
      for(int i = 0; i < N; i++)
      {
        auto cld = local_device.createChild(std::to_string(i));
        local_addr.push_back(cld->createAddress((ossia::val_type) i));
      }

      proto_p->exposeTo(std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9996, 9997));

      auto remote_proto = std::make_unique<ossia::net::osc_protocol>("127.0.0.1", 9997, 9996);
      ossia::net::generic_device remote_device{
        std::move(remote_proto), "test" };

      for(int i = 0; i < N; i++)
      {
        auto cld = remote_device.createChild(std::to_string(i));
        auto addr = cld->createAddress((ossia::val_type) i);
        remote_addr.push_back(addr);
      }

      for(int i = 0; i < N; i++)
      {
        local_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      for(int i = 0; i < N; i++)
      {
        remote_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }

      for(auto val : local_addr)
      {
        proto_p->observe(*val, true);
      }
      for(auto val : remote_addr)
      {
        proto_p->observe(*val, true);
      }

      for(int i = 0; i < N; i++)
      {
        local_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      for(int i = 0; i < N; i++)
      {
        remote_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
    }

    void test_comm_minuit()
    {
      int N = 10;
      auto proto = std::make_unique<ossia::net::local_protocol>();
      auto proto_p = proto.get();
      ossia::net::generic_device local_device{
        std::move(proto), "i-score" };

      std::vector<ossia::net::address_base*> local_addr, remote_addr;
      for(int i = 0; i < N; i++)
      {
        auto cld = local_device.createChild(std::to_string(i));
        local_addr.push_back(cld->createAddress((ossia::val_type) i));
      }

      proto_p->exposeTo(std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13579, 13580));

      auto remote_proto = std::make_unique<ossia::net::minuit_protocol>("i-score-remote", "127.0.0.1", 13580, 13579);
      ossia::net::generic_device remote_device{
        std::move(remote_proto), "i-score-remote" };

      for(int i = 0; i < N; i++)
      {
        auto cld = remote_device.createChild(std::to_string(i));
        auto addr = cld->createAddress((ossia::val_type) i);
        remote_addr.push_back(addr);
      }

      for(int i = 0; i < N; i++)
      {
        local_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      for(int i = 0; i < N; i++)
      {
        remote_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }

      for(auto val : local_addr)
      {
        proto_p->observe(*val, true);
      }
      for(auto val : remote_addr)
      {
        proto_p->observe(*val, true);
      }

      for(int i = 0; i < N; i++)
      {
        local_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
      for(int i = 0; i < N; i++)
      {
        remote_addr[i]->pushValue(ossia::init_value((ossia::val_type) i));
      }
    }

    void test_http()
    {
      int argc{}; char** argv{};
      QCoreApplication app(argc, argv);

      ossia::context context;

      QFile f("testdata/http/http_example.qml");
      f.open(QFile::ReadOnly);

      ossia::net::http_device http_device{
        std::make_unique<ossia::net::http_protocol>(
              f.readAll()),
            "test" };

      // We have to wait a bit for the event loop to run.
      QTimer t;
      connect(&t, &QTimer::timeout, [&] () {
        auto node = ossia::net::find_node(http_device, "/tata/tutu");
        if(node)
        {
          node->getAddress()->pushValue(ossia::Impulse{});
        }
      });
      t.setInterval(1000);
      t.setSingleShot(true);
      t.start();

      QTimer::singleShot(3000, [&] () { app.exit(); });

      app.exec();
    }
};


QTEST_APPLESS_MAIN(DeviceTest)

#include "DeviceTest.moc"

