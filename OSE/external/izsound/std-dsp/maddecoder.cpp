/*
 * IzSound - Copyright (c) 2003, 2004 Julien Ponge - All rights reserved.
 *
 * Portions are:
 * - Copyright (c) 2000-2003 Underbit Technologies, Inc. [minimad.c]
 * - Copyright (c) 1995-1999 Michael Hipp. [xmms mpg123 plugin]
 * - Copyright (c) 2004 Karl Pitrich.
 *
 * The MadDecoder DSP unit (izsound/maddecoder.h + maddecoder.cpp) is published
 * under the terms of the following license agreement:
 *
 * -----------------------------------------------------------------------------
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -----------------------------------------------------------------------------
 *
 * A copy of the GNU General Public License is available in the IzSound source
 * code distribution (COPYING.GPL).
 */

/*
 * The MadDecoder DSP unit implementation is based on the madlld program from
 * Bertrand Petit: http://www.bsd-dk.dk/~elrond/audio/madlld/ .
 */

/****************************************************************************
 * madlld.c -- A simple program decoding an mpeg audio stream to 16-bit     *
 * PCM from stdin to stdout. This program is just a simple sample           *
 * demonstrating how the low-level libmad API can be used.                  *
 *--------------------------------------------------------------------------*
 * (c) 2001, 2002 Bertrand Petit                                            *
 *                                                                          *
 * Redistribution and use in source and binary forms, with or without       *
 * modification, are permitted provided that the following conditions       *
 * are met:                                                                 *
 *                                                                          *
 * 1. Redistributions of source code must retain the above copyright        *
 *    notice, this list of conditions and the following disclaimer.         *
 *                                                                          *
 * 2. Redistributions in binary form must reproduce the above               *
 *    copyright notice, this list of conditions and the following           *
 *    disclaimer in the documentation and/or other materials provided       *
 *    with the distribution.                                                *
 *                                                                          *
 * 3. Neither the name of the author nor the names of its contributors      *
 *    may be used to endorse or promote products derived from this          *
 *    software without specific prior written permission.                   *
 *                                                                          *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''       *
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED        *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A          *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR       *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,             *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF         *
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,       *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT       *
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       *
 * SUCH DAMAGE.                                                             *
 *                                                                          *
 ****************************************************************************/

#include "maddecoder.h"

using namespace std;
using namespace izsound;

/*
 * A frame.
 */
struct frame
{
  int lsf;
  int mpeg25;
  int lay;
  int bitrate_index;
  int sampling_frequency;
  int padding;
  int framesize;
};

/*
 * A Xing header.
 */
typedef struct
{
  int frames;   /* total bit stream frames from Xing header data */
  int bytes;    /* total bit stream bytes from Xing header data */
  unsigned char toc[100]; /* "table of contents" */
} xing_header_t;

/*
 * Various helper functions.
 */
static long convert_to_header(unsigned char *buf);
static bool head_check(unsigned long head);
static double compute_tpf(struct frame *fr);
static double compute_bpf(struct frame *fr);
static int decode_header(struct frame *fr, unsigned long newhead);
static void *map_file(int fd, unsigned long length);
static int unmap_file(void *fdm, unsigned long length);
static int get_xing_header(xing_header_t * xing, const char *buf);

MadDecoder::MadDecoder(const unsigned int &sampleRate)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  commonInits();
  m_playerStatus = STOP;
}

MadDecoder::MadDecoder(const char* filename,
                       const unsigned int &sampleRate) throw(IzSoundException)
  : DspUnit(sampleRate, 0, 1)
{
  // Init
  commonInits();
  m_playerStatus = PLAY;
  open(filename);
}

void MadDecoder::commonInits()
{
  m_frameCount      = 0;
  m_inputFile       = 0;
  m_fileSize        = 0;
  m_endReached      = false;
}

void MadDecoder::initMadStructs()
{
  mad_stream_init(&m_stream);
  mad_frame_init(&m_frame);
  mad_synth_init(&m_synth);
  mad_timer_reset(&m_timer);
}

void MadDecoder::releaseMadStructs()
{
  mad_synth_finish(&m_synth);
  mad_frame_finish(&m_frame);
  mad_stream_finish(&m_stream);
  mad_timer_reset(&m_timer);
}

MadDecoder::~MadDecoder()
{
  closeFile();
}

void MadDecoder::closeFile()
{
  if (m_inputFile == 0) return;
  unmap_file((void *)m_fdm, m_fileSize);
  close(m_inputFile);
  releaseMadStructs();
}

