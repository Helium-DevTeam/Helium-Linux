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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <libintl.h>
#include <toml.hpp>

export module Helium.Capabilities;

import Helium.Logger;
import Helium.Misc.UUIDManager;

import <memory>;
import <map>;
import <string>;
import <filesystem>;
import <string_view>;
import <tuple>;

using namespace std;
namespace fs = std::filesystem;

export namespace Helium {
	using hcap_t             = unsigned long long int;
	enum HeliumCapabilities : hcap_t {
		HCAP_EMPTY           = 0,
		HCAP_EXECUTE_CMD     = 1 << 0,
		HCAP_HELIUM_INPUT    = 1 << 1,
		HCAP_HELIUM_OUTPUT   = 1 << 2,
		HCAP_BASIC_CTL       = HCAP_EXECUTE_CMD | HCAP_HELIUM_INPUT | HCAP_HELIUM_OUTPUT,
		HCAP_INSTALL_EXT     = 1 << 3,
		HCAP_UNINSTALL_EXT   = 1 << 4,
		HCAP_UPDATE_EXT      = 1 << 5,
		HCAP_EDIT_HEPM_SRC   = 1 << 6,
		HCAP_HEPM_CTL        = HCAP_INSTALL_EXT | HCAP_UNINSTALL_EXT | HCAP_UPDATE_EXT | HCAP_EDIT_HEPM_SRC,
		HCAP_EXT_LOAD        = 1 << 7,
		HCAP_EXT_UNLOAD      = 1 << 8,
		HCAP_EXT_RELOAD      = 1 << 9,
		HCAP_EXT_ENABLE      = 1 << 10,
		HCAP_EXT_DISABLE     = 1 << 11,
		HCAP_EXT_STAT        = 1 << 12,
		HCAP_EXT_CTL         = HCAP_EXT_LOAD | HCAP_EXT_UNLOAD | HCAP_EXT_RELOAD | HCAP_EXT_ENABLE | HCAP_EXT_DISABLE | HCAP_EXT_STAT,
		HCAP_EXT_FULLCTL     = HCAP_HEPM_CTL | HCAP_EXT_CTL,
		HCAP_SERVER_START    = 1 << 13,
		HCAP_SERVER_STOP     = 1 << 14,
		HCAP_SERVER_RESTART  = 1 << 15,
		HCAP_SERVER_STAT     = 1 << 16,
		HCAP_SERVER_CTL      = HCAP_SERVER_START | HCAP_SERVER_STOP | HCAP_SERVER_RESTART | HCAP_SERVER_STAT,
		HCAP_SCRIPT_STAT     = 1 << 17,
		HCAP_SCRIPT_EXECUTE  = 1 << 18,
		HCAP_SCRIPT_ABORT    = 1 << 19,
		HCAP_SCRIPT_PAUSE    = 1 << 20,
		HCAP_SCRIPT_RESUME   = 1 << 21,
		HCAP_SCRIPT_LOAD     = 1 << 22,
		HCAP_SCRIPT_UNLOAD   = 1 << 23,
		HCAP_SCRIPT_CTL      = HCAP_SCRIPT_ABORT | HCAP_SCRIPT_EXECUTE | HCAP_SCRIPT_LOAD | HCAP_SCRIPT_PAUSE | HCAP_SCRIPT_PAUSE | HCAP_SCRIPT_RESUME | HCAP_SCRIPT_UNLOAD,
		HCAP_USER_CREATE     = 1 << 24,
		HCAP_USER_DELETE     = 1 << 25,
		HCAP_USER_CAP_MODIFY = 1 << 26,
		HCAP_USER_LIST       = 1 << 27,
		HCAP_USER_CTL        = HCAP_USER_CAP_MODIFY | HCAP_USER_CREATE | HCAP_USER_DELETE | HCAP_USER_LIST,
		HCAP_DEBUG           = 1 << 28,
		HCAP_UPDATE          = 1 << 29,
		HCAP_EXIT            = 1 << 30,
		HCAP_GUEST           = HCAP_BASIC_CTL,
		HCAP_ADMIN           = HCAP_BASIC_CTL | HCAP_EXT_CTL | HCAP_SERVER_CTL | HCAP_SCRIPT_CTL,
		HCAP_FULLCTL         = HCAP_BASIC_CTL | HCAP_EXT_FULLCTL | HCAP_SCRIPT_CTL | HCAP_SERVER_CTL | HCAP_USER_CTL | HCAP_UPDATE | HCAP_GUEST
	};//need 24 bit

	class HeliumUserManager final {
	public:
		using HeliumUserID = boost::uuids::uuid;
		using HeliumUserGroupID = boost::uuids::uuid;

		class HeliumUserGroup;
		class HeliumCapability;
		class HeliumUser;

