/*
 * spectrum_analyzer.c: The maths behind it
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

#ifndef ___SPECTRUM_ANALYZER_C
#define ___SPECTRUM_ANALYZER_C

#include <fftw3.h>

#include "spectrum_analyzer.h"
#include "setup.h"

#define BUFFSIZE 20000

// cAudioChannel ##############################################################
cAudioChannel::cAudioChannel()
{
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * FFTW_SIZE);
	plan = fftw_plan_dft_r2c_1d(NUM_BANDS, in, out, FFTW_PATIENT);
	
	cAudioChannel::reset();
}
// ----------------------------------------------------------------------------

cAudioChannel::~cAudioChannel()
{
	fftw_destroy_plan(plan);
	fftw_free(out);
}
// ----------------------------------------------------------------------------

void cAudioChannel::reset(void)
{
	int i;
	
	for (i = 0; i < FFTW_SIZE; i++)
	{
	  	in[i] = 0;
	  	out[i/2][0] = 0;
	  	out[i/2][1] = 0;
	}
	
	for (i = 0; i < NUM_BANDS; i++)
	{
	  	spectrumData[i] = 0;
	}
}
// ----------------------------------------------------------------------------

/*
 * Copy the PCM data to the FFTW input array.
 */ 
void cAudioChannel::initInput(int *data, int length)
{
    int i;
 
	for (i = 0; i < length && i < FFTW_SIZE; i++)
	{
		in[i] = (double)(data[i]);
	}
}
// ----------------------------------------------------------------------------

/*
 * Calculate the FFTW output freq amplitudes from the
 * real and imaginary parts of out[] array.
 */ 
void cAudioChannel::initOutput ()
{
    int i;
    double rl, im;

    for (i = 0; i < NUM_BANDS; i++)
	{
        rl = out[i][0]/FFTW_SIZE;
        im = out[i][1]/FFTW_SIZE;
		spectrumData[i] = (sqrt((rl * rl) + (im * im)));
    }
}
// ----------------------------------------------------------------------------

/*
 * The channel 0 of the stereo PCM data is used to calculate
 * the spectrum using FFTW. FFT/DFT on data sample of size N
 * produces a spectrum of size N/2 since the frequencies are
 * mirror images beyond N/2 (but the phases are opposite).
 */ 
void cAudioChannel::renderPcm(int *data, int length)
{
	reset();
    initInput(data, length);
    fftw_execute(plan);       
    initOutput();
}
// ----------------------------------------------------------------------------

// cSpectrumAnalyzer ##########################################################
cSpectrumAnalyzer::cSpectrumAnalyzer()
{
	right = new cAudioChannel;
	left = new cAudioChannel;
	playerControl = NULL;
	ringBuffer = NULL;
	dataBuff = NULL;
	bigEndian = true;
	
	resetRingBuffer();
}
// ----------------------------------------------------------------------------

cSpectrumAnalyzer::~cSpectrumAnalyzer()
{
	delete right;
	delete left;
	playerControl = NULL;
	delete ringBuffer;
	ringBuffer = NULL;
	delete dataBuff;
	dataBuff = NULL;
}
// ----------------------------------------------------------------------------

