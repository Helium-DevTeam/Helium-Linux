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

export module Helium.Events;

import Helium.Logger;
import Helium.Misc.UUIDManager;
import Helium.Internationalization;

import <string>;
import <iostream>;
import <vector>;
import <thread>;
import <list>;
import <any>;
import <map>;
import <eventpp/eventdispatcher.h>;
import <string_view>;
import <functional>;

using namespace std;
using namespace eventpp;

export{
	namespace Helium {
		int TestEventListener(list<any> param);
		class HeliumEventManager : public UUIDHeliumObject {
		public:
			typedef function<int(string_view, list<any>)> HeliumEventFunction;

			class HeliumEventFunctionObject;

			HeliumEventManager();
			~HeliumEventManager();

			bool IsEventExists(const string& s) const;
			int RegisterEventListener(const string& s, HeliumEventFunction func);
			int DispatchEvent(const string& s, const list<any>& param);
			void BlockEvent(const string& s);
			void UnblockEvent(const string& s);
			bool IsEventBlocked(const string& s);

		private:
			EventDispatcher<string, int(string_view, list<any>)> helium_event_dispatcher;
			unordered_map<string, bool> blocked_list;
		} helium_event_manager;
	}
}

namespace Helium {
	HeliumEventManager::HeliumEventManager() {
	}
	HeliumEventManager::~HeliumEventManager() {
	}
}

namespace Helium {
	HeliumLogger log("Helium", "Event");

	int TestEventListener(list<any> param) {
		log.trace("{}", "Enter TestEventListener(list<any> param)!");
		return 0;
	}
	bool HeliumEventManager::IsEventExists(const string& s) const {
		return this->helium_event_dispatcher.hasAnyListener(s);
	}
	int HeliumEventManager::RegisterEventListener(const string& s, HeliumEventFunction func) {
		if (func) {
			if (!s.empty()) {
				this->helium_event_dispatcher.appendListener(s, func);
			}
		}
		return -1;
	}
	int HeliumEventManager::DispatchEvent(const string& s, const list<any>& param) {
		this->helium_event_dispatcher.dispatch(s, s, param);
		//log.debug("{} {} {}", gettext("Dispatching event"), s, param.size());
		return 0;
	}
	void HeliumEventManager::BlockEvent(const string& s) {
		this->blocked_list[s] = true;
	}
	void HeliumEventManager::UnblockEvent(const string& s) {
		this->blocked_list[s] = false;
	}
	bool HeliumEventManager::IsEventBlocked(const string& s) {
		return this->blocked_list[s];
	}
}