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

#include "opusfile_winrt.h"

namespace Opusfile {

	namespace WindowsRuntime {

		const ::OpusFileCallbacks op_winrt_callbacks = {
			&OggOpusFile::read_func,
			&OggOpusFile::seek_func,
			&OggOpusFile::tell_func,
			&OggOpusFile::close_func
		};


		OggOpusFile::OggOpusFile()
			: of_(nullptr), file_stream_(nullptr), file_reader_(nullptr)
		{
		}

		OggOpusFile::~OggOpusFile()
		{
			Free();
		}

		bool OggOpusFile::IsValid::get()
		{
			return nullptr != of_ && nullptr != file_stream_ && nullptr != file_reader_;
		}

		void OggOpusFile::Open(Windows::Storage::Streams::IRandomAccessStream^ fileStream)
		{
			Open(fileStream, nullptr);
		}

		void OggOpusFile::Open(Windows::Storage::Streams::IRandomAccessStream^ fileStream, Windows::Storage::Streams::IBuffer^ initial)
		{
			file_stream_ = fileStream;
			file_reader_ = ref new Windows::Storage::Streams::DataReader(file_stream_);

			uint8 *initial_data = nullptr;
			size_t initial_size = 0;
			if (initial) {
				initial_data = get_array(initial);
				initial_size = initial->Length;
			}

			int error = 0;
			of_ = ::op_open_callbacks((void *)this, &op_winrt_callbacks, initial_data, initial_size, &error);

			if (0 != error) {
				Free();
				switch (error) {
				case OP_EFAULT:
					throw ref new Platform::FailureException();
				case OP_EIMPL:
					throw ref new Platform::NotImplementedException();
				case OP_EINVAL:
				case OP_ENOTFORMAT:
					throw ref new Platform::InvalidArgumentException();
				default:
					throw ref new Platform::COMException(error);
				}
			}
		}

		void OggOpusFile::Free()
		{
			if (of_) {
				(void)::op_free(of_);
				of_ = nullptr;
			}

			if (file_reader_) {
				(void)file_reader_->DetachStream();
				delete file_reader_;
			}

			file_reader_ = nullptr;
			file_stream_ = nullptr;
		}

		//static OggOpusFile^ OggOpusFile::TestOpen(Windows::Storage::Streams::IRandomAccessStream^ fileStream);

		//static OggOpusFile^ OggOpusFile::TestOpen(Windows::Storage::Streams::IRandomAccessStream^ fileStream, Windows::Storage::Streams::IBuffer^ initial);

		bool OggOpusFile::Seekable()
		{
			assert(IsValid);
			return !!(::op_seekable(of_));
		}

		int OggOpusFile::LinkCount()
		{
			assert(IsValid);
			return ::op_link_count(of_);
		}

		opus_uint32 OggOpusFile::Serialno()
		{
			return Serialno(-1);
		}

		opus_uint32 OggOpusFile::Serialno(int li)
		{
			assert(IsValid);
			return ::op_serialno(of_, li);
		}

		int OggOpusFile::ChannelCount()
		{
			return ChannelCount(-1);
		}

		int OggOpusFile::ChannelCount(int li)
		{
			assert(IsValid);
			return ::op_channel_count(of_, li);
		}

		opus_int64 OggOpusFile::RawTotal()
		{
			return RawTotal(-1);
		}

		opus_int64 OggOpusFile::RawTotal(int li)
		{
			assert(IsValid);
			return ::op_raw_total(of_, li);
		}

		ogg_int64_t OggOpusFile::PcmTotal()
		{
			return PcmTotal(-1);
		}

		ogg_int64_t OggOpusFile::PcmTotal(int li)
		{
			assert(IsValid);
			return ::op_pcm_total(of_, li);
		}

		OpusHead^ OggOpusFile::Head()
		{
			return Head(-1);
		}

		OpusHead^ OggOpusFile::Head(int li)
		{
			assert(IsValid);
			const ::OpusHead *head = ::op_head(of_, li);
			return ref new OpusHead(head);
		}

		OpusTags^ OggOpusFile::Tags()
		{
			return Tags(-1);
		}

		OpusTags^ OggOpusFile::Tags(int li)
		{
			assert(IsValid);
			const ::OpusTags *tags = ::op_tags(of_, li);
			return ref new OpusTags(tags);
		}

		int OggOpusFile::CurrentLink()
		{
			assert(IsValid);
			return ::op_current_link(of_);
		}

		opus_int32 OggOpusFile::Bitrate()
		{
			return Bitrate(-1);
		}

		opus_int32 OggOpusFile::Bitrate(int li)
		{
			assert(IsValid);
			return ::op_bitrate(of_, li);
		}

		opus_int32 OggOpusFile::BitrateInstant()
		{
			assert(IsValid);
			return ::op_bitrate_instant(of_);
		}

		opus_int64 OggOpusFile::RawTell()
		{
			assert(IsValid);
			return ::op_raw_tell(of_);
		}

		ogg_int64_t OggOpusFile::PcmTell()
		{
			assert(IsValid);
			return ::op_pcm_tell(of_);
		}

		void OggOpusFile::SetGainOffset(GainType gainType, opus_int32 gainOffsetQ8)
		{
			assert(IsValid);
			int ret = ::op_set_gain_offset(of_, (int)gainType, gainOffsetQ8);
			if (ret < 0) {
				if (OP_EINVAL == ret) throw ref new Platform::InvalidArgumentException("gainType");
				throw ref new Platform::COMException(ret);
			}
		}

