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

#include<boost/dll.hpp>
#include<libintl.h>

export module Helium.Extension;

import <string>;
import <vector>;
import <list>;
import <any>;
import <map>;

import Helium.Misc.UUIDManager;
import Helium.Logger;
import Helium.Events;

using namespace std;
using namespace boost::dll;
using namespace boost::filesystem;

export{
	namespace Helium {
		enum ExtStat {
			EXT_STATUS_EMPTY,
			EXT_STATUS_LOADING,
			EXT_STATUS_LOADED,
			EXT_STATUS_UNLOADING,
			EXT_STATUS_UNLOADED,
			EXT_STATUS_LOCKED
		};

		void HeliumExtensionDebugPrint(string extprint);

		class HeliumExtension : public UUIDHeliumObject {
		public:
			HeliumExtension() {
				//this->extension_eventmgr = make_shared<HeliumEventManager>();
			}
			HeliumExtension(string cfgname);
			~HeliumExtension();

			int InitExt();
			int LoadExt();
			int LockExt();
			int UnlockExt();
			int UnloadExt();
			int ScanEventFunc();
			int GetExtStat() noexcept;
			string GetExtConfigPath();
			string GetExtPath();
			void* GetFuncPtr(string funcname);
			void* SetFuncPtr(string funcname, void* newptr);
			bool HasFunc(string funcname);
			string GetExtName();

			class HeliumExtensionConfig {
				friend class HeliumExtension;
			public:
				string GetExtConfigPath() {
					return configpath.string();
				}
				void PutExtConfigPath(string path) {
					this->configpath = path;
				}

				string GetExtName() {
					return configpath.string();
				}
				void PutExtName(string path) {
					this->configpath = path;
				}

				int ReadConfig();
			private:
				fs::path configpath;
				fs::path extpath;
				string extname;
			};

		private:
			HeliumExtensionConfig config;
			map<string, void*> funcs;
			int extstat;
			shared_library extins;
			//shared_ptr<HeliumEventManager> extension_eventmgr;
			bool isinited;
		};

		vector<shared_ptr<HeliumExtension>> extensions;

		weak_ptr<HeliumExtension> GetExtensionPointerByName(const string& ext_name);
		list<any> GetExtensionMetadata(const string& ext_name);
		string GetExtensionWorkingDirectory(const string& ext_name);
		string GetExtensionConfigDirectory(const string& ext_name);
		int GetExtensionStatus(const string& ext_name);

		list<any> CreateExtension(const string& ext_config_path);
		int InitExtension(const string& ext_name);
		int ReinitExtension(const string& ext_name);
		int LoadExtension(const string& ext_name);
		int UnloadExtension(const string& ext_name);
		int ReloadExtension(const string& ext_name);
		int LockExtension(const string& ext_name);
		int UnlockExtension(const string& ext_name);

		int FindAllExtensionConfig();
		int InitAllExtension();
		int ReinitAllExtension();
		int LoadAllExtension();
		int UnloadAllExtension();
		int ReloadAllExtension();
		int LockAllExtension();
		int UnlockAllExtension();

		vector<string> GetExtensionList(int mask = -1);
	}
}

namespace Helium {
	static HeliumLogger log("Helium", "ExtensionManager");

	void HeliumExtensionDebugPrint(string extprint) {
		log.info("{} : {}", gettext("Debug print from extension"), extprint);
	}

	int HeliumExtension::HeliumExtensionConfig::ReadConfig() {
		return 0;
	}

