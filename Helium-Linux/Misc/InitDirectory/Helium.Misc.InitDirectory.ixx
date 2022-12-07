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
#include<io.h>

export module Helium.Misc.InitDirectory;

import Helium.Logger;
import Helium.Exception;

import <string>;
import <filesystem>;
import <list>;
import <map>;

using namespace std;
namespace fs = std::filesystem;

export{
	namespace Helium {
		enum HeliumDirectoryInitPolicy {
			HDIP_BLOCKED,
			HDIP_CREATE_WARING,
			HDIP_CREATE_SLIENT,
			HDIP_CREATE_QUIT,
			HDIP_IGNORE_WARING,
			HDIP_IGNORE_SLIENT,
			HDIP_IGNORE_QUIT,
		};
		using HDIP = HeliumDirectoryInitPolicy;

		int InitHeliumDirectory();
		int AddHeliumDirectory(fs::path path, string hint, HeliumDirectoryInitPolicy policy);

		map<fs::path, tuple<HeliumDirectoryInitPolicy, string>> policystat;
	}
}

namespace Helium {
	HeliumLogger log("Helium", "Directory");
	int InitHeliumDirectory() {
		int ret = 0;

		for (auto& dir : policystat) {
			auto [pol, hint] = dir.second;
			auto path = dir.first;
			log.info("{} \"{}\"", gettext("Checking directory"), path.string());
			if (!fs::exists(path)) {
				switch (pol)
				{
				case Helium::HDIP_BLOCKED:
					break;
				case Helium::HDIP_CREATE_WARING:
					log.warning(hint);
					ret = fs::create_directories(path);
					break;
				case Helium::HDIP_CREATE_SLIENT:
					ret = fs::create_directories(path);
					break;
				case Helium::HDIP_CREATE_QUIT:
					ret = fs::create_directories(path);
					HeliumErrorExit(true, true, hint);
					break;
				case Helium::HDIP_IGNORE_WARING:
					log.warning(hint);
					break;
				case Helium::HDIP_IGNORE_SLIENT:
					break;
				case Helium::HDIP_IGNORE_QUIT:
					HeliumErrorExit(true, true, hint);
					break;
				default:
					break;
				}
			}
		}
		return ret;
	}
	int AddHeliumDirectory(fs::path path, string hint, HeliumDirectoryInitPolicy policy) {
		//log.debug("{} {}", gettext("Adding Helium directory"), path);
		policystat[path] = make_tuple(policy, hint);
		return 0;
	}
}