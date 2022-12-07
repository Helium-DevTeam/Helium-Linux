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

#define EXP_FUNCPTR_TYPE(ret, name, ...) ret (*fp##name)(__VA_ARGS__)
#define ADD_EXP_FUNC(namestr, name) HeliumExportFunctionMap.insert(pair<string, void*>(namestr, (void*)name))

#include<map>
#include<string>
#include<boost/dll.hpp>
#include<boost/dll/import.hpp>
#include<libintl.h>

export module Helium.ExportFunction;

import <string>;
import Helium.Commands;
import Helium.Config;
import Helium.Events;
import Helium.Exception;
import Helium.Extension;
import Helium.Internationalization;
import Helium.Logger;
import Helium.MinecraftServer;
import Helium.Misc;
import Helium.Parser;

using namespace std;
using namespace boost::dll;

export {
	namespace Helium {
		shared_library heliumapilib;
		const fs::path apipath("./extensions/HeliumAPI.dll");
		map<string, void*> HeliumExportFunctionMap;

		[[deprecated("Need refactoring")]] int InitFuncMap();
		[[deprecated("Need refactoring")]] int LoadHeliumAPI();
		[[deprecated("Need refactoring")]] int TransferFuncMap();
		[[deprecated("Need refactoring")]] int UnloadHeliumAPI();
	}
}

namespace Helium {
	static HeliumLogger log("Helium", "ExportFunction");

