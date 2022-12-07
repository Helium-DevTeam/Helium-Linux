/*
* Helium is a customizable extension system for Minecraft server.
* You can get the lastest source code and releases of Helium at :
* https:github.com/Helium-DevTeam/Helium
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
* along with Helium.  If not, see <https:www.gnu.org/licenses/>.
* ----------------------------------------------------------------------------------------
*/
module;

#include<chaiscript/chaiscript.hpp>
#include<neargye/semver.hpp>

export module Helium.Script.ChaiScript.ScriptManager;

import <chrono>;
import <filesystem>;

import Helium.Script.AbstractScriptManager;
import Helium.Logger;

using namespace std;
using namespace semver;
namespace cs = chaiscript;
namespace fs = std::filesystem;

export namespace Helium {
	class HeliumChaiScriptManager
		: public HeliumScriptManager {
	public:
		class HeliumChaiScript
			: public HeliumScriptManager::HeliumScript {
		public:
			enum HeliumChaiScriptStatus {
				HCSS_EMPTY,
				HCSS_READY,
				HCSS_ABORTED,
				HCSS_SUSPENDED,
				HCSS_ERROR_OCCURRED
			};

			HeliumChaiScript() {}
			virtual ~HeliumChaiScript() {}

			int LoadScript();
			int ExecuteScript();
			int SuspendScript();
			int AbortScript();
			int InvokeScriptFunction();

			int Load(const fs::path& script_path, const optional<tuple<string, string>>& executor) override { return 0; };
			int Unload(const optional<tuple<string, string>>& executor) override { return 0; };
			int Execute(const optional<tuple<string, string>>& executor) override { return 0; };
			int Abort(const optional<tuple<string, string>>& executor)  override { return 0; };
			int Suspend(const optional<tuple<string, string>>& executor) override { return 0; };
			int Resume(const optional<tuple<string, string>>& executor) override { return 0; };
		private:
		};

		HeliumChaiScriptManager() {}
		virtual ~HeliumChaiScriptManager() {}

		int LoadAllScript();
		int ExecuteScript(string script_name);
		int AbortScript(string script_name);
		int SuspendScript(string script_name);
		int ResumeScript(string script_name);
		HeliumChaiScript* GetScriptInstance(string script_name);
		int ConfigureAngelScriptEngine();
		bool CheckAngelScriptManager() const;
		int RegisterHeliumAPIFunction();
		int RegisterGlobalAPIFunction(const string& decl, void* api_ptr, auto call = asCALL_CDECL);

	private:
	} helium_chaiscript_manager;

	void TestChaiScriptEnvironment();
}

namespace Helium {
	HeliumLogger log("Helium", "ChaiScriptManager");
	void TestChaiScriptEnvironment() {
		try {
			cs::ChaiScript chai;
			chai.use("./scripts/helium_test_script_chai.chai");
			auto start = chrono::system_clock::now();
			chai.eval("helium_main()");
			auto end = chrono::system_clock::now();
			log.debug("ChaiScript Execute Time : {}ms", chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		}
		catch (exception& e) {
			log.error(e.what());
		}
	}
}