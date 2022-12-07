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

#define SOL_EXCEPTIONS_SAFE_PROPAGATION 1
#define SOL_ALL_SAFETIES_ON 1
#include<sol/sol.hpp>
#include<libintl.h>
#include<neargye/semver.hpp>
#include<boost/uuid/uuid.hpp>

export module Helium.Script.LuaScript.ScriptManager;

import <string>;
import <memory>;
import <tuple>;
import <vector>;
import <filesystem>;
import <map>;
import <list>;
import <any>;
import <chrono>;
import <optional>;

import Helium.Config;
import Helium.Events;
import Helium.Logger;
import Helium.Misc.UUIDManager;
import Helium.Script.AbstractScriptManager;
import Helium.Script.GeneralScriptManager;
import Helium.Script.LuaScript.API;
import Helium.Misc.Version;

using namespace std;
using namespace semver;
namespace fs = std::filesystem;

export namespace Helium {
	void TestLuaEnvironment();

	class HeliumLuaScriptManager : public HeliumScriptManager {
	public:
		using LuaScriptID = boost::uuids::uuid;

		HeliumLuaScriptManager();
		virtual ~HeliumLuaScriptManager();

		tuple<int, int> LoadAllScripts();

		class HeliumLuaScript : public HeliumScriptManager::HeliumScript {
		public:
			HeliumLuaScript(bool is_lib = false) : is_lib(is_lib), helium_version_req(">=0.0.1"s) {};
			virtual ~HeliumLuaScript() {};

			int Load(const fs::path& script_path, const optional<tuple<string, string>>& executor) override;
			int Unload(const optional<tuple<string, string>>& executor) override { return 0; };
			int Execute(const optional<tuple<string, string>>& executor) override;
			int Abort(const optional<tuple<string, string>>& executor)  override { return 0; };
			int Suspend(const optional<tuple<string, string>>& executor) override { return 0; };
			int Resume(const optional<tuple<string, string>>& executor) override { return 0; };

			int RunHeliumMain(const optional<tuple<string, string>>& executor);

			static string GetScriptType(const fs::path& script_path);
			static string GetScriptName(const fs::path& script_path);
		private:
			int __inner_PrepareLuaEnv();
			int __inner_ReadMetadata();
			int __inner_RegisterHeliumAPI();
			int __inner_PrepareHeliumAPIEnv();
			int __inner_LoadScript();
			int __inner_RegisterEventListener();
			sol::state lua_env;
			shared_ptr<HeliumLogger> logger;
			fs::path scr_path;
			semver::range::detail::range helium_version_req;
			string scr_name;
			bool is_lib;
		};

	private:
		map<LuaScriptID, shared_ptr<HeliumLuaScript>> scr_map;
		map<string, LuaScriptID> id_map;
	} helium_lua_manager;
}

namespace Helium {
	HeliumLogger log("Helium", "LuaManager");
	void DebugPrint(string s) {
		log.debug(s);
	}
	void TestLuaEnvironment() {
		auto start = chrono::system_clock::now();

		if (!helium_config_manager.GetTestScriptEnv())
			return;
		log.info("{}", gettext("Start testing Lua environment"));
		sol::state L;
		try {
			L.open_libraries(
				sol::lib::base,
				sol::lib::package,
				sol::lib::coroutine,
				sol::lib::string,
				sol::lib::os,
				sol::lib::math,
				sol::lib::table,
				sol::lib::debug,
				sol::lib::bit32,
				sol::lib::io,
				sol::lib::utf8);
			//L.open_libraries("./scripts/helium_test_module.lua");
			L.set_function("DebugPrint", DebugPrint);
			L.script_file("./scripts/helium_test_script_lua.lua");
			start = chrono::system_clock::now();
			L["helium_main"]();
		}
		catch (exception& e) {
			log.error(e.what());
		}

		auto end = chrono::system_clock::now();
		log.debug("Lua Execute Time : {}ms", chrono::duration_cast<std::chrono::microseconds>(end - start).count());
	}
}

namespace Helium {
	HeliumLuaScriptManager::
		HeliumLuaScriptManager() {
	}
	HeliumLuaScriptManager::
		~HeliumLuaScriptManager() {
	}
}

