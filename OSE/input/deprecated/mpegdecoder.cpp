/* mpegdecoder.cpp
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
 
 
#include <mpegdecoder.h>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace izsound;




MpegDecoder::MpegDecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DecoderBase(bufferSize, sampleRate)
{
  // Init
  previous_frame_position = 0;
//   loader = new Soundinputstreamfromfile();
//   mpeg_decoder = new Mpegtoraw((Soundinputstream*) loader);

  // Our internal buffer is larger than needed, cause we expect
  // to have some buffer issue...
  pcmBuffer = new short[/*bufferSize * 4*/ RAWDATASIZE*sizeof(short)*bufferSize];
  mpeg_decoder = NULL;
  m_status = DECODER_OK;
}


MpegDecoder::~MpegDecoder()
{
  if ( mpeg_decoder != NULL )
  	delete mpeg_decoder;
  if ( loader != NULL )
  	delete loader;
}


void MpegDecoder::decode(int direction, unsigned int frame_position, 
                            unsigned int block_size, double* buffer)
{
  // Check !
  if (mpeg_decoder == NULL) return;
  
#ifdef DEBUG
  cout << "Decoder running..." << endl;
#endif
  
  unsigned int sample_offset = 0;
  
  // Check if we should perform a seek...
  if ( previous_frames_decoded != 0 )
  {
	if (( frame_position != previous_frame_position + previous_frames_decoded ) || 
	    ( frame_position != previous_frame_position - previous_frames_decoded ))
	{
		int seek_point = (frame_position * m_channels) / pcm_per_frame;
		
		// we set already the offset for all directions...
		if ( direction == FORWARD )
			sample_offset = pcm_per_frame - ((frame_position * m_channels) % pcm_per_frame);
		else
			sample_offset = ((frame_position * m_channels) % pcm_per_frame);
		
		if ((seek_point < mpeg_decoder->gettotalframe()) && (seek_point >= 0))
		{
			mpeg_decoder->setframe( seek_point );
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
  }
  
  previous_frame_position = frame_position;

  int nr_encoded_frames_to_decode =  ( block_size * m_channels * 4 ) / pcm_per_frame;
  
#ifdef DEBUG
  cout << "nr_encoded_frames_to_decode = " << nr_encoded_frames_to_decode << endl;
  cout << "block_size = " << block_size << endl;
  cout << "pcm_per_frame = " << pcm_per_frame << endl;
#endif
  
  unsigned int samples_decoded = 0;	
  if ( direction == BACKWARD )
  {
//   	sf_count_t frs = 0;
// 	for ( int j = 0, offset = 0; j < block_size; j++, offset += m_channels )
// 	{
// 		sf_seek(file, (sf_count_t) (frame_position - 1) - j, SEEK_SET);
// 		
// 		frs = sf_readf_double(file, buffer + offset, 1);
// 		if ( frs == 1 )
// 		{
// 			frames_decoded++;
// 		}
// 	}
  } 
  else
  {
  	// direction == FORWARD (all is simple!)
  	samples_decoded = mpeg_decoder->run( pcmBuffer, nr_encoded_frames_to_decode );
  }
  
#ifdef DEBUG
  cout << "Decoded..." << endl;
#endif
  
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  // We put that on the stream
  unsigned int i = sample_offset;
  unsigned int sample_count = 0;
  short ileft, iright;
  double left, right;
  while (i < samples_decoded)
  {
    // Data unpacking
    ileft  = ((short)pcmBuffer[i++]) & 0xff;
    ileft  = ileft | (short)(((short)(pcmBuffer[i++])) << 8);
    iright = ((short)pcmBuffer[i++]) & 0xff;
    iright = iright | (short)(((short)(pcmBuffer[i++])) << 8);

    // Bijection to [-1.0 ; 1.0]
    left   = ((double)ileft) / 32768.0;
    right  = ((double)iright) / 32768.0;

    // We add it
    buffer[ sample_count++ ] = left;
    buffer[ sample_count++ ] = right;
  }
  
  // m_channels is always 2 actually... 
  frames_in_buffer += sample_count / m_channels;
  
  // Check if end reached...
  if ( mpeg_decoder->geterrorcode() == SOUND_ERROR_FINISH ) 
  {
  	m_endReached = true;
	stop();
  }
  
#ifdef DEBUG
	cout << "frame_position..." << frame_position << endl;
	cout << "frames_in_buffer..." << frames_in_buffer << endl;
#endif 
  
}


void MpegDecoder::open(const char* filename)
{
  // Before doing something, it is better to stop
  // the decoder: we avoid mutex issues!
  stop();
  
#ifdef DEBUG
  cout << "Decoder stopped: trying with opening" << endl;
#endif

  // avoid const error
  char* fname;
  sprintf(fname, "%s", filename);
 
//   Soundinputstream* loader;
  int err;

  /**** 
   * the following code is pretty inspired from OSALP project 
   * (http://osalp.sourceforge.net)
   * written by Bruce W. Forsberg and released under LGPL license.
   ****/
   
  if((loader = Soundinputstream::hopen( fname, &err )) == NULL)
  {
	cerr << "ERROR: mpegsound load error." << endl;
	m_status = DECODER_FORMAT_UNKNOWN;
	return;
  }

  mpeg_decoder = new Mpegtoraw( loader );

  if( mpeg_decoder == NULL )
  {
	delete loader;
	cerr << "ERROR: mpegsound load error." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }

  mpeg_decoder->initialize( fname );
  pcm_per_frame = (unsigned int)(mpeg_decoder->run( pcmBuffer, 1 ));
  if( pcm_per_frame == 0 )
  {
	delete loader;
	delete mpeg_decoder;
	mpeg_decoder = NULL;
	cerr << "ERROR: mpegsound load error." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  // trick if position is not 1!
  pcm_per_frame /= mpeg_decoder->getcurrentframe();
  
  
  if (mpeg_decoder->geterrorcode() != SOUND_ERROR_OK)
  {
  	if (mpeg_decoder->geterrorcode() == SOUND_ERROR_FILEOPENFAIL)
	{
		m_status = DECODER_FORMAT_UNKNOWN;
		return;
	}
	else
	{
  		cerr << "ERROR: mpegsound load error." << endl;
		m_status = DECODER_FORMAT_UNKNOWN;
		return;
	}
  }
  
  // Init
  m_endReached = false;
  m_frame_position = 0;
  mpeg_decoder->setframe( 0 );
  m_direction = FORWARD;
  previous_frame_position = 0;
  previous_frames_decoded = 0;
  buffer_offset = 0;
  
  // setting parameters...
  if(mpeg_decoder->isstereo())
	m_channels = 2;
  else
// 	m_channels = 1;
  {
  	cerr << "ERROR: cannot parse mono stream." << endl;
		m_status = DECODER_ERROR_UNKNOWN;
		return;
  }

  m_samplerate = mpeg_decoder->getfrequency();
  m_frames = (unsigned int )(pcm_per_frame * mpeg_decoder->gettotalframe() / m_channels);
  max_frames_in_buffer = (unsigned int)( m_readBufferSize / m_channels );
  frames_in_buffer = 0;

  // now we are done!
  m_status = DECODER_OK;
}


unsigned int MpegDecoder::calculateEncodedFrameToSeek( unsigned int pcm_frame_to_seek )
{
  
}

