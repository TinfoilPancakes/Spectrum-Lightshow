/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   PulseAudioSource.cpp                          <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 04:38:15 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/09 08:39:45 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "PulseAudioSource.hpp"

#include "DebugTools.hpp"

using namespace Lightshow;
using namespace TF::Debug;

namespace {

/* Utility struct to pass data around with (void*)... */
struct s_bento {
	/* Loop pointer to close it later. */
	pa_mainloop* loop_ptr = nullptr;
	/* The device name I need at the end of callbacks. */
	std::string source_name;
};

/* Typedef to make this consistent. */
using Bento = struct s_bento;

/* Final Pulseaudio callback:
 * Used to get default sink name. */
void pa_get_info_callback(pa_context*           context_ptr,
                          const pa_server_info* server,
                          void*                 userdata) {
	// Void reference to quiet compiler warnings.
	(void)context_ptr;

	// Error case :(
	if (userdata == nullptr)
		return;

	// Populate box :)
	Bento* bento       = (Bento*)userdata;
	bento->source_name = server->default_sink_name;

	// Quit mainloop to release waiting thread.
	pa_mainloop_quit(bento->loop_ptr, 0);
}

/* Pulseaudio callback to listen for connection changes.
 * If connected successfully will release waiting thread and populate data. */
void pa_state_change_callback(pa_context* context_ptr, void* userdata) {
	auto state = pa_context_get_state(context_ptr);
	auto bento = (Bento*)userdata;

	if (state == PA_CONTEXT_READY)
		pa_operation_unref(pa_context_get_server_info(context_ptr,
		                                              pa_get_info_callback,
		                                              userdata));
	else if (state == PA_CONTEXT_TERMINATED)
		pa_mainloop_quit(bento->loop_ptr, 0);
}
} // namespace

// Because GCC < v5 does not support proper static constexpr I guess... :/
PASampleSpec PulseAudioSource::default_spec = {.format   = PA_SAMPLE_S16NE,
                                               .rate     = 44100,
                                               .channels = 2};

std::string PulseAudioSource::get_default_source_name() {

	Bento helper_box;

	pa_mainloop*     mainloop_ptr = nullptr;
	pa_mainloop_api* api_ptr      = nullptr;
	pa_context*      context_ptr  = nullptr;

	// Only way to access the pulseaudio server info through API is with
	// mainloop :/
	mainloop_ptr = pa_mainloop_new();
	api_ptr      = pa_mainloop_get_api(mainloop_ptr);
	context_ptr  = pa_context_new(api_ptr, "Lightshow device list");

	helper_box.loop_ptr = mainloop_ptr;

	pa_context_connect(context_ptr, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

	pa_context_set_state_callback(context_ptr,
	                              pa_state_change_callback,
	                              &helper_box);

	// pa_mainloop_run will block until mainloop has exited.
	int loop_return = 0;
	int loop_result = pa_mainloop_run(mainloop_ptr, &loop_return);
	if (loop_result < 0) {
		print_error_line("ERR -> [PulseAudioSource::get_default_source_name]: "
		                 "pa_mainloop_run(...) failed.\n",
		                 "> errno #",
		                 loop_result,
		                 ": ",
		                 pa_strerror(loop_result));
	}

	// Add ".monitor" to the device name because I have no idea... :(
	return helper_box.source_name + ".monitor";
}

PulseAudioSource::PulseAudioSource(
    const std::string&    application_name,
    const std::string&    application_description,
    const pa_sample_spec* audio_sample_specification,
    const std::string&    audio_source_name,
    const pa_channel_map* audio_channel_mapping,
    const pa_buffer_attr* sample_buffer_attribs) {

	this->pulse_ptr = pa_simple_new(nullptr,
	                                application_name.c_str(),
	                                PA_STREAM_RECORD,
	                                audio_source_name.c_str(),
	                                application_description.c_str(),
	                                audio_sample_specification,
	                                audio_channel_mapping,
	                                sample_buffer_attribs,
	                                &this->error_code);

	if (this->pulse_ptr == nullptr || this->error_code != 0) {
		print_error_line("ERR -> [PulseAudioSource::PulseAudioSource]: "
		                 "pa_simple_new(...) failed.\n",
		                 "> errno #",
		                 this->error_code,
		                 ": ",
		                 pa_strerror(this->error_code));
		return;
	}

	// Debugging stuff...
	auto latency = pa_simple_get_latency(this->pulse_ptr, &this->error_code);

	print_debug_line("DBG -> [PulseAudioSource::PulseAudioSource]: Source "
	                 "started.\n",
	                 "[pa_simple_get_latency]: ",
	                 latency);
}

PulseAudioSource::~PulseAudioSource() {
	if (this->pulse_ptr)
		pa_simple_free(this->pulse_ptr);
}

bool PulseAudioSource::read_into(void* buffer, size_t buffer_size) {
	if (this->pulse_ptr == nullptr) {
		print_error_line("ERR -> [PulseAudioSource::read_into]: Attempted read "
		                 "of invalid source.");
		return false;
	}

	int32_t read_error = 0;

	int result =
	    pa_simple_read(this->pulse_ptr, buffer, buffer_size, &read_error);

	if (result < 0) {
		print_error_line(
		    "ERR -> [PulseAudioSource::read_into]: pa_simple_read failed.\n",
		    "> errno #",
		    read_error,
		    ": ",
		    pa_strerror(read_error));
		return false;
	}

	return true;
}
