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

#include "Log.h"

#include "CustomLogStream.h"

#include <sstream>

namespace logger {

std::mutex    Log::mutex;
std::string   Log::prefix;
Log::ostreamPtr Log::dstream = nullptr;
Log::ostreamPtr Log::estream = nullptr;
Log::ostreamPtr Log::istream = nullptr;

std::string Log::getLocator(std::string file, int line) {
	std::stringstream ss;
	size_t start = file.rfind('/') == std::string::npos ? 0 : file.rfind('/') + 1;
	ss << "[" << prefix << std::string(file, start, std::string::npos)
	   << ":" << line << "] ";
	return ss.str();
}

void Log::putLogMessage(const std::ostream &message, std::ostream &stream, std::string file, int line) {
	mutex.lock();
	stream << getLocator(file, line) << message.rdbuf() << std::endl;
	mutex.unlock();
}

void Log::debug(const std::ostream &message, std::string file, int line) {
	if (dstream)
		putLogMessage(message, *dstream, file, line);
	else
		putLogMessage(message, std::clog, file, line);
}

void Log::error(const std::ostream &message, std::string file, int line) {
	if (estream)
		putLogMessage(message, *estream, file, line);
	else
		putLogMessage(message, std::cerr, file, line);
}

void Log::info(const std::ostream &message, std::string file, int line) {
	if (istream)
		putLogMessage(message, *istream, file, line);
	else
		putLogMessage(message, std::cout, file, line);
}

void Log::setLogStreams(Log::ostreamPtr &&elog, Log::ostreamPtr &&dlog, Log::ostreamPtr &&ilog) {
	estream = std::move(elog);
	istream = std::move(ilog);
	dstream = std::move(dlog);
}

void Log::setCustomLogger(customLogFunction logError, customLogFunction logInfo, customLogFunction logDebug) {
	setLogStreams(std::unique_ptr<CustomLogStream>(new CustomLogStream(logError)),
			      std::unique_ptr<CustomLogStream>(new CustomLogStream(logInfo)),
				  std::unique_ptr<CustomLogStream>(new CustomLogStream(logDebug)));
}

} /* namespace fritz */
