/*
 * spectrum_analyzer.h: The maths behind it
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

#ifndef ___SPECTRUM_ANALYZER_H
#define ___SPECTRUM_ANALYZER_H

#include <queue>
using namespace std;

#include <fftw3.h>
#include <math.h>

#include <vdr/plugin.h>
#include <vdr/status.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

#include "defines.h"
#include "client.h"


/**
 * Represents an audio channel and stores the data that are needed to do the
 * FFT and to hoöd the results for the computation by each client that is registered.
 * */
class cAudioChannel
{

public:
	// FFTW variables.
	fftw_plan plan;			// the fftw plan that executes the FFT for us
	double in[FFTW_SIZE];	// PCM-data
	fftw_complex *out;		// the frequencies' amplitudes
	
	// data that every cSaClient needs as a basis of it's calculations
	double spectrumData[NUM_BANDS];
	
	cAudioChannel();
	virtual ~cAudioChannel();
	
	/**
	 * Just resets all data.
	 * */
	void reset(void);
	
	/**
	 * Initiates the input array for the FFT.
	 * @param data PCM-data that shall be analyzed.
	 * @param length Length of the PCM-data.
	 * */
	void initInput(int *data, int length);
	
	/**
	 * Calculates the output array of the FFT.
	 * */
	void initOutput();
	
	/**
	 * Does "all the work".
	 * @see init_input()
	 * @see FFTW.fftw_execute()
	 * @see init_output()
	 * */
	void renderPcm(int *data, int length);
};

/**
 * Represents the spectrum analyzer.
 * */
class cSpectrumAnalyzer : public cStatus
{
private:
	cAudioChannel *right, *left;
	cTimeMs time;	// needed to trigger the removal of clients
	cControl *playerControl;
	queue<int> fifo; //TODO remove the queue
	cRingBufferFrame *ringBuffer;
	int samplerate;
	int readPosition;
	int visPosition;
	int writePosition;
	unsigned char *dataBuff;
	int dataBuffPos;
	cMutex ringBufferMutex;
	bool bigEndian;
	
	/**
	 * Executes the render_pcm-method of each channel.
	 * @param data PCM-data that shall be analyzed (left and right channel!).
	 * @param length Length of the PCM16-data.
	 * */
	void saRenderPcm(const unsigned char *data, int length);
	
	/**
	  * Resets the ringbuffer and all associated data.
	  * */
	void resetRingBuffer();
	
public:
	cTimeMs requestTime;	// needed to trigger the removal of clients
	
	cList<cSaClient> clients;
	cSpectrumAnalyzer();
	virtual ~cSpectrumAnalyzer();
	
	/**
	 * Adds some PCM16-data to the ringbuffer
	 * @param data PCM16-data to be stored (left and right channel!).
	 * @param length Length of the PCM16-data.
	 * @param index The timestamp (ms) of the frame(s) to be visualized.
	 * @param bufferSize For span-internal bookkeeping of the data to be visualized.
	 * @return true, if the entire data were written or false otherwise
	 * */
	bool putPcmData( const unsigned char *data, int length, int index,
						unsigned int bufferSize, bool bigEndian );
	
	/**
	 * Sets the current playindex.
	 * @param index The timestamp (ms) of the frame(s) being currently played.
	 * @return true, if it was possible to set the index, false otherwise (should never happen though).
	 * */
	bool setPlayindex( int index );

	/**
	 * The interface to the span-plugin and called with the pointers to all
	 * data needed to calculate and store the return values.
	 * @param bands Number of bands to compute for visualization.
	 * @param dataBothChannels Shall contain the bar heights afterwards.
	 * @param dataLeftChannel Shall contain the bar heights of the left channel afterwards.
	 * @param dataRightChannel Shall contain the bar heights of the right channel afterwards.
	 * @param sVolumeLeftChannel Shall contain the combined volume afterwards.
	 * @param sVolumeRightChannel Shall contain the right channel's volume afterwards.
	 * @param sVolumeBothChannels Shall contain the left channel's volume afterwards.
	 * @param client The client which is requesting all the stuff above.
	 * */
	void getAllData( int bands, int *dataBothChannels, int *dataLeftChannel,
						int *dataRightChannel, int *sVolumeLeftChannel,
						int *sVolumeRightChannel, int *sVolumeBothChannels,
						int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp,
						cSaClient *client
					);
	
	/**
	 * Triggers replays to be able to detach all clients on stopping the replay.
	 * @param DvbPlayerControl
	 * @param Name Name of the "Replayer".
	 * @param FileName Name of the file being replayed.
	 * @param On If true, the replay just started - otherwise it stopped.
	 * */
	virtual void Replaying(const cControl *DvbPlayerControl, const char *Name, const char *FileName, bool On);
	
	/**
	 * Triggers the removal of clients if there were no data for some time.
	 * @return true, if there were no data for a time period that's too long - false otherwise.
	 * */
	bool timedOut( void ) { return time.TimedOut(); };
	
	/**
	 * Deletes (detaches) all registered clients.
	 * @return Number of deleted clients.
	 * */
	int deleteClients( void );
};

#endif //___SPECTRUM_ANALYZER_H
