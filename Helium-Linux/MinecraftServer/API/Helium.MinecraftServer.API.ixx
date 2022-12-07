/*
* Helium is a customizable extension system for Minecraft server.
* You can get the lastest source code and releases of Helium at :
* https://github.com/Helium-DevTeam/Helium
* ----------------------------------------------------------------------------------------
* Helium Copyright (C) 2021-2022 HeliumDevTeam
*
* This file is a part of Helium
*
* Helium is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Helium is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Helium.  If not, see <https://www.gnu.org/licenses/>.
* ----------------------------------------------------------------------------------------
*/

module;

#define WIN32_LEAN_AND_MEAN

#include<libintl.h>
#include<boost/uuid/uuid.hpp>
#include<windows.h>

export module Helium.MinecraftServer.API;

import Helium.MinecraftServer.Class;
import Helium.Logger;

import <string>;
import <memory>;
import <vector>;
import <any>;
import <list>;
import <vector>;

using namespace std;
using namespace boost::uuids;

export namespace Helium {
	int StartServer(string_view name);
	int AutoStartServer(string_view name);
	int StopServer(string_view name);
	int TerminateServer(string_view name);
	int ResumeServer(string_view name);
	int PauseServer(string_view name);

	int StartAllServer();
	int AutoStartAllServer();
	int StopAllServer();
	int TerminateAllServer();
	int ResumeAllServer();
	int PauseAllServer();
}

namespace Helium {
	HeliumLogger log("Helium", "HeliumServerAPI");

	int StartServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->StartServer();
		}
		return -1;
	}
	int AutoStartServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->AutoStartServer();
		}
		return -1;
	}
	int StopServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->StopServer();
		}
		return -1;
	}
	int TerminateServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->TerminateServer();
		}
		return -1;
	}
	int ResumeServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->ResumeServer();
		}
		return -1;
	}
	int PauseServer(string_view name) {
		if (auto ptr = helium_server_manager.GetServer(name); ptr != nullptr) {
			return ptr->PauseServer();
		}
		return -1;
	}

	int StartAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->StartServer();
		}
		return 0;
	}
	int AutoStartAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->AutoStartServer();
		}
		return 0;
	}
	int StopAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->StopServer();
		}
		return 0;
	}
	int TerminateAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->TerminateServer();
		}
		return 0;
	}
	int ResumeAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->ResumeServer();
		}
		return 0;
	}
	int PauseAllServer() {
		for (auto& ptr : helium_server_manager.GetServerList()) {
			ptr->PauseServer();
		}
		return 0;
	}
}