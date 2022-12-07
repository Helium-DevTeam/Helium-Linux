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

#include <libintl.h>

export module Helium.TerminalUI.Global;

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

import <stddef.h>;    // for size_t
import <array>;       // for array
import <atomic>;      // for atomic
import <chrono>;      // for operator""s, chrono_literals
import <cmath>;       // for sin
import <functional>;  // for ref, reference_wrapper, function
import <memory>;      // for allocator, shared_ptr, __shared_ptr_access
import <string>;      // for string, basic_string, operator+, to_string, char_traits
import <thread>;      // for sleep_for, thread
import <utility>;     // for move
import <vector>;      // for vector
import <list>;
import <any>;

import Helium.TerminalUI.Extensions;
import Helium.TerminalUI.Functions;
import Helium.TerminalUI.Log;
import Helium.TerminalUI.Scripts;
import Helium.TerminalUI.Servers;
import Helium.TerminalUI.Shared;
import Helium.TerminalUI.Summary;
import Helium.TerminalUI.Users;

import Helium.Events;
import Helium.Logger;

import Helium.Misc.Version;

using namespace std;
using namespace ftxui;

export namespace Helium {
	int StartHeliumTUI();
	int StopHeliumTUI(string_view event_name, list<any> param);
}

namespace Helium {
	HeliumLogger log("Helium", "TerminalUI");

	shared_ptr<thread> refresh_thread;
	atomic<bool> refresh_ui_continue;
	auto screen = ScreenInteractive::Fullscreen();

	int StopHeliumTUI(string_view event_name, list<any> param) {
		log.info("Stopping Helium Teminal UI");
		refresh_ui_continue = false;
		refresh_thread->join();
		screen.ExitLoopClosure()();

		return 0;
	}

	int StartHeliumTUI() {
		int shift = 0;

		int tab_index = 0;
		vector<string> tab_entries = {
			"Logs", "Summary", "Servers", "Extensions", "Scripts", "Functions", "Users"
		};
		auto menu_option = MenuOption::HorizontalAnimated();
		menu_option.underline.follower_delay = 70ms;
		auto tab_selection =
			Menu(&tab_entries, &tab_index, menu_option);

		auto tab_content = Container::Tab(
			{
				log_renderer,
				Window("Summary", Container::Horizontal({
							summary_tab | border | vscroll_indicator | frame,
							summary_tab_content | border | flex
						})),
				Empty(),
				Empty(),
				Empty(),
				Empty(),
				Empty(),
			},
			&tab_index);

		string input_cmd;
		auto input_option = InputOption();
		input_option.on_enter = [&]() {
			helium_event_manager.DispatchEvent("helium.input.console", { input_cmd });
			helium_event_manager.DispatchEvent("helium.input.general", { "console"s, input_cmd });
			input_cmd = "";
		};
		Component input_cmd_com = Input(&input_cmd, "Command : ", input_option);

		auto main_container = Container::Vertical({
			tab_selection,
			Container::Horizontal({
					tab_content,
					input_cmd_com,
				})
			});

		auto main_renderer = Renderer(main_container, [&] {
			return vbox({
				tab_content->Render() | flex,
				hbox({
					tab_selection->Render() | border,
					input_cmd_com->Render() | flex | border,
					})
				});
			});

		refresh_ui_continue = true;

		refresh_thread = make_shared<thread>([&] {
			while (refresh_ui_continue) {
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(0.05s);
				// The |shift| variable belong to the main thread. `screen.Post(task)`
				// will execute the update on the thread where |screen| lives (e.g. the
				// main thread). Using `screen.Post(task)` is threadsafe.
				// After updating the state, request a new frame to be drawn. This is done
				// by simulating a new "custom" event to be handled.
				screen.Post(Event::Custom);
			}
			log.info(gettext("Exiting Terminal UI refresh thread"));
			});

		screen.Loop(main_renderer);
		return 0;
	}
}