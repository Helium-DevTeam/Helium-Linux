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

export module Helium.Commands.Manager;

import <string>;
import <string_view>;
import <memory>;
import <any>;
import <brigadier-plusplus/include/single-header/brigadier.hpp>;
import <map>;

import Helium.Logger;
import Helium.Misc.UUIDManager;
import Helium.Commands.Callback;
import Helium.Commands.Shared;
import Helium.Config;
import Helium.Events;

using namespace std;
using namespace brigadier;

export namespace Helium {
	class HeliumCommandManager {
	public:
		int Initialize();

		int ExecuteCommand(string_view cmd, const HeliumCommandSource& src);

		HeliumCommandDispatcher& operator()() { return this->dispatcher; }
	private:
		void __inner_InitBuiltinCommand();
		void __inner_InitDebugCommand();

		HeliumCommandDispatcher dispatcher;
	}helium_command_manager;
	int CommandInput(string_view event_name, list<any> param);
}

namespace Helium {
	HeliumLogger log("Helium", "Command");

	int HeliumCommandManager::Initialize() {
		helium_event_manager.RegisterEventListener("helium.input.console", CommandInput);
		helium_event_manager.RegisterEventListener("helium.input.general", CommandInput);
		this->__inner_InitBuiltinCommand();
		if (helium_config_manager.GetEnableDebugCommands()) {
			log.warning(gettext("Debug commands enabled"));
			this->__inner_InitDebugCommand();
		}
		return 0;
	}

	int HeliumCommandManager::ExecuteCommand(string_view cmd, const HeliumCommandSource& src) {
		auto cmd_s = static_cast<string>(cmd);
		try {
			return this->dispatcher.Execute(cmd_s, src);
		}
		catch (CommandSyntaxException& e) {
			log.error(e.What());
		}
		catch (exception& e) {
			log.error(e.what());
		}
		return 0;
	}

	void HeliumCommandManager::__inner_InitBuiltinCommand() {
		auto& d = this->dispatcher;
		auto helium = d.Register<Literal>("#helium");
		auto help = d.Register<Literal>("#help");
		auto hepm = d.Register<Literal>("#hepm");

		auto ext = helium.Then<Literal>("extension");
		auto scr = helium.Then<Literal>("script");
		auto svr = helium.Then<Literal>("server");
		auto usr = helium.Then<Literal>("user");
		auto usr_grp = helium.Then<Literal>("user-group");
		auto show = helium.Then<Literal>("show");

		ext.Then<Literal>("list").ThenOptional<Argument, String>("[extension-status]").Executes(helium_extension_list);
		ext.Then<Literal>("status").Then<Argument, GreedyString>("<extension-name>").Executes(helium_extension_list);
		ext.Then<Literal>("load").Then<Argument, GreedyString>("[extension-path]").Executes(helium_extension_list);
		ext.Then<Literal>("unload").Then<Argument, GreedyString>("[extension-name]").Executes(helium_extension_list);
		ext.Then<Literal>("reload").Then<Argument, GreedyString>("[extension-name]").Executes(helium_extension_list);
		ext.Then<Literal>("enable").Then<Argument, GreedyString>("[extension-name]").Executes(helium_extension_list);
		ext.Then<Literal>("disable").Then<Argument, GreedyString>("[extension-name]").Executes(helium_extension_list);

		scr.Then<Literal>("list").Then<Argument, String>("[script-type]").Executes(helium_script_list);
		scr.Then<Literal>("status").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_status);
		scr.Then<Literal>("run").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_run);
		scr.Then<Literal>("abort").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_abort);
		scr.Then<Literal>("suspend").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_suspend);
		scr.Then<Literal>("resume").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_resume);
		scr.Then<Literal>("load").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_load);
		scr.Then<Literal>("unload").Then<Argument, GreedyString>("[script-name]").Executes(helium_script_unload);
		scr.Then<Literal>("environment").Executes(helium_script_environment);

		svr.Then<Literal>("list").Then<Argument, String>("[serve-status]").Executes(helium_server_list);
		svr.Then<Literal>("status").Then<Argument, GreedyString>("[server-name]").Executes(helium_server_status);
		svr.Then<Literal>("start").Then<Argument, GreedyString>("[server-name]").Executes(helium_server_start);
		svr.Then<Literal>("stop").Then<Argument, GreedyString>("[server-name]").Executes(helium_server_stop);
		svr.Then<Literal>("restart").Then<Argument, GreedyString>("[server-name]").Executes(helium_server_restart);

		usr.Then<Literal>("list").Then<Argument, GreedyString>("[capabilities]").Executes(helium_user_list);
		usr.Then<Literal>("set").Then<Argument, GreedyString>("[capabilities]").Executes(helium_user_set);

		usr_grp.Then<Literal>("list").Then<Argument, GreedyString>("[capabilities]").Executes(helium_user_group_list);
		usr_grp.Then<Literal>("set").Then<Literal>("inheritable").Then<Argument, GreedyString>("[capabilities]").Executes(helium_user_group_set_inheritable);
		usr_grp.Then<Literal>("set").Then<Literal>("banned").Then<Argument, GreedyString>("[capabilities]").Executes(helium_user_group_set_banned);

		show.Then<Literal>("warranty").Executes(helium_show_warranty);
		show.Then<Literal>("condition").Executes(helium_show_condition);
		show.Then<Literal>("copyrights").Executes(helium_show_copyrights);

		help.Then<Argument, GreedyString>("[command]").Executes(helium_help);

		hepm.Then<Literal>("list").Executes(hepm_list);
		hepm.Then<Literal>("install").Then<Argument, GreedyString>("[package-name]").Executes(hepm_install);
		hepm.Then<Literal>("remove").Then<Argument, GreedyString>("[package-name]").Executes(hepm_remove);
		hepm.Then<Literal>("search").Then<Argument, GreedyString>("[package-name]").Executes(hepm_search);
		hepm.Then<Literal>("fix").Then<Argument, GreedyString>("[package-name]").Executes(hepm_fix);

		helium.Then<Literal>("update").Executes(helium_update);
		helium.Then<Literal>("exit").Executes(helium_exit);
	}
	void HeliumCommandManager::__inner_InitDebugCommand() {
		auto& d = this->dispatcher;

		auto dbg = d.Register<Literal>("#debug");

		auto ent = dbg.Then<Literal>("event");

		ent.Then<Literal>("create").Then<Argument, String>("<event-id>").Executes(debug_event_create);
		ent.Then<Literal>("block").Then<Argument, String>("<event-id>").Executes(debug_event_block);
		ent.Then<Literal>("unblock").Then<Argument, String>("<event-id>").Executes(debug_event_unblock);
		ent.Then<Literal>("trace").Then<Argument, String>("<event-id>").Executes(debug_event_trace);
		ent.Then<Literal>("untrace").Then<Argument, String>("<event-id>").Executes(debug_event_untrace);
	}

	int CommandInput(string_view event_name, list<any> param) {
		if (event_name == "helium.input.console") {
			if (!any_cast<string>(param.back()).empty()) {
				log.info(any_cast<string>(param.back()));
				helium_command_manager.ExecuteCommand(any_cast<string>(param.back()), HeliumCommandSource{});
			}
		}
		return 0;
	}
}