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

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

export module Helium.ExtensionManager;

import Helium.Logger;

import <iostream>;
import <boost/json/src.hpp>;
import <cstdlib>;
import <string>;
import <vector>;

using namespace std;
using namespace httplib;

export
namespace Helium {
	class HeliumExtensionPackageManager {
	public:
		string GetExtensionJSONUrl(const string& ext_name);
		string GetExtensionJSON(const string& ext_name);
		int InstallExtension(const string& ext_name, const string& version = "latest");
		int UninstallExtension(const string& ext_name);
		int UpdateExtension(const string& ext_name, const string& version = "latest");
		int CheckExtensionUpdate(const string& ext_name);
		int AddSource(const string& source);
		int CheckSource(const string& source);
		int DeleteSource(const string& source);
		vector<string> GetSource() const;
	private:
		vector<string> hepm_source;
	} hepm;
}

module: private;

namespace Helium {
	string GetExtensionJSON(const string& name, const string& version) {
		string url = "https://raw.githubusercontent.com/Helium-DevTeam/Helium-Extensions-Main/main/main";
		string raw_json;
		string get = "";
		get.append(name).append("/").append(version).append("/").append(name).append(".json");
		Client cli(url);
		if (auto res = cli.Get(get)) {
			if (res->status == 200) {
				raw_json = res->body;
				return raw_json;
			}
		}
		else {
			auto err = res.error();
			throw err;
			return "";
		}
		return raw_json;
	}
	int InstallExtension(const string& name, const string& version) {
		auto raw_json = GetExtensionJSON(name, version);
		cout << raw_json << endl;
		return 0;
	}
	int UpdateExtension(const string& name) {
		return 0;
	}
	int UninstallExtension(const string& name) {
		return 0;
	}
}