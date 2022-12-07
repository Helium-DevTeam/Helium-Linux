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

export module Helium.Parser.AbstractParser;

import <map>;
import <string>;
import <string_view>;
import <regex>;
import <tuple>;

using namespace std;

export namespace Helium {
	class AbstractParser {
	public:
		AbstractParser() {}
		[[nodiscard]] virtual tuple<string, map<string, string>> Parse(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerStarting(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerStarted(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerPlayerChat(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerOverload(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerStopping(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParseServerStopped(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParsePlayerLogin(string_view raw) = 0;
		[[nodiscard]] virtual tuple<bool, map<string, string>> ParsePlayerLogout(string_view raw) = 0;
		[[nodiscard]] virtual string GetServerPureOutput(string_view raw) = 0;
		[[nodiscard]] virtual string GetServerStop() = 0;
		[[nodiscard]] virtual int GetServerLogLevel(string_view raw) = 0;
	};
}