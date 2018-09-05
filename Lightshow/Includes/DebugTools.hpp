/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   DebugTools.hpp                                <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/06/17 12:12:42 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/05 07:40:18 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef TF_DEBUG_TOOLS_HPP
#define TF_DEBUG_TOOLS_HPP

#include <iostream>
#include <sstream>

#include "TerminalColors.hpp"

// Uncomment to enable debug messages.
// #define DEBUG_ENABLE

namespace TF {
namespace Debug {

template <typename... Ts>
void stream_all(std::ostream&	  out,
				const std::string& delim,
				const Ts&... items) {
#ifdef TF_DEBUG_ENABLE
	std::stringstream ss;
	(void)std::initializer_list<uint8_t>{
		((out << items << delim), static_cast<uint8_t>(0u))...};
	out << ss.str();
#else
	(void)out;
	(void)delim;
	(void)sizeof...(items);
#endif
}

template <typename... Ts> void print(const Ts&... items) {
	stream_all(std::cout, "", items...);
}

template <typename... Ts> void print(std::ostream& out, const Ts&... items) {
	stream_all(out, "", items...);
}

template <typename... Ts> void print_line(const Ts&... items) {
	stream_all(std::cout, "", items..., "\n");
}

template <typename... Ts> void print_list(const Ts&... items) {
	stream_all(std::cout, "\n", items...);
}

template <typename... Ts> void print_error(const Ts&... items) {
	stream_all(std::cerr, "", "\033[0;31m", items..., "\033[0m");
}

template <typename... Ts> void print_error_line(const Ts&... items) {
	stream_all(std::cerr, "", "\033[0;31m", items..., "\033[0m\n");
}

template <typename... Ts> void print_error_list(const Ts&... items) {
	stream_all(std::cerr, "\n", "\033[0;31m", items..., "\033[0m");
}

template <typename... Ts> void print_warning(const Ts&... items) {
	stream_all(std::cerr, "", "\033[0;33m", items..., "\033[0m");
}

template <typename... Ts> void print_warning_line(const Ts&... items) {
	stream_all(std::cerr, "", "\033[0;33m", items..., "\033[0m\n");
}

template <typename... Ts> void print_warning_list(const Ts&... items) {
	stream_all(std::cerr, "\n", "\033[0;33m", items..., "\033[0m");
}

template <typename... Ts> void print_debug(const Ts&... items) {
	stream_all(std::cout,
			   "",
			   TF::Terminal::Color::fg_black_bright(),
			   items...,
			   TF::Terminal::Color::reset_all());
}

template <typename... Ts> void print_debug_line(const Ts&... items) {
	stream_all(std::cout,
			   "",
			   TF::Terminal::Color::fg_black_bright(),
			   items...,
			   TF::Terminal::Color::reset_all(),
			   "\n");
}

template <typename... Ts> void print_debug_list(const Ts&... items) {
	stream_all(std::cout,
			   "\n",
			   TF::Terminal::Color::fg_black_bright(),
			   items...,
			   TF::Terminal::Color::reset_all());
}

} // namespace Debug
} // namespace TF

#endif