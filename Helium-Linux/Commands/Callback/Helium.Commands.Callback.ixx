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

export module Helium.Commands.Callback;

import <vector>;
import <string>;
import <brigadier-plusplus/include/single-header/brigadier.hpp>;

import Helium.Logger;
import Helium.Events;
import Helium.Extension;
import Helium.ExtensionManager;
import Helium.Misc.Version;
import Helium.MinecraftServer;
import Helium.Script;
import Helium.Commands.Shared;
import Helium.TerminalUI;

using namespace std;
using namespace brigadier;

export namespace Helium {
	int helium_extension_list(HeliumCommandContext& ctx);
	int helium_extension_status(HeliumCommandContext& ctx);
	int helium_extension_load(HeliumCommandContext& ctx);
	int helium_extension_unload(HeliumCommandContext& ctx);
	int helium_extension_reload(HeliumCommandContext& ctx);
	int helium_extension_enable(HeliumCommandContext& ctx);
	int helium_extension_disable(HeliumCommandContext& ctx);

	int helium_script_list(HeliumCommandContext& ctx);
	int helium_script_status(HeliumCommandContext& ctx);
	int helium_script_run(HeliumCommandContext& ctx);
	int helium_script_abort(HeliumCommandContext& ctx);
	int helium_script_suspend(HeliumCommandContext& ctx);
	int helium_script_resume(HeliumCommandContext& ctx);
	int helium_script_load(HeliumCommandContext& ctx);
	int helium_script_unload(HeliumCommandContext& ctx);
	int helium_script_environment(HeliumCommandContext& ctx);

	int helium_server_list(HeliumCommandContext& ctx);
	int helium_server_status(HeliumCommandContext& ctx);
	int helium_server_start(HeliumCommandContext& ctx);
	int helium_server_stop(HeliumCommandContext& ctx);
	int helium_server_restart(HeliumCommandContext& ctx);

	int helium_user_list(HeliumCommandContext& ctx);
	int helium_user_group_list(HeliumCommandContext& ctx);
	int helium_user_set(HeliumCommandContext& ctx);
	int helium_user_group_set_inheritable(HeliumCommandContext& ctx);
	int helium_user_group_set_banned(HeliumCommandContext& ctx);

	int debug_event_create(HeliumCommandContext& ctx);
	int debug_event_block(HeliumCommandContext& ctx);
	int debug_event_unblock(HeliumCommandContext& ctx);
	int debug_event_trace(HeliumCommandContext& ctx);
	int debug_event_untrace(HeliumCommandContext& ctx);

	int helium_update(HeliumCommandContext& ctx);
	int helium_show_warranty(HeliumCommandContext& ctx);
	int helium_show_condition(HeliumCommandContext& ctx);
	int helium_show_copyrights(HeliumCommandContext& ctx);
	int helium_exit(HeliumCommandContext& ctx);

	int helium_help(HeliumCommandContext& ctx);

	int hepm_install(HeliumCommandContext& ctx);
	int hepm_remove(HeliumCommandContext& ctx);
	int hepm_list(HeliumCommandContext& ctx);
	int hepm_search(HeliumCommandContext& ctx);
	int hepm_fix(HeliumCommandContext& ctx);
}

namespace Helium {
	HeliumLogger log("Helium", "BuiltinCommands");
}

