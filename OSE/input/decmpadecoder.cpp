/* decmpadecoder.cpp
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
 
 
#include <decmpadecoder.h>
#include <iostream>

using namespace std;
using namespace izsound;




DecMPADecoder::DecMPADecoder(const unsigned int &bufferSize,
                               const unsigned int &sampleRate)
  : DecoderBase(bufferSize, sampleRate)
{
  // Init
  previous_frame_position = 0;
  pcmBufferSizeBytes = (long)(bufferSize * sizeof(short));
  pcmBuffer = new short[ bufferSize ];
  pDecoder = NULL;
  pcmBuffer = NULL;
  m_status = DECODER_OK;
}


DecMPADecoder::~DecMPADecoder()
{
  if ( pDecoder != NULL )
 	DecMPA_Destroy( pDecoder );
  if ( pcmBuffer != NULL )
 	delete[] pcmBuffer;
}


void DecMPADecoder::decode(int direction, unsigned int frame_position, 
                            unsigned int block_size, double* buffer)
{

  /*****************************************************
   * Some things to be said. We have intention
   * to seek by frame, but DecMPA (MAD too although) allows
   * only millisecond time seek. A millisecond has a number of
   * frames which depends from samplerate in this way:
   *
   * (frames per millisecond) = (samplerate / 1000)
   *
   * We are working with Mpeg standard and we will have
   * samplerate of 32000 / 44100 / 48000 Hz tipically. So
   * (frames per millisecond) will be 32, 44.1, 48
   * respectively. 44.1 it's a very ugly value: too
   * simple to have some issue with the 0.1.
   * So we can choose the minimum integer multiple
   * of our value: 441 which is 10 ms of data. The
   * other values are, another time, not a problem:
   * 320, 480.
   * Finally my choise: I define the block_unit value as
   * the number of frames in 10 milliseconds. This
   * is always an integer value and allow aritmethic
   * in our operations. The other side of the medal
   * is than block_unit introduces another aproximation...
   *
   * THIS STUFF HAS BEEN NOT YET IMPLEMENTED 
   *****************************************************/
   
  // Check !
  if (pDecoder == NULL) return;
  if (block_unit == 0)  return;
  
// #ifdef DEBUG
//   cout << "Decoder running..." << endl;
// #endif
  
  
  // Check if we should perform a seek...
  if ( previous_frames_decoded != 0 )
  {
	if (( frame_position != previous_frame_position + previous_frames_decoded ) || 
	    ( frame_position != previous_frame_position - previous_frames_decoded ))
	{
		// seek_point is a millisecond value, so an "about" value...
		// the error depends from samplerate and is 
		// between 0 to (m_samplerate / 1000) frames. If samplerate
		// is 44100 Hz we can have a fault of 44.1 frames.
		long seek_point = frame_position / (m_samplerate / 1000);
		
		
		if ( DecMPA_SeekToTime( pDecoder, seek_point) != DECMPA_OK )
		{
			cerr << "ERROR: DecMPA seek error." << endl;
			m_status = DECODER_ERROR_UNKNOWN;
			return;
		}
  
// 		if ((seek_point < mpeg_decoder->gettotalframe()) && (seek_point >= 0))
// 		{
// 			mpeg_decoder->setframe( seek_point );
// 		} 
// 		else
// 		{
// 			m_endReached = true;
// 			stop();
// 	// 		cerr << "ERROR: seek out of bounds" << endl;
// 	// 		m_status = DECODER_ERROR_UNKNOWN;
// 			return;
// 		}
	}
  }
  
  previous_frame_position = frame_position;

  
// #ifdef DEBUG
//   cout << "nr_encoded_frames_to_decode = " << nr_encoded_frames_to_decode << endl;
//   cout << "block_size = " << block_size << endl;
//   cout << "pcm_per_frame = " << pcm_per_frame << endl;
// #endif
  
  unsigned int samples_decoded = 0;
  long pBytesDecoded = 0;
  int status = 0;	
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
  	// direction == FORWARD
  	if ( (status = DecMPA_Decode( pDecoder, pcmBuffer, pcmBufferSizeBytes, &pBytesDecoded)) != DECMPA_OK )
	{
		if (status == DECMPA_END)
		{
			m_endReached = true;
			stop();
		}
		else
		{
			cerr << "ERROR: DecMPA decode error." << endl;
			m_status = DECODER_ERROR_UNKNOWN;
#ifdef DEBUG
  cout << "status..." << status << endl;
#endif
			return;
		}
	}
	
	samples_decoded = pBytesDecoded / sizeof(short);
  }
  