void MadDecoder::performDsp()
{
  switch (m_playerStatus)
  {
    case PLAY  : playNextChunk(); break;
    case PAUSE : playNothing();   break;
    case STOP  : playNothing();   break;
  }
}

void MadDecoder::playNothing()
{
  SlotData* output = m_outSlots[0];
  (*output)[0].clear();
  (*output)[1].clear();
}

void MadDecoder::playNextChunk()
{
  // Fills the input buffer
fill_me:
  if(m_stream.buffer == NULL || m_stream.error == MAD_ERROR_BUFLEN)
  {
    long chunkSize;
    long remaining;

    // There can be residual data in the input buffer
    if(m_stream.next_frame != NULL)
    {
      m_offset  = m_stream.next_frame - m_fdm;
      remaining = m_fileSize - m_offset;

      chunkSize  = INPUT_BUFFER_SIZE;
      if(remaining < INPUT_BUFFER_SIZE)
      {
        chunkSize = remaining + MAD_BUFFER_GUARD;
      }
    }
    else
    {
      chunkSize = INPUT_BUFFER_SIZE;
      remaining = 0;
    }

    if(m_offset >= m_fileSize)
    {
      m_endReached = true;
      stop();
      return;
    }

    // Fill mad buffer
    mad_stream_buffer(&m_stream, m_fdm + m_offset, chunkSize);
    m_stream.error = MAD_ERROR_NONE;
  }

  // Decoding
  if(mad_frame_decode(&m_frame, &m_stream))
  {
    if(MAD_RECOVERABLE(m_stream.error))
    {
      goto fill_me;
    }
    else
    {
      if(m_stream.error == MAD_ERROR_BUFLEN)
      {
        goto fill_me;
      }
      else
      {
        stop();
        return;
      }
    }
  }

  // Frames & timer update
  ++m_frameCount;
  mad_timer_add(&m_timer, m_frame.header.duration);

  // PCM synth
  mad_synth_frame(&m_synth, &m_frame);

  // Now, we have some fresh samples
  SlotData* output = m_outSlots[0];
  short sample;
  double left;
  double right;
  (*output)[0].clear();
  (*output)[1].clear();
  for(int i = 0; i < m_synth.pcm.length; ++i)
  {
    // Left channel
    sample = scale(m_synth.pcm.samples[0][i]);
    left = ((double)sample) / 32768.0;

    // Right channel
    if(MAD_NCHANNELS(&m_frame.header) == 2)
    {
      sample = scale(m_synth.pcm.samples[1][i]);
    }
    right = ((double)sample) / 32768.0;

    // We send it
    (*output)[0].push_back(left);
    (*output)[1].push_back(right);
  }
}