namespace Helium {
	tuple<int, int> HeliumLuaScriptManager::
		LoadAllScripts() {
		log.info("{} ({} {})", gettext("Helium Lua ScriptManager"), gettext("Lua"), "5.4.4");
		int lib_tot = 0, lib_actu = 0, scr_tot = 0, scr_actu = 0, tot = 0;
		vector<fs::path> lua_lib_paths, lua_scr_paths;
		for (auto& p : fs::recursive_directory_iterator(helium_config_manager.GetScriptDir())) {
			if (p.path().has_extension() && p.path().has_filename() && p.path().extension().string() == ".lua") {
				tot++;
				string type = HeliumLuaScript::GetScriptType(p);
				if (type == "script") {
					log.info("{} \"{}\"", gettext("Found a Helium Lua Script File"), p.path().string());
					scr_tot++;
					lua_scr_paths.push_back(p);
				}
				if (type == "library") {
					log.info("{} \"{}\"", gettext("Found a Helium Lua Library File"), p.path().string());
					lib_tot++;
					lua_lib_paths.push_back(p);
				}
			}
		}

		try {
			for (auto& p : lua_lib_paths) {
				shared_ptr<HeliumLuaScript> lib_ptr = make_shared<HeliumLuaScript>(true);
				auto name = HeliumLuaScript::GetScriptName(p);
				if (this->id_map.count(name) == 0 && !helium_script_manager.HasName(name)) {
					this->scr_map[lib_ptr->GetUUID()] = lib_ptr;
					this->id_map[name] = lib_ptr->GetUUID();
					helium_script_manager.RegisterScript(static_pointer_cast<HeliumScript>(lib_ptr), HeliumScriptType::LUA_LIBRARY, name);
					lib_actu++;
				}
				else {
					log.warning("{} {}, {}", gettext("There is already a script named"), name, gettext("the script will not be loaded"));
				}
			}
		}
		catch (exception& e) {
			log.error("{} \"{}\"", gettext("Error loading Helium Lua Library"), e.what());
		}

		try {
			for (auto& p : lua_scr_paths) {
				shared_ptr<HeliumLuaScript> scr_ptr = make_shared<HeliumLuaScript>(false);
				auto name = HeliumLuaScript::GetScriptName(p);
				if (this->id_map.count(name) == 0 && !helium_script_manager.HasName(name)) {
					this->scr_map[scr_ptr->GetUUID()] = scr_ptr;
					this->id_map[HeliumLuaScript::GetScriptName(p)] = scr_ptr->GetUUID();
					helium_script_manager.RegisterScript(static_pointer_cast<HeliumScript>(scr_ptr), HeliumScriptType::LUA_LIBRARY, name);
					scr_ptr->Load(p, optional<tuple<string, string>>());
					scr_actu++;
				}
				else {
					log.warning("{} {}, {}", gettext("There is already a script named"), name, gettext("the script will not be loaded"));
				}
			}
		}
		catch (exception& e) {
			log.error("{} \"{}\"", gettext("Error loading Helium Lua Script"), e.what());
		}

		log.info("{} {}/{}({}%) {} {} {}/{}({}%) {}",
			gettext("Loaded"),
			scr_actu,
			scr_tot,
			static_cast<float>(scr_actu) / static_cast<float>(scr_tot) * 100.0f,
			gettext("Helium Lua Scripts"),
			gettext("and"),
			lib_actu,
			lib_tot,
			static_cast<float>(lib_actu) / static_cast<float>(lib_tot) * 100.0f,
			gettext("Helium Lua Libraries"));

		return make_tuple(scr_actu, lib_actu);
	}
}

namespace Helium {
	string HeliumLuaScriptManager::HeliumLuaScript::
		GetScriptType(const fs::path& script_path) {
		sol::state L;
		try {
			L.open_libraries(
				sol::lib::base,
				sol::lib::package,
				sol::lib::coroutine,
				sol::lib::string,
				sol::lib::os,
				sol::lib::math,
				sol::lib::table,
				sol::lib::debug,
				sol::lib::bit32,
				sol::lib::io,
				sol::lib::utf8);
			L.script_file(script_path.string());
			return L["helium_script_metadata"]["file_type"];
		}
		catch (...) {
			return "library";
		}
		return "library";
	}
	string HeliumLuaScriptManager::HeliumLuaScript::
		GetScriptName(const fs::path& script_path) {
		sol::state L;
		try {
			L.open_libraries(
				sol::lib::base,
				sol::lib::package,
				sol::lib::coroutine,
				sol::lib::string,
				sol::lib::os,
				sol::lib::math,
				sol::lib::table,
				sol::lib::debug,
				sol::lib::bit32,
				sol::lib::io,
				sol::lib::utf8);
			L.script_file(script_path.string());
			return L["helium_script_metadata"]["script_name"];
		}
		catch (...) {
			return "";
		}
		return "";
	}

