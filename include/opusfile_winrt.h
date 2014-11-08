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

#include "opusfile.h"
#include "internal.h"

namespace Opusfile {

	namespace WindowsRuntime {

		public ref class OpusHead sealed {
		public:
			property int Version {
				int get() { return src_->version; }
			}

			property int ChannelCount {
				int get() { return src_->channel_count; }
			}

			property unsigned PreSkip {
				unsigned get() { return src_->pre_skip; }
			}

			property opus_uint32 InputSampleRate {
				opus_uint32 get() { return src_->input_sample_rate; }
			}

			property int OutputGain {
				int get() { return src_->output_gain; }
			}

			property int MappingFamily {
				int get() { return src_->mapping_family; }
			}

			property int StreamCount {
				int get() { return src_->stream_count; }
			}

			property int CoupledCount {
				int get() { return src_->coupled_count; }
			}

			property Platform::Array<uint8>^ Mapping {
				Platform::Array<uint8>^ get() {
					return mapping_ ? mapping_ : (mapping_ =
						Platform::ArrayReference<uint8>(const_cast<uint8 *>(src_->mapping), OPUS_CHANNEL_COUNT_MAX));
				}
			}

		internal:
			OpusHead(const ::OpusHead *src) : src_(src) { }

		private:
			const ::OpusHead *src_;

			Platform::Array<uint8>^ mapping_;
		};


		public ref class OpusTags sealed {
		public:
			property Platform::String^ Vendor {
				Platform::String^ get() { return vendor_ ? vendor_ : (vendor_ = string_from_utf8(src_->vendor)); }
			}

			property Platform::Array<Platform::String^>^ Comments {
				Platform::Array<Platform::String^>^ get() {
					return user_comments_ ? user_comments_ : (user_comments_ = InitializeUserComments());
				}
			}

		internal:
			OpusTags(const ::OpusTags *src) : src_(src) { }

		private:
			inline Platform::Array<Platform::String^>^ InitializeUserComments() {
				Platform::Array<Platform::String^>^ arr = ref new Platform::Array<Platform::String^>((unsigned)src_->comments);
				for (unsigned i = 0; i < arr->Length; i++) {
					arr[i] = string_from_utf8(src_->user_comments[i], src_->comment_lengths[i]);
				}
				return arr;
			}

			const ::OpusTags *src_;

			Platform::String^ vendor_;
			Platform::Array<Platform::String^>^ user_comments_;
		};


		public enum struct GainType {
			HeaderGain = OP_HEADER_GAIN,
			TrackGain = OP_TRACK_GAIN,
			AbsoluteGain = OP_ABSOLUTE_GAIN
		};


		public ref class OggOpusFile sealed {
		public:
			OggOpusFile();
			virtual ~OggOpusFile();

			/* Call after open to check that the object was created
			 * successfully.  If not, use Open() to try again.
			 */
			property bool IsValid { bool get(); }

			void Open(Windows::Storage::Streams::IRandomAccessStream^ fileStream);
			void Open(Windows::Storage::Streams::IRandomAccessStream^ fileStream, Windows::Storage::Streams::IBuffer^ initial);
			void Free();

			//static OggOpusFile^ TestOpen(Windows::Storage::Streams::IRandomAccessStream^ fileStream);
			//static OggOpusFile^ TestOpen(Windows::Storage::Streams::IRandomAccessStream^ fileStream, Windows::Storage::Streams::IBuffer^ initial);

			bool Seekable();
			int LinkCount();
			opus_uint32 Serialno();
			opus_uint32 Serialno(int li);
			int ChannelCount();
			int ChannelCount(int li);
			opus_int64 RawTotal();
			opus_int64 RawTotal(int li);
			ogg_int64_t PcmTotal();
			ogg_int64_t PcmTotal(int li);
			OpusHead^ Head();
			OpusHead^ Head(int li);
			OpusTags^ Tags();
			OpusTags^ Tags(int li);
			int CurrentLink();
			opus_int32 Bitrate();
			opus_int32 Bitrate(int li);
			opus_int32 BitrateInstant();
			opus_int64 RawTell();
			ogg_int64_t PcmTell();

			void SetGainOffset(GainType gainType, opus_int32 gainOffsetQ8);
			void SetDitherEnabled(bool enabled);
			Windows::Storage::Streams::IBuffer^ Read(int bufSize);
			Windows::Storage::Streams::IBuffer^ Read(int bufSize, int *li);
			Windows::Storage::Streams::IBuffer^ ReadStereo(int bufSize);

			void RawSeek(opus_int64 byteOffset);
			void PcmSeek(ogg_int64_t pcmOffset);

		private:
			::OggOpusFile *of_;
			Windows::Storage::Streams::IRandomAccessStream^ file_stream_;
			Windows::Storage::Streams::DataReader^ file_reader_;

			static int read_func_(void *stream, unsigned char *ptr, int nbytes);
			static int seek_func_(void *stream, opus_int64 offset, int whence);
			static opus_int64 tell_func_(void *stream);
			static int close_func_(void *stream);
		};

	}

}
