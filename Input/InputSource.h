/*
 *  Copyright (C) 2010, 2011, 2012, 2013 Stephen F. Booth <me@sbooth.org>
 *  All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - Neither the name of Stephen F. Booth nor the names of its 
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <memory>

#include <CoreFoundation/CoreFoundation.h>

/*! @file InputSource.h @brief Support for arbitrary bytestream input to \c AudioDecoder */

/*! @brief \c SFBAudioEngine's encompassing namespace */
namespace SFB {

	/*! @brief The \c CFErrorRef error domain used by \c InputSource */
	extern const CFStringRef		InputSourceErrorDomain;

	/*! Possible \c CFErrorRef error codes used by \c InputSource */
	enum InputSourceErrorCodes {
		/*! File not found */
		InputSourceFileNotFoundError			= 0,
		/*! Input/output error */
		InputSourceInputOutputError				= 1
	};


	/*! Flags used in \c InputSource::CreateInputSourceForURL */
	enum InputSourceFlags {
		/*! Files should be mapped in memory using \c mmap() */
		InputSourceFlagMemoryMapFiles			= 1 << 0,
		/*! Files should be fully loaded in memory */
		InputSourceFlagLoadFilesInMemory		= 1 << 1
	};


	/*! @brief An abstract class allowing access to a stream of bytes */
	class InputSource
	{

	public:

		// ========================================
		/*! @name Factory Methods */
		//@{

		/*! @brief A \c std::unique_ptr for \c InputSource objects */
		typedef std::unique_ptr<InputSource> unique_ptr;

		/*!
		 * Create a new \c InputSource for the given URL
		 * @param url The URL
		 * @param flags Optional flags affecting how \c url is handled
		 * @param error An optional pointer to a \c CFErrorRef to receive error information
		 * @return An \c InputSource for the specified URL, or \c nullptr on failure
		 * @see InputSourceFlags
		 */
		static unique_ptr CreateInputSourceForURL(CFURLRef url, int flags = 0, CFErrorRef *error = nullptr);

		//@}


		// ========================================
		/*! @name Creation and Destruction */
		// @{

		/*! @brief Destroy this \c InputSource */
		virtual ~InputSource();

		/*! @cond */

		/*! @internal This class is non-copyable */
		InputSource(const InputSource& rhs) = delete;

		/*! @internal This class is non-assignable */
		InputSource& operator=(const InputSource& rhs) = delete;

		/*! @endcond */

		//@}


		// ========================================
		/*! @name URL access */
		//@{

		/*! @brief Get the URL for this \c InputSource */
		inline CFURLRef GetURL() const							{ return mURL; }

		//@}


		// ========================================
		/*! @name Opening and Closing */
		//@{

		/*!
		 * @brief Open the input for reading
		 * @param error An optional pointer to a \c CFErrorRef to receive error information
		 * @return \c true on success, \c false otherwise
		 */
		virtual bool Open(CFErrorRef *error = nullptr) = 0;

		/*!
		 * @brief Close the input
		 * @param error An optional pointer to a \c CFErrorRef to receive error information
		 * @return \c true on success, \c false otherwise
		 */
		virtual bool Close(CFErrorRef *error = nullptr) = 0;


		/*! @brief Query whether this \c InputSource is open */
		inline bool IsOpen() const								{ return mIsOpen; }


		// ========================================
		/*! @name Bytestream access */
		//@{

		/*!
		 * @brief Read bytes from the input
		 * @param buffer The destination buffer
		 * @param byteCount The maximum number of bytes to read
		 * @return The number of bytes read
		 *
		 */
		virtual SInt64 Read(void *buffer, SInt64 byteCount) = 0;

		/*! @brief Determine whether the end of input has been reached */
		virtual bool AtEOF() const = 0;


		/*! @brief Get the current offset in the input, in bytes */
		virtual SInt64 GetOffset() const = 0;

		/*! @brief Get the length of the input, in bytes */
		virtual SInt64 GetLength() const = 0;


		/*! @brief Query whether this \c InputSource is seekable */
		virtual bool SupportsSeeking() const					{ return false; }

		/*!
		 * Seek to the specified byte offset
		 * @param offset The desired byte offset
		 * @return \c true on success, \c false otherwise
		 */
		virtual bool SeekToOffset(SInt64 offset)				{ return false; }

		//@}

	protected:

		/*! @brief The location of the bytes to be read */
		CFURLRef mURL;

		/*! @brief Subclasses should set this to \c true if Open() is successful and \c false if Close() is successful */
		bool mIsOpen;


		/*! @brief Create a new \c InputSource and initialize \c InputSource::mURL to \c nullptr */
		InputSource();
		
		/*! @brief Create a new \c InputSource and initialize \c InputSource::mURL to \c url */
		InputSource(CFURLRef url);
		
	};

}

/*! @brief Compatibility typedef */
typedef SFB::InputSource InputSource __attribute__((deprecated));
