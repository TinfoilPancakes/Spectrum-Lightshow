/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   TerminalColors.hpp                            <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2018/09/02 17:48:17 by prp              2E 54 65 63 68          */
/*   Updated: 2018/09/09 08:38:31 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef TF_TERMINAL_COLOR_HPP
#define TF_TERMINAL_COLOR_HPP

namespace TF {
namespace Terminal {
namespace Color {
// Reset
constexpr auto reset_all() { return "\e[0m"; }
// Regular Foreground
constexpr auto fg_black() { return "\e[30m"; }
constexpr auto fg_red() { return "\e[31m"; }
constexpr auto fg_green() { return "\e[32m"; }
constexpr auto fg_yellow() { return "\e[33m"; }
constexpr auto fg_blue() { return "\e[34m"; }
constexpr auto fg_purple() { return "\e[35m"; }
constexpr auto fg_cyan() { return "\e[36m"; }
constexpr auto fg_white() { return "\e[37m"; }
// Bright Foreground
constexpr auto fg_black_bright() { return "\e[30;1m"; }
constexpr auto fg_red_bright() { return "\e[31;1m"; }
constexpr auto fg_green_bright() { return "\e[32;1m"; }
constexpr auto fg_yellow_bright() { return "\e[33;1m"; }
constexpr auto fg_blue_bright() { return "\e[34;1m"; }
constexpr auto fg_purple_bright() { return "\e[35;1m"; }
constexpr auto fg_cyan_bright() { return "\e[36;1m"; }
constexpr auto fg_white_bright() { return "\e[37;1m"; }
// Regular Background
constexpr auto bg_black() { return "\e[40m"; }
constexpr auto bg_red() { return "\e[41m"; }
constexpr auto bg_green() { return "\e[42m"; }
constexpr auto bg_yellow() { return "\e[43m"; }
constexpr auto bg_blue() { return "\e[44m"; }
constexpr auto bg_purple() { return "\e[45m"; }
constexpr auto bg_cyan() { return "\e[46m"; }
constexpr auto bg_white() { return "\e[47m"; }
// Bright Background
constexpr auto bg_black_bright() { return "\e[40;1m"; }
constexpr auto bg_red_bright() { return "\e[41;1m"; }
constexpr auto bg_green_bright() { return "\e[42;1m"; }
constexpr auto bg_yellow_bright() { return "\e[43;1m"; }
constexpr auto bg_blue_bright() { return "\e[44;1m"; }
constexpr auto bg_purple_bright() { return "\e[45;1m"; }
constexpr auto bg_cyan_bright() { return "\e[46;1m"; }
constexpr auto bg_white_bright() { return "\e[47;1m"; }
// Text Decorations
constexpr auto dc_bold() { return "\e[1m"; }
constexpr auto dc_underline() { return "\e[4m"; }
constexpr auto dc_invert() { return "\e[7m"; }
} // namespace Color
} // namespace Terminal
} // namespace TF

#endif