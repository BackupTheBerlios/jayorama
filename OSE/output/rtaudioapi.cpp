/* rtaudioapi.cpp
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

#include <rtaudioapi.h>

using namespace std;
using namespace izsound;

RtAudioAPI::RtAudioAPI()
 :OutputAPIBase()
{
  m_apiName = "RtAudio";
  
#ifdef _FLOAT_ENGINE
  rt_audioFormat = RTAUDIO_FLOAT32;
#endif 
#ifdef _DOUBLE_ENGINE
  rt_audioFormat = RTAUDIO_FLOAT64;
#endif
  
  RtAudio *temp_dev;
  // Opening the default and get some data
  try {
    temp_dev = new RtAudio();
  }
  catch (RtError &error) {
    // Handle the exception here
    error.printMessage();
    cerr << "Error: opening default RtAudio device " << endl;
    m_status = OSE_ERROR_OPENING;
    return;
  }
  
  m_nDevices = temp_dev->getDeviceCount();
  delete temp_dev;
  
#ifdef DEBUG
  cout << "m_nDevices = " << m_nDevices << endl;
#endif

  // Set the last rt device opened to the first available value 
  last_rt_device_id = 1;
  // Set the free channels on las device to a neutral value
  free_output_channels_on_last_id = -1;

  m_status = OSE_OK;
}


RtAudioAPI::~RtAudioAPI()
{
  for ( int i = 0; i < m_outputObjects.size(); i++)
  {
	delete m_outputObjects[ i ];
  }
}


void RtAudioAPI::getDeviceInfo( int device_id, OSEDeviceInfo* info )
{
  if ( (device_id > m_nDevices) && (device_id <= 0)  )
  {
  	cerr << "Error: RtAudioAPI::getDeviceInfo() trying a non-existant device!" << endl;
	return;
  }
  if ( info == NULL )
  {
  	cerr << "Error: RtAudioAPI::getDeviceInfo() trying a non-existant info object!" << endl;
	return;
  }
  
  RtAudio *temp_dev;
  bool delete_temp_dev = false;
  if ( m_outputObjects.size() > 0 )
  {
  	RtAudioOutObject* obj = (RtAudioOutObject*)m_outputObjects[0];
  	temp_dev = obj->getStreamObject();
  }
  else 
  {
  	// Opening the default and get some data
	try {
		temp_dev = new RtAudio();
	}
	catch (RtError &error) {
		// Handle the exception here
		error.printMessage();
		cerr << "Error: opening default RtAudio device " << endl;
		m_status = OSE_ERROR_OPENING;
		return;
		}
	delete_temp_dev = true;
  }
  
  RtAudioDeviceInfo rt_info = temp_dev->getDeviceInfo(device_id);
  info->name = rt_info.name;
  info->probed = rt_info.probed;
  info->outputChannels = rt_info.outputChannels;
  info->inputChannels = rt_info.inputChannels;
  info->duplexChannels = rt_info.duplexChannels;
  info->isDefault = rt_info.isDefault;
  for ( unsigned int y = 0; y < rt_info.sampleRates.size(); y++)
  	info->sampleRates.push_back( rt_info.sampleRates[y] );
  if ( delete_temp_dev )
  	delete temp_dev;
}


OutputObjectBase* RtAudioAPI::getOutputUnit( int channels, int bufferSize, 
                                           int numOfBuffers, int sampleRate,
					   int device )
{
  RtAudio *obj;
  // if device parameter is not used, we try with the first
  // useful physical device to get out output object
  if ( device == -1 )
  {
	for ( int u = 1; u <= m_nDevices; u++ )
	{
		OSEDeviceInfo rt_info;
		getDeviceInfo( u, &rt_info );
		
		// if not probed the device is busy or unavailable
		if ( !rt_info.probed )
			continue;
	
		if ( channels > rt_info.outputChannels )
			continue;
		
		bool sample_rate_ok = false;
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
			if (rt_info.sampleRates[y] == sampleRate)
			{
				sample_rate_ok = true;
			}
#ifdef DEBUG
			cout << "rt_info.sampleRates[y] = " << rt_info.sampleRates[y] << endl;
#endif
		}
		
		if ( sample_rate_ok == false )
		{
			cerr << "Error: can't set up samplerate " << sampleRate << " for device "
			     << u << endl;
			continue;
		}
		
		try {
			obj = new RtAudio(u, channels, 0, 0, rt_audioFormat,
			                  sampleRate, &bufferSize, numOfBuffers);
		}
			catch (RtError &error) {
			cerr << "Error: opening RtAudio device " << u << endl;
			m_status = OSE_ERROR_OPENING;
			return NULL;
		}
		
		RtAudioOutObject *rt_object = new RtAudioOutObject( obj, bufferSize, sampleRate );
		
		if ( rt_object->status() == OSE_OK )
		{
			m_outputObjects.push_back( (OutputObjectBase*)rt_object );
			return (OutputObjectBase*)rt_object;
		}
		else
		{
			cerr << "Error: cannot create RtAudioOutObject " << u << endl;
			m_status = OSE_ERROR_UNKNOWN;
			delete rt_object;
			delete obj;
			return NULL;
		}
	}
	return NULL;
  }
  else
  {
  	OSEDeviceInfo rt_info;
	getDeviceInfo( device, &rt_info );
		
	// if not probed the device is busy or unavailable
	if ( !rt_info.probed )
	{
		cerr << "Error: device " << device << " busy or unavailable." << endl;
		return NULL;
	}

	if ( channels > rt_info.outputChannels )
	{
		cerr << "Error: device " << device << " has less channels than needed." << endl;
		return NULL;
	}
	
	bool sample_rate_ok = false;
	unsigned int sr_vector_size = rt_info.sampleRates.size();
#ifdef DEBUG
	cout << "sr_vector_size = " << sr_vector_size << endl;
#endif
	if ( sr_vector_size == 0 )
	{
		cerr << "Error: empty samplerate vector found at device " << device << endl;
		return NULL;
	}
	for ( unsigned int y = 0; y < sr_vector_size; y++ )
	{
		if (rt_info.sampleRates[y] == sampleRate)
		{
			sample_rate_ok = true;
		}
#ifdef DEBUG
		cout << "rt_info.sampleRates[y] = " << rt_info.sampleRates[y] << endl;
#endif
	}
	
	if ( sample_rate_ok == false )
	{
		cerr << "Error: can't set up samplerate " << sampleRate << " for device "
		<< device << endl;
		return NULL;
	}
	
	try {
		obj = new RtAudio(device, channels, 0, 0, rt_audioFormat,
				sampleRate, &bufferSize, numOfBuffers);
	}
		catch (RtError &error) {
		cerr << "Error: opening RtAudio device " << device << endl;
		m_status = OSE_ERROR_OPENING;
		return NULL;
	}
	
	RtAudioOutObject *rt_object = new RtAudioOutObject( obj, bufferSize, sampleRate );
	
	if ( rt_object->status() == OSE_OK )
	{
		m_outputObjects.push_back( (OutputObjectBase*)rt_object );
		return (OutputObjectBase*)rt_object;
	}
	else
	{
		cerr << "Error: cannot create RtAudioOutObject " << device << endl;
		m_status = OSE_ERROR_UNKNOWN;
		delete rt_object;
		delete obj;
		return NULL;
	}
	return NULL;
  }
}

