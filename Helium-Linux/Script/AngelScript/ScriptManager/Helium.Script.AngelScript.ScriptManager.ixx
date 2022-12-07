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

#include<string>

//#include<AngelScript-JIT-Compiler/as_jit.h>

#include<libintl.h>
#include<neargye/semver.hpp>

export module Helium.Script.AngelScript.ScriptManager;

import <string>;
import <filesystem>;
import <any>;
import <map>;
import <vector>;
import <chrono>;
import <functional>;

import <angelscript.h>;
import <scriptany/scriptany.h>;
import <scriptarray/scriptarray.h>;
import <scriptbuilder/scriptbuilder.h>;
import <scriptdictionary/scriptdictionary.h>;
import <scriptfile/scriptfile.h>;
import <scriptfile/scriptfilesystem.h>;
import <scriptgrid/scriptgrid.h>;
import <scripthandle/scripthandle.h>;
import <scripthelper/scripthelper.h>;
import <scriptmath/scriptmath.h>;
import <scriptmath/scriptmathcomplex.h>;
import <scriptstdstring/scriptstdstring.h>;
import <weakref/weakref.h>;

import Helium.Config;
import Helium.Logger;
import Helium.Script.AbstractScriptManager;
import Helium.Script.AngelScript.API;
import Helium.Script.AngelScript.Callback;
import Helium.Misc.Version;

using namespace std;
using namespace semver;
namespace fs = std::filesystem;

export namespace Helium {
	void TestAngelScriptEnvironment();

	class HeliumAngelScriptManager
		: public HeliumScriptManager {
	public:
		class HeliumAngelScript
			: public HeliumScriptManager::HeliumScript {
		public:
			enum HeliumAngelScriptStatus {
				HASS_EMPTY,
				HASS_READY,
				HASS_ABORTED,
				HASS_SUSPENDED,
				HASS_ERROR_OCCURRED
			};

			HeliumAngelScript();
			virtual ~HeliumAngelScript();

			int LoadScript();
			int ExecuteScript();
			int SuspendScript();
			int AbortScript();
			int InvokeScriptFunction();
			CScriptArray* ConvertListAny(const list<any>& param);
			list<any> ConvertArrayAny(CScriptArray* arr);
			void SetScriptEngine(asIScriptEngine* engine_ptr);
			void SetScriptPath(const fs::path& path);

			int Load(const fs::path& script_path, const optional<tuple<string, string>>& executor) override { return 0; };
			int Unload(const optional<tuple<string, string>>& executor) override { return 0; };
			int Execute(const optional<tuple<string, string>>& executor) override { return 0; };
			int Abort(const optional<tuple<string, string>>& executor)  override { return 0; };
			int Suspend(const optional<tuple<string, string>>& executor) override { return 0; };
			int Resume(const optional<tuple<string, string>>& executor) override { return 0; };

		private:
			CScriptBuilder builder;
			asIScriptModule* mod;
			asIScriptEngine* engine;
			HeliumAngelScriptStatus status;
			string module_name;
			asIScriptContext* ctx;
			filesystem::path script_path;
			vector<function<int(list<any>)>> listeners;
		};

		HeliumAngelScriptManager();
		virtual ~HeliumAngelScriptManager();

		int LoadAllScripts();
		int ExecuteScript(string script_name);
		int AbortScript(string script_name);
		int SuspendScript(string script_name);
		int ResumeScript(string script_name);
		HeliumAngelScript* GetScriptInstance(string script_name);
		int ConfigureAngelScriptEngine();
		bool CheckAngelScriptManager() const;
		int RegisterHeliumAPIFunction();
		int RegisterGlobalAPIFunction(const string& decl, void* api_ptr, auto call = asCALL_CDECL);

	private:
		asIScriptEngine* engine;
		bool enabled;
		unordered_map<fs::path, HeliumAngelScript> script_map;
		list<fs::path> script_paths;
	} helium_angelscript_manager;
}

namespace Helium {
	static HeliumLogger log("Helium", "AngelScriptManager");

	void print(const string& in) {
		log.info(in);
	}