		HeliumUserManager(string_view cfg_name) { this->cfg_path = cfg_name; }
		[[nodiscard]] int ReadCapabilitiesConfig();
		[[nodiscard]] HeliumCapability GetPlayerCap(string_view name);
		[[nodiscard]] HeliumUserID GetUserIDByName(string_view name);
		[[nodiscard]] HeliumUserGroupID GetUserGroupIDByName(string_view name);
		[[nodiscard]] HeliumCapability GetUserCap(const HeliumUserID& user_id);
		[[nodiscard]] tuple<HeliumCapability, HeliumCapability> GetUserGroupCap(const HeliumUserGroupID& group_id);
		[[nodiscard]] static hcap_t CapabilitiyStringToEnum(string_view s);
		[[nodiscard]] static bool CheckCapability(const hcap_t& have, const hcap_t& required);

		class HeliumCapability {
		public:
			HeliumCapability(hcap_t cap) { this->inner_cap = cap; }
			hcap_t Inherit(const HeliumCapability& cap) {
				this->inner_cap |= cap.GetCapabilities();
				return this->inner_cap;
			}
			hcap_t Ban(const HeliumCapability& cap) {
				hcap_t a = this->inner_cap & cap.GetCapabilities();
				hcap_t b = ~this->inner_cap;
				hcap_t c = a ^ b;
				this->inner_cap = ~c;
				return this->inner_cap;
			}
			[[nodiscard]] hcap_t GetCapabilities() const { return this->inner_cap; }
			hcap_t operator=(hcap_t c) {
				this->inner_cap = c;
				return this->inner_cap;
			}

			[[nodiscard]] bool CheckCapabilities(hcap_t cap) const {
				return false;
			}
		private:
			hcap_t inner_cap;
		};
		class HeliumUserGroup : public UUIDHeliumObject {
		public:
			HeliumUserGroup() : inherit(HeliumCapability(HCAP_EMPTY)), ban(HeliumCapability(HCAP_EMPTY)), name("") {}

			[[nodiscard]] HeliumCapability GetInheritableCapabilities() const { return this->inherit; }
			[[nodiscard]] HeliumCapability GetBannedCapabilities() const { return this->ban; }
			[[nodiscard]] string GetName() const { return this->name; }
			void SetInheritableCapabilities(hcap_t c) { this->inherit = c; }
			void SetBannedCapabilities(hcap_t c) { this->ban = c; }
			void SetName(string_view s) { this->name = s; }
		private:
			HeliumCapability inherit;
			HeliumCapability ban;
			string name;
		};
		class HeliumUser : public UUIDHeliumObject {
		public:
			HeliumUser() : cap(HeliumCapability(HCAP_EMPTY)), user_group(nullptr) {}

			[[nodiscard]] bool TryLogin(string_view name, string_view pwd) const {
				if (name == this->user_name && pwd == this->password)
					return true;
				return false;
			}
			[[nodiscard]] HeliumCapability GetCapabilities() const {
				auto ret = this->cap;
				ret.Inherit(this->user_group->GetInheritableCapabilities());
				ret.Ban(this->user_group->GetBannedCapabilities());
				return ret;
			}
			[[nodiscard]] string GetPassword() const { return this->password; }
			[[nodiscard]] string GetUserName() const { return this->user_name; }
			[[nodiscard]] string GetUserGroupName() const { return this->group_name; }
			[[nodiscard]] shared_ptr<HeliumUserGroup> GetUserGroup() const { return this->user_group; }
			void SetCapabilities(hcap_t c) { this->cap = c; }
			void SetPassword(string_view s) { this->password = s; }
			void SetUserName(string_view s) { this->user_name = s; }
			void SetUserGroup(shared_ptr<HeliumUserGroup> ptr) { this->user_group = ptr; }
			void SetUserGroupName(string_view s) { this->group_name = s; }
		private:
			shared_ptr<HeliumUserGroup> user_group;
			HeliumCapability cap;
			string user_name;
			string password;
			string group_name;
		};
	private:
		map<HeliumUserID, shared_ptr<HeliumUser>> user_map;
		map<HeliumUserGroupID, shared_ptr<HeliumUserGroup>> user_group_map;
		map<string, HeliumUserGroupID> group_id_map;
		map<string, HeliumUserID> user_id_map;
		map<string, HeliumUserID> player_map;
		fs::path cfg_path;
	} helium_user_manager("user.toml");

	using HeliumCapability = HeliumUserManager::HeliumCapability;
	using HeliumUserID = HeliumUserManager::HeliumUserID;
	using HeliumUserGroupID = HeliumUserManager::HeliumUserGroupID;
}