int MadDecoder::scale(mad_fixed_t sample)
{
  // Rounding
  sample += (1L << (MAD_F_FRACBITS - 16));

  // Clipping
  if (sample >= MAD_F_ONE)
  {
    sample = MAD_F_ONE - 1;
  }
  else if (sample < -MAD_F_ONE)
  {
    sample = -MAD_F_ONE;
  }

  // Quantization
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

void MadDecoder::play()
{
  m_playerStatus = PLAY;
}

void MadDecoder::pause()
{
  m_playerStatus = PAUSE;
}

void MadDecoder::stop()
{
  m_playerStatus = STOP;
  m_frameCount = 0;
  releaseMadStructs();
  initMadStructs();
  m_offset = 0;
}

void MadDecoder::open(const char* filename) throw(IzSoundException)
{
  // Release the previous resources
  closeFile();
  initMadStructs();

  // File opening
  m_inputFile = ::open(filename, O_RDONLY);
  if(m_inputFile != -1)
  {
    struct stat stat;
    if (fstat(m_inputFile, &stat) != -1)
    {
      m_fileSize = S_ISREG(stat.st_mode) ? stat.st_size : 0;
      if (S_ISREG(stat.st_mode) && stat.st_size > 0)
      {
        m_fileSize = stat.st_size;
        m_fdm = (unsigned char *)map_file(m_inputFile, m_fileSize);
        if(m_fdm == 0)
        {
          throw IzSoundException("Could not mmap the file.");
        }
        m_totalTime = getSongTime();
      }
    }
  }

  // Get ready
  if ((m_inputFile == -1) || (m_totalTime <= 0.0))
  {
    throw IzSoundException("File opening failed.");
  };
  m_endReached = false;
  m_frameCount = 0;
}

bool MadDecoder::isEndReached()
{
  return m_endReached;
}

double MadDecoder::getTotalTime()
{
  return m_totalTime;
}

void MadDecoder::seek(const double &pos)
{
  // Sanity checks
  double timePos = pos;
  if (pos > m_totalTime)
  {
    timePos = m_totalTime;
  }
  else if (pos < 0.0)
  {
    timePos = 0.0;
  }

  /* We compute a file offset to perform the seeking.
   * Yes, it is not perfectly accurate, but the libmad sucks a lot.
   * In contrast, libogg was made by _real_ programmers.
   */
  double ratio  = timePos / m_totalTime;
  double offset = m_fileSize * ratio;
  m_offset = (unsigned long)offset;

  // We need to reset the libmad structs
  initMadStructs();
}

double MadDecoder::getCurrentTime()
{
  double msecs = (double)mad_timer_count(m_timer, MAD_UNITS_MILLISECONDS);
  return (msecs / 1000.0);
}


long MadDecoder::getSongLength()
{
  // Vars
  char tmp[4];
  int len = m_fileSize;

  // We get it
  memcpy(&tmp, m_fdm + m_fileSize - 128, 3);
  if (!strncmp(tmp, "TAG", 3))
  {
    len -= 128;
  }
  return len;
}

double MadDecoder::getSongTime()
{
  // Vars
  unsigned long head;
  unsigned char tmp[4];
  struct frame frm;
  double tpf, bpf;
  unsigned long len;
  unsigned char *mapPtr = m_fdm;
  unsigned char *buf;
  xing_header_t xing_header;

  // Inits
  memcpy(&tmp, mapPtr, 4);
  mapPtr += 4;
  head = convert_to_header(tmp);

  // We find a header
  while (!head_check(head))
  {
    head <<= 8;
    memcpy(&tmp, ++mapPtr, 1);
    head |= tmp[0];
  }

  // We decode it
  if (decode_header(&frm, head))
  {
    tpf = compute_tpf(&frm);
    buf = (unsigned char *)malloc(frm.framesize + 4);
    mapPtr -= 4;
    memcpy(buf, mapPtr, frm.framesize + 4);
    if (get_xing_header(&xing_header, (const char*)buf))
    {
      free(buf);
      if (xing_header.bytes == 0)
      {
        xing_header.bytes = getSongLength();
      }
      return (double)tpf * xing_header.frames;
    }
    free(buf);
    bpf = compute_bpf(&frm);
    len = getSongLength();
    return (double)(len / bpf) * tpf;
  }

  return 0;
}

static void *map_file(int fd, unsigned long length)
{
  void *fdm;

  fdm = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED)
  {
    return 0;
  }

  madvise(fdm, length, MADV_SEQUENTIAL);
  return fdm;
}

static int unmap_file(void *fdm, unsigned long length)
{
  return (munmap(fdm, length) == -1) ? -1 : 0;
}

// Song time determination stuff.
// Stolen from xmms-mpg123 plugin.

static const int bitrates_table[2][3][16] =
{
  {
    {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,},
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384,},
    {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320,}
  },
  {
    {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256,},
    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,},
    {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160,}
  }
};

static const int samplefreq_table[9] =
{
  44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000
};

#define GET_INT32BE(b) \
  (i = (b[0] << 24) | (b[1] << 16) | b[2] << 8 | b[3], b += 4, i)

#define FRAMES_FLAG     0x0001
#define BYTES_FLAG      0x0002
#define TOC_FLAG        0x0004
#define VBR_SCALE_FLAG  0x0008

