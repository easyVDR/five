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

#ifndef CUSTOMLOGSTREAM_H_
#define CUSTOMLOGSTREAM_H_

#include <iostream>
#include <functional>

namespace logger {

typedef std::function<void(const std::string &)> customLogFunction;


class CustomLogBuf : public std::streambuf {
private:
	void	putBuffer(void);
	void	putChar(char c);
	customLogFunction loggerFunction;
protected:
	int	overflow(int) override;
	int	sync() override;
public:
	CustomLogBuf(customLogFunction loggerFunction);
	virtual ~CustomLogBuf();
};

class CustomLogStream : public std::ostream {
public:
	CustomLogStream(customLogFunction loggerFunction);
};

} /* namespace logger */
#endif /* CUSTOMLOGSTREAM_H_ */