namespace Helium {
	HeliumLogger log("Helium", "Capability");
	HeliumCapability HeliumUserManager::GetPlayerCap(string_view name) {
		if (this->player_map.count(static_cast<string>(name)) > 0) {
			return this->user_map[this->player_map[static_cast<string>(name)]]->GetCapabilities();
		}
		return HeliumCapability(HCAP_EMPTY);
	}
	int HeliumUserManager::ReadCapabilitiesConfig() {
		log.info("{}", gettext("Start reading Helium user config"));
		auto user_actu = 0, user_tot = 0, group_actu = 0, group_tot = 0;
		toml::value value;
		vector<toml::table> group_tbl, user_tbl;
		try {
			value = toml::parse(this->cfg_path);
			group_tbl = toml::find<vector<toml::table>>(value, "user_group");
			user_tbl = toml::find<vector<toml::table>>(value, "user");
		}
		catch (exception& e) {
			log.error("{} ({})", gettext("Exception when reading the user config"), e.what());
		}
		for (auto& ele : group_tbl) {
			group_tot++;
			try {
				shared_ptr<HeliumUserGroup> group_ptr = make_shared<HeliumUserGroup>();
				auto& inherit_tbl = ele["inheritable"].as_array(), & banned_tbl = ele["banned"].as_array();
				hcap_t inherit = HCAP_EMPTY, banned = HCAP_EMPTY;
				group_ptr->SetName(static_cast<string>(ele["name"].as_string()));
				for (auto& inh_ele : inherit_tbl) {
					inherit |= HeliumUserManager::CapabilitiyStringToEnum(static_cast<string>(inh_ele.as_string()));
				}
				for (auto& ban_ele : banned_tbl) {
					inherit |= HeliumUserManager::CapabilitiyStringToEnum(static_cast<string>(ban_ele.as_string()));
				}
				group_ptr->SetBannedCapabilities(banned);
				group_ptr->SetInheritableCapabilities(inherit);
				this->user_group_map[group_ptr->GetUUID()] = group_ptr;
				this->group_id_map[group_ptr->GetName()] = group_ptr->GetUUID();
				group_actu++;
			}
			catch (exception& e) {
				log.error("{} ({})", gettext("Exception when reading the user config"), e.what());
			}
		}
		for (auto& ele : user_tbl) {
			user_tot++;
			try {
				shared_ptr<HeliumUser> user_ptr = make_shared<HeliumUser>();
				auto& cap_tbl = ele["capabilities"].as_array();
				auto& ply_tbl = ele["player"].as_array();
				hcap_t cap = HCAP_EMPTY;
				user_ptr->SetUserName(static_cast<string>(ele["name"].as_string()));
				user_ptr->SetUserGroupName(static_cast<string>(ele["user_group"].as_string()));
				user_ptr->SetUserGroup(this->user_group_map[this->group_id_map[static_cast<string>(ele["user_group"].as_string())]]);
				for (auto& inh_ele : cap_tbl) {
					cap |= HeliumUserManager::CapabilitiyStringToEnum(static_cast<string>(inh_ele.as_string()));
				}
				for (auto& inh_ele : ply_tbl) {
					this->player_map[static_cast<string>(inh_ele.as_string())] = user_ptr->GetUUID();
				}
				user_ptr->SetCapabilities(cap);
				this->user_map[user_ptr->GetUUID()] = user_ptr;
				this->user_id_map[user_ptr->GetUserName()] = user_ptr->GetUUID();
				user_actu++;
			}
			catch (exception& e) {
				log.error("{} ({})", gettext("Exception when reading the user config"), e.what());
			}
		}
		log.info("{} {}/{}({}%) {}", gettext("Loaded"), group_actu, group_tot, static_cast<float>(group_actu) / static_cast<float>(group_tot) * 100.0f, gettext("user group informations"));
		log.info("{} {}/{}({}%) {}", gettext("Loaded"), user_actu, user_tot, static_cast<float>(user_actu) / static_cast<float>(user_tot) * 100.0f, gettext("user informations"));
		return user_actu;
	}
	hcap_t HeliumUserManager::CapabilitiyStringToEnum(string_view s) {
		if (s == "HCAP_EMPTY") return HCAP_EMPTY;
		if (s == "HCAP_EXECUTE_CMD") return HCAP_EXECUTE_CMD;
		if (s == "HCAP_HELIUM_INPUT") return HCAP_HELIUM_INPUT;
		if (s == "HCAP_HELIUM_OUTPUT") return HCAP_HELIUM_OUTPUT;
		if (s == "HCAP_BASIC_CTL") return HCAP_BASIC_CTL;
		if (s == "HCAP_INSTALL_EXT") return HCAP_INSTALL_EXT;
		if (s == "HCAP_UNINSTALL_EXT") return HCAP_UNINSTALL_EXT;
		if (s == "HCAP_UPDATE_EXT") return HCAP_UPDATE_EXT;
		if (s == "HCAP_EDIT_HEPM_SRC") return HCAP_EDIT_HEPM_SRC;
		if (s == "HCAP_HEPM_CTL") return HCAP_HEPM_CTL;
		if (s == "HCAP_EXT_LOAD") return HCAP_EXT_LOAD;
		if (s == "HCAP_EXT_UNLOAD") return HCAP_EXT_UNLOAD;
		if (s == "HCAP_EXT_RELOAD") return HCAP_EXT_RELOAD;
		if (s == "HCAP_EXT_ENABLE") return HCAP_EXT_ENABLE;
		if (s == "HCAP_EXT_DISABLE") return HCAP_EXT_DISABLE;
		if (s == "HCAP_EXT_STAT") return HCAP_EXT_STAT;
		if (s == "HCAP_EXT_CTL") return HCAP_EXT_CTL;
		if (s == "HCAP_EXT_FULLCTL") return HCAP_EXT_FULLCTL;
		if (s == "HCAP_SERVER_START") return HCAP_SERVER_START;
		if (s == "HCAP_SERVER_STOP") return HCAP_SERVER_STOP;
		if (s == "HCAP_SERVER_RESTART") return HCAP_SERVER_RESTART;
		if (s == "HCAP_SERVER_STAT") return HCAP_SERVER_STAT;
		if (s == "HCAP_SERVER_CTL") return HCAP_SERVER_CTL;
		if (s == "HCAP_SCRIPT_STAT") return HCAP_SCRIPT_STAT;
		if (s == "HCAP_SCRIPT_EXECUTE") return HCAP_SCRIPT_EXECUTE;
		if (s == "HCAP_SCRIPT_ABORT") return HCAP_SCRIPT_ABORT;
		if (s == "HCAP_SCRIPT_PAUSE") return HCAP_SCRIPT_PAUSE;
		if (s == "HCAP_SCRIPT_RESUME") return HCAP_SCRIPT_RESUME;
		if (s == "HCAP_SCRIPT_LOAD") return HCAP_SCRIPT_LOAD;
		if (s == "HCAP_SCRIPT_UNLOAD") return HCAP_SCRIPT_UNLOAD;
		if (s == "HCAP_SCRIPT_CTL") return HCAP_SCRIPT_CTL;
		if (s == "HCAP_USER_CREATE") return HCAP_USER_CREATE;
		if (s == "HCAP_USER_DELETE") return HCAP_USER_DELETE;
		if (s == "HCAP_USER_CAP_MODIFY") return HCAP_USER_CAP_MODIFY;
		if (s == "HCAP_USER_LIST") return HCAP_USER_LIST;
		if (s == "HCAP_USER_CTL") return HCAP_USER_CTL;
		if (s == "HCAP_GUEST") return HCAP_GUEST;
		if (s == "HCAP_ADMIN") return HCAP_ADMIN;
		if (s == "HCAP_FULLCTL") return HCAP_FULLCTL;
		return HCAP_EMPTY;
	}
	bool HeliumUserManager::CheckCapability(const hcap_t& have, const hcap_t& required) {
		hcap_t combined = have & required;
		if ((have & combined) == have)
			return true;
		return false;
	}
	HeliumUserManager::HeliumUserID
		HeliumUserManager::GetUserIDByName(string_view name) {
		if (this->user_id_map.count(static_cast<string>(name)) > 0)
			return this->user_id_map[static_cast<string>(name)];
		return boost::uuids::nil_generator()();
	}
	HeliumUserManager::HeliumUserGroupID
		HeliumUserManager::GetUserGroupIDByName(string_view name) {
		if (this->group_id_map.count(static_cast<string>(name)) > 0)
			return this->group_id_map[static_cast<string>(name)];
		return boost::uuids::nil_generator()();
	}
	HeliumUserManager::HeliumCapability
		HeliumUserManager::GetUserCap(const HeliumUserID& user_id) {
		if (this->user_map.count(user_id) > 0)
			return this->user_map[user_id]->GetCapabilities();
		return HeliumUserManager::HeliumCapability(HCAP_EMPTY);
	}
	tuple<HeliumUserManager::HeliumCapability, HeliumUserManager::HeliumCapability>
		HeliumUserManager::GetUserGroupCap(const HeliumUserGroupID& group_id) {
		if (this->user_group_map.count(group_id) > 0)
			return make_tuple<HeliumUserManager::HeliumCapability, HeliumUserManager::HeliumCapability>
			(this->user_group_map[group_id]->GetInheritableCapabilities(), this->user_group_map[group_id]->GetBannedCapabilities());
		return make_tuple<HeliumUserManager::HeliumCapability, HeliumUserManager::HeliumCapability>(HCAP_EMPTY, HCAP_EMPTY);
	}
}