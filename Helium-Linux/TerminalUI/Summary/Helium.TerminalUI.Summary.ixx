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

export module Helium.TerminalUI.Summary;

import <ftxui/component/component.hpp>;
import <ftxui/component/component_base.hpp>;
import <ftxui/component/component_options.hpp>;
import <ftxui/component/event.hpp>;
import <ftxui/component/screen_interactive.hpp>;
import <ftxui/dom/elements.hpp>;
import <ftxui/dom/flexbox_config.hpp>;
import <ftxui/dom/table.hpp>;
import <ftxui/screen/color.hpp>;
import <ftxui/screen/color_info.hpp>;
import <ftxui/screen/terminal.hpp>;

import <stddef.h>;
import <array>;
import <functional>;
import <memory>;
import <string>;
import <utility>;
import <vector>;

import Helium.TerminalUI.Shared;
import Helium.Misc.Version;

using namespace std;
using namespace ftxui;

export namespace Helium {
	int summary_tab_index = 0;
	vector<string> summary_tab_entries = {
		"General", "Servers", "Extensions", "Scripts", "Functions", "Users", "About"
	};

	auto summary_general_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("General") | bold | color(Color::Red),
			});
		return vbox({ general_display });
		});

	ftxui::Table server_table = Table({ {"Server Name", "Status", "Type", "Startup Command", "UUID"} });
	void UpdateServerTable();
	auto summary_servers_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("Servers") | bold | color(Color::Red),
				//server_table.Render()
				Table(server_table).Render()
			});
		return vbox({ general_display });
		});

	auto summary_extension_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("Extensions") | bold | color(Color::Red),
			});
		return vbox({ general_display });
		});
	auto summary_script_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("Scripts") | bold | color(Color::Red),
			});
		return vbox({ general_display });
		});
	auto summary_function_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("Scripts") | bold | color(Color::Red),
			});
		return vbox({ general_display });
		});
	auto summary_user_tab_renderer = Renderer([&] {
		auto general_display = vbox({
				text("Users") | bold | color(Color::Red),
			});
		return vbox({ general_display });
		});
	auto summary_about_tab_renderer = Renderer([&] {
		auto about_display = vbox({
				text("About Helium") | bold | color(Color::Green),
				text(R"( _   _      _ _)"),
				text(R"(| | | | ___| (_)_   _ _ __ ___)"),
				text(R"(| |_| |/ _ \ | | | | | '_ ` _ \)"),
				text(R"(|  _  |  __/ | | |_| | | | | | |)"),
				text(R"(|_| |_|\___|_|_|\__,_|_| |_| |_|)"),
				text("Helium " + GetHeliumVersion().to_string() + ", Copyright(C) Helium DevTeam 2020-2022"),
				text("This program comes with ABSOLUTELY NO WARRANTY;"),
				text("for details type \'#Helium show warranty\'."),
				text("This is free software, and you are welcome to redistribute it"),
				text("under certain conditions; type \'#Helium show conditions\' for details."),
				text("Introduction") | bold | color(Color::Green),
				paragraph("This project is inspired by Fallen-Breath/MCDReforged(https://github.com/Fallen-Breath/MCDReforged). "
							"We would like to express our sincere thanks to all contributors to the project. "
							"Helium is a extension system for Minecraft servers and it doesn't need any modify or modding to work "
							"on diverse Minecraft servers. Helium uses the dlls as extensions, which means you can use any language that can write dlls "
							"and cooperate with C++, such as C, C# or Rust, to write Helium extensions. If you cannot handle C++ or just to have some simple "
							"extensions, Helium also has a powerful script system including AngelScript, ChaiScript, and Lua. To manage all of these "
							"extensions and scripts, Helium Extension Package Manager(hepm) could be useful."
							" Helium is based on Modern C++, which means Helium is lightweight and incredibly fast. But as a trade-off, based the C++ "
							"also means poor binary portability and relatively poor maintainability. If portability is important to you, "
							"you may like to use Fallen-Breath/MCDReforged instead of using Helium."
							" For now Helium only supports Windows(x64), but we are planning to make a Linux version."
							" If you find any bugs or want to contact us, just leave a issue at our repository(https://github.com/Helium-DevTeam/Helium)."
							" Helium is licensed by GPL-3.0-or-later license, check here for more details."
							" Follow our latest design notes of Helium at the wiki page(https://github.com/Helium-DevTeam/Helium/wiki)."),
			});
		return about_display;
		});
	Component summary_tab = Menu(&summary_tab_entries, &summary_tab_index, MenuOption::Vertical());

	auto summary_tab_content = Container::Tab(
		{
			summary_general_tab_renderer,
			summary_servers_tab_renderer,
			summary_extension_tab_renderer,
			summary_script_tab_renderer,
			summary_function_tab_renderer,
			summary_user_tab_renderer,
			summary_about_tab_renderer,
		},
		&summary_tab_index);
}

namespace Helium {
	void UpdateServerTable() {
	}
}