	void TestAngelScriptEnvironment() {
		if (!helium_config_manager.GetTestScriptEnv())
			return;
		log.info("{}", gettext("Start testing Helium AngelScript environment"));
		//Configure engine
		asIScriptEngine* engine = asCreateScriptEngine();
		//asCJITCompiler* jit = new asCJITCompiler(0);
		log.info("{} ({} {})", gettext("Helium AngelScript Manager"), gettext("AngelScript"), asGetLibraryVersion());

		int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		RegisterStdString(engine);
		RegisterScriptAny(engine);
		RegisterExceptionRoutines(engine);
		RegisterScriptArray(engine, true);
		RegisterScriptDateTime(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);
		RegisterScriptGrid(engine);
		RegisterScriptHandle(engine);
		RegisterScriptMath(engine);
		RegisterScriptMathComplex(engine);
		RegisterScriptWeakRef(engine);
		RegisterStdStringUtils(engine);

		//engine->SetEngineProperty(asEP_INCLUDE_JIT_INSTRUCTIONS, 1);
		//engine->SetJITCompiler(jit);

		//Build as script
		CScriptBuilder builder;
		r = builder.StartNewModule(engine, "Helium_Module_AsScript");
		if (r < 0) {
			log.error("{}", gettext("Unrecoverable error while starting Helium_Module_AsScript"));
			return;
		}
		r = builder.AddSectionFromFile("./scripts/helium_test_script_as.as");
		if (r < 0) {
			log.error("{}", gettext("Please correct the errors in the script and try again"));
			return;
		}
		r = builder.BuildModule();
		if (r < 0) {
			log.error("{}", gettext("Please correct the errors in the script and try again"));
			return;
		}

		//jit->finalizePages();

		//Execute script
		asIScriptModule* mod = engine->GetModule("Helium_Module_AsScript");
		asIScriptFunction* func = mod->GetFunctionByDecl("void helium_main()");
		if (func == 0) {
			log.error("{}", gettext("The AngelScript file for Helium must has a void helium_script_main_as() function"));
			return;
		}
		log.info("{}", gettext("Helium AngelScript envrionment test start"));
		asIScriptContext* ctx = engine->CreateContext();
		ctx->Prepare(func);

		auto start = chrono::system_clock::now();
		r = ctx->Execute();
		auto end = chrono::system_clock::now();

		if (r != asEXECUTION_FINISHED) {
			if (r == asEXECUTION_EXCEPTION) {
				log.error("{} {} {}", gettext("An exception"), ctx->GetExceptionString(), gettext("has occurred"));
			}
		}
		log.info("{}", gettext("Helium AngelScript envrionment tested"));
		//delete jit;

		log.debug("AngelScript Execute Time : {}ms", chrono::duration_cast<std::chrono::microseconds>(end - start).count());
		return;
	}
}

namespace Helium {
	HeliumAngelScriptManager::
		HeliumAngelScriptManager() :
		enabled(false) {
		this->engine = asCreateScriptEngine();
		int r = this->ConfigureAngelScriptEngine();
		if (r >= 0)
			this->enabled = true;
		this->RegisterHeliumAPIFunction();

		log.info("{} ({} {})", gettext("Helium AngelScript Manager"), gettext("AngelScript"), asGetLibraryVersion());
	}
	HeliumAngelScriptManager::
		~HeliumAngelScriptManager() {
		this->engine->ShutDownAndRelease();
	}

	HeliumAngelScriptManager::HeliumAngelScript::
		HeliumAngelScript() {
		this->status = HASS_EMPTY;
	}
	HeliumAngelScriptManager::HeliumAngelScript::
		~HeliumAngelScript() {
		this->status = HASS_EMPTY;
	}
}

