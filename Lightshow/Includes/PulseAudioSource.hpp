/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   PulseAudioSource.hpp                          <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 04:38:07 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/09 08:37:55 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef PULSE_AUDIO_SOURCE_HPP
#define PULSE_AUDIO_SOURCE_HPP

/* Pulseaudio Headers. */
#include <pulse/error.h>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

/* stdlib Headers. */
#include <string>

namespace Lightshow {

/* Audio sample type defenition.
 * TODO: Should be moved elsewhere. */
struct s_pcm_16_stereo_sample {
	int16_t left;
	int16_t right;
};

using PCMStereoSample = struct s_pcm_16_stereo_sample;

/* Typedefs to make type names more consistent. */
using PASimpleInterface = pa_simple;
using PASampleSpec      = pa_sample_spec;

class PulseAudioSource {

	/* Reference for Pulseaudio Simple API. */
	PASimpleInterface* pulse_ptr  = nullptr;
	int                error_code = 0;

public:
	/* Default sample specification to use when launching the sink:
	 * {Bits/Sample, Frequency, ChannelCount} */
	static PASampleSpec default_spec;

	/* Method to get the current default Pulseaudio sink. */
	static std::string get_default_source_name();

	PulseAudioSource(const std::string&    application_name,
	                 const std::string&    application_description,
	                 const pa_sample_spec* audio_sample_specification,
	                 const std::string&    audio_source_name,
	                 const pa_channel_map* audio_channel_mapping = nullptr,
	                 const pa_buffer_attr* sample_buffer_attribs = nullptr);

	~PulseAudioSource();

	/* Method to read audio data of N bytes into a buffer. */
	bool read_into(void* buffer, size_t buffer_size);
};
} // namespace Lightshow

#endif