/* rtaudiooutput.cpp
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <rtaudiooutput.h>

using namespace std;
using namespace izsound;

RtAudioOutput::RtAudioOutput( const unsigned int &bufferSize ,
                              const unsigned int &sampleRate )
  : OutputBase( bufferSize, sampleRate )
{
#ifdef _FLOAT_ENGINE
  RtAudioFormat format = RTAUDIO_FLOAT32;
#endif 
#ifdef _DOUBLE_ENGINE
  RtAudioFormat format = RTAUDIO_FLOAT64;
#endif
  
  for ( int k = 0; k < MAX_STEREO_OUTPUTS; k++ )
  	device_rack[ k ] = NULL;
  
  // Opening the default and get some data
  try {
    device_rack[ 0 ] = new RtAudio(0, 2, 0, 0, format,
                      sampleRate, (int*)&m_outBufferSize, DEFAULT_NUM_OF_BUFFERS);
  }
  catch (RtError &error) {
    // Handle the exception here
    error.printMessage();
    cerr << "Error: opening default RtAudio device " << endl;
    m_status = OUTPUT_ERROR_OPEN;
    return;
  }
  
  m_nDevices = device_rack[0]->getDeviceCount();
  
  int to_be_opened = 0;
  if ( m_nDevices > MAX_STEREO_OUTPUTS )
  	to_be_opened = m_nDevices;
  else
  	to_be_opened = MAX_STEREO_OUTPUTS;
  
  for ( int u = 1; u < to_be_opened; u++ )
  {
	// stream opening... (always stereo ==> channels = 2!)
	try {
		device_rack[ u ] = new RtAudio(0, 2, 0, 0, format,
                      sampleRate, (int*)&m_outBufferSize, DEFAULT_NUM_OF_BUFFERS);
	}
		catch (RtError &error) {
		cerr << "Error: opening RtAudio device " << u << endl;
		m_status = OUTPUT_ERROR_OPEN;
    		return;
	}
  }
  
  // Delete already instantiated buffers to get RtAudio buffer
  // pointers.
#ifdef _FLOAT_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	if ( m_outBuffer[i] != NULL )
		delete[] (float*)m_outBuffer[i];
  }
#endif 
#ifdef _DOUBLE_ENGINE
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	if ( m_outBuffer[i] != NULL )
  		delete[] (double*)m_outBuffer[i];
  }
#endif

  // Start streaming and get buffer pointers
  for ( int u = 0; u < to_be_opened; u++ )
  {
  	try {
#ifdef _FLOAT_ENGINE
		m_outBuffer[ u ] = (float*)device_rack[ u ]->getStreamBuffer();
#endif 
#ifdef _DOUBLE_ENGINE
  		m_outBuffer[ u ] = (double*)device_rack[ u ]->getStreamBuffer();
#endif
		device_rack[ u ]->startStream();
	}
	catch (RtError &error) {
		error.printMessage();
		cerr << "Error: RtAudio error starting the stream "<< endl;
		m_status = OUTPUT_ERROR_OPEN;
    		return;
	}
  }

  m_status = OUTPUT_OK;
}


RtAudioOutput::~RtAudioOutput()
{
  for ( int i = 0; i < MAX_STEREO_OUTPUTS; i++)
  {
  	m_outBuffer[i] = NULL;
	
	if ( device_rack[ i ] != NULL )
	{
		try {
			// Stop and close the stream
			device_rack[ i ]->stopStream();
			device_rack[ i ]->closeStream();
		}
		catch (RtError &error) {
			error.printMessage();
			cerr << "Error: RtAudio error closing the stream "<< endl;
  		}
	}
	delete device_rack[ i ];
  }
}


void RtAudioOutput::getDeviceInfo( int device_id, OSEDeviceInfo* info )
{
  if ( device_id >= m_nDevices )
  {
  	cerr << "Error: RtAudioOutput::getDeviceInfo() trying a non-existant device!" << endl;
	return;
  }
  if ( info == NULL )
  {
  	cerr << "Error: RtAudioOutput::getDeviceInfo() trying a non-existant info object!" << endl;
	return;
  }
  
  RtAudioDeviceInfo rt_info = device_rack[ 0 ]->getDeviceInfo(device_id);
  info->name = rt_info.name;
  info->probed = rt_info.probed;
  info->outputChannels = rt_info.outputChannels;
  info->inputChannels = rt_info.inputChannels;
  info->duplexChannels = rt_info.duplexChannels;
  info->isDefault = rt_info.isDefault;
  for ( unsigned int y = 0; y < rt_info.sampleRates.size(); y++)
  	info->sampleRates.push_back( rt_info.sampleRates[y] );
}
  

void RtAudioOutput::write( int device_id, unsigned int , void* )
{
  if ( device_id >= m_nDevices )
  {
  	cerr << "Error: RtAudioOutput::write() trying a non-existant device!" << endl;
	return;
  }
  
  RtAudio *audio = device_rack[ device_id ];
  
  try {
	audio->tickStream();
  }
  catch (RtError &error) {
	error.printMessage();
	cerr << "Error: RtAudio error writing stream on device " << device_id << endl;
	m_status = OUTPUT_ERROR_WRITE;
	return;
  }
}

