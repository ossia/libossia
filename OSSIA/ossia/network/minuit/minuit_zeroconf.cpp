#include "minuit_zeroconf.hpp"
#include <iostream>
#if defined(OSSIA_ZEROCONF)

#include <KDNSSD/DNSSD/ServiceBrowser>
#include <KDNSSD/DNSSD/RemoteService>
#include <KDNSSD/DNSSD/PublicService>
#include <QHostInfo>
#include <chrono>
#include <QCoreApplication>
#include <QDataStream>


namespace ossia
{
namespace net
{
const auto minuit_service_name = QStringLiteral("_minuit._tcp");

class zeroconf_browser :
    public QObject
{
  public:
    zeroconf_browser()
    {
      using namespace KDNSSD;
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


bool zeroconf_supported()
{
  return true;
}

std::vector<minuit_connection_data> list_minuit_devices()
{
  using namespace std::chrono_literals;

  zeroconf_browser b;
  auto t = std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();
  while((t2 - t) < 5s)
  {
    t2 = std::chrono::high_resolution_clock::now();
    QCoreApplication::processEvents();
  }
  return b.mData;
}

zeroconf_server make_zeroconf_server(
    std::string description,
    std::string service,
    std::string local_name,
    int32_t local_port,
    int32_t remote_port)
{
  auto server = std::make_unique<KDNSSD::PublicService>(
                  QString::fromStdString(description),
                  QString::fromStdString(service),
                  local_port);

  server->setTextData(
    QMap<QString, QByteArray>{
          {"LocalPort", QString::number(local_port).toUtf8()},
          {"LocalName", QByteArray::fromStdString(local_name)},
          {"RemotePort", QString::number(remote_port).toUtf8()}
    });

  server->publishAsync();

  return std::move(server);
}

}
}

#else

namespace ossia
{
namespace net
{
bool zeroconf_supported()
{
  return false;
}

std::vector<minuit_connection_data> list_minuit_devices()
{
  return {};
}

std::unique_ptr<KDNSSD::PublicService> make_zeroconf_server(std::__cxx11::string description, std::__cxx11::string service, std::__cxx11::string local_name, int32_t local_port, int32_t remote_port)
{
  return {};
}

}
}

namespace KDNSSD
{
// Fake class when we don't link with the library.
class PublicService
{
};
}

#endif

namespace ossia
{
namespace net
{

zeroconf_server::zeroconf_server() = default;
zeroconf_server::zeroconf_server(zeroconf_server&&) = default;
zeroconf_server& zeroconf_server::operator=(zeroconf_server&&) = default;
zeroconf_server::~zeroconf_server() = default;

zeroconf_server::zeroconf_server(std::unique_ptr<KDNSSD::PublicService> s):
  server{std::move(s)}
{

}

}
}
