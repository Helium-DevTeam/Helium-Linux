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

export module Helium.Script.AbstractScriptManager;

import <optional>;
import <filesystem>;

import Helium.Misc.UUIDManager;

using namespace std;
namespace fs = std::filesystem;

export namespace Helium {
	class HeliumScriptManager : public UUIDHeliumObject {	//‘§¡ÙŒª÷√(bushi
	public:
		HeliumScriptManager() {}
		virtual ~HeliumScriptManager() {}

		class HeliumScript : public UUIDHeliumObject {
		public:
			HeliumScript() {}
			virtual ~HeliumScript() {}

			virtual int Load(const fs::path& script_path, const optional<tuple<string, string>>& executor) { return 0; }
			virtual int Unload(const optional<tuple<string, string>>& executor) { return 0; }
			virtual int Execute(const optional<tuple<string, string>>& executor) { return 0; }
			virtual int Abort(const optional<tuple<string, string>>& executor) { return 0; }
			virtual int Suspend(const optional<tuple<string, string>>& executor) { return 0; }
			virtual int Resume(const optional<tuple<string, string>>& executor) { return 0; }
		};
	private:
	};
	using HeliumScript = HeliumScriptManager::HeliumScript;
}

namespace Helium {
}