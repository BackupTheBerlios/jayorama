/* decmpadecoder.h
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
 

#ifndef	_DECMPADECODER_H
#define	_DECMPADECODER_H



#include <decmpa/include/decmpa.h>
#include <decoderbase.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 


#define	MP3FRAMESIZE	(2*2*2*32*18)


using namespace std;

namespace izsound
{

/**
 * Decodes an audio mpeg stream using DecMPA. All frames facilities
 * are a fake, cause DecMPA does not support them. Although DecMPA allow
 * millisecond shifts which are sharp enough to make the fake
 * possible. Time and frame handling functions has been reimplemented.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class DecMPADecoder : public DecoderBase
{

private:
    
  /** DecMPA decoder object pointer. */
  void *pDecoder;
  
  /** Previous position in frames (pcm). Used to know when seeking. */
  unsigned int previous_frame_position;
  
  /** Previous nr of decoded frames. */
  unsigned int previous_frames_decoded;
  
  /** Mpeg frame decoded size in bytes. */
  long mpeg_frame_decoded_size;
  
  /** Nr of mpeg frames. */
  long mpeg_frames;
  
  /** Nr of pcm frames per mpeg frame. */
  int pcm_frames_per_mpeg_frame;
  
  /** That's out blocksize unit. We will work with this value 
    * in all seek and decoding operations. See decode function
    * for more explainations. */
  int block_unit;

  /** Internal pcm buffer. */
  short* pcmBuffer;
  
  /** Internal pcm buffer size in bytes. */
  long pcmBufferSizeBytes;
  
  /** Milliseconds per Mpeg frame. The value depends from samplerate. */
  long ms_per_mpeg_frame;
  
  /** Milliseconds of audio data in the pcm buffer. */
  long ms_in_buffer;
  
public:


  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  DecMPADecoder(const unsigned int &bufferSize = 4096,
              const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~DecMPADecoder();

   /**
   * Opens a file from a filename.
   *
   * WARNING! "open" function fills some info data about the file decoded:
   * 
   * m_channels, 
   * m_samplerate, 
   * m_frames, 
   * max_frames_in_buffer,
   * m_audioFormat (depending from what format the support library can handle).
   *
   *
   * @param filename The filename.
   * @throw IzSoundException An exception is thrown when the opening operation
   *                         process encounters a failure.
   */
  virtual void open(const char* filename);
  
private:

  /**
   * Decodes pcm data frames ( left sample + right sample)
   * and adds them to the buffer. This function must have a trace of
   * the previous frame position and seek if needed.
   * 
   * WARNING! "decode" function update some info data about status:
   *
   * frames_in_buffer.
   *
   * @param frame_position Where we start decoding (set in pcm frames).
   * @param block_size Size of buffer in frames (we should fill it).
   * @param buffer The buffer pointer.
   */
  virtual void decode( unsigned int frame_position, 
                       unsigned int buffer_size, void* buffer);


};


}


#endif

