/*
 *	Code is freely inspired and reused from IzSound library
 *	by Julien PONGE  (See the copyright note later).
 *	The author of this file is, although, Gianluca Romanin (aka J_Zar)
 *	(See the first copyright note).
 */


/* sndfiledecoder.cpp
 *
 *  Copyright (C) 2004 Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 *  Some parts: 
 *  Copyright (C) 1998-2003 Andy Lo A Foe <andy_AT_alsaplayer.org>
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
 
 /*
 * IzSound - Copyright (c) 2003, 2004 Julien PONGE - All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
 
#include <formats/sndfiledecoder.h>
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
  m_status = DECODER_OK;
}


SndFileDecoder::~SndFileDecoder()
{
  int close_error = sf_close( file );
  if ( close_error )
  {
  	cerr << "ERROR: closing file error code: " << sf_error_number(close_error) << endl;
  }
}


void SndFileDecoder::decode(int direction, unsigned int frame_position, 
                            unsigned int block_size, double* buffer)
{
  // Check !
  if (file == NULL) return;
  
  // Check if we should perform a seek...
  if (( frame_position != previous_frame_position + block_size ) || 
      ( frame_position != previous_frame_position - block_size ))
  {
  	unsigned int seek_point = frame_position;
	if ((seek_point < getFrames()) && (seek_point >= 0))
	{
		sf_seek(file, (sf_count_t) seek_point, SEEK_SET);
	} 
	else
	{
		cerr << "ERROR: seek out of bounds" << endl;
		m_status = DECODER_ERROR_UNKNOWN;
		return;
	}
  }
  
  previous_frame_position = frame_position;

  
  sf_count_t frames_decoded = 0;	
  if ( direction == BACKWARD )
  {
  	sf_count_t frs = 0;
	for ( int j = 0, offset = 0; j < block_size; j++, offset++ )
	{
		sf_seek(file, (sf_count_t) (frame_position - 1) - j, SEEK_SET);
		
		frs = sf_readf_double(file, buffer + offset, 1);
		if ( frs == 1 )
		{
			frames_decoded++;
		}
	}
  
  
//   	// get a temp buffer
//   	double* temp_buffer = new double[ block_size ];
// 	
// 	for ( unsigned int i = 0, j = (block_size - 1); i < block_size; i++, j--)
// 	{
// 		// stereo mode only!
// 		unsigned int right = j--;
// 		unsigned int left = j;
// 		buffer[ left ] = temp_buffer[ i++ ];
// 		buffer[ right ] = temp_buffer[ i ];
// 	}
// 	delete[] temp_buffer;
  } 
  else
  {
  	// direction == FORWARD (all is simple!)
  	// To decode we use internal double conversion of libsndfile...
  	frames_decoded =   sf_readf_double(file, buffer, block_size);
  }
  frames_in_buffer += block_size;
  
  // Check if end reached...
  if ( frames_decoded != block_size ) 
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
	m_status = DECODER_FORMAT_UNKNOWN;
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

  // now we are done!
  m_status = DECODER_OK;
}
