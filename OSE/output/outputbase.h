/* outputbase.h
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


#ifndef _OUTPUTBASE_H
#define _OUTPUTBASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <common_def.h>
#include <dspunit.h>
#include <pthread.h>
#include <string>



using namespace std;

namespace izsound
{

/**
 * 
 *
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
 
 
// Output Status definitions	
#define		OUTPUT_OK		0
#define		OUTPUT_ERROR_UNKNOWN	1
#define		OUTPUT_ERROR_OPEN	2
#define		OUTPUT_ERROR_WRITE	3
 
// DspUnit parameters definitions
#define		MAX_STEREO_OUTPUTS	8



typedef struct OSEDeviceInfo
{
    std::string name;             // Character string device identifier.
    bool probed;                  // true if the device capabilities were successfully probed.
    int outputChannels;           // Maximum output channels supported by device.
    int inputChannels;            // Maximum input channels supported by device.
    int duplexChannels;           // Maximum simultaneous input/output channels supported by device.
    bool isDefault;               // true if this is the default output or input device.
    std::vector<int> sampleRates; // Supported sample rates.
//     RtAudioFormat nativeFormats;  // Bit mask of supported data formats.
};
  

// typedef struct OutputObject
// {
//     OutputBase *the_output;
//     int device_id;
// };

  

class OutputBase : public DspUnit
{
  
  /*****************************************************************
   * ---- >> Functions TO BE reimplemented... << ----
   */
   
public:  
  
  /**
   * Retrieve the number of available devices.
   */
  virtual int getDeviceCount() = 0;
  
  /**
   * Retrieve info of a specific device.
   *
   * @param device_id The device id.
   */
  virtual void getDeviceInfo( int device_id, OSEDeviceInfo* info ) = 0;
  
private:

  /**
   * Write to the device a buffer of data.
   *
   * @param device_id The device id of the device to be written.
   * @param buffer_size Size of buffer in frames.
   * @param buffer The buffer pointer.
   */
  virtual void write( int device_id, unsigned int buffer_size, void* buffer ) = 0;
  
  /*
   * ---- >> End of Functions TO BE reimplemented... << ----
   ******************************************************************/
    
public:
 
 /**
   * The constructor. This ones starts with no file loaded.
   *
   * @param bufferSize The buffer size to use for the decoding. Note that this
   *        is a maximum size, but nothing guaranties that the buffer will be
   *        full at each <code>performDsp()</code> invocation.
   * @param sampleRate DspUnit samplerate parameter. Actually UNUSED.
   */
  OutputBase( const unsigned int &bufferSize = 4096,
              const unsigned int &sampleRate = 44100);
  
  /**
   * Destructor.
   */
  ~OutputBase();
  
  /**
   * The pcm worker function.
   */
  virtual void performDsp();
  
  
// private:
//   
//   /**
//    * The write callback function.
//    * Calls function write(...) for the right device object.
//    */
//   static int writeCallback(char *buffer, int bufferSize, void *output_object);

public:
  
  /**
   * Info about the status of output API.
   *
   * @return The status of the output API.
   */
  int status() { return m_status; }
  
protected:
  
  /** Status of the decoder unit. **/
  int m_status;
  
  /** The output buffer. */
  void* m_outBuffer[MAX_STEREO_OUTPUTS];
  
  /** The out buffer size. */
  unsigned int m_outBufferSize;
  
  /** Internal buffer position. Used by writeToBuffer() */
  unsigned int m_bufferPosition[MAX_STEREO_OUTPUTS];

};

}

#endif
