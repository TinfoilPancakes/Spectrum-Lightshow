/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   FFTransformer.cpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:14:36 by prp              2E 54 65 63 68          */
/*   Updated: 2018/03/02 11:11:20 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#include "FFTransformer.hpp"

using namespace Lightshow;

FFTransformer::FFTransformer(size_t channel_count, size_t sample_count) {

	this->channel_count = channel_count;
	this->sample_count  = sample_count;

	input_buffers.resize(channel_count, nullptr);
	output_buffers.resize(channel_count, nullptr);

	this->output_sample_count = (sample_count / 2) + 1;

	for (size_t i = 0; i < channel_count; ++i) {
		auto input_buffer = (double*)fftw_malloc(sample_count * sizeof(double));

		FFTResult* output_buffer =
			(FFTResult*)fftw_malloc(output_sample_count * sizeof(FFTResult));

		this->input_buffers[i]  = input_buffer;
		this->output_buffers[i] = output_buffer;
	}
}

FFTransformer::~FFTransformer() {
	for (double* buffer : this->input_buffers) {
		if (buffer != nullptr)
			fftw_free(buffer);
	}
	for (FFTResult* buffer : this->output_buffers) {
		if (buffer != nullptr)
			fftw_free(buffer);
	}
}

uint32_t FFTransformer::fill_input_buffer(
	size_t channel_index,
	void*  samples,
	std::function<double(void*, size_t)> parse_sample) {

	uint32_t loud_samples = 0;
	for (size_t i = 0; i < sample_count; ++i) {

		this->input_buffers[channel_index][i] = parse_sample(samples, i);

		if (this->input_buffers[channel_index][i] != 0)
			++loud_samples;
	}
	return loud_samples;
}

void FFTransformer::calculate_dft() {
	for (size_t i = 0; i < this->channel_count; ++i) {
		fftw_plan channel_plan = fftw_plan_dft_r2c_1d((int)this->sample_count,
													  this->input_buffers[i],
													  this->output_buffers[i],
													  FFTW_ESTIMATE);
		fftw_execute(channel_plan);
		fftw_destroy_plan(channel_plan);
	}
}

fftw_complex* FFTransformer::get_output(size_t index) const {
	return this->output_buffers[index];
}

double* FFTransformer::get_input(size_t index) const {
	return this->input_buffers[index];
}

size_t FFTransformer::get_output_count() const {
	return this->output_sample_count;
}
