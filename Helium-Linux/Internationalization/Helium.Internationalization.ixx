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

export module Helium.Internationalization;

import <clocale>;
import <locale>;
import <cstdlib>;
import <string>;

using namespace std;

export {
	namespace Helium {
		void i18nInit(const string& locale = "");
	}
}

namespace Helium {
	void i18nInit(const string& locale) {
		//setlocale(LC_ALL, locale.c_str());
		bindtextdomain("Helium", "./lang/");
		textdomain("Helium");
	}
}