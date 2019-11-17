/*
 * client.c: data structure for SA-clients
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

#ifndef ___CLIENT_C
#define ___CLIENT_C

#include "client.h"

// cClientBarHeights ##############################################################
cClientBarHeights::cClientBarHeights(int sBands)
{
	bands = sBands;
	barHeightsOld = new int[bands];
	barPeaks = new int[bands][2];
	reset();
}
// ----------------------------------------------------------------------------

cClientBarHeights::~cClientBarHeights()
{
	delete [] barHeightsOld;
	delete [] barPeaks;
}
// ----------------------------------------------------------------------------

void cClientBarHeights::reset(void)
{
	int i;
	
	for (i = 0; i < bands; i++)
	{
	  	barHeightsOld[i]	= 0;
	  	barPeaks[i][0]		= 0;
	  	barPeaks[i][1]		= 0;
	}
}

// cClientData ################################################################
cClientData::cClientData(int sBands)
{
	leftChannel = new cClientBarHeights(sBands);
	rightChannel = new cClientBarHeights(sBands);
}

// ----------------------------------------------------------------------------
cClientData::~cClientData()
{
	delete leftChannel;
	delete rightChannel;
}

// cSaClient ##################################################################
cSaClient::cSaClient(const char *sName, int sBands, int sFalloff)
{
	name = sName ? strdup(sName):0;
	bands = sBands;
	falloff = sFalloff;
	barPeaks = new int[bands][2];
	
	int i;
	
	for (i = 0; i < bands; i++)
	{
	  	barPeaks[i][0]		= 0;
	  	barPeaks[i][1]		= 0;
	}
	
	clientData = new cClientData(sBands);
	time.Set(DATA_TIMEOUT);
}

cSaClient::~cSaClient()
{
	delete clientData;
	delete [] barPeaks;
	free(name);
}
// ----------------------------------------------------------------------------

int cSaClient::Compare(const cListObject &ListObject) const
{
  cSaClient *obj=(cSaClient *)&ListObject;
  return strcasecmp(name,obj->name);
}
// ----------------------------------------------------------------------------

int cSaClient::Compare(const char *sName)
{
  return strcasecmp(sName,this->name);
}
// ----------------------------------------------------------------------------

void cSaClient::getBarHeightsOfSingleChannel( int *barHeightsTmp,
										int *barHeightsTmpOld,
										double *spectrumDataTmp, int *sVolumeChannel,
										int *barPeaksTmp, int (*barPeaks)[2] )
{
	int steps = NUM_BANDS/(2*bands);
/*
 * (Taken from xmms-apectrum-analyzer plugin)
 * Given the frequency spectrum for 256 values, divide these 
 * 256 values among the ranges mentioned in scaleXY below and 
 * select the largest frequency among those falling in one of
 * the scale ranges as the height of the "bar" representing
 * that range.
 */
 	// "completely arbitrary" steps
	int steps16[] = {0,	1,	3,	5,	9,	14,	21,	29,	40,	50,		60,		75,		90,	110, 137, 187, 255};
	int steps20[] = {0, 1, 3, 5, 9, 13, 19, 26, 34, 43, 53, 64, 76, 89, 105, 120, 140, 160, 187, 220, 255};
	int steps32[] = {0,	1,	2,	3,	4,	6,	8,	11,	14,	17,		20,		23,		27,	31, 35, 39, 43,
					48, 53, 58, 64, 71, 79, 88, 97, 107, 120, 140, 160, 180, 205, 230, 255};

	int i,j;
	int sum;
	int interval;
	bool optimized = !SpanSetup.UseLogarithmic && !SpanSetup.PureData && ( bands == 16 || bands == 20 || bands == 32 );
	double scale = (optimized && (bands == 32)) ? 3 : 1.4; // for 64 bands it would be about 6?!
	scale = (optimized && (bands == 20)) ? 1.3 : scale; // for 64 bands it would be about 6?!
	
	j=0;	
	sum = 0;
	interval = steps;
	*sVolumeChannel = 0;

	if ( bands > NUM_BANDS ) // more bands than processed here are not supported
		return;				 // (should be caught before calling this method)
	else
	{
		for ( i=0; i<NUM_BANDS && j<bands; i++)
		{
			sum += ((int)spectrumDataTmp[i]);
			
			if ( optimized && bands == 16 )
			{ // optimized for 16 bands
				steps = steps16[j+1]*(NUM_BANDS/512) + 1;
				interval = steps16[j+1]*(NUM_BANDS/512) + 1 - steps16[j]*(NUM_BANDS/512) + 1;
			}
			else if ( optimized && bands == 20 )
			{ // optimized for 20 bands
				steps = steps20[j+1]*(NUM_BANDS/512) + 1;
				interval = steps20[j+1]*(NUM_BANDS/512) + 1 - steps20[j]*(NUM_BANDS/512) + 1;
			}
			else if ( optimized && bands == 32 )
			{ // optimized for 32 bands
				steps = steps32[j+1]*(NUM_BANDS/512) + 1;
				interval = steps32[j+1]*(NUM_BANDS/512) + 1 - steps32[j]*(NUM_BANDS/512) + 1;
			}

			if ( (i+1) % (int)(steps) == 0 )
			{
				if (SpanSetup.PureData || (!SpanSetup.PureData && !optimized)) // generic computation
					barHeightsTmp[j] = (int)(sum/steps);
				else
					barHeightsTmp[j] = (int)(sum*scale);
				
				// Best practice - please let me know if there is a better way
				// to "equalize" the visualization			
				if (SpanSetup.UseLogarithmic)	// it's a try to reduce the "dynamic range" of various frequencies
				{								// e.g. lower frequencies are prevailing (very tall bars "on the left").
					if (barHeightsTmp[j] > 0)
					{
						barHeightsTmp[j] = (int)(log((double)barHeightsTmp[j]*barHeightsTmp[j]*barHeightsTmp[j])*log((double)(i*i+50)));
						barHeightsTmp[j] = (int)(barHeightsTmp[j]*0.7);
					}
					else
						barHeightsTmp[j] = 0;
				}
				else if ( !SpanSetup.PureData )
				{
					if ( !optimized )
					{
						barHeightsTmp[j] = (int)(barHeightsTmp[j]*log((double)(i))*log((double)(i)));
//						barHeightsTmp[j] = (int)(barHeightsTmp[j]*log((double)(i))*(j+1)*log((double)(j+1)));
//						barHeightsTmp[j] >>= 3;
					}
					barHeightsTmp[j] >>= 5;
				}
				else if ( SpanSetup.PureData ) // don't equalize the data in any way
				{
					barHeightsTmp[j] >>= 1;
				}
				
				// "Cutoff to 100" (HEIGHT == 100), not exact but it works
				if (barHeightsTmp[j] > HEIGHT)
					barHeightsTmp[j] = HEIGHT;
				
				// compute the bar-falloff depending on the height of the last run
				if(barHeightsTmpOld[j] - barHeightsTmp[j] > falloff && falloff != 0)
				{
					barHeightsTmp[j] = barHeightsTmpOld[j] - falloff;
				}
				
				// store the height for the next run
				barHeightsTmpOld[j] = barHeightsTmp[j];
				*sVolumeChannel += barHeightsTmp[j];
				
				// get the peak
				barPeaksTmp[j] = barHeightsTmp[j];
				getPeak( &barPeaks[j], &barPeaksTmp[j] );

				j++;
				sum = 0;
			}
		}
		*sVolumeChannel /= bands;
	}
	time.Set(DATA_TIMEOUT);
}
// ----------------------------------------------------------------------------