	HeliumExtension::HeliumExtension(string cfgname) {
		this->extstat = EXT_STATUS_EMPTY;
		this->config.configpath = fs::path(cfgname);
		log.info("{} {}", gettext("Reading extension config file"), this->config.configpath.filename().string());
		if (auto ret = this->config.ReadConfig(); ret != 0)
			return;
		this->extstat = EXT_STATUS_UNLOADED;
		//this->extension_eventmgr = make_shared<HeliumEventManager>();
		this->isinited = true;
		return;
	}
	HeliumExtension::~HeliumExtension() {
		this->extstat = EXT_STATUS_EMPTY;
		return;
	}
	int HeliumExtension::LoadExt() {
		this->extstat = EXT_STATUS_LOADING;
		typedef int (*funcptr)();
		log.info("{} {}({})", gettext("Loading extension"), this->config.extpath.string(), this->config.extname);
		try {
			this->extins.load(this->config.extpath);
		}
		catch (exception& e) {
			log.critical("{} {} {}, {}", gettext("Extension"),
				this->config.extname,
				gettext("loading failed with a exception"),
				gettext("the extension will not be loaded"));
			this->extstat = EXT_STATUS_EMPTY;
			return -1;
		}
		try {
			this->ScanEventFunc();

			list<any> temp_param;
			any temp_any;
			temp_any = this->GetExtName();
			temp_param.push_back(temp_any);
			temp_any = this->GetExtPath();
			temp_param.push_back(temp_any);
			temp_any = this->GetExtConfigPath();
			temp_param.push_back(temp_any);
			//this->extension_eventmgr->DispatchEvent(HeliumEventList::EXTENSION_LOAD, temp_param);
		}
		catch (exception& e) {
			log.error("{} {} {}, {}", gettext("Event listener function scanner for extension"),
				this->config.extname,
				e.what(),
				gettext("The extension's behaviour might be incorrect."));
		}
		this->extstat = EXT_STATUS_LOADED;
		return 0;
	}
	int HeliumExtension::InitExt() {
		if (this->extstat == EXT_STATUS_LOADED) return -1;
		this->extstat = EXT_STATUS_EMPTY;
		auto cfgname = this->config.configpath;
		this->config.configpath = fs::path(cfgname);
		log.info("{}", gettext("Reading extension config file"), this->config.configpath.filename().string());
		if (auto ret = this->config.ReadConfig(); ret != 0)
			return -1;
		log.info("{}", gettext("Done"));
		this->extstat = EXT_STATUS_UNLOADED;
		//this->extension_eventmgr = make_shared<HeliumEventManager>();
		this->isinited = true;
		return 0;
	}
	int HeliumExtension::LockExt() {
		return 0;
	}
	int HeliumExtension::UnloadExt() {
		this->extstat = EXT_STATUS_UNLOADING;
		list<any> temp_param;
		any temp_any;
		temp_any = this->GetExtName();
		temp_param.push_back(temp_any);
		temp_any = this->GetExtPath();
		temp_param.push_back(temp_any);
		temp_any = this->GetExtConfigPath();
		temp_param.push_back(temp_any);
		//this->extension_eventmgr->DispatchEvent(HeliumEventList::EXTENSION_UNLOAD, temp_param);
		//this->extstat = EXT_STATUS_UNLOADED;
		return 0;
	}
	int HeliumExtension::UnlockExt() {
		return 0;
	}
	int HeliumExtension::ScanEventFunc() {
		auto ret = 0;
		/*for (int i = HeliumEventList::EMPTY_EVENT + 1;
			i < HeliumEventList::BUILT_IN_MAX;
			i++) {
			try {
				if (this->extins.has(EventIDToListenerFunc(i))) {
					helium_event_manager.RegisterEventListener(i
						, this->extins.get<int(list<any>)>(EventIDToListenerFunc(i)));
					this->extension_eventmgr->RegisterEventListener(i
						, this->extins.get<int(list<any>)>(EventIDToListenerFunc(i)));
					ret++;
				}
			}
			catch (exception& e) {
				log << HLL::LL_ERR << "Event listener function scanner for extension " << this->config.extname
					<< " has failed with exception ";
				log << HLL::LL_ERR << e.what() << hendl;
				log << HLL::LL_ERR << "The extension's behaviour might be incorrect." << hendl;
			}
		}
		*/
		return ret;
	}
	string HeliumExtension::GetExtName() {
		return this->config.extname;
	}
	void* HeliumExtension::GetFuncPtr(string funcname) {
		if (this->funcs.count(funcname) != 0) {
			return this->funcs.at(funcname);
		}
		return nullptr;
	}
	void* HeliumExtension::SetFuncPtr(string funcname, void* newptr) {
		auto tempptr = this->funcs.at(funcname);
		this->funcs[funcname] = newptr;
		return tempptr;
	}
	bool HeliumExtension::HasFunc(string funcname) {
		if (this->funcs.count(funcname) > 0) return true;
		return false;
	}
	int HeliumExtension::GetExtStat() noexcept {
		return this->extstat;
	}
	string HeliumExtension::GetExtConfigPath() {
		return this->config.GetExtConfigPath();
	}
	string HeliumExtension::GetExtPath() {
		return this->config.extpath.string();
	}

