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

export module Helium.Commands.Shared;

import <map>;
import <string>;
import <string_view>;

import <brigadier-plusplus/include/single-header/brigadier.hpp>;

import Helium.Capabilities;

using namespace std;
using namespace brigadier;

export namespace Helium {
	enum class CommandSource {
		EMPTY,
		CONSOLE,
		PLAYER,
		REMOTE,
		API,
		CUSTOM
	};

	class HeliumCommandSource {
	public:
		HeliumCommandSource() : source(CommandSource::EMPTY), custom_hcap(HeliumCapabilities::HCAP_EMPTY) {}
		HeliumCommandSource(CommandSource s, const map<string, string>& i, hcap_t cap = HeliumCapabilities::HCAP_EMPTY) : source(s), source_info(i), custom_hcap(cap) {}

		CommandSource GetSource() const { return this->source; }
		auto GetSourceInfo() const { return this->source_info; }
		hcap_t GetCapabilities() const {
			if (this->source == CommandSource::PLAYER) {
				return helium_user_manager.GetPlayerCap(this->source_info.at("player_name")).GetCapabilities();
			}
			else if (this->source == CommandSource::CONSOLE or this->source == CommandSource::REMOTE) {
				return HeliumCapabilities::HCAP_FULLCTL;
			}
			return this->custom_hcap;
		}

		void SetSource(CommandSource s) { this->source = s; }
		void SetInfo(const map<string, string>& i) { this->source_info = i; }
		void SetCustomCapabilities(hcap_t c) { this->custom_hcap = c; }
	private:
		CommandSource source;
		map<string, string> source_info;
		hcap_t custom_hcap;
	};

	using HeliumCommandDispatcher = CommandDispatcher<HeliumCommandSource>;
	using HeliumCommandContext = CommandContext<HeliumCommandSource>;
}