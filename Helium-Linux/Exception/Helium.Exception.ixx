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

#include<libintl.h>
#include<cstdarg>
#include<spdlog/spdlog.h>
#include<boost/stacktrace.hpp>

export module Helium.Exception;

import Helium.Logger;
import Helium.Misc.Version;

import <string>;
import <iostream>;
import <exception>;
import <fstream>;
import <chrono>;

using namespace std;
using namespace boost::stacktrace;

export namespace Helium {
	void HeliumErrorExit(bool ispause, bool uselogger, string str, ...);
	void HeliumError_exit(bool ispause, bool uselogger, string str, ...);
	void HeliumTerminateHandler();
	void PrintStacktrace();
	void GenerateCrashReport();
}

namespace Helium {
	static HeliumLogger log("Helium", "ExceptionHandler");
	void HeliumErrorExit(bool ispause, bool uselogger, string str, ...) {
		va_list arglist;

		va_start(arglist, str);
		cout << arglist << endl;
		va_end(arglist);

		spdlog::drop_all();
		_flushall();
		if (ispause) system("pause");
		exit(EXIT_FAILURE);
	}
	void HeliumError_exit(bool ispause, bool uselogger, string str, ...) {
		va_list arglist;

		va_start(arglist, str);
		cout << arglist << endl;
		va_end(arglist);

		spdlog::drop_all();
		if (ispause) system("pause");
		_exit(EXIT_FAILURE);
	}
	void HeliumTerminateHandler() {
		try {
			log.critical("{}", gettext("Helium crashed with a uncaught exception!"));
			log.critical("{}", gettext("THIS IS A CRASH. Please inform us at helium_devteam@outlook.com"));

			cout << "STACKTRACE:" << endl;
			cout << stacktrace() << endl;
		}
		catch (...) {
			try {
				cout << "Helium crashed with a unexpected exception!" << endl;
				cout << "THIS IS A CRASH. Please inform us at helium_devteam@outlook.com" << endl;
				cout << "STACKTRACE" << endl;
				cout << stacktrace() << endl;
			}
			catch (...) {
				cout << "Failed to get the boost::stacktrace::stacktrace()" << endl;
			}
		}
		GenerateCrashReport();
		abort();
	}
	void PrintStacktrace() {
		cout << "----------------------Stacktrace----------------------" << endl;
		cout << stacktrace() << endl;
		cout << "----------------------Stacktrace----------------------" << endl;
	}
	void GenerateCrashReport() {
		try {
			auto now
				= chrono::system_clock::to_time_t(
					chrono::system_clock::now()
				);
			char buf[16];
			strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
			string report_name = "./helium-crash-report-";
			report_name.append(buf).append(".txt");
			fstream fs(report_name,
				fstream::trunc | fstream::out);

			fs << "Helium " << GetHeliumVersion().to_string() << " Crash Report :(\n";
		}
		catch (...) {
			cout << "Error occurred when generating crash report" << endl;
		}
	}
}