	int InitFuncMap() {
		log.info("{}", gettext("Building Helium API map..."));
		ADD_EXP_FUNC("HeliumExtensionDebugPrint", HeliumExtensionDebugPrint);
		ADD_EXP_FUNC("CreateExtLogger", CreateExtLogger);
		ADD_EXP_FUNC("DeleteExtLogger", DeleteExtLogger);
		ADD_EXP_FUNC("ExtLoggerDebug", ExtLoggerDebug);
		ADD_EXP_FUNC("ExtLoggerInfo", ExtLoggerInfo);
		ADD_EXP_FUNC("ExtLoggerWarn", ExtLoggerWarn);
		ADD_EXP_FUNC("ExtLoggerError", ExtLoggerError);
		ADD_EXP_FUNC("ExtLoggerCrit", ExtLoggerCrit);

		ADD_EXP_FUNC("GetHeliumVersion", GetHeliumVersion);

		//ADD_EXP_FUNC("EventIDToDesc"              , EventIDToDesc);
		//ADD_EXP_FUNC("EventIDToName"              , EventIDToName);
		//ADD_EXP_FUNC("TraceEvent"                 , TraceEvent);
		//ADD_EXP_FUNC("UntraceEvent"               , UntraceEvent);
		//ADD_EXP_FUNC("IsEventTraced"              , IsEventTraced);
		//ADD_EXP_FUNC("BlockEvent"                 , BlockEvent);
		//ADD_EXP_FUNC("UnblockEvent"               , UnblockEvent);
		//ADD_EXP_FUNC("IsEventBlocked"             , IsEventBlocked);
		//ADD_EXP_FUNC("SetEventDesc"               , SetEventDesc);
		//ADD_EXP_FUNC("SetEventName"               , SetEventName);
		//ADD_EXP_FUNC("DispatchEvent"              , DispatchEvent);
		//ADD_EXP_FUNC("RegisterEventListner"       , RegisterEventListner);
		/*
		ADD_EXP_FUNC("GetExtensionMetadata", GetExtensionMetadata);
		ADD_EXP_FUNC("GetExtensionDirectory", GetExtensionDirectory);
		ADD_EXP_FUNC("GetExtensionConfigDirectory", GetExtensionConfigDirectory);
		ADD_EXP_FUNC("GetExtensionStatus", GetExtensionStatus);
		ADD_EXP_FUNC("CreateExtension", CreateExtension);
		ADD_EXP_FUNC("InitExtension", InitExtension);
		ADD_EXP_FUNC("ReinitExtension", ReinitExtension);
		ADD_EXP_FUNC("LoadExtension", LoadExtension);
		ADD_EXP_FUNC("UnloadExtension", UnloadExtension);
		ADD_EXP_FUNC("ReloadExtension", ReloadExtension);
		ADD_EXP_FUNC("LockExtension", LockExtension);
		ADD_EXP_FUNC("UnlockExtension", UnlockExtension);
		ADD_EXP_FUNC("InitAllExtension", InitAllExtension);
		ADD_EXP_FUNC("ReinitAllExtension", ReinitAllExtension);
		ADD_EXP_FUNC("LoadAllExtension", LoadAllExtension);
		ADD_EXP_FUNC("UnloadAllExtension", UnloadAllExtension);
		ADD_EXP_FUNC("ReloadAllExtension", ReloadAllExtension);
		ADD_EXP_FUNC("LockAllExtension", LockAllExtension);
		ADD_EXP_FUNC("LockAllExtension", LockAllExtension);
		ADD_EXP_FUNC("UnlockAllExtension", UnlockAllExtension);
		ADD_EXP_FUNC("GetExtensionList", GetExtensionList);

		ADD_EXP_FUNC("GetServerList", GetServerList);
		ADD_EXP_FUNC("SendCommandToServer", SendCommandToServer);
		ADD_EXP_FUNC("GetServerStatus", GetServerStatus);
		ADD_EXP_FUNC("StartServer", _StartServer);
		ADD_EXP_FUNC("StopServer", _StopServer);
		ADD_EXP_FUNC("PauseServer", _PauseServer);
		ADD_EXP_FUNC("RestartServer", RestartServer);
		ADD_EXP_FUNC("ActivateServer", ActivateServer);
		ADD_EXP_FUNC("DeactivateServer", DeactivateServer);
		ADD_EXP_FUNC("IsServerActivated", IsServerActivated);
		ADD_EXP_FUNC("GetServerPID", GetServerPID);
		ADD_EXP_FUNC("GetServerMetadata", GetServerMetadata);
		ADD_EXP_FUNC("Execute", Execute);
		ADD_EXP_FUNC("Tell", Tell);
		ADD_EXP_FUNC("Say", Say);
		ADD_EXP_FUNC("Boardcast", Boardcast);
		ADD_EXP_FUNC("Reply", Reply);
		ADD_EXP_FUNC("Save", Reply);
		ADD_EXP_FUNC("CanDoCommandAction", CanDoCommandAction);

		ADD_EXP_FUNC("ExecuteCommand", _ExecuteCommand);
		ADD_EXP_FUNC("RegisterCommandCallback", _RegisterCommandCallback);
		ADD_EXP_FUNC("AddCommand", _AddCommand);
		ADD_EXP_FUNC("DeleteCommand", _DeleteCommand);
		ADD_EXP_FUNC("QueryCommand", _QueryCommand);
		ADD_EXP_FUNC("GetCommandTreeNodeMetadata", _GetCommandTreeNodeMetadata);
		*/
		log.info("{}", gettext("Helium API map was successfully built"));
		return 0;
	}

	int LoadHeliumAPI() {
		log.info("{}", gettext("Start loading HeliumAPI"));
		heliumapilib.load(apipath);
		if (heliumapilib.has("TransferFuncMap")) {
			log.info("{}", gettext("HeliumAPI loaded successfully!"));
			auto& transfer = heliumapilib.get<int(map<string, void*> funcmap)>("TransferFuncMap");
			transfer(HeliumExportFunctionMap);
		}
		else {
			HeliumErrorExit(true, "Can't find TransferFuncMap() in the HeliumAPI.dll", ", try to use another HeliumAPI.dll");
		}
		return 0;
	}

	int TransferFuncMap() {
		return 0;
	}

	int UnloadHeliumAPI() {
		return 0;
	}
}