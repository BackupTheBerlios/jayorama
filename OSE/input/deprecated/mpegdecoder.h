/* mpegdecoder.h
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
 

#ifndef	_MPEGDECODER_H
#define	_MPEGDECODER_H



#include <mpegsound/mpegsound.h>
#include <decoderbase.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 



using namespace std;

namespace izsound
{

/**
 * Decodes an audio mpeg stream using mpegsound.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class MpegDecoder : public DecoderBase
{

private:
    
  /** MpegSound raw decoder pointer. */
  Mpegtoraw *mpeg_decoder;

  /** MpegSound file loader. */
  Soundinputstream *loader;
  
  /** Previous position in frames (pcm). Used to know when seeking. */
  unsigned int previous_frame_position;
  
  /** Previous nr of decoded frames. */
  unsigned int previous_frames_decoded;
  
  /** Pcm samples per encoded frame. */
  unsigned int pcm_per_frame;
  
  /** Nr of encoded frames we can decode in the buffer. */
//   unsigned int nr_encoded_frames_in_buffer;
  
  /** Not always lucky, so we can have some more pcm than needed. */
  unsigned int buffer_offset;

  /** Internal pcm buffer. */
  short* pcmBuffer;
  
  /**
   * Calculate the encoded frame point to seek.
   *
   * @param pcm_frame_to_seek The frame (pcm frame) point to seek
   * @return The encoded frame number where to seek to obtain pcm_frame_to_seek.
   */
  unsigned int calculateEncodedFrameToSeek( unsigned int pcm_frame_to_seek );

public:


  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  MpegDecoder(const unsigned int &bufferSize = 4096,
              const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~MpegDecoder();

   /**
   * Opens a file from a filename.
   *
   * WARNING! "open" function fills some info data about the file decoded:
   * m_channels, m_samplerate, m_frames, max_frames_in_buffer
   *
   *
   * @param filename The filename.
   * @throw IzSoundException An exception is thrown when the opening operation
   *                         process encounters a failure.
   */
  virtual void open(const char* filename);
  
private:
  /**
   * Decodes a single pcm data frame ( left sample + right sample)
   * and adds it to the buffer. This function act in a different
   * way depending from what direction is set.
   *
   * @param direction Could be FORWARD or BACKWARD.
   */
  virtual void decode(int direction, unsigned int frame_position, 
                       unsigned int block_size, double* buffer);

};


}


#endif

