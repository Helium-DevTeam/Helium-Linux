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
#include <spdlog/spdlog.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

export module Helium.MinecraftServer.Class;

import Helium.Events;
import Helium.Logger;
import Helium.Misc.UUIDManager;
import Helium.Parser;
import Helium.MinecraftServer.Shared;

import <thread>;
import <iostream>;
import <string>;
import <list>;
import <any>;
import <vector>;
import <map>;
import <filesystem>;
import <memory>;

using namespace std;
using namespace boost;
using namespace boost::uuids;
namespace fs = std::filesystem;

export namespace Helium {
	class HeliumServerManager {
	public:
		using HeliumServerID = boost::uuids::uuid;

		class RedirectInformation {
		public:
			HANDLE hStdInRead = NULL;   //子进程用的stdin的读入端
			HANDLE hStdInWrite = NULL;  //主程序用的stdin的写入端

			HANDLE hStdOutRead = NULL;  //主程序用的stdout的读入端
			HANDLE hStdOutWrite = NULL; //子进程用的stdout的写入端

			HANDLE hStdErrWrite = NULL; //子进程用的stderr的写入端
		};

		class RCONInfo {
		public:
			bool IsRCONEnabled() const { return this->enabled; }
			int RCONPort() const { return this->port; }
			string RCONPassword() const { return this->pwd; }

			void SetRCONEnable(const bool& n) { this->enabled = n; };
			void SetRCONPort(const int& n) { this->port = n; }
			void SetRCONPassword(string_view n) { this->pwd = static_cast<string>(n); }
		private:
			bool enabled;
			int port;
			string pwd;
		};

		class HeliumServer : public UUIDHeliumObject {
		public:
			HeliumServer() : status(HeliumServerStatus::TERMINATED) {}
			~HeliumServer() {
				if (this->status != HeliumServerStatus::TERMINATED) this->StopServer();
			}

			int StartServer();
			int AutoStartServer();
			int StopServer();
			int PauseServer();
			int ResumeServer();
			int TerminateServer();
			int SendToServer(string_view text);

			RCONInfo& RCON() { return static_cast<RCONInfo&>(this->rcon); }
			RedirectInformation& Redirect() { return static_cast<RedirectInformation&>(this->redir); }

			HeliumServerStatus GetStatus() const { return this->status; }
			HeliumServerType GetType() const { return this->type; }
			fs::path GetServerDirectory() const { return this->server_dir; }
			string GetServerName() const { return this->server_name; }
			string GetStartCommand() const { return this->start_cmd; }
			bool IsOutputVisible() const { return this->output; }
			bool IsAutoStart() const { return this->auto_start; }

			void SetServerDirectory(const fs::path& n) { this->server_dir = n; }
			void SetServerType(HeliumServerType n) { this->type = n; }
			void SetServerName(string_view n) { this->server_name = n; }
			void SetStartCommand(string_view n) { this->start_cmd = n; }
			void SetOutputVisible(const bool& n) { this->output = n; }
			void SetAutoStart(const bool& n) { this->auto_start = n; }
		private:
			fs::path server_dir;
			string server_name;
			string start_cmd;
			RCONInfo rcon;
			HeliumServerStatus status;
			HeliumServerType type;
			bool output;
			bool auto_start;
			HANDLE proc;
			HANDLE thread;
			RedirectInformation redir;
			std::thread output_thread;
		};

		int AddServer(std::shared_ptr<HeliumServer> server_ptr) {
			this->server_map[server_ptr->GetUUID()] = server_ptr;
			this->id_map[server_ptr->GetServerName()] = server_ptr->GetUUID();
			return 0;
		}
		std::shared_ptr<HeliumServer> GetServer(string_view server_name) const {
			string name = static_cast<string>(server_name);
			if (this->id_map.count(name) > 0) {
				return this->server_map.at(this->id_map.at(name));
			}
			return nullptr;
		}
		vector<std::shared_ptr<HeliumServer>> GetServerList() const {
			vector<std::shared_ptr<HeliumServer>> ret_vec;
			for (auto& p : this->server_map) {
				ret_vec.push_back(p.second);
			}
			return ret_vec;
		}
	private:
		map<HeliumServerID, std::shared_ptr<HeliumServer>> server_map;
		map<string, HeliumServerID> id_map;
	}helium_server_manager;

