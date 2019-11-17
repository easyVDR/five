/*
 * AudioRipper plugin to VDR
 *
 * (C) 2005
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <string.h>

#include "setup.h"

cRipitSetup RipitSetup;

// --- cRipitSetup ---------------------------------------------------------------

cRipitSetup::cRipitSetup(void)
{
  Ripit_hidden = 0;
  Ripit_lastlog = 17;
  Ripit_excludespecialchars = 1;
  Ripit_underscores = 1;
  Ripit_playlist = 0;
  Ripit_verbosity = 1;
  Ripit_fastrip = 0;
  Ripit_lowbitrate = 4;
  Ripit_maxbitrate = 8;
  Ripit_crc = 1;
  Ripit_encoding_type = 3;
  strcpy(Ripit_ripopts, "--coder \"2\" --flacopt \"--ogg --best\"");
  strcpy(Ripit_encopts1, "-h");
  strcpy(Ripit_encopts2, "-h");
  strcpy(Ripit_encopts3, "-h");
  strcpy(Ripit_encopts4, "--best");
  strcpy(Ripit_encopts5, "");
  Ripit_oggquality = 5;
  strcpy(Ripit_dir,DEFAULT_RIPIT_DIR);
  Ripit_nice = 19;
}
