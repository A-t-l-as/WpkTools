#ifndef WPK_COMPILATOR_GLOBALS_HPP
#define WPK_COMPILATOR_GLOBALS_HPP

#include <cstdint>

namespace WpkCompilatorGlobals
{

constexpr int64_t initialize_signal_value = -1;
constexpr int64_t push_back_signal_value = 0;

constexpr uint32_t old_wpk_format_type = 0;
constexpr uint32_t new_wpk_format_type = 1;

constexpr uint32_t delayed_const_value = 1;
constexpr uint32_t null_const_value = 0;

enum class ENStringFormatMode
{
    OldStringFormat,
    NewStringFormat
};

};


#endif // !WPK_COMPILATOR_GLOBALS_HPP
