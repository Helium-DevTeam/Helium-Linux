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

export module Helium.Misc.ClassInstanceCounter;

export namespace Helium {
	template<typename T>
	class HeliumClassInstanceCounter {
	public:
		HeliumClassInstanceCounter();
		HeliumClassInstanceCounter(const HeliumClassInstanceCounter& c);
		~HeliumClassInstanceCounter();

		static size_t Count();
	private:
		static size_t count;
	};
}

namespace Helium {
	template<typename T>
	HeliumClassInstanceCounter<T>::HeliumClassInstanceCounter() {
		++count;
	}
	template<typename T>
	HeliumClassInstanceCounter<T>::HeliumClassInstanceCounter(const HeliumClassInstanceCounter& c) {
		++count;
	}
	template<typename T>
	HeliumClassInstanceCounter<T>::~HeliumClassInstanceCounter() {
		--count;
	}
	template<typename T>
	size_t HeliumClassInstanceCounter<T>::Count() {
		return count;
	}

	template<typename T>
	size_t HeliumClassInstanceCounter<T>::count = 0;
}