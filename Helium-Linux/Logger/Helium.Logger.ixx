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

#define INVALID_LOGGER NULL

#include<spdlog/spdlog.h>
#include<spdlog/sinks/stdout_color_sinks.h>
#include<spdlog/sinks/daily_file_sink.h>
#include<spdlog/fmt/fmt.h>

export module Helium.Logger;

import <thread>;
import <iostream>;
import <map>;
import <string>;
import <format>;
import <string_view>;
import <memory>;
import <semaphore>;
import <queue>;

import Helium.TerminalUI.Log;

using namespace std;

export{
	namespace Helium {
		int CreateExtLogger(string name);
		int DeleteExtLogger(string name);
		int ExtLoggerDebug(string loggername, string raw);
		int ExtLoggerInfo(string loggername, string raw);
		int ExtLoggerWarn(string loggername, string raw);
		int ExtLoggerCrit(string loggername, string raw);
		int ExtLoggerError(string loggername, string raw);

		enum HeliumLoggerLevel {
			trace = spdlog::level::trace,
			debug = spdlog::level::debug,
			info = spdlog::level::info,
			warn = spdlog::level::warn,
			err = spdlog::level::err,
			crit = spdlog::level::critical
		};

		class HeliumLoggerThread final {
		public:
			class HeliumLoggerOutInfo final {
			public:
				spdlog::level::level_enum lvl;
				string out_str;
			};

			HeliumLoggerThread() {
				try {
					this->global_file_sink = make_shared<spdlog::sinks::daily_file_sink_st>("./logs/helium-log.log", 23, 59);
					this->logger = make_shared<spdlog::logger>("helium_logger", spdlog::sinks_init_list{ this->global_file_sink });
#ifdef NOT_STABLE
					this->logger->set_level(spdlog::level::trace);
					this->global_file_sink->set_level(spdlog::level::trace);
#else
					this->logger->set_level(spdlog::level::info);
					this->global_file_sink->set_level(spdlog::level::info);
#endif
					this->logger->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");
					spdlog::register_logger(this->logger);
					this->fake_this = this;
					stop = false;
					this->logger_thread = make_shared<jthread>(jthread(
						[&](HeliumLoggerThread* fakethis) {
							while (true) {
								if (!this->log_pool.empty()) {
									try {
										this->logger->log(fakethis->log_pool.front().lvl, this->log_pool.front().out_str);
										GeneralLog(static_cast<spdlog::level::level_enum>(this->log_pool.front().lvl), this->log_pool.front().out_str);
										this->log_pool.pop();
									}
									catch (exception& e) {
										cout << "Unexcepted exception in HeliumLoggerThread::__inner_log_thread()" << endl;
									}
									catch (...) {
										cout << "Unexcepted exception in HeliumLoggerThread::__inner_log_thread()" << endl;
									}
								}
								if (this->logger_thread->get_stop_token().stop_requested() && this->log_pool.empty()) return;
							}
						}
					, this));
					this->logger_thread->detach();
				}
				catch (...) {
					return;
				}
			}
			~HeliumLoggerThread() {
				while (!this->log_pool.empty()) this->log_pool.pop();
				this->logger_thread->request_stop();
				this->logger->flush();
				spdlog::drop("helium_logger");
			}
			void AddLogInfo(spdlog::level::level_enum lvl, const string& out_str) {
				//cout << "AddLogInfo : " << out_str << endl;
				this->log_pool.push(HeliumLoggerOutInfo{ lvl, out_str });
			}
		private:
			shared_ptr<spdlog::sinks::daily_file_sink_st> global_file_sink;
			shared_ptr<spdlog::logger> logger;
			shared_ptr<jthread> logger_thread;
			queue<HeliumLoggerOutInfo> log_pool;
			HeliumLoggerThread* fake_this;
			bool stop;
		};

		class HeliumLogger final {
		public:
			HeliumLogger(string_view logger_name, string_view thread_name);
			~HeliumLogger();
			template <typename... Args>
			void trace(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void debug(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void info(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void warning(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void error(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void critical(std::string_view rt_fmt_str, Args&&... args);

			template <typename... Args>
			void log(HeliumLoggerLevel lvl, std::string_view rt_fmt_str, Args&&... args);
		private:
			template <typename... Args>
			void inner_logger(spdlog::level::level_enum lvl, std::string_view rt_fmt_str, Args&&... args);

			string logger_name;
			string thread_name;
		};
	}
}

namespace Helium {
	binary_semaphore logger_sem(1);
	HeliumLoggerThread log_thread;

	HeliumLogger::HeliumLogger(string_view logger_name, string_view thread_name) {
		this->logger_name = logger_name;
		this->thread_name = thread_name;
	}
	HeliumLogger::~HeliumLogger() {
	}
	template <typename... Args>
	void HeliumLogger::trace(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::trace, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::debug(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::debug, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::info(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::info, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::warning(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::warn, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::error(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::err, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::critical(std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(spdlog::level::critical, rt_fmt_str, args...);
	}

	template <typename... Args>
	void HeliumLogger::log(HeliumLoggerLevel lvl, std::string_view rt_fmt_str, Args&&... args) {
		this->inner_logger(static_cast<spdlog::level::level_enum>(lvl), rt_fmt_str, args...);
	}
	template <typename... Args>
	void HeliumLogger::inner_logger(spdlog::level::level_enum lvl, std::string_view rt_fmt_str, Args&&... args) {
		string out_str = std::vformat(rt_fmt_str, std::make_format_args(args...));
		string final_str = std::format("[{}/{}] {}", this->logger_name, this->thread_name, out_str);
		//cout << "inner_logger : " << final_str << endl;
		log_thread.AddLogInfo(lvl, final_str);
	}
}

namespace Helium {
	int CreateExtLogger(string name) {
		return 0;
	}
	int DeleteExtLogger(string name) {
		return 0;
	}
	int ExtLoggerDebug(string loggername, string raw) {
		return 0;
	}
	int ExtLoggerInfo(string loggername, string raw) {
		return 0;
	}
	int ExtLoggerError(string loggername, string raw) {
		return 0;
	}
	int ExtLoggerWarn(string loggername, string raw) {
		return 0;
	}
	int ExtLoggerCrit(string loggername, string raw) {
		return 0;
	}
}