namespace Helium {
	int helium_extension_list(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_status(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_load(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_unload(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_reload(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_enable(HeliumCommandContext& ctx) { return 0; }
	int helium_extension_disable(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int helium_script_list(HeliumCommandContext& ctx) { return 0; }
	int helium_script_status(HeliumCommandContext& ctx) { return 0; }
	int helium_script_run(HeliumCommandContext& ctx) {
		string a = ctx.GetArgumentOr<GreedyString>("[script-name]", "");
		helium_script_manager.Execute(a, optional<tuple<string, string>>(make_tuple("", "")));
		return 0;
	}
	int helium_script_abort(HeliumCommandContext& ctx) { return 0; }
	int helium_script_suspend(HeliumCommandContext& ctx) { return 0; }
	int helium_script_resume(HeliumCommandContext& ctx) { return 0; }
	int helium_script_load(HeliumCommandContext& ctx) { return 0; }
	int helium_script_unload(HeliumCommandContext& ctx) { return 0; }
	int helium_script_environment(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int helium_server_list(HeliumCommandContext& ctx) {
		string a = ctx.GetArgumentOr<GreedyString>("[server-status]", "");
		auto list = helium_server_manager.GetServerList();
		return 0;
	}
	int helium_server_status(HeliumCommandContext& ctx) { return 0; }
	int helium_server_start(HeliumCommandContext& ctx) { return 0; }
	int helium_server_stop(HeliumCommandContext& ctx) { return 0; }
	int helium_server_restart(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int helium_user_list(HeliumCommandContext& ctx) { return 0; }
	int helium_user_group_list(HeliumCommandContext& ctx) { return 0; }
	int helium_user_set(HeliumCommandContext& ctx) { return 0; }
	int helium_user_group_set_inheritable(HeliumCommandContext& ctx) { return 0; }
	int helium_user_group_set_banned(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int debug_event_create(HeliumCommandContext& ctx) { return 0; }
	int debug_event_block(HeliumCommandContext& ctx) { return 0; }
	int debug_event_unblock(HeliumCommandContext& ctx) { return 0; }
	int debug_event_trace(HeliumCommandContext& ctx) { return 0; }
	int debug_event_untrace(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int helium_update(HeliumCommandContext& ctx) { return 0; }
	int helium_show_warranty(HeliumCommandContext& ctx) {
		log.info("Disclaimer of Warranty.");
		log.info("THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.");
		log.info("EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER");
		log.info("PARTIES PROVIDE THE PROGRAM ¡°AS IS¡± WITHOUT WARRANTY OF ANY KIND, EITHER");
		log.info("EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF");
		log.info("MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS TO THE");
		log.info("QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE");
		log.info("DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.");
		return 0;
	}
	int helium_show_condition(HeliumCommandContext& ctx) {
		log.info("IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY");
		log.info("COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS THE PROGRAM");
		log.info("AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,");
		log.info("INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE");
		log.info("PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED");
		log.info("INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM");
		log.info("TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN");
		log.info("ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.");
		return 0;
	}
	int helium_show_copyrights(HeliumCommandContext& ctx) {
		log.info(R"( _   _      _ _)");
		log.info(R"(| | | | ___| (_)_   _ _ __ ___)");
		log.info(R"(| |_| |/ _ \ | | | | | '_ ` _ \)");
		log.info(R"(|  _  |  __/ | | |_| | | | | | |)");
		log.info(R"(|_| |_|\___|_|_|\__,_|_| |_| |_|)");
		log.info("{} {}", gettext("Helium"), GetHeliumVersion().to_string());
		log.info("{}", gettext("Copyright(C) 2020-2022 Helium DevTeam"));
		log.info("{}", gettext("This program comes with ABSOLUTELY NO WARRANTY;"));
		log.info("{}", gettext("for details type \'#Helium show warranty\'."));
		log.info("{}", gettext("This is free software, and you are welcome to redistribute it"));
		log.info("{}", gettext("under certain conditions; type \'#Helium show conditions\' for details."));
		return 0;
	}
	int helium_exit(HeliumCommandContext& ctx) {
		helium_event_manager.DispatchEvent("helium.__internal__.lifecycle.finalize", {});
		return 0;
	}
}

namespace Helium {
	int helium_help(HeliumCommandContext& ctx) { return 0; }
}

namespace Helium {
	int hepm_install(HeliumCommandContext& ctx) { return 0; }
	int hepm_remove(HeliumCommandContext& ctx) { return 0; }
	int hepm_list(HeliumCommandContext& ctx) { return 0; }
	int hepm_search(HeliumCommandContext& ctx) { return 0; }
	int hepm_fix(HeliumCommandContext& ctx) { return 0; }
}