static int get_xing_header(xing_header_t * xing, const char *buf)
{
  // Inits
  int i, head_flags;
  int id, mode;
  memset(xing, 0, sizeof(xing_header_t));

  // Get the selected MPEG header data
  id = (buf[1] >> 3) & 1;
  mode = (buf[3] >> 6) & 3;
  buf += 4;

  // Skip the sub band data
  if (id) // mpeg1
  {
    if (mode != 3)
    {
      buf += 32;
    }
    else
    {
      buf += 17;
    }
  }
  else // mpeg2
  {
    if (mode != 3)
    {
      buf += 17;
    }
    else
    {
      buf += 9;
    }
  }

  if (strncmp(buf, "Xing", 4))
  {
    return 0;
  }
  buf += 4;

  head_flags = GET_INT32BE(buf);

  if (head_flags & FRAMES_FLAG)
  {
    xing->frames = GET_INT32BE(buf);
  }
  if (xing->frames < 1)
  {
    return 0;
  }
  if (head_flags & BYTES_FLAG)
  {
    xing->bytes = GET_INT32BE(buf);
  }

  if (head_flags & TOC_FLAG)
  {
    for (i = 0; i < 100; ++i)
    {
      xing->toc[i] = buf[i];
      if (i > 0 && xing->toc[i] < xing->toc[i - 1])
      {
        return 0;
      }
    }
    if (xing->toc[99] == 0)
    {
      return 0;
    }
    buf += 100;
  }
  else
  {
    for (i = 0; i < 100; ++i)
    {
      xing->toc[i] = (i * 256) / 100;
    }
  }

  return 1;
}

static long convert_to_header(unsigned char *buf)
{
  return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

static bool head_check(unsigned long head)
{
  if ((head & 0xffe00000) != 0xffe00000)
  {
    return false;
  }
  if (!((head >> 17) & 3))
  {
    return false;
  }
  if (((head >> 12) & 0xf) == 0xf)
  {
    return false;
  }
  if (!((head >> 12) & 0xf))
  {
    return false;
  }
  if (((head >> 10) & 0x3) == 0x3)
  {
    return false;
  }
  if (((head >> 19) & 1) == 1 &&
      ((head >> 17) & 3) == 3 &&
      ((head >> 16) & 1) == 1)
  {
    return false;
  }
  if ((head & 0xffff0000) == 0xfffe0000)
  {
    return false;
  }
  return true;
}

static double compute_tpf(struct frame *fr)
{
  const int bs[4] = {0, 384, 1152, 1152};
  double tpf;
  tpf = bs[fr->lay];
  tpf /= samplefreq_table[fr->sampling_frequency] << (fr->lsf);
  return tpf;
}

static double compute_bpf(struct frame *fr)
{
  double bpf;

  switch (fr->lay)
  {
  case 1:
    bpf = bitrates_table[fr->lsf][0][fr->bitrate_index];
    bpf *= 12000.0 * 4.0;
    bpf /= samplefreq_table[fr->sampling_frequency] << (fr->lsf);
    break;

  case 2:
  case 3:
    bpf = bitrates_table[fr->lsf][fr->lay - 1][fr->bitrate_index];
    bpf *= 144000;
    bpf /= samplefreq_table[fr->sampling_frequency] << (fr->lsf);
    break;

  default:
    bpf = 1.0;
  }

  return bpf;
}

static int decode_header(struct frame *fr, unsigned long newhead)
{
  if (newhead & (1 << 20))
  {
    fr->lsf = (newhead & (1 << 19)) ? 0x0 : 0x1;
    fr->mpeg25 = 0;
  }
  else
  {
    fr->lsf = 1;
    fr->mpeg25 = 1;
  }
  fr->lay = 4 - ((newhead >> 17) & 3);
  if (fr->mpeg25)
  {
    fr->sampling_frequency = 6 + ((newhead >> 10) & 0x3);
  }
  else
  {
    fr->sampling_frequency = ((newhead >> 10) & 0x3) + (fr->lsf * 3);
  }

  fr->bitrate_index = ((newhead >> 12) & 0xf);
  fr->padding = ((newhead >> 9) & 0x1);

  if (!fr->bitrate_index)
  {
    return (0);
  }

  switch (fr->lay)
  {
  case 1:
    fr->framesize = (long)bitrates_table[fr->lsf][0][fr->bitrate_index] *
                    12000;
    fr->framesize /= samplefreq_table[fr->sampling_frequency];
    fr->framesize = ((fr->framesize + fr->padding) << 2) - 4;
    break;

  case 2:
    fr->framesize = (long)bitrates_table[fr->lsf][1][fr->bitrate_index] *
                    144000;
    fr->framesize /= samplefreq_table[fr->sampling_frequency];
    fr->framesize += fr->padding - 4;
    break;

  case 3:
    fr->framesize = (long) bitrates_table[fr->lsf][2][fr->bitrate_index] *
                    144000;
    fr->framesize /= samplefreq_table[fr->sampling_frequency] << (fr->lsf);
    fr->framesize = fr->framesize + fr->padding - 4;
    break;

  default:
    return (0);
  }

  if(fr->framesize > 1792 /*MAXFRAMESIZE*/)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