	weak_ptr<HeliumExtension> GetExtensionPointerByName(const string& ext_name) {
		shared_ptr<HeliumExtension> nullret;
		for (auto e : extensions) {
			if (e->GetExtName() == ext_name) {
				return e;
			}
		}
		return nullret;
	}
	list<any> GetExtensionMetadata(const string& ext_name) {
		list<any> ret;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			any temp_any;
			temp_any = ptr->GetExtName();
			ret.push_back(temp_any);
			temp_any = ptr->GetExtPath();
			ret.push_back(temp_any);
			temp_any = ptr->GetExtConfigPath();
			ret.push_back(temp_any);
		}
		return ret;
	}
	string GetExtensionWorkingDirectory(const string& ext_name) {
		string ret = "";
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->GetExtPath();
		}
		return ret;
	}
	string GetExtensionConfigDirectory(const string& ext_name) {
		string ret = "";
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->GetExtConfigPath();
		}
		return ret;
	}
	int GetExtensionStatus(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->GetExtStat();
		}
		return ret;
	}

	list<any> CreateExtension(const string& ext_config_path) {
		list<any> ret;
		auto wptr = GetExtensionPointerByName(ext_config_path);
		auto ptr = wptr.lock();
		extensions.push_back(ptr);
		ret = GetExtensionMetadata(ptr->GetExtName());
		return ret;
	}
	int InitExtension(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->InitExt();
		}
		return ret;
	}
	int ReinitExtension(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			ptr->UnloadExt();
			return ptr->InitExt();
		}
		return ret;
	}
	int LoadExtension(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->LoadExt();
		}
		return ret;
	}
	int UnloadExtension(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			return ptr->UnloadExt();
		}
		return ret;
	}
	int ReloadExtension(const string& ext_name) {
		int ret = 0;
		auto wptr = GetExtensionPointerByName(ext_name);
		auto ptr = wptr.lock();
		if (ptr) {
			ptr->UnloadExt();
			ptr->InitExt();
			return ptr->LoadExt();
		}
		return ret;
	}
	int LockExtension(const string& ext_name) {
		int ret = 0;
		return ret;
	}
	int UnlockExtension(const string& ext_name) {
		int ret = 0;
		return ret;
	}

	int FindAllExtensionConfig() {
		auto ret = 0;
		vector<string> files;
		fs::path extcfgpath("./extensions/extconfigs");
		string tempstr;
		for (auto& fileiter : directory_iterator(extcfgpath)) {
			if (is_directory(fileiter)) continue;
			tempstr = fileiter.path().string();
			if (!tempstr.find(".xml")) continue;
			log.info("{} {}", gettext("Founded a extension config"), fileiter.path().filename().string());
			files.push_back(tempstr);
			ret++;
		}
		for (auto s : files) {
			log.debug(s);
			extensions.push_back(make_shared<HeliumExtension>(s));
		}
		return ret;
	}
	int InitAllExtension() {
		int ret = 0;
		for (auto e : extensions) {
			if (e->InitExt() == 0)
				ret++;
		}
		return ret;
	}
	int ReinitAllExtension() {
		int ret = 0;
		for (auto e : extensions) {
			e->UnloadExt();
			if (e->InitExt() == 0)
				ret++;
		}
		return ret;
	}
	int LoadAllExtension() {
		auto ret = 0, tot = 0;
		for (auto ext : extensions) {
			if (!ext->LoadExt()) {
				ret++;
			}
			tot++;
		}
		auto suc = (static_cast<double>(ret) / static_cast<double>(tot) * 100.0f);
		log.info("{} {}/{} {}({}%)",
			gettext("Successfully loaded"),
			ret,
			tot,
			gettext("extensions"),
			suc);
		return ret;
	}
	int UnloadAllExtension() {
		auto ret = 0, tot = 0;
		for (auto ext : extensions) {
			if (!ext->UnloadExt()) {
				ret++;
			}
			tot++;
		}
		auto suc = (static_cast<double>(ret) / static_cast<double>(tot) * 100.0f);
		log.info("{} {}/{} {}({}%)",
			gettext("Successfully unloaded"),
			ret,
			tot,
			gettext("extensions"),
			suc);
		return ret;
	}
	int ReloadAllExtension() {
		int ret = 0, tot = 0;
		for (auto e : extensions) {
			e->UnloadExt();
			if (e->LoadExt() == 0)
				ret++;
			tot++;
		}
		auto suc = (static_cast<double>(ret) / static_cast<double>(tot) * 100.0f);
		log.info("{} {}/{} {}({}%)",
			gettext("Successfully reloaded"),
			ret,
			tot,
			gettext("extensions"),
			suc);
		return ret;
	}
	int LockAllExtension() {
		int ret;
		for (auto e : extensions) {
			ret = e->LockExt();
		}
		return ret;
	}
	int UnlockAllExtension() {
		int ret;
		for (auto e : extensions) {
			ret = e->UnlockExt();
		}
		return ret;
	}
	vector<string> GetExtensionList(int mask) {
		vector<string> ret;
		if (mask == -1) {
			for (auto e : extensions) {
				ret.push_back(e->GetExtName());
			}
		}
		else {
			for (auto e : extensions) {
				if (e->GetExtStat() == mask)
					ret.push_back(e->GetExtName());
			}
		}
		return ret;
	}
}