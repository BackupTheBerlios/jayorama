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

#ifndef IZSOUND_DSPUNIT_H
#define IZSOUND_DSPUNIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <deque>
#include <vector>
#include <algorithm>

#define CLIPPING_VALUE 0.999999

using namespace std;

namespace izsound
{

/**
 * Defines a slot data. A slot data is made of a vector of 2 stereo channels,
 * the first beeing left and the second beeing right. Then a channel is made of
 * a <code>deque</code> of <code>double</code> whose values range is -1.0 to
 * 1.0.
 */
typedef vector<deque<double> > SlotData;

/**
 * Defines a DSP unit. A DSP unit is used to do a single type of sound data
 * processing. For instance it can change the volume, touch the pitch, read from
 * a file and so on. DSP units can be (multi-)chained together to make complex
 * sound processing operations. Each DSP unit has slots for input and output.
 * The slots are identified by unsigned integers starting from zero.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class DspUnit
{

private:

  /**
   * The copy constructor, unavailable.
   */
  DspUnit(const DspUnit &unit) { }

  /**
   * The assignment operator, unavailable.
   *
   * @param unit The unit to assign from.
   */
  DspUnit& operator=(DspUnit &unit) { return unit; }

protected:

  /** The audio chain sample rate. */
  unsigned int m_sampleRate;

  /** The maximum number of in-slots. */
  unsigned int m_maxInSlots;

  /** The maximum number of out-slots. */
  unsigned int m_maxOutSlots;

  /** The in-slots data. */
  vector<SlotData*> m_inSlots;

  /** The out-slots data. */
  vector<SlotData*> m_outSlots;

  /** The binded slots tracker. */
  bit_vector m_bindedSlots;

  /** The output dsp units. */
  vector<DspUnit*> m_outDspUnits;

  /** Data reception counter. */
  unsigned int m_receptionCounter;

  /** Data reception trigger. */
  unsigned int m_receptionTrigger;

  /**
   * The constructor.
   *
   * @param sampleRate The audio chain sample rate.
   * @param maxIn The maximum number of in-slots.
   * @param maxOut The maximum number of out-slots.
   */
  DspUnit(const unsigned int &sampleRate = 44100,
          const unsigned int &maxIn = 1, const unsigned int &maxOut = 1);

  /**
   * This is the method that performs the Dsp job. It can read from the input
   * slots and it must send the processed data to all its output slots. When
   * making a new Dsp unit, implementing this method is nearly all your job,
   * the data-chaining logic is already handled by the base class
   * (<code>izsound::DspUnit</code>).
   */
  virtual void performDsp() = 0;

public:

  /**
   * The destructor.
   */
  virtual ~DspUnit();

  /**
   * Returns the audio chain sample rate.
   *
   * @return The sample rate.
   */
  inline unsigned int getSampleRate() const { return m_sampleRate; }

  /**
   * Returns the maximum number of in-slots.
   *
   * @return The maximum number of in-slots.
   */
  inline unsigned int getMaxInSlots() const { return m_maxInSlots; }

  /**
   * Returns the maximum number of out-slots.
   *
   * @return The maximum number of out-slots.
   */
  inline unsigned int getMaxOutSlots() const { return m_maxOutSlots; }

  /**
   * Connects the DSP unit with another one. The DSP unit will precede the
   * unit it is connected to. If the provided slots numbers are not in the valid
   * ranges, then nothing is done.
   *
   * @param dspUnit The dspUnit to connect to.
   * @param mySlot The output slot number to connect from.
   * @param itsSlot The input slot number to connect to.
   */
  void connect(DspUnit* dspUnit, const unsigned int &mySlot,
               const unsigned int &itsSlot);

  /**
   * Disconnects the DSP unit from another one. The connected DSP unit slot
   * data is cleared.
   *
   * @param mySlot The output slot number to disconnect from.
   */
  void disconnect(const unsigned int &mySlot);

  /**
   * Runs the DSP unit in the chain. It calls the <code>performDsp()</code>
   * method then runs the following Dsp units. Consequently, a Dsp units chain
   * should be run inside a loop where the source units <code>run()</code>
   * methods are invocated. <b>NEVER call this method until all your
   * connections are made !</b>. You could get memory errors if you did so.
   */
  void run();

  /**
   * Binds an input slot of the unit to another DspUnit input slot.
   * <b>Use with extreme care, this can be dangerous and is only recommended
   * when building a DSP chain inside a DSP unit. There is no checking
   * performed.</b>
   *
   * @param dsp The dsp to bind the input to.
   * @param mySlot The slot number of this DSP.
   * @param itsSlot The other unit slot number.
   */
  void bindInput(DspUnit* dsp, const unsigned int &mySlot,
                 const unsigned int itsSlot);

  /**
   * Binds an output slot of the unit to a <code>SlotData</code> instance.
   * <b>Use with extreme care, this can be dangerous and is only recommended
   * when building a DSP chain inside a DSP unit. There is no checking
   * performed.</b>
   *
   * @param data The data to use as the output.
   * @param slot The slot number.
   */
  void bindOutput(SlotData* data, const unsigned int &slot);

  /**
   * Resets the DSP unit. By default, it will not do anything. However, some
   * units can have some kind of state associated with them. For instance, it
   * can have some internal buffer containing work-in-progress data. If the unit
   * is disconnected and connected again latter, it will start working again
   * with some older (and maybe inconsistant) data. That's why a reset makes
   * sense.
   *
   * As a consequence, redefine this method if the DSP unit that you are
   * developping could be affected by beeing used at discontinuous times because
   * it maintains some kind of state.
   *
   * As a DSP units user, call this method on DSP units that redefine it
   * whenever you take them out of a chain, but might put them back in later.
   */
  virtual void reset() {}

};

}

#endif
