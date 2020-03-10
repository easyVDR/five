/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * i18n.h: Internationalization
 *
 * Copyright (C) 2002-2012 Oliver Endriss <o.endriss@gmx.de>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */


#ifndef __PLUGIN_REMOTE_I18N_H
#define __PLUGIN_REMOTE_I18N_H


#include <vdr/i18n.h>


#if APIVERSNUM <= 10506
#define trNOOP(s) (s)
extern const tI18nPhrase remotePhrases[];
#endif


#endif // __PLUGIN_REMOTE_I18N_H
