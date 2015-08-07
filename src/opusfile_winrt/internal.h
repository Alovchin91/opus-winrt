/* opusfile_winrt - Opus Codec for Windows Runtime
 * Copyright (C) 2014-2015  Alexander Ovchinnikov
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

#include <robuffer.h>
#include <ppltasks.h>
#include <windows.storage.streams.h>

#define THROW_IF_FAILED(hr) { if (FAILED((hr))) throw Platform::Exception::CreateException((hr)); }


/* get internal IBuffer array */
static
uint8 *get_array(Windows::Storage::Streams::IBuffer^ buffer)
{
	using Microsoft::WRL::ComPtr;

	HRESULT hr = S_OK;
	uint8 *pBytes = nullptr;

	ComPtr<Windows::Storage::Streams::IBufferByteAccess> spBuffAccess;
	ComPtr<ABI::Windows::Storage::Streams::IBuffer> spBuff = reinterpret_cast<ABI::Windows::Storage::Streams::IBuffer*>(buffer);

	THROW_IF_FAILED(spBuff.As(&spBuffAccess));
	THROW_IF_FAILED(spBuffAccess->Buffer(&pBytes));

	return pBytes;
}


/* convert UTF-8 to String */
static
Platform::String^ string_from_utf8(const char *str, int len = -1)
{
	if (!str) return nullptr;

	int wlen = MultiByteToWideChar(CP_ACP, 0, str, len, NULL, 0);
	if (0 == wlen) {
		throw ref new Platform::InvalidArgumentException();
	}

	wchar_t *widestr = new wchar_t[wlen];
	if (MultiByteToWideChar(CP_ACP, 0, str, len, widestr, wlen) == 0) {
		delete[] widestr;
		throw ref new Platform::InvalidArgumentException();
	}

	Platform::String^ ret = ref new Platform::String(widestr);
	delete[] widestr;
	return ret;
}


/* convert int16 array to IBuffer */
static
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


template<typename _Ty>
static
typename concurrency::details::_TaskTypeFromParam<_Ty>::_Type perform_synchronously(_Ty param)
{
	concurrency::task<typename concurrency::details::_TaskTypeFromParam<_Ty>::_Type> task = concurrency::create_task(param);
	concurrency::event synchronizer;
	task.then([&](typename concurrency::details::_TaskTypeFromParam<_Ty>::_Type) {
		synchronizer.set();
	}, concurrency::task_continuation_context::use_arbitrary());
	synchronizer.wait();
	return task.get();
}


#endif
