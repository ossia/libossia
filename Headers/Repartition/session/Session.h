#pragma once

#include "../permission/full/PermissionManager.h"
#include "../group/GroupManager.h"
#include "../client/ClientManager.h"
#include "../client/RemoteMaster.h"

#include <type_traits>

#include <vector>
#include <utility>

#include <cstdint>
#include <limits>
#include <random>

inline int32_t generateRandom64()
{
	using namespace std;
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<int32_t>
			dist(numeric_limits<int32_t>::min(),
				 numeric_limits<int32_t>::max());

	return dist(gen);
}

class ClientSessionBuilder;
class Session : public hasName, public hasId
{
		friend class ClientSessionBuilder;

	public:
		Session(std::string name):
			Session(name, generateRandom64())
		{
		}

		// Debug only
		Session(std::string name, int id):
			hasName(name),
			hasId(id)
		{
		}

		Session(Session&&) = default;
		Session(const Session&) = default;
		virtual ~Session() = default;

		virtual void sendCommand(std::string parentName,
								 std::string name,
								 const char * data, int len) = 0;

		template<typename... K>
		Group& group(K&&... args)
		{
			return _groups(std::forward<K>(args)...);
		}

		GroupManager& groups()
		{
			return _groups;
		}

		template<typename... K>
		RemoteClient& client(K&&... args)
		{
			return _clients(std::forward<K>(args)...);
		}

		ClientManager& clients()
		{
			return _clients;
		}

		template<typename... K>
		bool getPermission(K&&... args)
		{
			return _localPermissions.getPermission(std::forward<K>(args)...);
		}

		PermissionManager& localPermissions()
		{
			return _localPermissions;
		}


		virtual Client& getClient() = 0;

		std::function<void(std::string, std::string, const char*, int)> cmdCallback;
		void handle__edit_command(osc::ReceivedMessageArgumentStream args)
		{
			osc::int32 sessionId;
			osc::int32 clientId;
			const char* par_name;
			const char* cmd_name;
			osc::Blob blob;

			args >> sessionId >> clientId >> par_name >> cmd_name >> blob;
			if(sessionId != getId()/* || clientId != _localClient->getId()*/) return;

			cmdCallback(std::string(par_name),
						std::string(cmd_name),
						static_cast<const char*>(blob.data), blob.size);

		}

	protected:
		// Accessible uniquement à masterSession, builder, et handlers...
		template<typename... K>
		RemoteClient& private__createClient(K&&... args)
		{
			auto& client = _clients.createConnection(std::forward<K>(args)...);
			createRelatedPermissions(client);

			return client;
		}

		template<typename... K>
		void private__removeClient(K&&... args)
		{
			RemoteClient& client = _clients(std::forward<K>(args)...); // Faire has()
			for(Group& group : _groups)
			{
				_localPermissions.remove(group, client);
			}

			_clients.remove(std::forward<K>(args)...);
		}

		template<typename... K>
		Group& private__createGroup(K&&... args)
		{
			Group& group = _groups.createGroup(std::forward<K>(args)...);
			for(RemoteClient& client : _clients)
			{
				_localPermissions.create(group, client);
			}

			_localPermissions.create(group, getClient());
			return group;
		}

		template<typename... K>
		void private__removeGroup(K&&... args)
		{
			Group& group = _groups(std::forward<K>(args)...); // Faire has()
			for(RemoteClient& client : _clients)
			{
				_localPermissions.remove(group, client);
			}

			_localPermissions.remove(group, getClient());
			_groups.remove(std::forward<K>(args)...);
		}

	private:
		// For standard clients : their own permissions
		// For the master : permissions of everyone else
		PermissionManager _localPermissions;

		ClientManager _clients;
		GroupManager _groups;

		virtual void createRelatedPermissions(RemoteClient& client) = 0;
		virtual void createRelatedPermissions(Group& group) = 0;
		virtual void removeRelatedPermissions(RemoteClient& client) = 0;
		virtual void removeRelatedPermissions(Group& group) = 0;
};

using Session_p = std::unique_ptr<Session>;