#ifdef DEBUG
  cout << "Decoded..." << endl;
#endif
  
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();

  // We put that on the stream
  unsigned int i = 0;
  unsigned int sample_count = 0;
  double left, right;
  while (i < samples_decoded)
  {
    // Bijection to [-1.0 ; 1.0]
    left   = ((double)pcmBuffer[ i++ ]) / 32768.0;
    right  = ((double)pcmBuffer[ i++ ]) / 32768.0;

    // We add it
    buffer[ sample_count++ ] = left;
    buffer[ sample_count++ ] = right;
  }
  
  // m_channels is always 2 actually... 
  previous_frames_decoded = sample_count / m_channels;
  frames_in_buffer += previous_frames_decoded;
  
#ifdef DEBUG
	cout << "frame_position..." << frame_position << endl;
	cout << "frames_in_buffer..." << frames_in_buffer << endl;
#endif 
  
}


void DecMPADecoder::open(const char* filename)
{
  // Before doing something, it is better to stop
  // the decoder: we avoid mutex issues!
  stop();

  int error = 0;
  DecMPA_MPEGHeader pHeader;
  DecMPA_OutputFormat pFormat;
  long pDecodedBytes = 0;
  long fileBytes = 0; 

  if ( (error = DecMPA_CreateUsingFile( &pDecoder, filename, DECMPA_VERSION )) != DECMPA_OK )
  {
  	cerr << "ERROR: DecMPA scores an error opening." << endl;
  	if ( error == DECMPA_ERROR_OPENFILE )
		m_status = DECODER_FORMAT_UNKNOWN;
	else
		m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
    
  // Before retrieving the Mpeg header we need to call decode once...
  if ( DecMPA_DecodeNoData( pDecoder, &pDecodedBytes) != DECMPA_OK )
  {
  	cerr << "ERROR: DecMPA init error." << endl;
  	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  fileBytes += pDecodedBytes;
  mpeg_frame_decoded_size = pDecodedBytes;
  	
  if ( DecMPA_GetMPEGHeader( pDecoder, &pHeader) != DECMPA_OK )
  {
  	cerr << "ERROR: DecMPA init error." << endl;
  	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  pcm_frames_per_mpeg_frame = pHeader.nDecodedSamplesPerFrame;
  
  if ( DecMPA_GetOutputFormat( pDecoder, &pFormat) != DECMPA_OK )
  {
  	cerr << "ERROR: DecMPA init error." << endl;
  	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
   
#ifdef DEBUG
  cout << "pFormat.nType = " << pFormat.nType << endl;
#endif
  
  if ( pFormat.nType != DECMPA_OUTPUT_INT16 )
  {
  	if ( DecMPA_SetParam( pDecoder, DECMPA_PARAM_OUTPUT, DECMPA_OUTPUT_INT16) != DECMPA_OK )
	{
		cerr << "ERROR: cannot set DecMPA output format." << endl;
		m_status = DECODER_ERROR_UNKNOWN;
		return;
	}
  }
  
  // Calculate file length..
  while( DecMPA_DecodeNoData( pDecoder, &pDecodedBytes) != DECMPA_END )
  	fileBytes += pDecodedBytes;
  
  // reset position to zero
  if ( DecMPA_SeekToTime( pDecoder, 0) != DECMPA_OK )
  {
	cerr << "ERROR: DecMPA seek error." << endl;
	m_status = DECODER_ERROR_UNKNOWN;
	return;
  }
  
  // Init
  m_endReached = false;
  m_frame_position = 0;
  m_direction = FORWARD;
  previous_frame_position = 0;
  m_samplerate = pFormat.nFrequency;
  mpeg_frames = (long)( fileBytes / mpeg_frame_decoded_size );
  m_frames = (unsigned int )(mpeg_frames * pcm_frames_per_mpeg_frame);
  m_channels = pFormat.nChannels;
  max_frames_in_buffer = (unsigned int)( m_readBufferSize / m_channels );
  frames_in_buffer = 0;
  block_unit = (m_samplerate / 1000) * 10;
  
#ifdef DEBUG
  cout << "pcm_frames_per_mpeg_frame = " << pcm_frames_per_mpeg_frame << endl;
  cout << "pcmBufferSizeBytes = " << pcmBufferSizeBytes << endl;
  cout << "fileBytes = " << fileBytes << endl;
  cout << "mpeg_frames = " << mpeg_frames << endl;
  cout << "mpeg_frame_decoded_size = " << mpeg_frame_decoded_size << endl;
#endif

  // now we are done!
  m_status = DECODER_OK;
}

