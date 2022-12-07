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

#include<compare>
#include<angelscript.h>

export module Helium.Script.AngelScript.Callback;

import <vector>;
import <fstream>;
import <string>;

import Helium.Logger;

using namespace std;

export {
	namespace Helium {
		void MessageCallback(const asSMessageInfo* msg, void* param);
	}
}



namespace Helium {
	static HeliumLogger log("Helium", "HeliumAngelScriptCallback");

	void MessageCallback(const asSMessageInfo* msg, void* param)
	{
		auto msg_type = err;
		if (msg->type == asMSGTYPE_WARNING)
			msg_type = warn;
		else if (msg->type == asMSGTYPE_INFORMATION)
			msg_type = info;

		string mes = msg->message, sec = msg->section;
		int row = msg->row, col = msg->col;

		log.log(msg_type, "{} ({}, {}) : {}", static_cast<string>(sec), static_cast<int>(row), static_cast<int>(col), static_cast<string>(mes));

		if (msg_type == err) {
			int linecount = 0;
			string tmp;
			ifstream ifs;
			ifs.open(msg->section);
			if (ifs.fail()) {
				return;
			}
			else {
				for (int i = 0; i < msg->row; i++) {
					if (!getline(ifs, tmp, '\n'))
						return;
				}
				while (tmp.find_first_of(' ') == 0 || tmp.find_first_of('\t') == 0)
					tmp.erase(tmp.begin());
				log.error("{}", tmp);
				string tempfmt;
				tempfmt.insert(0, " ", col - 1);
				tempfmt.append("{}");
				log.error(tempfmt, "^");
			}
		}
	}
}