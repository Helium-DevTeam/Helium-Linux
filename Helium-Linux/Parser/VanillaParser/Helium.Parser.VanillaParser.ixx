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

#include <spdlog/spdlog.h>

export module Helium.Parser.VanillaParser;

import <map>;
import <string>;
import <string_view>;
import <regex>;
import <tuple>;

import Helium.Parser.AbstractParser;
import Helium.Logger;

using namespace std;

export namespace Helium {
	class VanillaParser : public AbstractParser {
	public:
		VanillaParser() {}
		[[nodiscard]] tuple<string, map<string, string>> Parse(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerStarting(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerStarted(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerPlayerChat(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerOverload(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerStopping(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParseServerStopped(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParsePlayerLogin(string_view raw) override;
		[[nodiscard]] tuple<bool, map<string, string>> ParsePlayerLogout(string_view raw) override;
		[[nodiscard]] string GetServerPureOutput(string_view raw) override;
		[[nodiscard]] string GetServerStop() override { return "stop"s; }
		[[nodiscard]] int GetServerLogLevel(string_view raw) override;
	private:
		tuple<int, int, int> __inner_DeleteTimeStamp(string& raw);
		tuple<string, string> __inner_DeleteThreadName(string& raw);
	};
}

namespace Helium {
	HeliumLogger log("Helium", "VanillaParser");
	tuple<string, map<string, string>> VanillaParser::Parse(string_view raw) {
		string raw_str = static_cast<string>(raw);
		raw_str = this->GetServerPureOutput(raw_str);
		if (auto [suc_b, ret_map] = this->ParseServerStarting(raw_str); suc_b) {
			return make_tuple("helium.server.starting", ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParseServerStarted(raw_str); suc_b) {
			return make_tuple("helium.server.start"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParseServerPlayerChat(raw_str); suc_b) {
			return make_tuple("helium.input.player"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParsePlayerLogin(raw_str); suc_b) {
			return make_tuple("helium.player.join"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParsePlayerLogout(raw_str); suc_b) {
			return make_tuple("helium.player.leave"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParseServerOverload(raw_str); suc_b) {
			return make_tuple("helium.server.overload"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParseServerStopping(raw_str); suc_b) {
			return make_tuple("helium.server.stopping"s, ret_map);
		}
		if (auto [suc_b, ret_map] = this->ParseServerStopped(raw_str); suc_b) {
			return make_tuple("helium.server.stopped"s, ret_map);
		}
		return make_tuple("helium.empty"s, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerStarting(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"(Starting minecraft server version \d+.\d+\d+)");
		smatch t_res;
		if (regex_search(raw_str, start_r)) {
			regex major_r(R"(\d+.)"), minor_r(R"(.\d+.)"), patch_r(R"(.\d+)");
			string major, minor, patch;
			regex_search(raw_str, t_res, major_r);
			major = t_res[0];
			major.erase(major.end() - 1);

			regex_search(raw_str, t_res, minor_r);
			minor = t_res[0];
			minor.erase(minor.begin());
			minor.erase(minor.end() - 1);

			regex_search(raw_str, t_res, patch_r);
			patch = t_res[0];
			patch.erase(patch.begin());
			return make_tuple(true, map<string, string>{ {"major", major}, { "minor", minor }, { "patch", patch }});
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerStarted(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"(Done \(\d+.\d+s\)! For help, type "help")");
		if (regex_search(raw_str, start_r)) {
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerPlayerChat(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex player_name_r(R"(<[a-zA-Z0-9_]+> )");
		smatch t_res;
		if (regex_search(raw_str, t_res, player_name_r)) {
			string player_name_s = t_res[0];
			player_name_s.erase(player_name_s.begin());
			player_name_s.erase(player_name_s.end() - 1);
			player_name_s.erase(player_name_s.end() - 1);
			string player_input = regex_replace(raw_str, player_name_r, "");
			return make_tuple(true, map<string, string>{ {"player_name", player_name_s}, { "player_input", player_input }});
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerOverload(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"(Can't keep up! Is the server overloaded\? Running \d+ms or \d+ ticks behind)");
		if (regex_search(raw_str, start_r)) {
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerStopping(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"(Saving worlds)");
		if (regex_search(raw_str, start_r)) {
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParseServerStopped(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"(ThreadedAnvilChunkStorage)");//some bugs
		if (regex_search(raw_str, start_r)) {
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParsePlayerLogin(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"([a-zA-Z0-9_\-]+ joined the game)");
		smatch t_res;
		if (regex_search(raw_str, start_r)) {
			regex_search(raw_str, t_res, regex(R"([a-zA-Z0-9_\-]+ j)"));
			string player_name = t_res[0];
			player_name.erase(player_name.end() - 1);
			player_name.erase(player_name.end() - 1);
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	tuple<bool, map<string, string>> VanillaParser::ParsePlayerLogout(string_view raw) {
		string raw_str = static_cast<string>(raw);
		regex start_r(R"([a-zA-Z0-9_\-]+ left the game)");
		smatch t_res;
		if (regex_search(raw_str, t_res, start_r)) {
			regex_search(raw_str, t_res, regex(R"([a-zA-Z0-9_\-]+ l)"));
			string player_name = t_res[0];
			player_name.erase(player_name.end() - 1);
			player_name.erase(player_name.end() - 1);
			return make_tuple(true, map<string, string>());
		}
		return make_tuple(false, map<string, string>());
	}
	string VanillaParser::GetServerPureOutput(string_view raw) {
		string raw_str = static_cast<string>(raw);
		this->__inner_DeleteTimeStamp(raw_str);
		this->__inner_DeleteThreadName(raw_str);
		return raw_str;
	}
	int VanillaParser::GetServerLogLevel(string_view raw) {
		string raw_str = static_cast<string>(raw);
		auto [thread_s, lvl_s] = this->__inner_DeleteThreadName(raw_str);
		if (lvl_s == "INFO") return spdlog::level::level_enum::info;
		if (lvl_s == "WARN") return spdlog::level::level_enum::warn;
		if (lvl_s == "INFO") return spdlog::level::level_enum::err;
		return spdlog::level::level_enum::info;
	}

	tuple<int, int, int> VanillaParser::__inner_DeleteTimeStamp(string& raw) {
		regex tm(R"(\[\d{2}:\d{2}:\d{2}\] )");
		string _raw = raw;
		smatch t_res;
		if (regex_search(_raw, t_res, tm)) {
			regex hour(R"(\[\d{2}:)"), minute(R"(:\d{2}:)"), second(R"(:\d{2}\])");
			int h = 0, m = 0, s = 0;
			regex_search(_raw, t_res, hour);
			auto hour_str = t_res[0].str().substr(1, 2);
			h = stoi(hour_str);

			regex_search(_raw, t_res, minute);
			auto min_str = t_res[0].str().substr(1, 2);
			m = stoi(min_str);

			regex_search(_raw, t_res, second);
			auto sec_str = t_res[0].str().substr(1, 2);
			s = stoi(sec_str);

			raw = regex_replace(_raw, tm, "");
			return make_tuple(h, m, s);
		}
		return make_tuple(0, 0, 0);
	}
	tuple<string, string> VanillaParser::__inner_DeleteThreadName(string& raw) {
		regex thread_name_r(R"(\[[a-zA-Z_ \-0-9]+/[a-zA-Z_ \-0-9]+\]: )");
		string _raw = raw;
		smatch t_res;
		if (regex_search(_raw, t_res, thread_name_r)) {
			regex thread_r(R"(\[[a-zA-Z_ \-0-9]+/)"), level_r(R"(/[a-zA-Z_ \-0-9]+\]: )");
			string thread_s, level_s;
			regex_search(_raw, t_res, thread_r);
			thread_s = t_res[0].str();
			thread_s.erase(thread_s.begin());
			thread_s.erase(thread_s.end() - 1);

			regex_search(_raw, t_res, level_r);
			level_s = t_res[0].str();
			level_s.erase(level_s.begin());
			level_s.erase(level_s.end() - 1);
			level_s.erase(level_s.end() - 1);
			level_s.erase(level_s.end() - 1);

			raw = regex_replace(_raw, thread_name_r, "");
			return make_tuple(thread_s, level_s);
		}
		return make_tuple(""s, ""s);
	}
}