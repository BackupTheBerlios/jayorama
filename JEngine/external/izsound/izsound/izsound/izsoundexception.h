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

#ifndef IZSOUND_IZSOUND_EXCEPTION_H
#define IZSOUND_IZSOUND_EXCEPTION_H

#include <string>
#include <exception>

namespace izsound
{

/**
 * Defines an IzSound exception. This is the most generic IzSound exception.
 * Subclasses can be defined to provide a more specific errors-handling.
 *
 * @author Julien PONGE <julien@izforge.com>
 */
class IzSoundException : public std::exception
{

private:

  /** The exception message. */
  std::string m_what;

public:

  /**
   * The constructor.
   *
   * @param what The exception message.
   */
  IzSoundException(const std::string &what) throw() : std::exception()
  { m_what = what; }

  /**
   * The destructor.
   */
  virtual ~IzSoundException() throw() { }

  /**
   * Gives the exception message.
   *
   * @return The exception message.
   */
  virtual const char* what() const throw()
  { return m_what.c_str(); }

};

}

#endif
