/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   FFTransformer.hpp                             <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/03/01 11:14:32 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/09 08:37:36 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef FF_TRANSFORMER_HPP
#define FF_TRANSFORMER_HPP

#include <functional>
#include <vector>

#include <fftw3.h>

namespace Lightshow {

using FFTResult = fftw_complex;

class FFTransformer {

	std::vector<FFTResult*> output_buffers;
	std::vector<double*>    input_buffers;

	size_t sample_count        = 1;
	size_t channel_count       = 1;
	size_t output_sample_count = 1;

public:
	FFTransformer(size_t channel_count, size_t sample_count);

	~FFTransformer();

	uint32_t
	fill_input_buffer(size_t                               channel_index,
	                  void*                                samples,
	                  std::function<double(void*, size_t)> parse_sample);

	void calculate_dft();

	fftw_complex* get_output(size_t index) const;

	double* get_input(size_t index) const;

	size_t get_output_count() const;
};

} // namespace Lightshow

#endif