	using HeliumServer = HeliumServerManager::HeliumServer;

	int ProcessServerOutput(HeliumServer* server_ptr);
	bool isinit = false;
}

namespace Helium {
	HeliumLogger log("Helium", "ServerManager");

	int HeliumServer::StartServer() {
		log.info("{} {}({})", gettext("Starting server"), this->server_name, to_string(this->GetUUID()));
		log.info("{} \"{}\"", gettext("Server start command"), this->start_cmd);

		if (this->status != HeliumServerStatus::TERMINATED) {
			log.warning("{} {}({})", gettext("Trying to start a non-terminated server"), this->server_name, to_string(this->GetUUID()));
			return -1;
		}

		SECURITY_ATTRIBUTES sa;
		PROCESS_INFORMATION pi;
		STARTUPINFOA si;
		string workingdir;

		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = true;
		sa.lpSecurityDescriptor = NULL;

		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);

		workingdir = this->server_dir.string();

		if (!CreatePipe(&this->redir.hStdOutRead, &this->redir.hStdOutWrite, &sa, 0)) {
			log.error("{} {} {} {}", gettext("Failed to create stdout pipe for server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}

		if (!CreatePipe(&this->redir.hStdInRead, &this->redir.hStdInWrite, &sa, 0)) {
			log.error("{} {} {} {}", gettext("Failed to create stdin pipe for server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}

		if (!SetHandleInformation(this->redir.hStdOutRead, HANDLE_FLAG_INHERIT, 0)) {
			log.error("{} {} {} {}", gettext("SetHandelInformation() of stdout pipe for server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}

		if (!SetHandleInformation(this->redir.hStdInWrite, HANDLE_FLAG_INHERIT, 0)) {
			log.error("{} {} {} {}", gettext("SetHandelInformation() of stdin pipe for server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}

		si.hStdError = this->redir.hStdOutWrite;
		si.hStdOutput = this->redir.hStdOutWrite;
		si.hStdInput = this->redir.hStdInRead;
		si.dwFlags |= STARTF_USESTDHANDLES;

		if (auto suc = CreateProcessA(NULL,
			const_cast<char*>(this->start_cmd.c_str()),
			NULL,
			NULL,
			true,
			CREATE_NO_WINDOW,
			NULL,
			workingdir.c_str(),
			&si,
			&pi); !suc) {
			log.error("{} {} {} {}", gettext("CreateProcess() of server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}
		else {
			CloseHandle(this->redir.hStdOutWrite);
			CloseHandle(this->redir.hStdInRead);

			this->proc = pi.hProcess;
			this->thread = pi.hThread;

			this->status = HeliumServerStatus::RUNNING;

			this->output_thread = std::move(std::thread(ProcessServerOutput, this));

			log.info("{} {}({})", gettext("Successfully started server"), this->server_name, to_string(this->GetUUID()));
			log.info("{} {}", gettext("Server running on PID"), (long)pi.dwProcessId);
		}
	}
	int HeliumServer::AutoStartServer() {
		if (this->auto_start)
			return this->StartServer();
		return 0;
	}
	int HeliumServer::StopServer() {
		int ret = 0;
		auto parser_ptr = GetParserPtr(this->type);
		this->SendToServer(parser_ptr->GetServerStop());

		this->status = HeliumServerStatus::TERMINATED;
		return ret;
	}
	int HeliumServer::TerminateServer() {
		int ret = 0;

		TerminateProcess(this->proc, 0);
		helium_event_manager.DispatchEvent("helium.server.stop", { this->server_name });

		if (this->output_thread.joinable())
			this->output_thread.join();

		this->status = HeliumServerStatus::TERMINATED;
		return ret;
	}
	int HeliumServer::PauseServer() {
		int ret = 0;

		if (this->status != HeliumServerStatus::RUNNING) {
			log.error("{} {}({})", gettext("Trying to pause a non-running server"), this->server_name, to_string(this->GetUUID()));
			return -1;
		}
		if (ret = SuspendThread(this->thread); ret == -1) {
			log.error("{} {} {} {}", gettext("SuspendThread() of server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}
		helium_event_manager.DispatchEvent("helium.server.pause", { this->server_name });
		this->status = HeliumServerStatus::PAUSED;
		return ret;
	}
	int HeliumServer::ResumeServer() {
		int ret = 0;

		if (this->status != HeliumServerStatus::PAUSED) {
			log.error("{} {}({})", gettext("Trying to resume a non-paused server"), this->server_name, to_string(this->GetUUID()));
			return -1;
		}

		if (ret = ResumeThread(this->thread); ret == -1) {
			log.error("{} {} {} {}", gettext("ResumeThread() of server"), this->server_name, gettext("failed with error code"), (long)GetLastError());
			return GetLastError();
		}
		helium_event_manager.DispatchEvent("helium.server.resume", { this->server_name });
		this->status = HeliumServerStatus::RUNNING;

		return ret;
	}
	int HeliumServer::SendToServer(string_view text) {
		string raw_text = static_cast<string>(text);
		DWORD dwWritten;
		CHAR in_buffer[4101];
		strcpy(in_buffer, raw_text.c_str());
		if (text.size() <= 4096) {
			in_buffer[text.size()] = '\r';
			in_buffer[text.size() + 1] = '\n';
			in_buffer[text.size() + 2] = '\0';
		}
		else {
			in_buffer[4096] = '\r';
			in_buffer[4097] = '\n';
			in_buffer[4098] = '\0';
		}
		BOOL bSuccess = FALSE;
		bSuccess = WriteFile(this->redir.hStdInWrite, in_buffer, text.size(), &dwWritten, NULL);
		return bSuccess;
	}
	int ProcessServerOutput(HeliumServer* server_ptr) {
		DWORD dwRead = 0, dwWritten = 0;
		CHAR chBuf[4097] = { 0 };
		BOOL bSuccess = false;
		auto parser_ptr = GetParserPtr(server_ptr->GetType());
		HeliumLogger out_log("Server", server_ptr->GetServerName());
		string name = server_ptr->GetServerName();
		for (;;)
		{
			bSuccess = ReadFile(server_ptr->Redirect().hStdOutRead, chBuf, 4096, &dwRead, NULL);
			if (!bSuccess) continue;

			chBuf[dwRead - 1] = '\0';
			string output(chBuf);
			vector<string> lines;
			split(lines, output, is_any_of("\n"), token_compress_on);

			for (auto s : lines) {
				helium_event_manager.DispatchEvent("helium.input.server", { name, s });
				out_log.log(static_cast<HeliumLoggerLevel>(parser_ptr->GetServerLogLevel(s)), "{}", parser_ptr->GetServerPureOutput(s));
				auto [event_name, ret_map] = parser_ptr->Parse(s);
				helium_event_manager.DispatchEvent("helium.input.server", { name, s });
				helium_event_manager.DispatchEvent("helium.input.general", { name, s });
				if (event_name != "helium.empty") {
					if (event_name == "helium.input.player") {
						helium_event_manager.DispatchEvent(event_name, { name, ret_map["player_name"], ret_map["player_input"] });
					}
					else {
						helium_event_manager.DispatchEvent(event_name, { name });
						log.debug(event_name);
					}
				}
			}
		}

		return 0;
	}
}