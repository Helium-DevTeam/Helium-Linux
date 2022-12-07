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

#include<spdlog/spdlog.h>

export module Helium.TerminalUI.Log;

import <ftxui/component/component.hpp>;
import <ftxui/component/component_base.hpp>;
import <ftxui/component/component_options.hpp>;
import <ftxui/component/event.hpp>;
import <ftxui/component/screen_interactive.hpp>;
import <ftxui/dom/elements.hpp>;
import <ftxui/dom/flexbox_config.hpp>;
import <ftxui/screen/color.hpp>;
import <ftxui/screen/color_info.hpp>;
import <ftxui/screen/terminal.hpp>;

import <stddef.h>;
import <array>;
import <chrono>;
import <functional>;
import <memory>;
import <string>;
import <utility>;
import <vector>;
import <format>;

import Helium.TerminalUI.Shared;
import Helium.Misc.Version;

using namespace std;
using namespace ftxui;

export namespace Helium {
	Elements log_eles;

	string cmd_input;
	auto cmd_input_com = Input(&cmd_input, "Command : ");

	Component log_tab_renderer = Renderer([&]() {return vbox({ log_eles }) | flex; });
	auto log_renderer = Scroller(Renderer([&]() {return vbox({ log_eles }) | flex | border; }));

	int GeneralLog(spdlog::level::level_enum lvl, string_view str) {
		string lvl_str;
		auto lvl_color = Color::Green;
		switch (lvl) {
		case spdlog::level::trace:
			lvl_str = "trace"s;
			lvl_color = Color::GrayLight;
			break;
		case spdlog::level::debug:
			lvl_str = "debug"s;
			lvl_color = Color::BlueLight;
			break;
		case spdlog::level::info:
			lvl_str = "info"s;
			lvl_color = Color::Green;
			break;
		case spdlog::level::warn:
			lvl_str = "warn"s;
			lvl_color = Color::YellowLight;
			break;
		case spdlog::level::err:
			lvl_str = "error"s;
			lvl_color = Color::RedLight;
			break;
		case spdlog::level::critical:
			lvl_str = "critical"s;
			lvl_color = Color::Red;
			break;
		default:
			lvl_str = "info"s;
			lvl_color = Color::Green;
			break;
		}
		log_eles.push_back(hbox({
			text(format("[{:%F} {:%T}] [", chrono::system_clock::now(), chrono::system_clock::now())),
			text(lvl_str) | color(lvl_color),
			paragraph(format("] {}", str)),
			}) | xflex);
		if (log_eles.size() > 65535) {
			while (log_eles.size() > 65535)
				log_eles.erase(log_eles.begin());
		}
		return 0;
	}
}

namespace Helium {
}