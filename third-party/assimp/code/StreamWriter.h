/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2012, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the following 
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file Defines the StreamWriter class which writes data to
 *  a binary stream with a well-defined endianess. */

#ifndef AI_STREAMWRITER_H_INCLUDED
#define AI_STREAMWRITER_H_INCLUDED

#include "ByteSwap.h"

namespace Assimp {

// --------------------------------------------------------------------------------------------
/** Wrapper class around IOStream to allow for consistent writing of binary data in both 
 *  little and big endian format. Don't attempt to instance the template directly. Use 
 *  StreamWriterLE to read from a little-endian stream and StreamWriterBE to read from a 
 *  BE stream. Alternatively, there is StreamWriterAny if the endianess of the output
 *  stream is to be determined at runtime.
 */
// --------------------------------------------------------------------------------------------
template <bool SwapEndianess = false, bool RuntimeSwitch = false>
class StreamWriter
{
	enum {
		INITIAL_CAPACITY = 1024
	};

public:

	// ---------------------------------------------------------------------
	/** Construction from a given stream with a well-defined endianess.
	 * 
	 *  The StreamReader holds a permanent strong reference to the
	 *  stream, which is released upon destruction.
	 *  @param stream Input stream. The stream is not re-seeked and writing
	      continues at the current position of the stream cursor.
	 *  @param le If @c RuntimeSwitch is true: specifies whether the
	 *    stream is in little endian byte order. Otherwise the
	 *    endianess information is defined by the @c SwapEndianess
	 *    template parameter and this parameter is meaningless.  */
	StreamWriter(std::shared_ptr<IOStream> stream, bool le = false)
		: stream(stream)
		, le(le)
		, cursor()
	{
		ai_assert(stream); 
		buffer.reserve(INITIAL_CAPACITY);
	}

	// ---------------------------------------------------------------------
	StreamWriter(IOStream* stream, bool le = false)
		: stream(std::shared_ptr<IOStream>(stream))
		, le(le)
		, cursor()
	{
		ai_assert(stream);
		buffer.reserve(INITIAL_CAPACITY);
	}

	// ---------------------------------------------------------------------
	~StreamWriter() {
		stream->Write(&buffer[0], 1, buffer.size());
		stream->Flush();
	}

public:

	// ---------------------------------------------------------------------
	/** Write a float to the stream  */
	void PutF4(float f)
	{
		Put(f);
	}

	// ---------------------------------------------------------------------
	/** Write a double to the stream  */
	void PutF8(double d)	{
		Put(d);
	}

	// ---------------------------------------------------------------------
	/** Write a signed 16 bit integer to the stream */
	void PutI2(int16_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write a signed 8 bit integer to the stream */
	void PutI1(int8_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write an signed 32 bit integer to the stream */
	void PutI4(int32_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write a signed 64 bit integer to the stream */
	void PutI8(int64_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write a unsigned 16 bit integer to the stream */
	void PutU2(uint16_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write a unsigned 8 bit integer to the stream */
	void PutU1(uint8_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write an unsigned 32 bit integer to the stream */
	void PutU4(uint32_t n)	{
		Put(n);
	}

	// ---------------------------------------------------------------------
	/** Write a unsigned 64 bit integer to the stream */
	void PutU8(uint64_t n)	{
		Put(n);
	}

public:

	// ---------------------------------------------------------------------
	/** overload operator<< and allow chaining of MM ops. */
	template <typename T>
	StreamWriter& operator << (T f) {
		Put(f); 
		return *this;
	}

	// ---------------------------------------------------------------------
	std::size_t GetCurrentPos() const {
		return cursor;
	}

	// ---------------------------------------------------------------------
	void SetCurrentPos(std::size_t new_cursor) {
		cursor = new_cursor;
	}

private:

	// ---------------------------------------------------------------------
	/** Generic write method. ByteSwap::Swap(T*) *must* be defined */
	template <typename T>
	void Put(T f)	{
		Intern :: Getter<SwapEndianess,T,RuntimeSwitch>() (&f, le);
		
		if (cursor + sizeof(T) >= buffer.size()) {
			buffer.resize(cursor + sizeof(T));
		}

		void* dest = &buffer[cursor];

		// reinterpret_cast + assignment breaks strict aliasing rules
		// and generally causes trouble on platforms such as ARM that
		// do not silently ignore alignment faults.
		::memcpy(dest, &f, sizeof(T));
		cursor += sizeof(T);
	}

private:

	std::shared_ptr<IOStream> stream;
	bool le;

	std::vector<uint8_t> buffer;
	std::size_t cursor;
};


// --------------------------------------------------------------------------------------------
// `static` StreamWriter. Their byte order is fixed and they might be a little bit faster.
#ifdef AI_BUILD_BIG_ENDIAN
	typedef StreamWriter<true>  StreamWriterLE;
	typedef StreamWriter<false> StreamWriterBE;
#else
	typedef StreamWriter<true>  StreamWriterBE;
	typedef StreamWriter<false> StreamWriterLE;
#endif

// `dynamic` StreamWriter. The byte order of the input data is specified in the
// c'tor. This involves runtime branching and might be a little bit slower.
typedef StreamWriter<true,true> StreamWriterAny;

} // end namespace Assimp

#endif // !! AI_STREAMWriter_H_INCLUDED