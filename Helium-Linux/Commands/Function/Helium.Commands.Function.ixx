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

export module Helium.Commands.Function;

import <map>;
import <filesystem>;
import <memory>;

import Helium.Misc.UUIDManager;
import Helium.Commands.Manager;
import Helium.Config;
import Helium.Logger;

using namespace std;
namespace fs = std::filesystem;

export namespace Helium {
	class HeliumFunctionManager {
	public:
		using HeliumFunctionID = boost::uuids::uuid;

		int LoadAllHeliumFunction();

		class HeliumFunction : public UUIDHeliumObject {
		};

	private:
		map<HeliumFunctionID, shared_ptr<HeliumFunction>> func_map;
		map<string, HeliumFunctionID> id_map;
	}helium_function_manager;

	using HeliumFunction = HeliumFunctionManager::HeliumFunction;
}

namespace Helium {
	HeliumLogger log("Helium", "Function");

	int HeliumFunctionManager::LoadAllHeliumFunction() {
		for (auto& p : fs::recursive_directory_iterator(helium_config_manager.GetFunctionDir())) {
			auto func_path = p.path();
			if (func_path.has_filename() && func_path.has_extension() && func_path.extension() == ".heliumfunc") {
				log.info("{} \"{}\"", gettext("Find a Helium function"), func_path.string());
			}
		}
		return 0;
	}
}