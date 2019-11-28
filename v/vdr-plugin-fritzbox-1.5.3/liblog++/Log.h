/*
 * liblog++
 *
 * Copyright (C) 2007-2013 Joachim Wilke <liblog@joachim-wilke.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <mutex>
#include <memory>
#include <sstream>

#include "CustomLogStream.h"

namespace logger {

class Log {
private:
	typedef std::unique_ptr<std::ostream> ostreamPtr;

	static std::mutex  mutex;
	static std::string prefix;
	static ostreamPtr  dstream;
	static ostreamPtr  estream;
	static ostreamPtr  istream;

	static std::string getLocator(std::string file, int line);
	static void        putLogMessage(const std::ostream &message, std::ostream &stream, std::string file, int line);

public:
	static void debug(const std::ostream &message, std::string file, int line);
	static void error(const std::ostream &message, std::string file, int line);
	static void info(const std::ostream &message, std::string file, int line);

	static void setLogStreams(ostreamPtr &&elog, ostreamPtr &&ilog, ostreamPtr &&dlog);
	static void setPrefix(const std::string &prefix) { Log::prefix = prefix + " - "; }
	static void setCustomLogger(customLogFunction logError, customLogFunction logInfo, customLogFunction logDebug);
};

#define DBG(x) {logger::Log::debug(std::stringstream().flush() << x, std::string(__FILE__), __LINE__);}
#define INF(x) {logger::Log::info (std::stringstream().flush() << x, std::string(__FILE__), __LINE__);}
#define ERR(x) {logger::Log::error(std::stringstream().flush() << x, std::string(__FILE__), __LINE__);}

}
#endif /* LOG_H_ */
