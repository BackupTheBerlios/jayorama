/* decoderbase.h
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


#ifndef _DECODERBASE_H
#define _DECODERBASE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <dspunit.h>
#include <pthread.h>



using namespace std;

namespace izsound
{

/**
 * Defines the base interface for file decoder classes.
 * Here you find all basic functions and the whole
 * thread stuff.
 *
 * DecoderBase class should be inherited by any format specific decoding
 * class. Each one SHOULD reimplement the following functions:
 *
 * -> void void decode(int direction, unsigned int frame_position, 
                       unsigned int block_size, double* buffer);
 *
 * -> void open(const char* filename);
 *
 * These are, in some way, specific for each format, so are not
 * implemented in the DecoderBase class. There is although
 * some freedom level, with the ability of reimplementing
 * virtual functions in the inheriting class (not suggested!). 
 * Pay attention that this action could compromise the whole decoder if
 * operations coded in DecoderBase are not executed!!! 
 *
 *
 * WARNING! A "frame" is intended in the libsndfile way,
 * as a group of samples, one for each channel. So, if we have
 * a stereo stream, a frame would have 2 samples.
 * We are using pcm data frames. So if we have a samplerate
 * of 44100 Hz and a stereo stream, each frame plays 1/44100 
 * of second. This is less than a millisecond and our decoder
 * class must be capable to seek frames.
 *
 * @author Gianluca Romanin (aka J_Zar) <jayorama_at_users.sourceforge.net>
 */
 
 
 
// Decoder Status definitions	
#define		DECODER_OK		0
#define		DECODER_ERROR_UNKNOWN	1
#define		DECODER_FORMAT_UNKNOWN	2
#define		DECODER_THREAD_ERROR	3 
 
// Decoder stream directions
#define 	FORWARD			1
#define 	BACKWARD		2
 

class DecoderBase : public DspUnit
{
  
  /*****************************************************************
   * ---- >> Functions TO BE reimplemented... << ----
   */
public:  
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
  virtual void open(const char* filename) = 0;
  
private:
  /**
   * Decodes a single pcm data frame ( left sample + right sample)
   * and adds it to the buffer. This function act in a different
   * way depending from what direction is set.
   *
   * @param direction Could be FORWARD or BACKWARD.
   */
  virtual void decode(int direction, unsigned int frame_position, 
                       unsigned int block_size, double* buffer) = 0;
  
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
  DecoderBase(const unsigned int &bufferSize = 4096,
              const unsigned int &sampleRate = 44100);
  
  /**
   * Destructor.
   */
  ~DecoderBase();
  
  /**
   * The pcm worker function.
   */
  virtual void performDsp();
   
  /**
   * Play operation. The expected behaviour is to start playing from the
   * current position.
   */
  virtual void play();

  /**
   * Pause operation.
   */
  virtual void pause();

  /**
   * Stop operation. The expected behaviour is to go back to the beginning of
   * the source.
   */
  virtual void stop();

  /**
   * Indicates if we have reached the end of the stream or not.
   *
   * @return <code>true</code> if we have reached the end, <code>false</code>
   *         otherwise.
   */
  virtual bool isEndReached() { return m_endReached; }

  /**
   * Returns the total stream playing time, in seconds.
   *
   * @return The total playing time.
   */
  virtual unsigned int getTotalTime();
  
  /**
   * Returns the total stream frames.
   *
   * @return The total frames.
   */
  virtual unsigned int getFrames();
  
  /**
   * Seek to frame.
   *
   * @param frame The frame position to seek.
   */
  virtual void setFrame(unsigned int frame);
  
  /**
   * Returns the current frame.
   *
   * @return The current frame.
   */
  virtual unsigned int getFrame();

  /**
   * Seeks to a position in seconds.
   *
   * @param pos The position in seconds.
   */
  virtual void setCurrentTime(int pos);

  /**
   * Gets the current time, in seconds.
   *
   * @return The current time, in seconds.
   */
  virtual int getCurrentTime();
  
  /**
   * Set the read direction of the audio stream.
   *
   * @param direction Could be FORWARD or BACKWARD.
   */
  virtual void setReadDirection(int direction);
  
  /**
   * Info about the status of decoder unit.
   *
   * @return The status of the decoder unit.
   */
  int status() { return m_status; }
  
private:
  
  /**
   * Create a detached thread, starts the main loop 
   * around performDsp() and set up mutexes.
   */
  virtual void startup();
  
  /**
   * Destroy thread and mutexes.
   */
  virtual void shutdown();
  
  /**
   * Performs all seek operations.
   */
//   virtual void seek();
  
  /**
   * When thread is started, loop function is called.
   * Here we process all decoding operation control.
   */
  static void* threadLoop(void *_decoder);
  
protected:
  
  /** Status of the decoder unit. **/
  int m_status;
  
  /** Decoder main thread. **/
  pthread_t decoder_thread;
  
  /** Mutex to prevent read/write errors on decoder configuration. **/
  pthread_mutex_t decoder_mutex;
  
  /**
   * Operation enumeration.
   */
  enum OperationRegister {PLAY, PAUSE, STOP/*, SEEK*/, SHUTDOWN};

  /** The decoder current operation. */
  OperationRegister m_operation;
  
  /**
   * Lock mutexes and set operation.
   */
//   virtual void doOperation( OperationRegister opcode );
  
  /** Tells wether we've reached the end of the stream or not. */
  bool m_endReached;
  
  /** Stream direction. */
  int m_direction;
  
  /** The reading buffer. */
  double* m_readBuffer;
  
  /** Position in frames */
  unsigned int m_frame_position;
  
  /** Nr of channels */
  int m_channels;
  
  /** Samplerate */
  int m_samplerate;
  
  /** Total Nr of frames */
  unsigned int m_frames;
  
  /** Max Nr of frames contained in the buffer */
  unsigned int max_frames_in_buffer;
  
  /** Actual Nr of frames contained in the buffer */
  unsigned int frames_in_buffer;
  
  /** The reading buffer size. */
  unsigned int m_readBufferSize;

};

}

#endif
