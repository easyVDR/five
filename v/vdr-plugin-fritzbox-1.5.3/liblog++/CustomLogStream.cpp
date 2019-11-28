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

#include "CustomLogStream.h"

#include <string.h>

namespace logger {

CustomLogBuf::CustomLogBuf(customLogFunction loggerFunction)
: loggerFunction{loggerFunction} {
	const unsigned int BUFFER_SIZE = 1024;
	char	*ptr = new char[BUFFER_SIZE];
	setp(ptr, ptr + BUFFER_SIZE);
	setg(0, 0, 0);
}

void CustomLogBuf::putBuffer(void)
{
	if (pbase() != pptr())
	{
		int     len = (pptr() - pbase());
		char    *buffer = new char[len + 1];

		strncpy(buffer, pbase(), len);
		buffer[len] = 0;

		loggerFunction(buffer);

		setp(pbase(), epptr());
		delete [] buffer;
	}
}

int CustomLogBuf::overflow(int c)
{
	putBuffer();

	if (c != EOF) {
		sputc(c);
	}
	return 0;

}

int CustomLogBuf::sync()
{
	putBuffer();
	return 0;
}

CustomLogBuf::~CustomLogBuf() {
	sync();
	delete[] pbase();
}

CustomLogStream::CustomLogStream(customLogFunction loggerFunction)
:std::ostream(new CustomLogBuf(loggerFunction)) {}

} /* namespace logger */