bool cSpectrumAnalyzer::putPcmData( const unsigned char *data, int length,
									int index, unsigned int bufferSize, bool bigEndian )
{
	bool returnCode = false;
	
	this->bigEndian = bigEndian;
	
	if ( ringBuffer == NULL )
	{
		ringBuffer = new cRingBufferFrame( (int)(bufferSize*1.1) );
	}
	if ( dataBuff == NULL )
	{
		dataBuff = new unsigned char[BUFFSIZE]; // TODO: check this for oversize
	}
	
	if ( index == writePosition || writePosition == 0 )
	{
		if ( dataBuffPos + length < BUFFSIZE )
		{
			memcpy( dataBuff + dataBuffPos, data, length );
			dataBuffPos += length;
			returnCode = true;
//			dsyslog("[span]: append data: index=%d, length=%d (%s()(%s,%d))", index, length, __func__, __FILE__, __LINE__ );
		}
		else
		{
			esyslog("[span]: Temp-Buffer overflow: %d bytes too much! (%s()(%s,%d))", dataBuffPos + length - BUFFSIZE, __func__, __FILE__, __LINE__ );
		}
	}
	else //if ( index > writePosition )
	{
		cFrame *frame = new cFrame( dataBuff, dataBuffPos, ftUnknown, index );
		
		// check for buffer overflow
		int neededBytes = ( ringBuffer->Available() + frame->Count() ) - (int)(bufferSize*1.1);
		// clean up a bit (remove oldest frame) to be able to store the latest frame
		if ( neededBytes > 0 )
		{
			cFrame *f = NULL;
			do
			{
				f = ringBuffer->Get();
				if( f )
				{
					neededBytes -= f->Count();
					ringBuffer->Drop(f);
				}
			} while( f && ( neededBytes > 0 ) );
		}
		
		// store the finished frame in the ringbuffer and (on success) begin the next frame
		if ( ringBuffer->Put( frame ) )
		{
//			dsyslog("[span]: store frame: index=%d, dataBuffPos=%d, available=%d (%s()(%s,%d))", index, dataBuffPos, ringBuffer->Available(), __func__, __FILE__, __LINE__ );
			returnCode = true;
			dataBuffPos = 0;
			
			memcpy( dataBuff + dataBuffPos, data, length );
			dataBuffPos += length;
		}
	}
	
	writePosition = index;
	
	time.Set(DATA_TIMEOUT);
	
	return returnCode;
}
// ----------------------------------------------------------------------------

void cSpectrumAnalyzer::resetRingBuffer()
{
	if ( ringBuffer != NULL )
	{
		ringBuffer->Clear();
		delete ringBuffer;
	}
	if ( dataBuff != NULL )
	{
		delete dataBuff;
	}
	
	ringBuffer = NULL;
	dataBuff = NULL;
	readPosition = 0;
	writePosition = 0;
	dataBuffPos = 0;
	visPosition = 0;
}
// ----------------------------------------------------------------------------

bool cSpectrumAnalyzer::setPlayindex( int index )
{
	readPosition = index;
//	dsyslog("[span]: Playindex set to readPosition=%d (%s()(%s,%d))", index, __func__, __FILE__, __LINE__ );
	time.Set(DATA_TIMEOUT);
	return true;
}
// ----------------------------------------------------------------------------
/*
 * The channel 0 of the stereo PCM data is used to calculate
 * the spectrum using FFTW. FFT/DFT on data sample of size N(=length)
 * produces a spectrum of size N/2 since the frequencies are
 * mirror images beyond N/2 (but the phases are opposite).
 */ 
void cSpectrumAnalyzer::saRenderPcm(const unsigned char *data, int length)
{
// Spectrum Analyzer: Push data to analyze
// Since I needed some time to figure it out:
// As reminder: "pc" is the counter for the bytes that have been written to p (unsigned char)
//				However, PCM data seems to consist of ([signed|unsigned]int).
//				That's why the length that's communicated to the span-plugin has to be smaller,
//				i.e pc/sizeof/(signed int) = pc/4.
// The PCM-data is stored interleaved: e.g. 16-bit stereo means, that there are:
//
// |--;--|--;--|--;--|--;--|	(|=border of int, - = 1 byte, ; = border of channels)
//  LL RR LL RR LL RR LL RR		LL/RR = two byte for the left/right channel
 
	int *dataAligned = new int[length];
	int z;
	
	if (bigEndian)
	{
		for (z = 0; z < length; z+=4)
		{
			dataAligned[z/4] = (int)((char)data[z+1]<<8) + (int)((char)data[z]);
			dataAligned[((int)length/sizeof(int))/2 + z/4] = (int)((char)data[z+3]<<8) + (int)((char)data[z+2]);
		}
	}
	else
	{
		for (z = 0; z < length; z+=4)
		{
			dataAligned[z/4] = (int)((char)data[z]<<8) + (int)((char)data[z+1]);
			dataAligned[((int)length/sizeof(int))/2 + z/4] = (int)((char)data[z+2]<<8) + (int)((char)data[z+3]);
		}
	}
	length = (int)length/sizeof(int);
	
	right->renderPcm( dataAligned, length/2 );
	left->renderPcm( dataAligned+length/2, length/2 );
	time.Set(DATA_TIMEOUT);

	delete [] dataAligned;
}
// ----------------------------------------------------------------------------