namespace Helium {
	int HeliumAngelScriptManager::
		LoadAllScripts() {
		if (!this->enabled)
			return -1;
		int total = 0, actu = 0;
		for (auto p : fs::recursive_directory_iterator(helium_config_manager.GetScriptDir())) {
			if (p.path().has_extension() && p.path().has_filename() && p.path().extension().string() == ".as") {
				this->script_paths.push_back(p);
				total++;
				log.info("{} \"{}\"", gettext("Found a Helium AngelScript File"), p.path().string());
			}
		}
		for (auto& p : this->script_paths) {
			HeliumAngelScript tempscript;
			tempscript.SetScriptEngine(this->engine);
			tempscript.SetScriptPath(p);
			this->script_map[p] = tempscript;
			if (this->script_map[p].LoadScript() >= 0)
				actu++;
		}

		log.info("{} {}/{}({}%) {}", gettext("Loaded"), actu, total, static_cast<float>(actu) / static_cast<float>(total) * 100.0f, "Helium AngelScript File");

		return actu;
	}
	int HeliumAngelScriptManager::
		ExecuteScript(string script_name) {
		return 0;
	}
	HeliumAngelScriptManager::HeliumAngelScript* HeliumAngelScriptManager::
		GetScriptInstance(string script_name) {
		return nullptr;
	}
	int HeliumAngelScriptManager::
		ConfigureAngelScriptEngine() {
		int r = this->engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		RegisterStdString(this->engine);
		RegisterScriptAny(this->engine);
		RegisterExceptionRoutines(this->engine);
		RegisterScriptArray(this->engine, true);
		RegisterScriptDateTime(this->engine);
		RegisterScriptDictionary(this->engine);
		RegisterScriptFile(this->engine);
		RegisterScriptFileSystem(this->engine);
		RegisterScriptGrid(this->engine);
		RegisterScriptHandle(this->engine);
		RegisterScriptMath(this->engine);
		RegisterScriptMathComplex(this->engine);
		RegisterScriptWeakRef(this->engine);
		RegisterStdStringUtils(this->engine);
		return 0;
	}
	int HeliumAngelScriptManager::
		RegisterHeliumAPIFunction() {
		if (!this->enabled)
			return -1;
		//this->engine->RegisterGlobalFunction("void helium_script_debug_print(const string& output_str)"
		//	, asFUNCTION(helium_script_debug_print)
		//	, asCALL_CDECL);
		return 0;
	}
	int HeliumAngelScriptManager::
		RegisterGlobalAPIFunction(const string& decl, void* api_ptr, auto call) {
		int r = this->engine->RegisterGlobalFunction(decl.c_str(), asFUNCTION(api_ptr), call);
		if (r < 0) {
			log.error("{} \"{}\"", gettext("Error occurred when registering AngelScript Global API"), decl);
		}
		return r;
	}

	int HeliumAngelScriptManager::HeliumAngelScript::
		LoadScript() {
		log.info("{} \"{}\"", gettext("Loading Script"), script_path.string().c_str());
		this->module_name = this->script_path.filename().string();
		int r = builder.StartNewModule(this->engine,
			this->module_name.c_str());
		if (r < 0) {
			log.error("{} \"{}\"", gettext("Unrecoverable error while starting AngelScript module"), this->module_name);
			return -1;
		}
		r = builder.AddSectionFromFile(this->script_path.string().c_str());
		if (r < 0) {
			log.error("{} \"{}\" {}", "Please correct the errors in the script", this->script_path.string(), " and try again");
			return -1;
		}
		r = builder.BuildModule();
		if (r < 0) {
			log.error("{} \"{}\" {}", "Please correct the errors in the script", this->script_path.string(), " and try again");
			return -1;
		}
		this->ctx = this->engine->CreateContext();
		this->status = HASS_READY;
		return 0;
	}
	int HeliumAngelScriptManager::HeliumAngelScript::
		ExecuteScript() {
		if (this->status == HASS_EMPTY)
			return -1;
		this->mod = this->engine->GetModule(this->module_name.c_str());
		asIScriptFunction* main_func = this->mod->GetFunctionByDecl("int helium_script_main(array<any> param)");
		if (main_func == 0) {
			log.error("{}", gettext("An executable AngelScript file for Helium must has a \"int helium_script_main(array<any> param)\" function"));
			return -1;
		}
		this->ctx->Prepare(main_func);
		this->status = HASS_READY;
		int r = this->ctx->Execute();
		if (r != asEXECUTION_FINISHED) {
			if (r == asEXECUTION_EXCEPTION) {
				log.error("{} {} {}", gettext("An exceptio"), this->ctx->GetExceptionString(), gettext("has occurred"));
				this->status = HASS_ERROR_OCCURRED;
				return -1;
			}
		}
		return 0;
	}
	int HeliumAngelScriptManager::HeliumAngelScript::
		SuspendScript() {
		if (this->status == HASS_EMPTY)
			return -1;
		this->ctx->Suspend();
		this->status = HASS_SUSPENDED;
		return 0;
	}
	int HeliumAngelScriptManager::HeliumAngelScript::
		AbortScript() {
		if (this->status == HASS_EMPTY)
			return -1;
		this->ctx->Abort();
		this->status = HASS_ABORTED;
		return 0;
	}
	int HeliumAngelScriptManager::HeliumAngelScript::
		InvokeScriptFunction() {
		if (this->status == HASS_EMPTY)
			return -1;
		return 0;
	}
	void HeliumAngelScriptManager::HeliumAngelScript::
		SetScriptEngine(asIScriptEngine* engine_ptr) {
		this->engine = engine_ptr;
	}
	void HeliumAngelScriptManager::HeliumAngelScript::
		SetScriptPath(const fs::path& path) {
		this->script_path = path;
	}
}