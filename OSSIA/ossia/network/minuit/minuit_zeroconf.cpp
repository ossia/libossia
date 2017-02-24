#include "minuit_zeroconf.hpp"
#include <iostream>

#include <servus/servus.h>
#include <chrono>


namespace ossia
{
namespace net
{
// const auto minuit_service_name = QStringLiteral("_minuit._tcp");
/*
class zeroconf_browser :
    public QObject
{
  public:
    zeroconf_browser()
    {

      auto browser = new ServiceBrowser(minuit_service_name);
      connect(browser, &ServiceBrowser::serviceAdded,
              this, [this] (auto service)
      {
        KDNSSD::RemoteService* data = service.data();
        data->resolve();

        auto ipAddressesList = QHostInfo::fromName(data->hostName()).addresses();
        QString ipAddress;

        qDebug() << data->hostName() << ipAddressesList;

        if(!ipAddressesList.empty())
        {
          for(int i = 0; i < ipAddressesList.size(); ++i)
          {
            if(ipAddressesList.at(i).toIPv4Address())
            {
              ipAddress = ipAddressesList.at(i).toString();
              break;
            }
          }
        }
        else
        {
          ipAddress = "127.0.0.1";
        }

        qDebug() << data->isResolved()<< ipAddress << data->port();


        auto txt = data->textData();
        if(txt.contains("RemoteName") && txt.contains("RemotePort") && txt.contains("LocalPort"))
        {
          mData.push_back(
                minuit_connection_data{
                  txt["RemoteName"].toStdString(),
                  ipAddress.toStdString(),
                  txt["RemotePort"].toInt(),
                  txt["LocalPort"].toInt()});
        }

      });

      browser->startBrowse();

    }

    std::vector<minuit_connection_data> mData;

};
*/

bool zeroconf_supported()
{
  return true;
}

std::vector<minuit_connection_data> list_minuit_devices()
{
  using namespace std::chrono_literals;

  //zeroconf_browser b;
  auto t = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();
  while((t2 - t) < 5s)
  {
    t2 = std::chrono::high_resolution_clock::now();
  //  QCoreApplication::processEvents();
  }
return {};
  //return b.mData;
}

zeroconf_server make_zeroconf_server(
    std::string description,
    std::string service,
    std::string local_name,
    int32_t local_port,
    int32_t remote_port)
{
  auto server = std::make_unique<servus::Servus>(service);
  server->set("LocalPort", std::to_string(local_port));
  server->set("LocalName", local_name);
  server->set("RemotePort", std::to_string(remote_port));
  server->set("Description", description);

  server->announce(local_port, description);

  return zeroconf_server{std::move(server)};
}

}
}
