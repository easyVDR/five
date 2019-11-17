/*
 * defines.h: All macros for the plugin
 */

/*
 * Spectrum Analyzer plugin for VDR (C++)
 *
 * (C) 2006 Christian Leuschen <christian.leuschen@gmx.de>
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

#ifndef ___DEFINES_H
#define ___DEFINES_H


#define SPAN_PROVIDER_CHECK_ID  "Span-ProviderCheck-v1.0"
#define SPAN_CLIENT_CHECK_ID    "Span-ClientCheck-v1.0"
#define SPAN_SET_PCM_DATA_ID    "Span-SetPcmData-v1.1"
#define SPAN_SET_PLAYINDEX_ID   "Span-SetPlayindex-v1.0"
#define SPAN_GET_BAR_HEIGHTS_ID "Span-GetBarHeights-v1.0"

//Span requests to collect possible providers / clients
struct Span_Provider_Check_1_0 {
    bool *isActive;
	bool *isRunning;
};

struct Span_Client_Check_1_0 {
    bool *isActive;
	bool *isRunning;
};		

// SpanData
struct Span_SetPcmData_1_0 {
	unsigned int length;		// the length of the PCM-data
	const unsigned char *data;	// the PCM-Data
	int index;					// the timestamp (ms) of the frame(s) to be visualized
	unsigned int bufferSize;	// for span-internal bookkeeping of the data to be visualized
	bool bigEndian;				// are the pcm16-data coded bigEndian?
};

struct Span_SetPlayindex_1_0 {
	int index;					// the timestamp (ms) of the frame(s) being currently played
};

struct Span_GetBarHeights_v1_0 {
// all heights are normalized to 100(%)
	unsigned int bands;						// number of bands to compute
	unsigned int *barHeights;				// the heights of the bars of the two channels combined
	unsigned int *barHeightsLeftChannel;	// the heights of the bars of the left channel
	unsigned int *barHeightsRightChannel;	// the heights of the bars of the right channel
	unsigned int *volumeLeftChannel;		// the volume of the left channels
	unsigned int *volumeRightChannel;		// the volume of the right channels
	unsigned int *volumeBothChannels;		// the combined volume of the two channels
	const char *name;						// name of the plugin that wants to get the data
											// (must be unique for each client!)
	unsigned int falloff;                   // bar falloff value
	unsigned int *barPeaksBothChannels;     // bar peaks of the two channels combined
	unsigned int *barPeaksLeftChannel;      // bar peaks of the left channel
	unsigned int *barPeaksRightChannel;     // bar peaks of the right channel
};

// Define a "nice" (human understandable) height of the bars.
// So each client may scale the heights with the guarantee that it is represented as a percentage of 100%.
#define HEIGHT 100

// TODO: Check the following:
// yes, FFTW_SIZE should be at least as big as PCM_FRAME_SIZE/4 (=2048/4) optimally
// however, for slower CPUs (<200MHz) half of it is enough. Then some data to analyze is omitted.
#define FFTW_SIZE 1024
//#define FFTW_SIZE_MIN 512
#define NUM_BANDS (FFTW_SIZE/2)

// DATA_TIMEOUT = max. mseconds without requests for visualisation data of a client, after that time the
// data shall be cleared
#define DATA_TIMEOUT 2000

#endif //___DEFINES_H
