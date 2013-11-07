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

#include "InputSource.h"
#include "FileInputSource.h"
#include "MemoryMappedFileInputSource.h"
#include "InMemoryFileInputSource.h"
#include "HTTPInputSource.h"
#include "CFWrapper.h"

// ========================================
// Error Codes
// ========================================
const CFStringRef	SFB::InputSourceErrorDomain		= CFSTR("org.sbooth.AudioEngine.ErrorDomain.InputSource");

#pragma mark Static Methods

SFB::InputSource::unique_ptr SFB::InputSource::CreateInputSourceForURL(CFURLRef url, int flags, CFErrorRef *error)
{
	if(nullptr == url)
		return nullptr;

	// If there is no scheme the URL is invalid
	SFB::CFString scheme = CFURLCopyScheme(url);
	if(!scheme) {
		if(error)
			*error = CFErrorCreate(kCFAllocatorDefault, kCFErrorDomainPOSIX, EINVAL, nullptr);
		return nullptr;
	}

	if(kCFCompareEqualTo == CFStringCompare(CFSTR("file"), scheme, kCFCompareCaseInsensitive)) {
		if(InputSourceFlagMemoryMapFiles & flags)
			return unique_ptr(new MemoryMappedFileInputSource(url));
		else if(InputSourceFlagLoadFilesInMemory & flags)
			return unique_ptr(new InMemoryFileInputSource(url));
		else
			return unique_ptr(new FileInputSource(url));
	}
	else if(kCFCompareEqualTo == CFStringCompare(CFSTR("http"), scheme, kCFCompareCaseInsensitive))
		return unique_ptr(new HTTPInputSource(url));

	return nullptr;
}

#pragma mark Creation and Destruction

SFB::InputSource::InputSource()
	: mURL(nullptr), mIsOpen(false)
{}

SFB::InputSource::InputSource(CFURLRef url)
	: mURL(nullptr), mIsOpen(false)
{
	assert(nullptr != url);
	
	mURL = (CFURLRef)CFRetain(url);
}

SFB::InputSource::~InputSource()
{
	if(mURL)
		CFRelease(mURL), mURL = nullptr;
}