	int HeliumLuaScriptManager::HeliumLuaScript::
		Load(const fs::path& script_path, const optional<tuple<string, string>>& executor) {
		try {
			this->scr_path = script_path;
			this->__inner_PrepareLuaEnv();
			this->__inner_LoadScript();
			this->__inner_ReadMetadata();
			this->__inner_PrepareHeliumAPIEnv();
			this->__inner_RegisterHeliumAPI();
			this->__inner_RegisterEventListener();
			helium_event_manager.DispatchEvent("helium.script.lua.load", { this->scr_name, this->is_lib });
			//this->RunHeliumMain("console");
		}
		catch (exception& e) {
			log.error("{} {} \"{}\"", gettext("Error loading Helium Lua File"), this->scr_path.string(), e.what());
			throw e;
		}
		catch (...) {
			log.error("{} {}", gettext("Unrecognized error loading Helium Lua File"), this->scr_path.string());
			throw;
		}
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		Execute(const optional<tuple<string, string>>& executor) {
		auto ret = this->RunHeliumMain(executor);
		helium_event_manager.DispatchEvent("helium.script.lua.execute", { this->scr_name, ret });
		return ret;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		RunHeliumMain(const optional<tuple<string, string>>& executor) {
		if (this->is_lib) {
			log.error("{}", gettext("Trying to run a non-runnable Lua library"));
		}
		int ret = this->lua_env["helium_main"](static_cast<string>(""));	//need fix
		helium_event_manager.DispatchEvent("helium.script.lua.execute", { this->scr_name, ret });
		return ret;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_ReadMetadata() {
		string req = this->lua_env["helium_script_metadata"]["helium_version"];
		this->helium_version_req = semver::range::detail::range(req);
		if (!this->helium_version_req.satisfies(GetHeliumVersion(), true)) {
			log.error("{} {} ({} {} {} {})",
				gettext("Helium version requirement dissatisfied"),
				this->scr_path.string(),
				gettext("need Helium version"),
				req,
				gettext("but we found"),
				to_string(GetHeliumVersion()));
			throw;
			return -1;
		}
		this->scr_name = this->lua_env["helium_script_metadata"]["script_name"];
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_PrepareLuaEnv() {
		this->lua_env.open_libraries(
			sol::lib::base,
			sol::lib::package,
			sol::lib::coroutine,
			sol::lib::string,
			sol::lib::os,
			sol::lib::math,
			sol::lib::table,
			sol::lib::debug,
			sol::lib::bit32,
			sol::lib::io,
			sol::lib::utf8);
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_RegisterHeliumAPI() {
		this->lua_env.set_function("trace", [&](const string& out_str) {
			this->logger->trace(out_str);
			});
		this->lua_env.set_function("debug", [&](const string& out_str) {
			this->logger->debug(out_str);
			});
		this->lua_env.set_function("info", [&](const string& out_str) {
			this->logger->info(out_str);
			});
		this->lua_env.set_function("warning", [&](const string& out_str) {
			this->logger->warning(out_str);
			});
		this->lua_env.set_function("error", [&](const string& out_str) {
			this->logger->error(out_str);
			});
		this->lua_env.set_function("critical", [&](const string& out_str) {
			this->logger->critical(out_str);
			});
		this->lua_env.set_function("log", [&](const int& lvl, const string& out_str) {
			this->logger->log(static_cast<HeliumLoggerLevel>(lvl), out_str);
			});

		this->lua_env.set_function("get_helium_version", [&]() {
			return to_string(GetHeliumVersion());
			});

		this->lua_env.set_function("dispatch_event", [&](string event_name, list<any> param) {
			helium_event_manager.DispatchEvent(event_name, param);
			});
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_PrepareHeliumAPIEnv() {
		this->logger = make_shared<HeliumLogger>("Lua", this->scr_name);
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_LoadScript() {
		log.debug(this->scr_path.string());
		this->lua_env.script_file(this->scr_path.string());
		return 0;
	}
	int HeliumLuaScriptManager::HeliumLuaScript::
		__inner_RegisterEventListener() {
		return 0;
	}
}