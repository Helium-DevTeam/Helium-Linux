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

#include <map>
#include <vector>
#include <iostream>
#include <list>
#include <strstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <string.h>

export module Helium.Misc.Utils;

import Helium.MinecraftServer.Shared;

import <string>;

using namespace std;

export namespace Helium {
	//从字符串list生成字符串,以指定字符串分隔
	string makestring(list<string> list_, string spiliter);

	//从字符串vector生成字符串,以指定字符串分隔
	string makestring(vector<string> list_, string spiliter);

	//字符串a中含有字符串b
	bool have(string a, string b);

	string ReplaceString(string rawstr, string deststr, string repstr);

	HeliumServerType ServerTypeStrToID(string str);
	string ServerTypeIDToStr(HeliumServerType id);
}

namespace Helium {
	string ReplaceString(string rawstr, string deststr, string repstr)
	{
		string text = rawstr;
		auto iter = text.find(deststr);
		text.replace(iter, iter + deststr.size(), repstr);
		return text;
	}

	bool have(string a, string b)
	{
		if (a.find(b) != string::npos)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	string makestring(list<string> list_, string spiliter)
	{
		string result;
		for (auto iter = list_.begin(); iter != list_.end(); iter++)
		{
			result.append(iter->c_str());
			if (iter == list_.end())
			{
				//在结尾不加分隔符
			}
			else
			{
				result.append(spiliter);
			}
		}
		return result;
	}

	string makestring(vector<string> list_, string spiliter)
	{
		string result;
		for (auto iter = list_.begin(); iter != list_.end(); iter++)
		{
			result.append(iter->c_str());
			if (iter == list_.end())
			{
				//在结尾不加分隔符
			}
			else
			{
				result.append(spiliter);
			}
		}
		return result;
	}

	HeliumServerType ServerTypeStrToID(string str) {
		if (str == "beta18") return HeliumServerType::BETA18;
		if (str == "bukkit") return HeliumServerType::BUKKIT;
		if (str == "bukkit14") return HeliumServerType::BUKKIT14;
		if (str == "bungeecord") return HeliumServerType::BUNGEECORD;
		if (str == "cat") return HeliumServerType::CAT;
		if (str == "forge") return HeliumServerType::FORGE;
		if (str == "vanilla") return HeliumServerType::VANILLA;
		if (str == "waterfall") return HeliumServerType::WATERFALL;
		if (str == "velocity") return HeliumServerType::VELOCITY;
		return HeliumServerType::VANILLA;
	}

	string ServerTypeIDToType(HeliumServerType id) {
		switch (id) {
		case HeliumServerType::BETA18:
			return "beta18";
			break;
		case HeliumServerType::BUKKIT:
			return "bukkit";
			break;
		case HeliumServerType::BUKKIT14:
			return "bukkit14";
			break;
		case HeliumServerType::BUNGEECORD:
			return "bungeecord";
			break;
		case HeliumServerType::CAT:
			return "cat";
			break;
		case HeliumServerType::FORGE:
			return "forge";
			break;
		case HeliumServerType::VANILLA:
			return "vanilla";
			break;
		case HeliumServerType::WATERFALL:
			return "waterfall";
			break;
		case HeliumServerType::VELOCITY:
			return "velocity";
		default:
			return "vanilla";
			break;
		}
	}
}