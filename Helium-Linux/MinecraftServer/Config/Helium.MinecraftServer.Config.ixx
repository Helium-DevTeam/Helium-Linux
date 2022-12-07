/*
* Helium is a customizable extension system for Minecraft server.
* You can get the lastest source code and releases of Helium at :
* https:github.com/Helium-DevTeam/Helium
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
* along with Helium.  If not, see <https:www.gnu.org/licenses/>.
* ----------------------------------------------------------------------------------------
*/
module;

#include<libintl.h>
#include<toml.hpp>

export module Helium.MinecraftServer.Config;

import Helium.MinecraftServer.Class;
import Helium.MinecraftServer.API;
import Helium.Logger;
import Helium.Misc.Utils;

import <string>;
import <filesystem>;
import <string_view>;
import <memory>;

using namespace std;
namespace fs = std::filesystem;

export namespace Helium {
	class HeliumServerConfig final {
	public:
		HeliumServerConfig(string_view s) {
			this->server_config_path = s;
		}
		int ReadServerConfig();
	private:
		fs::path server_config_path;
	} helium_server_config("server.toml");
}

namespace Helium {
	static HeliumLogger log("Helium", "ServerConfig");
	int HeliumServerConfig::ReadServerConfig() {
		auto actu = 0, tot = 0;
		toml::value value;
		vector<toml::table> server_tbl;
		try {
			value = toml::parse(this->server_config_path);
			server_tbl = toml::find<vector<toml::table>>(value, "server");
		}
		catch (exception& e) {
			log.error("{} ({})", gettext("Exception when reading the server config"), e.what());
		}
		for (auto& ele : server_tbl) {
			tot++;
			try {
				auto server_ptr = make_shared<HeliumServer>();
				server_ptr->SetServerName(static_cast<string>(ele["name"].as_string()));
				log.info("{} \"{}\"", gettext("Loading server"), static_cast<string>(ele["name"].as_string()));
				server_ptr->SetServerDirectory(static_cast<string>(ele["directory"].as_string()));
				server_ptr->SetStartCommand(static_cast<string>(ele["start_command"].as_string()));
				server_ptr->SetServerType(static_cast<HeliumServerType>(ServerTypeStrToID(ele["type"].as_string())));
				server_ptr->SetAutoStart(static_cast<bool>(ele["auto_start"].as_boolean()));
				server_ptr->SetOutputVisible(static_cast<bool>(ele["show_output"].as_boolean()));
				server_ptr->RCON().SetRCONEnable(ele["rcon_enable"].as_boolean());
				server_ptr->RCON().SetRCONPort(ele["rcon_port"].as_integer());
				server_ptr->RCON().SetRCONPassword(static_cast<string>(ele["rcon_password"].as_string()));
				helium_server_manager.AddServer(server_ptr);
				actu++;
			}
			catch (exception& e) {
				log.error("{} ({})", gettext("Exception when reading the server config"), e.what());
			}
		}
		log.info("{} {}/{}({}%) {}", gettext("Loaded"), actu, tot, static_cast<float>(actu) / static_cast<float>(tot) * 100.0f, gettext("server informations"));
		return actu;
	}
}