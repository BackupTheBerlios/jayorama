/* sndfiledecoder.h
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
 

#ifndef	_SNDFILEDECODER_H
#define	_SNDFILEDECODER_H



#include <sndfile.h>
#include <decoderbase.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

//#include <common_def.h>


using namespace std;

namespace izsound
{

/**
 * Decodes an audio stream using libsndfile.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
class SndFileDecoder : public DecoderBase
{

private:
    
  /** SNDFILE pointer. Opened by libsndfile. */
  SNDFILE* file;
  
  /** Info structure allocated by libsndfile on file open. */
  SF_INFO sfinfo;
  
  /** Previous position in frames. Used to know when seeking. */
  unsigned int previous_frame_position;


public:


  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  SndFileDecoder(const unsigned int &bufferSize = 4096,
                 const unsigned int &sampleRate = 44100);

  /**
   * The destructor.
   */
  virtual ~SndFileDecoder();

   /**
   * Opens a file from a filename.
   *
   * WARNING! "openFile" function fills some info data about the file decoded:
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
  virtual void openFile(const char* filename);
  
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



