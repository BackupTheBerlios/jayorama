/* rtaudiooutobject.cpp
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

#include <rtaudiooutobject.h>

using namespace std;
using namespace izsound;

RtAudioOutObject::RtAudioOutObject( RtAudio *audio, int bufferSize, int sampleRate )
  : OutputObjectBase( bufferSize, sampleRate )
{
#ifdef _FLOAT_ENGINE
  RtAudioFormat format = RTAUDIO_FLOAT32;
  if ( m_outBuffer != NULL )
	delete[] (float*)m_outBuffer;
#endif 
#ifdef _DOUBLE_ENGINE
  RtAudioFormat format = RTAUDIO_FLOAT64;
  if ( m_outBuffer != NULL )
	delete[] (double*)m_outBuffer;
#endif
  
  m_streamObject = audio;
  
  try {
#ifdef _FLOAT_ENGINE
	m_outBuffer = (float*)m_streamObject->getStreamBuffer();
#endif 
#ifdef _DOUBLE_ENGINE
	m_outBuffer = (double*)m_streamObject->getStreamBuffer();
#endif
	m_streamObject->startStream();
  }
  catch (RtError &error) {
	error.printMessage();
	cerr << "Error: RtAudio error starting stream for the device." << endl;
	m_status = OSE_ERROR_OPENING;
	return;
  }
  m_status = OSE_OK;
}


RtAudioOutObject::~RtAudioOutObject()
{
  // avoid buffer delete and segfault
  m_outBuffer = NULL;
	
  if ( m_streamObject != NULL )
  {
	try {
		// Stop and close the stream
		m_streamObject->stopStream();
		m_streamObject->closeStream();
	}
	catch (RtError &error) {
		error.printMessage();
		cerr << "Error: RtAudio error closing the stream "<< endl;
	}
  	delete m_streamObject;
  }
}
 

void RtAudioOutObject::write( unsigned int , void* )
{
  try {
	m_streamObject->tickStream();
  }
  catch (RtError &error) {
	error.printMessage();
	cerr << "Error: RtAudio error writing stream on device. "<< endl;
	m_status = OSE_ERROR_WRITE;
	return;
  }
}