void cSpectrumAnalyzer::getAllData( int bands, int *dataBothChannels, int *dataLeftChannel,
						int *dataRightChannel, int *sVolumeLeftChannel,
						int *sVolumeRightChannel, int *sVolumeBothChannels,
						int *barPeaksBothTmp, int *barPeaksLeftTmp, int *barPeaksRightTmp,
						cSaClient *client )
{

	bool play = false;
	bool forward = false;
	int speed = 0;
	
	if ( playerControl != NULL )
	{
		playerControl->GetReplayMode( play, forward, speed );
		if ( !play )
		{
			if ( play == 0 && forward == 1 && speed == -1 ) // pausing
			{
				client->getFlatlineOfBothChannels( dataBothChannels,
									dataLeftChannel,
									dataRightChannel,
									sVolumeLeftChannel,
									sVolumeRightChannel,
									sVolumeBothChannels,
									left->spectrumData,
									right->spectrumData,
									barPeaksBothTmp,
									barPeaksLeftTmp,
									barPeaksRightTmp
								);
				return;
			}
			else // not pausing -> reset everything
			{
				left->reset();
				right->reset();
	            resetRingBuffer();
			}
		}
	}
	
//	dsyslog("%d %d", visPosition, readPosition);
	ringBufferMutex.Lock();
	if ( visPosition != readPosition )
	{
		cFrame *f = NULL;
		int tmpPosition = -1;
		do
		{
			f = ringBuffer->Get();
			if( f )
			{
//				dsyslog("got frame %d", f->Index() );
				tmpPosition = f->Index();
//				if ( tmpPosition < readPosition )
				{
//					dsyslog("dropping Frame %d", f->Index() );
                    if ( /*tmpPosition != readPosition && */readPosition > tmpPosition + SpanSetup.VisualizationDelay || readPosition < tmpPosition )
						ringBuffer->Drop(f);
                    else
                        break;
				}
//				else
//					dsyslog("keeping frame %d", f->Index() );
			}
		} while( f && ( tmpPosition != readPosition ) );

	
		if ( !f )
		{
			dsyslog("[span]: Ringbuffer is empty (%s()(%s,%d))", __func__, __FILE__, __LINE__ );
		}
		else
		{
			int max = 0;
			max = f->Count() > FFTW_SIZE*4 ? FFTW_SIZE*4 : f->Count();
			saRenderPcm( f->Data(), max );
			visPosition = readPosition;
			ringBuffer->Drop(f);
		}
	}
	ringBufferMutex.Unlock();
	
//	dsyslog("[span]: Visualizing visPosition=%d (%s()(%s,%d))", visPosition, __func__, __FILE__, __LINE__ );
	client->getBarHeightsOfBothChannels( dataBothChannels,
									dataLeftChannel,
									dataRightChannel,
									sVolumeLeftChannel,
									sVolumeRightChannel,
									sVolumeBothChannels,
									left->spectrumData,
									right->spectrumData,
									barPeaksBothTmp,
									barPeaksLeftTmp,
									barPeaksRightTmp
								);
}
// ----------------------------------------------------------------------------

void cSpectrumAnalyzer::Replaying( const cControl *DvbPlayerControl, const char *Name, const char *FileName, bool On )
{
	if ( DvbPlayerControl != NULL )
	{
		playerControl = (cControl *)DvbPlayerControl;
	}
	
	if (!On && left && right)
	{
		int deletedClients;
		deletedClients = deleteClients();
		
		if ( deletedClients > 0)
		{
			left->reset();
			right->reset();
		}
		DvbPlayerControl = NULL;
		
		resetRingBuffer();
	}
}
// ----------------------------------------------------------------------------

int cSpectrumAnalyzer::deleteClients( void )
{
	int i = 0;
	while ( clients.Count() != 0 )
	{
		clients.Del( clients.Get(0), true );
		i++;
	}
	if ( i>0 )
	{
		dsyslog("[span]: deleted clients: %d (%s()(%s,%d))", i, __func__, __FILE__, __LINE__ );
	}
	return i;
}

#endif //___SPECTRUM_ANALYZER_C