void cSaClient::getBarHeightsOfRightChannel( int *barHeightsTmp, double *spectrumDataTmp,
						int *sVolumeChannel, int *barPeaksTmp )
{
	getBarHeightsOfSingleChannel( barHeightsTmp, clientData->rightChannel->barHeightsOld,
					 				spectrumDataTmp, sVolumeChannel, barPeaksTmp, clientData->rightChannel->barPeaks );
}
// ----------------------------------------------------------------------------

void cSaClient::getBarHeightsOfLeftChannel( int *barHeightsTmp, double *spectrumDataTmp,
						int *sVolumeChannel, int *barPeaksTmp )
{
	getBarHeightsOfSingleChannel( barHeightsTmp, clientData->leftChannel->barHeightsOld,
					 				spectrumDataTmp, sVolumeChannel, barPeaksTmp, clientData->leftChannel->barPeaks );
}
// ----------------------------------------------------------------------------

void cSaClient::getBarHeightsOfBothChannels( int *dataBothChannels, int *dataLeftChannel,
										int *dataRightChannel, int *sVolumeLeftChannel,
										int *sVolumeRightChannel, int *sVolumeBothChannels,
										double *spectrumDataLeftChannel, double *spectrumDataRightChannel,
										int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp
						)
{
	int i;

	getBarHeightsOfRightChannel( dataRightChannel, spectrumDataRightChannel, sVolumeRightChannel, barPeaksRightTmp );
	getBarHeightsOfLeftChannel( dataLeftChannel, spectrumDataLeftChannel, sVolumeLeftChannel, barPeaksLeftTmp );
	
	for (i=0; i<bands; i++)
	{
		dataBothChannels[i] = (int)((dataRightChannel[i] + dataLeftChannel[i])/2);
		
		// get the peak
		barPeaksBothTmp[i] = dataBothChannels[i];
		getPeak( &barPeaks[i], &barPeaksBothTmp[i] );	
	}
	*sVolumeBothChannels = (int)((*sVolumeRightChannel + *sVolumeLeftChannel)/2);
}
// ----------------------------------------------------------------------------

void cSaClient::getFlatlineOfBothChannels( int *dataBothChannels, int *dataLeftChannel,
										int *dataRightChannel, int *sVolumeLeftChannel,
										int *sVolumeRightChannel, int *sVolumeBothChannels,
										double *spectrumDataLeftChannel, double *spectrumDataRightChannel,
										int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp
						)
{
	int i;
	
	for (i=0; i<bands; i++)
	{
		dataRightChannel[i] = 0;
		dataLeftChannel[i]  = 0;
		dataBothChannels[i] = 0;
		
		barPeaksRightTmp[i] = 0;
		barPeaksLeftTmp[i]  = 0;
		barPeaksBothTmp[i]  = 0;
	}
	
	*sVolumeRightChannel = 0;
	*sVolumeLeftChannel  = 0;
	*sVolumeBothChannels = 0;
}
// ----------------------------------------------------------------------------

void getPeak( int (*sBarPeaks)[2], int *sBarPeaksTmp )
{
	if ( sBarPeaksTmp[0] > sBarPeaks[0][0] )
	{
		sBarPeaks[0][0] = sBarPeaksTmp[0];
		sBarPeaks[0][1] = 0;
	}
	else
	{
		sBarPeaks[0][0] = sBarPeaks[0][0] - (sBarPeaks[0][1]*sBarPeaks[0][1])/10;
		sBarPeaks[0][1] += 1;
	}
	if ( sBarPeaks[0][0] < sBarPeaksTmp[0] )
	{
		sBarPeaks[0][0] = 0;
		sBarPeaks[0][1] = 0;
	}
	sBarPeaksTmp[0] = sBarPeaks[0][0];
}

#endif //___CLIENT_C
