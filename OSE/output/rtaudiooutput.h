/* rtaudiooutput.h
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

#ifndef __RTAUDIO_OUTPUT_H
#define __RTAUDIO_OUTPUT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rtaudio/RtAudio.h>
#include <rtaudio/RtError.h>
#include <outputbase.h>

#define		DEFAULT_NUM_OF_BUFFERS		8

namespace izsound
{

/**
 * 
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
class RtAudioOutput : public OutputBase
{

public:  
 
  /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  RtAudioOutput( const unsigned int &bufferSize = 4096,
                 const unsigned int &sampleRate = 44100);
  
  /**
   * Destructor.
   */
  ~RtAudioOutput();

  /**
   * Retrieve the number of available devices.
   */
  virtual int getDeviceCount()  { return m_nDevices; }
  
  /**
   * Retrieve info of a specific device.
   *
   * @param device_id The device id.
   */
  virtual void getDeviceInfo( int device_id, OSEDeviceInfo* info );
  
private:

  /**
   * Write to the device a buffer of data.
   *
   * @param device_id The device id of the device to be written.
   * @param buffer_size Size of buffer in frames.
   * @param buffer The buffer pointer.
   */
  virtual void write( int device_id, unsigned int buffer_size, void* buffer );
  
  
  /** RtAudio objects rack. Everyone represents a device. */
  RtAudio* device_rack[MAX_STEREO_OUTPUTS];
  
  /** Number of RtAudio buffers. */
  int m_nBuffers;
  
  /** Number of RtAudio devices. */
  int m_nDevices;
};



}

#endif
