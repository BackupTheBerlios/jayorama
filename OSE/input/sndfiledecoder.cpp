/* sndfiledecoder.cpp
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
 

 
#include <sndfiledecoder.h>
#include <iostream>

using namespace std;
using namespace izsound;




SndFileDecoder::SndFileDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DecoderBase(bufferSize, sampleRate)
{
  // Init
  previous_frame_position = 0;
  // format must be 0 if file is opened in read mode... (from sndfile docs)
  sfinfo.format = 0;
  m_status = OSE_OK;
}


SndFileDecoder::~SndFileDecoder()
{
  int close_error = sf_close( file );
  if ( close_error )
  {
  	cerr << "ERROR: closing file error code: " << sf_error_number(close_error) << endl;
  }
}


void SndFileDecoder::decode( unsigned int frame_position, 
                       unsigned int buffer_size, void* buffer)
{
  // Check !
  if (file == NULL) return;
  
  // Check if we should perform a seek...
  
  if ( frame_position != previous_frame_position + buffer_size )
  {
  	long seek_point = frame_position;
	if ((seek_point < (long)getFrames()) && (seek_point >= 0))
	{
		sf_seek(file, (sf_count_t) seek_point, SEEK_SET);
	} 
	else
	{
		m_endReached = true;
		stop();
// 		cerr << "ERROR: seek out of bounds" << endl;
// 		m_status = DECODER_ERROR_UNKNOWN;
		return;
	}
  }
  
  previous_frame_position = frame_position;

  
  sf_count_t frames_decoded = 0;	
  
  // To decode we use internal double conversion of libsndfile...
#ifdef _FLOAT_ENGINE
  frames_decoded =   sf_readf_double(file, (float*)buffer, buffer_size);
#endif 
#ifdef _DOUBLE_ENGINE
  frames_decoded =   sf_readf_double(file, (double*)buffer, buffer_size);
#endif

  frames_in_buffer += buffer_size;
  
  // Check if end reached...
  if ( frames_decoded != buffer_size ) 
  {
  	m_endReached = true;
	stop();
  }

#ifdef DEBUG
	cout << "Frame Decoded..." << frame_position << endl;
	cout << "Frames Decoded..." << frames_decoded << endl;
#endif 
  
}


void SndFileDecoder::open(const char* filename)
{
  // Before doing something, it is better to stop
  // the decoder: we avoid mutex issues!
  stop();
  
#ifdef DEBUG
  cout << "Decoder stopped: trying with opening" << endl;
#endif

  file = sf_open( filename, SFM_READ, &sfinfo);
  
  if (file == NULL)
  {
  	cerr << "ERROR: libsndfile error: " <<  sf_strerror(file) << endl;
	m_status = OSE_FORMAT_UNKNOWN;
	return;
  }
  
  // Init
  m_endReached = false;
  m_frame_position = 0;
  m_direction = FORWARD;
  previous_frame_position = 0;
  
  // setting parameters...
  m_channels = sfinfo.channels;
  m_samplerate = sfinfo.samplerate;
  m_frames = sfinfo.frames;
  max_frames_in_buffer = (unsigned int)( m_readBufferSize / m_channels );
  frames_in_buffer = 0;
  
  // Hope that Erik stuff is more performant than mine!
#ifdef _FLOAT_ENGINE
  m_audioFormat = OSE_AUDIO_FLOAT;
#endif 
#ifdef _DOUBLE_ENGINE
  m_audioFormat = OSE_AUDIO_DOUBLE;
#endif

  // now we are done!
  m_status = OSE_OK;
}

