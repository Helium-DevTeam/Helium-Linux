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

#include<libintl.h>
#include<toml.hpp>

export module Helium.Config;

import <string>;
import <vector>;
import <sstream>;
import <string_view>;
import <iostream>;
import <filesystem>;

import Helium.MinecraftServer;
import Helium.Logger;
import Helium.Misc.Utils;
import Helium.Misc.UUIDManager;

using namespace std;
namespace fs = std::filesystem;

export
namespace Helium {
	class HeliumConfigManager final {
	public:
		HeliumConfigManager(string_view s);
		~HeliumConfigManager();

		auto LoadConfig();

		string GetLanguage();
		string GetEncoding();
		string GetExtensionDir();
		string GetScriptDir();
		string GetFunctionDir();
		bool GetAutoUpdate();

		vector<pair<string, string>> GetHEPMSource();

		bool GetLoggerLevelDebug();
		bool GetTestScriptEnv();
		bool GetEnableScript();
		bool GetEnableExtension();
		bool GetHEPMDependencyCheck();
		bool GetEnableDebugCommands();
	private:
		fs::path cfg_path;
	} helium_config_manager("config.toml");
}

namespace Helium {
	static HeliumLogger log("Helium", "Config");

	HeliumConfigManager::HeliumConfigManager(string_view s) {
		this->cfg_path = s;
	}
	HeliumConfigManager::~HeliumConfigManager() {
	}

	auto HeliumConfigManager::LoadConfig() {
		return toml::parse(this->cfg_path);
	}

	string HeliumConfigManager::GetLanguage() {
		string r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<string>(set, "language");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	string HeliumConfigManager::GetEncoding() {
		string r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<string>(set, "encoding");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	string HeliumConfigManager::GetExtensionDir() {
		string r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<string>(set, "extension_dir");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	string HeliumConfigManager::GetScriptDir() {
		string r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<string>(set, "script_dir");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	string HeliumConfigManager::GetFunctionDir() {
		string r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<string>(set, "function_dir");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetAutoUpdate() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& set = toml::find(data, "setting");
			r = toml::find<bool>(set, "auto_update");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}

	vector<pair<string, string>> HeliumConfigManager::GetHEPMSource() {
		vector<pair<string, string>> ret;
		try {
			auto v = this->LoadConfig();
			auto& hepm = toml::find(v, "hepm");
			auto source = toml::find<vector<toml::table>>(hepm, "source");
			for (auto& ele : source) {
				string src = ele["source"].as_string();
				string brh = ele["branch"].as_string();
				ret.push_back({ src, brh });
			}
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return ret;
	}

	bool HeliumConfigManager::GetLoggerLevelDebug() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "logger_level_debug");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetTestScriptEnv() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "test_script_env");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetEnableScript() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "enable_script");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetEnableExtension() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "enable_extension");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetHEPMDependencyCheck() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "enable_extension_pack_dependency_check");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
	bool HeliumConfigManager::GetEnableDebugCommands() {
		bool r;
		try {
			auto data = this->LoadConfig();
			auto& debug = toml::find(data, "debug");
			r = toml::find<bool>(debug, "enable_debug_commands");
		}
		catch (exception& e) {
			log.error("{}", e.what());
		}
		return r;
	}
}