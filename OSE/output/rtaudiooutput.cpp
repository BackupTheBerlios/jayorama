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
  
  for ( int k = 0; k < MAX_NUM_OF_STREAMS; k++ )
  {
  	device_rack[ k ] = NULL;
#ifdef _FLOAT_ENGINE
	if ( m_outBuffer[k] != NULL )
		delete[] (float*)m_outBuffer[k];
#endif 
#ifdef _DOUBLE_ENGINE
	if ( m_outBuffer[k] != NULL )
		delete[] (double*)m_outBuffer[k];
#endif
  }
  
  RtAudio *temp_dev;
  // Opening the default and get some data
  try {
    temp_dev = new RtAudio();
  }
  catch (RtError &error) {
    // Handle the exception here
    error.printMessage();
    cerr << "Error: opening default RtAudio device " << endl;
    m_status = OUTPUT_ERROR_OPEN;
    return;
  }
  
  m_nDevices = temp_dev->getDeviceCount();
  
#ifdef DEBUG
  cout << "m_nDevices = " << m_nDevices << endl;
#endif


  int rack_index = 0;
  for ( int u = 1; u <= m_nDevices; u++ )
  {
  	int _samplerate = (int)sampleRate;
  	RtAudioDeviceInfo rt_info = temp_dev->getDeviceInfo( u );
	
	// We try to open the bigger number of stereo streams
	// on each device. If we could open more than MAX_NUM_OF_STREAMS
	// we stop when we reach MAX_NUM_OF_STREAMS.
	
	// if not probed the device is busy or unavailable
	if ( !rt_info.probed )
		continue;

	int num_of_out_stereo_streams_for_this_dev = rt_info.outputChannels / 2;
	
	bool sample_rate_ok = false;
	bool support_44100 = false;
	unsigned int sr_vector_size = rt_info.sampleRates.size();
#ifdef DEBUG
 	cout << "sr_vector_size = " << sr_vector_size << endl;
#endif
	if ( sr_vector_size == 0 )
	{
		cerr << "Error: empty samplerate vector found at device " << u << endl;
		continue;
	}
	for ( unsigned int y = 0; y < sr_vector_size; y++ )
	{
		if (rt_info.sampleRates[y] == _samplerate)
		{
			sample_rate_ok = true;
// 			break;
		}
		if ( rt_info.sampleRates[y] == 44100 )
			support_44100 = true;
#ifdef DEBUG
 		cout << "rt_info.sampleRates[y] = " << rt_info.sampleRates[y] << endl;
#endif
	}
	
	if ( ( sample_rate_ok == false ) && ( support_44100 == true ) )
	{
		cerr << "Error: can't set up samplerate " << sampleRate << endl;
		cerr << "	trying 44100 instead..." << endl;
		_samplerate = 44100;
	}
	else
	{
		if ( ( sample_rate_ok == false ) && ( support_44100 == false ) )
		{
			cerr << "Error: can't find a samplerate supported by device. Abort. " << endl;
			continue;
		}
	}
	
	for ( int g = 0; g < num_of_out_stereo_streams_for_this_dev; g++)
	{
		try {
			device_rack[ rack_index ] = new RtAudio(u, 2, 0, 0, format,
			_samplerate, (int*)&m_outBufferSize, DEFAULT_NUM_OF_BUFFERS);
		}
			catch (RtError &error) {
			cerr << "Error: opening RtAudio device " << u << endl;
			m_status = OUTPUT_ERROR_OPEN;
			return;
		}
		
		// Delete already instantiated buffers to get RtAudio buffer
  		// pointers. Start streaming then.
		try {
#ifdef _FLOAT_ENGINE
			m_outBuffer[ rack_index ] = (float*)device_rack[ rack_index ]->getStreamBuffer();
#endif 
#ifdef _DOUBLE_ENGINE
			m_outBuffer[ rack_index ] = (double*)device_rack[ rack_index ]->getStreamBuffer();
#endif
			device_rack[ rack_index ]->startStream();
		}
		catch (RtError &error) {
			error.printMessage();
			cerr << "Error: RtAudio error starting stream for device " << u << endl;
			m_status = OUTPUT_ERROR_OPEN;
			return;
		}
		rack_index++;
		if ( rack_index == MAX_NUM_OF_STREAMS )
			break;
	}
	
	if ( rack_index == MAX_NUM_OF_STREAMS )
		break;
  }
  

  m_status = OUTPUT_OK;
}


RtAudioOutput::~RtAudioOutput()
{
  for ( int i = 0; i < MAX_NUM_OF_STREAMS; i++)
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
  if ( (device_id > m_nDevices) && (device_id <= 0)  )
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

