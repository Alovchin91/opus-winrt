/* opusfile_winrt - Opus Codec for Windows Runtime
* Copyright (C) 2014  Alexander Ovchinnikov
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* - Neither the name of copyright holder nor the names of project's
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_opusfile_winrt_internal_h)
# define _opusfile_winrt_internal_h (1)

#include <inspectable.h>
#include <robuffer.h>
#include <ppltasks.h>


/* get internal IBuffer array */
static inline
uint8 *get_array(Windows::Storage::Streams::IBuffer^ buffer)
{
	IInspectable *inspectable = reinterpret_cast<IInspectable *>(buffer);
	Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> spBuffAccess;
	HRESULT hr = inspectable->QueryInterface(__uuidof(Windows::Storage::Streams::IBufferByteAccess), (void **)&spBuffAccess);
	uint8 *pReadBuffer = nullptr;
	spBuffAccess->Buffer(&pReadBuffer);
	return pReadBuffer;
}


/* convert UTF-8 to String */
static inline
Platform::String^ string_from_utf8(const char *str, int len = -1)
{
	if (!str) return nullptr;
	if (len < 0) len = (int)strlen(str) + 1;
	wchar_t *widestr = new wchar_t[len];
	if (MultiByteToWideChar(CP_ACP, 0, str, len, widestr, len) == 0) {
		delete[] widestr;
		throw ref new Platform::InvalidArgumentException();
	}
	Platform::String^ ret = ref new Platform::String(widestr);
	delete[] widestr;
	return ret;
}


/* convert int16 array to IBuffer */
static inline
Windows::Storage::Streams::IBuffer^ pack_sample(const opus_int16 *pcm, int size)
{
	Windows::Storage::Streams::IBuffer^ buffer = ref new Windows::Storage::Streams::Buffer((unsigned)size * 2);
	uint8 *buffer_array = get_array(buffer);

	for (int i = 0; i < size; i++) {
		*buffer_array++ = (pcm[i] & 0xff);
		*buffer_array++ = (pcm[i] >> 8);
	}

	buffer->Length = (unsigned)size * 2;
	return buffer;
}


#endif
