/* mpadecdecoder.cpp
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
 

 
#include <mpadecdecoder.h>
#include <iostream>
#include <fcntl.h>

using namespace std;
using namespace izsound;




MpadecDecoder::MpadecDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DecoderBase(bufferSize, sampleRate)
{
  // Init
  fd = -1;
  previous_frame_position = 0;
  mpadec_config_t _config = { MPADEC_CONFIG_FULL_QUALITY, MPADEC_CONFIG_AUTO,
                           MPADEC_CONFIG_16BIT, MPADEC_CONFIG_LITTLE_ENDIAN,
                           MPADEC_CONFIG_REPLAYGAIN_NONE, true, true, true, 0.0 };
  config = _config;
  mpa = NULL;
  mpa = mp3dec_init();
  if (!mpa) 
  {
	cerr << "ERROR: mp3dec_init() throw an error." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  if ( mp3dec_configure(mpa, &config) != MP3DEC_RETCODE_OK ) 
  {
    	cerr << "ERROR: mp3dec_configure() return an error value. Abort." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  m_status = DECODER_OK;
}


MpadecDecoder::~MpadecDecoder()
{
  if ( mp3dec_uninit(mpa) != MP3DEC_RETCODE_OK ) 
  {
    	cerr << "ERROR: mp3dec_uninit() return an error value. Close error." << endl;
  }
}


void MpadecDecoder::decode( unsigned int frame_position, 
                            unsigned int buffer_size, void* buffer)
{
//   // Check !
//   if (file == NULL) return;
//   
//   // Check if we should perform a seek...
//   
//   if ( frame_position != previous_frame_position + buffer_size )
//   {
//   	long seek_point = frame_position;
// 	if ((seek_point < (long)getFrames()) && (seek_point >= 0))
// 	{
// 		sf_seek(file, (sf_count_t) seek_point, SEEK_SET);
// 	} 
// 	else
// 	{
// 		m_endReached = true;
// 		stop();
// // 		cerr << "ERROR: seek out of bounds" << endl;
// // 		m_status = DECODER_ERROR_UNKNOWN;
// 		return;
// 	}
//   }
//   
//   previous_frame_position = frame_position;
// 
//   
//   sf_count_t frames_decoded = 0;	
//   
//   // To decode we use internal double conversion of libsndfile...
// #ifdef _FLOAT_ENGINE
//   frames_decoded =   sf_readf_double(file, (float*)buffer, buffer_size);
// #endif 
// #ifdef _DOUBLE_ENGINE
//   frames_decoded =   sf_readf_double(file, (double*)buffer, buffer_size);
// #endif
// 
//   frames_in_buffer += buffer_size;
//   
//   // Check if end reached...
//   if ( frames_decoded != buffer_size ) 
//   {
//   	m_endReached = true;
// 	stop();
//   }
// 
// #ifdef DEBUG
// 	cout << "Frame Decoded..." << frame_position << endl;
// 	cout << "Frames Decoded..." << frames_decoded << endl;
// #endif 
  
}


void MpadecDecoder::openFile(const char* filename)
{
  // Before doing something, it is better to stop
  // the decoder: we avoid mutex issues!
  stop();
  
#ifdef DEBUG
  cout << "Decoder stopped: trying with opening" << endl;
#endif

  fd = open(filename, O_RDONLY /*| O_BINARY*/);
  
  if (fd < 0) 
  {
    	cerr << "ERROR: error while opening file." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  
  if ( (int err = mp3dec_init_file(mpa, fd, 0, false)) != MP3DEC_RETCODE_OK) 
  {
    	if ( err == MP3DEC_RETCODE_NOT_MPEG_STREAM )
	{
		cerr << "ERROR: wrong file format. Mpadec can't handle it." << endl;
		m_status = DECODER_FORMAT_UNKNOWN;
	}
	else
	{
		cerr << "ERROR: mp3dec_init_file() return an error value." << endl;
		m_status = DECODER_ERROR_UNKNOWN;
	}
	return;
  } 
  else 
  	fd = -1;

  if ( mp3dec_get_info(mpa, &mpainfo, MPADEC_INFO_STREAM) != MP3DEC_RETCODE_OK) 
  {
    	cerr << "ERROR: ERROR: mp3dec_get_info() return an error value." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
  }
  
  
  // Init
  m_endReached = false;
  m_frame_position = 0;
  m_direction = FORWARD;
  previous_frame_position = 0;
  
  // setting parameters...
  m_channels = (int)mpainfo.channels;
  m_samplerate = (int)mpainfo.decoded_frequency;
  m_frames = (unsigned int)(mpainfo.decoded_frame_samples * mpainfo.frames);
  max_frames_in_buffer = (unsigned int)( m_readBufferSize / m_channels );
  frames_in_buffer = 0;

  m_audioFormat = OSE_AUDIO_FLOAT;
  
#ifdef DEBUG
  cout << "mpainfo.decoded_frequency = " << mpainfo.decoded_frequency << endl;
  cout << "mpainfo.decoded_frame_samples = " << mpainfo.decoded_frame_samples << endl;
  cout << "m_frames = " << m_frames << endl;
  cout << "mpainfo.frame_samples = " << mpainfo.frame_samples << endl;
  cout << "All encoded samples = " << mpainfo.frame_samples * mpainfo.frames << endl;
#endif

  // now we are done!
  m_status = DECODER_OK;
}

