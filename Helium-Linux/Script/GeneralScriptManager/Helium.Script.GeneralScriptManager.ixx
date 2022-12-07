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

#include <libintl.h>
#include <boost/uuid/uuid.hpp>

export module Helium.Script.GeneralScriptManager;

import <map>;
import <string>;
import <optional>;
import <tuple>;
import <variant>;
import <memory>;

import Helium.Script.AbstractScriptManager;
import Helium.Logger;

using namespace std;

export namespace Helium {
	class HeliumGeneralScriptManager : public HeliumScriptManager {
	public:
		using GeneralScriptID = boost::uuids::uuid;
		using GeneralScriptPointer = shared_ptr<HeliumScript>;

		enum class HeliumScriptType {
			ANGELSCRIPT_SCRIPT,
			ANGELSCRIPT_LIBRARY,
			CHAISCRIPT_SCRIPT,
			CHAISCRIPT_LIBRARY,
			LUA_SCRIPT,
			LUA_LIBRARY
		};

		int RegisterScript(GeneralScriptPointer ptr, HeliumScriptType scr_type, string_view name) {
			auto n = static_cast<string>(name);
			if (this->id_map.count(n) > 0)
				return -1;
			this->id_map[n] = ptr->GetUUID();
			this->type_map[ptr->GetUUID()] = scr_type;
			this->scr_map[ptr->GetUUID()] = ptr;
			return 0;
		}
		bool HasName(string_view name) {
			if (this->id_map.count((static_cast<string>(name))) > 0) return true;
			return false;
		}
		int Execute(string_view name, const optional<tuple<string, string>>& executor);
	private:
		map<string, GeneralScriptID> id_map;
		map<GeneralScriptID, HeliumScriptType> type_map;
		map<GeneralScriptID, GeneralScriptPointer> scr_map;
	}helium_script_manager;

	using HeliumScriptType = HeliumGeneralScriptManager::HeliumScriptType;
}

namespace Helium {
	HeliumLogger log("Helium", "Script");

	int HeliumGeneralScriptManager::Execute(string_view name, const optional<tuple<string, string>>& executor) {
		auto n = static_cast<string>(name);
		if (this->id_map.count(n) == 0) {
			log.error("{} \"{}\"", gettext("Try to execute a non-exist or unloaded script"), n);
			return -1;
		}
		auto id = this->id_map[n];
		return this->scr_map[id]->Execute(executor);
	}
}