		void OggOpusFile::SetDitherEnabled(bool enabled)
		{
			assert(IsValid);
			::op_set_dither_enabled(of_, enabled ? TRUE : FALSE);
		}

		Windows::Storage::Streams::IBuffer^ OggOpusFile::Read(int bufSize)
		{
			return Read(bufSize, NULL);
		}

		Windows::Storage::Streams::IBuffer^ OggOpusFile::Read(int bufSize, int *li)
		{
			assert(IsValid);

			std::vector<opus_int16> pcm(bufSize);
			int ret = ::op_read(of_, &pcm.front(), bufSize, li);

			if (ret < 0) {
				if (OP_EFAULT == ret) throw ref new Platform::FailureException();
				if (OP_EIMPL == ret) throw ref new Platform::NotImplementedException();
				throw ref new Platform::COMException(ret);
			}
			if (0 == ret)
				return ref new Windows::Storage::Streams::Buffer(0);

			int channels = ::op_channel_count(of_, li != NULL ? *li : -1);

			return pack_sample(&pcm.front(), ret * channels);
		}

		Windows::Storage::Streams::IBuffer^ OggOpusFile::ReadStereo(int bufSize)
		{
			assert(IsValid);

			std::vector<opus_int16> pcm(bufSize);
			int ret = ::op_read_stereo(of_, &pcm.front(), bufSize);

			if (ret < 0) {
				if (OP_EFAULT == ret) throw ref new Platform::FailureException();
				if (OP_EIMPL == ret) throw ref new Platform::NotImplementedException();
				throw ref new Platform::COMException(ret);
			}
			if (0 == ret)
				return ref new Windows::Storage::Streams::Buffer(0);

			const int channels = 2; // Since the decoded data is stereo

			return pack_sample(&pcm.front(), ret * channels);
		}

		void OggOpusFile::RawSeek(opus_int64 byteOffset)
		{
			assert(IsValid);

			int ret = ::op_raw_seek(of_, byteOffset);
			if (0 != ret) {
				if (OP_EINVAL == ret) throw ref new Platform::InvalidArgumentException();
				throw ref new Platform::COMException(ret);
			}
		}

		void OggOpusFile::PcmSeek(ogg_int64_t pcmOffset)
		{
			assert(IsValid);

			int ret = ::op_pcm_seek(of_, pcmOffset);
			if (0 != ret) {
				if (OP_EINVAL == ret) throw ref new Platform::InvalidArgumentException();
				throw ref new Platform::COMException(ret);
			}
		}


		int OggOpusFile::read_func(void *stream, unsigned char *ptr, int nbytes)
		{
			OggOpusFile^ instance = reinterpret_cast<OggOpusFile^>(stream);
			assert(instance && instance->file_reader_);
			if (nbytes > 0) {
				unsigned count = perform_synchronously(instance->file_reader_->LoadAsync(nbytes));
				if (count > 0) {
					instance->file_reader_->ReadBytes(Platform::ArrayReference<uint8>(reinterpret_cast<uint8 *>(ptr), count));
					return count;
				}
			}
			return 0;
		}

		int OggOpusFile::seek_func(void *stream, opus_int64 offset, int whence)
		{
			OggOpusFile^ instance = reinterpret_cast<OggOpusFile^>(stream);
			assert(instance && instance->file_stream_);
			switch (whence) {
			case SEEK_CUR:
				instance->file_stream_->Seek(instance->file_stream_->Position + (uint64)offset);
				break;
			case SEEK_END:
				instance->file_stream_->Seek(instance->file_stream_->Size - (uint64)offset);
				break;
			case SEEK_SET:
				instance->file_stream_->Seek((uint64)offset);
				break;
			default:
				throw ref new Platform::InvalidArgumentException("whence");
			}
			return 0;
		}

		opus_int64 OggOpusFile::tell_func(void *stream)
		{
			OggOpusFile^ instance = reinterpret_cast<OggOpusFile^>(stream);
			assert(instance && instance->file_stream_);
			return (long)instance->file_stream_->Position;
		}

		int OggOpusFile::close_func(void *stream)
		{
			OggOpusFile^ instance = reinterpret_cast<OggOpusFile^>(stream);
			assert(instance && instance->file_reader_);
			(void)instance->file_reader_->DetachStream();
			delete instance->file_reader_;
			instance->file_reader_ = nullptr;
			return 0;
		}


		Platform::Array<OpusPictureTag^>^ OpusPictureTag::Parse(OpusTags^ opusTags)
		{
			std::vector<OpusPictureTag^> pics;
			int i = 0;

			while (true) {
				char *comment = opusTags->GetComment(i++);
				if (nullptr == comment) break;
				if (0 != opus_tagncompare("METADATA_BLOCK_PICTURE", 22, comment)) continue;

				OpusPictureTag^ pic = ref new OpusPictureTag();
				int ret = ::opus_picture_tag_parse(pic->src_, comment);
				if (OP_EFAULT == ret) ref new Platform::OutOfMemoryException();
				if (0 == ret) pics.push_back(pic);
			}

			return Platform::ArrayReference<OpusPictureTag^>(pics.data(), pics.size());
		}

	}

}
