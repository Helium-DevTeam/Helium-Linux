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

export module Helium.Parser;

export import Helium.Parser.AbstractParser;
export import Helium.Parser.Beta18Parser;
export import Helium.Parser.Bukkit14Parser;
export import Helium.Parser.BukkitParser;
export import Helium.Parser.BungeeCordParser;
export import Helium.Parser.CatParser;
export import Helium.Parser.ForgeParser;
export import Helium.Parser.VanillaParser;
export import Helium.Parser.VelocityParser;
export import Helium.Parser.WaterfallParser;

import <memory>;

import Helium.MinecraftServer.Shared;

using namespace std;

export namespace Helium {
	shared_ptr<AbstractParser> GetParserPtr(HeliumServerType server_type) {
		switch (server_type) {
			/*case BETA18:
				return static_pointer_cast<AbstractParser>(make_shared<Beta18Parser>());
			case BUKKIT:
				return static_pointer_cast<AbstractParser>(make_shared<BukkitParser>());
			case BUKKIT14:
				return static_pointer_cast<AbstractParser>(make_shared<Bukkit14Parser>());
			case BUNGEECORD:
				return static_pointer_cast<AbstractParser>(make_shared<BungeeCordParser>());
			case CAT:
				return static_pointer_cast<AbstractParser>(make_shared<CatParser>());
			case FORGE:
				return static_pointer_cast<AbstractParser>(make_shared<ForgeParser>());*/
		case HeliumServerType::VANILLA:
			return static_pointer_cast<AbstractParser>(make_shared<VanillaParser>());
			/*case WATERFALL:
				return static_pointer_cast<AbstractParser>(make_shared<WaterfallParser>());
			case VELOCITY:
				return static_pointer_cast<AbstractParser>(make_shared<VelocityParser>());*/
		default:
			return static_pointer_cast<AbstractParser>(make_shared<VanillaParser>());
		}
	}
}