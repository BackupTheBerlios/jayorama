/* outputbase.cpp
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


#include <outputobjectbase.h>
#include <iostream>
#include <math.h>


using namespace std;
using namespace izsound;

OutputObjectBase::OutputObjectBase(  int bufferSize, int sampleRate )
:DspUnit(sampleRate, 1, 0)
{
  // Init
  m_status = OSE_OK;
  m_outBufferSize = bufferSize;
  m_outSampleRate = sampleRate;
  m_bufferPosition = 0;
#ifdef _FLOAT_ENGINE
  m_outBuffer = new float[m_outBufferSize];
#endif 
#ifdef _DOUBLE_ENGINE
  m_outBuffer = new double[m_outBufferSize];
#endif
}


OutputObjectBase::~OutputObjectBase()
{
#ifdef _FLOAT_ENGINE
  if ( m_outBuffer != NULL )
		delete[] (float*)m_outBuffer;
#endif 
#ifdef _DOUBLE_ENGINE
  if ( m_outBuffer != NULL )
  		delete[] (double*)m_outBuffer;
#endif
}


void OutputObjectBase::performDsp()
{ 
  if ( m_status != OSE_OK )
  {
  	cerr << "Error: errors before OutputObjectBase::performDsp(). Exiting..." << endl;
	return; 
  }
  
  if ( m_outBuffer == NULL ) // empty device!
		return;

  // Init
  SlotData* data = m_inSlots[0];
  bool dispair_exception = false;
#ifdef DEBUG
  cerr << "(*data)[0].size() = " << (*data)[0].size() << endl;
#endif 
  unsigned int nframes = min((*data)[0].size(), (*data)[1].size());

  if ( (nframes % 2) != 0 )
  {
//   	dispair_exception = true;
  	nframes--;
  }
  
#ifdef _FLOAT_ENGINE
	float *_buffer = (float*)m_outBuffer;
#endif 
#ifdef _DOUBLE_ENGINE
	double *_buffer = (double*)m_outBuffer;
#endif
  
  // Let's go
  for (unsigned int j = 0; j < nframes; ++j)
  {
	// working on stereo: always a couple
	_buffer[m_bufferPosition++] = (*data)[0][j];
	_buffer[m_bufferPosition] = (*data)[1][j];
	
	// We write to the device if needed
	if (m_bufferPosition == (m_outBufferSize - 1))
	{
		write( m_outBufferSize, m_outBuffer );
		m_bufferPosition = 0;
	}
	else
		m_bufferPosition++;
  }
}


