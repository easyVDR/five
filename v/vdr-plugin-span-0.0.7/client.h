/*
 * client.h: data structure for SA-clients
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

#ifndef ___CLIENT_H
#define ___CLIENT_H

#include <math.h>

#include <vdr/tools.h>

#include "defines.h"
#include "setup.h"

void getPeak( int (*sBarPeaks)[2], int *sBarPeaksTmp );

/**
 * Represents the bars of an audio channel.
 * Only left and right channel are stored. 
 * */
class cClientBarHeights
{
private:
	int bands;
public:
	//
	int *barHeightsOld; // needed to compute the bars with falloff
	// barPeaks[band][peak-value]
	// barPeaks[band][0] = peak-value
	// barPeaks[band][1] = time-indicator	
	int (*barPeaks)[2];

	/**
	 * @param sBands Number of bands of this channel to visualize.
	 * */
	cClientBarHeights(int sBands);
	virtual ~cClientBarHeights();
	/**
	 * Resets the barHeightsOld, e.g. when there was no request for data for some time.
	 * Otherwise, bars would decrease from their last height by the falloff-value.
	 * @see barHeightsOld
	 * */
	void reset(void);
};

/**
 * Represents left and right channel.
 * Contains just the cClientBarHeights as a container for both to simplify handling.
 * @see cClientBarHeights
 * */
class cClientData
{
public:
	cClientBarHeights *leftChannel, *rightChannel;
	
	/**
	 * @param sBands Number of bands of the channels to visualize.
	 * */
	cClientData(int sBands);
	virtual ~cClientData();
};

/**
 * Represents all data needed for each client registered at the span-plugin.
 * */
class cSaClient : public cListObject
{
private:
	char *name; // name of the client-plugin used as unique ID to manage various clients

	/**
	 * Calculate the bar heights of the right channel.
	 * @param barHeightsTmp Pointer to the array that shall contain the bar heights afterwards.
	 * @param spectrumDataTmp Pointer to the data-basis needed to compute the bar heights.
	 * @param spectrumDataTmp Shall contain the volume of the channel afterwards.
	 * @see getBarHeightsOfSingleChannel
	 * */
	void getBarHeightsOfRightChannel( int *barHeightsTmp, double *spectrumDataTmp,
						int *sVolumeChannel,
						int *barPeaksTmp );

	/**
	 * Calculate the bar heights of the left channel.
	 * @param barHeightsTmp Pointer to the array that shall contain the bar heights afterwards.
	 * @param spectrumDataTmp Pointer to the data-basis needed to compute the bar heights.
	 * @param spectrumDataTmp Shall contain the volume of the channel afterwards.
	 * @see getBarHeightsOfSingleChannel
	 * */
	void getBarHeightsOfLeftChannel( int *barHeightsTmp, double *spectrumDataTmp,
						int *sVolumeChannel,
						int *barPeaksTmp );

	/**
	 * Calculate the bar heights of a single channel. Simplifies handling both channels and
	 * just hands over the params to the method for the corresponding channel.
	 * @param barHeightsTmp Pointer to the array that shall contain the bar heights afterwards.
	 * @param barHeightsTmpOld Pointer to the old bar heights needed to compute the bars' falloff.
	 * @param spectrumDataTmp Pointer to the data-basis needed to compute the bar heights.
	 * @param sVolumeChannel Shall contain the volume of the channel afterwards.
	 * @see getBarHeightsOfRightChannel
	 * @see getBarHeightsOfLeftChannel
	 * */
	void getBarHeightsOfSingleChannel( int *barHeightsTmp,
						int *barHeightsTmpOld, double *spectrumDataTmp,
						int *sVolumeChannel,
						int *barPeaksTmp,
						int (*barPeaks)[2] );
						
public:
	int bands;		// number of bands this client wants visualize
	int falloff;	// the fall off value of this client 
	cTimeMs time;	// needed to trigger the removal of clients
	
	int (*barPeaks)[2];
	
	cClientData *clientData;
	
	/**
	 * @param sName The name of the client being registered.
	 * @param sBands Number of bands this client wants visualize.
	 * @param sFalloff The fall off value of this client.
	 * */
	cSaClient(const char *sName, int sBands, int sFalloff);
	virtual ~cSaClient();
	virtual int Compare(const cListObject &ListObject) const;
	
	/**
	 * Needed to identify clients by their corresponding name.
	 * @param sName Name to compare with the name of this client's name.
	 * @return 0, if name is equal to sName, value!=0 is name is not equal to sName.
	 * */
	virtual int Compare(const char *sName);
	
	/**
	 * Triggers e.g. when there was no request for data for some time.
	 * Then the client is removed. This is done since if _all_ clients once registered
	 * and later on stopped their requests, we do not have to compute the FFTs any more.
	 * A nice side-effect is that bars are reset;
	 * otherwise, bars would decrease from their last height by the falloff-value.
	 * */
	inline bool timedOut(void) { return time.TimedOut(); }
	
	/**
	 * Gets the fall off value without side-effects.
	 * @return falloff
	 * */
	inline int getFalloff(void) const { return falloff; }
	
	/**
	 * Sets the fall off value.
	 * @param sFalloff The new fall off value.
	 * */
	inline void setFalloff( int sFalloff ) { falloff = sFalloff; }

	/**
	 * Calculate the bar heights of all channels (right, left and their combination).
	 * @param dataBothChannels Pointer to the array that shall contain the combined bar heights afterwards.
	 * @param dataLeftChannel Pointer to the array that shall contain the left channel's bar heights afterwards.
	 * @param dataRightChannel Pointer to the array that shall contain the right channel's bar heights afterwards.
	 * @param sVolumeLeftChannel Shall contain the volume of the left channel afterwards.
	 * @param sVolumeRightChannel Shall contain the volume of the right channel afterwards.
	 * @param sVolumeBothChannels Shall contain the overall volume of the channel.
	 * @param spectrumDataLeftChannel 
	 * @param spectrumDataRightChannel
	 * */
	void getBarHeightsOfBothChannels( int *dataBothChannels, int *dataLeftChannel,
						int *dataRightChannel, int *sVolumeLeftChannel,
						int *sVolumeRightChannel, int *sVolumeBothChannels,
						double *spectrumDataLeftChannel, double *spectrumDataRightChannel,
						int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp );
						
	/**
	 * Just return a flat line (zeroes everywhere), e.g. during paused playback.
	 * @param dataBothChannels Pointer to the array that shall contain the combined bar heights afterwards.
	 * @param dataLeftChannel Pointer to the array that shall contain the left channel's bar heights afterwards.
	 * @param dataRightChannel Pointer to the array that shall contain the right channel's bar heights afterwards.
	 * @param sVolumeLeftChannel Shall contain the volume of the left channel afterwards.
	 * @param sVolumeRightChannel Shall contain the volume of the right channel afterwards.
	 * @param sVolumeBothChannels Shall contain the overall volume of the channel.
	 * @param spectrumDataLeftChannel 
	 * @param spectrumDataRightChannel
	 * */
	void getFlatlineOfBothChannels( int *dataBothChannels, int *dataLeftChannel,
						int *dataRightChannel, int *sVolumeLeftChannel,
						int *sVolumeRightChannel, int *sVolumeBothChannels,
						double *spectrumDataLeftChannel, double *spectrumDataRightChannel,
						int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp );
};

#endif //___CLIENT_H
