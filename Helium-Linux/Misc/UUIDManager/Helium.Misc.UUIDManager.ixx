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

export module Helium.Misc.UUIDManager;

import <map>;
import <memory>;
export import <boost/uuid/uuid.hpp>;
export import <boost/uuid/uuid_generators.hpp>;
export import <boost/uuid/uuid_io.hpp>;

import Helium.Logger;
import Helium.Misc.ClassInstanceCounter;

using namespace boost::uuids;
using namespace std;

export{
	namespace Helium {
		enum UUIDInfoType {
			SERVER,
			EXTENSION,
			SCRIPT,
			EVENT,
			UNDEF
		};

		struct UUIDInfo
		{
			UUIDInfoType type;
			void* infoptr;
		};

		class UUIDHeliumObject// -> HeliumManagerBaseWithUUID
			: private HeliumClassInstanceCounter<UUIDHeliumObject> {
		public:
			using HeliumClassInstanceCounter
				<UUIDHeliumObject>::Count;
			UUIDHeliumObject();
			uuid GetUUID() const;
		private:
			uuid class_uuid;
		};

		map<uuid, UUIDInfo> uuidpool;

		uuid RequestUUID(int type, void* infoptr);

		bool IsUUIDExists(uuid uuid);
		int UUIDType(uuid uuid);
		int ChangeUUIDInfo(uuid uuid, int type, void* infoptr);
	}
}

namespace Helium {
	uuid RequestUUID(int type, void* infoptr) {
		random_generator rgen;
		uuid ruuid = rgen();
		return ruuid;
	}

	bool IsUUIDExists(uuid uuid) {
		return false;
	}
	int UUIDType(uuid uuid) {
		return 0;
	}
	int ChangeUUIDInfo(uuid uuid, int type, void* infoptr) {
		return 0;
	}

	uuid UUIDHeliumObject::GetUUID() const {
		return this->class_uuid;
	}

	UUIDHeliumObject::UUIDHeliumObject() {
		random_generator rgen;
		this->class_uuid = rgen();
	}
}