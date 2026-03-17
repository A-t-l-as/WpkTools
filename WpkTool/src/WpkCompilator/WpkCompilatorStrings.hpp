#ifndef WPK_COMPILATOR_STRINGS_HPP
#define WPK_COMPILATOR_STRINGS_HPP

#include <cstddef>

namespace WpkCompilatorStrings
{
    constexpr char format_value_name_str[] = "format_";

    constexpr char wave_path_name_str[] = "wave_path";

    constexpr char wavepack_namespace_name_str[] = "wavepack";
    constexpr char wavepack_namespace_name_upper_str[] = "WAVEPACK";
    constexpr char wavepack_namespace_name_with_space_upper_str[] = "WAVEPACK ";

    constexpr char level_struct_name_str[] = "Level";
    constexpr char level_struct_name_upper_str[] = "LEVEL";
    constexpr char level_struct_name_with_space_upper_str[] = "LEVEL ";

    constexpr char wave_struct_name_str[] = "Wave";
    constexpr char wave_struct_name_upper_str[] = "WAVE";

    constexpr char wave_struct_name_with_space_str[] = "WAVE ";
    constexpr std::size_t wave_struct_name_with_space_len = sizeof(WpkCompilatorStrings::wave_struct_name_with_space_str) - 1;

    constexpr char type_instruction_name_str[] = "Type";

    constexpr char type_instruction_name_upper_str[] = "TYPE";

    constexpr char disable_level_2_instruction_name_str[] = "DisableLevel2";
    constexpr char disable_level_2_instruction_name_upper_str[] = "DISABLELEVEL2";

    constexpr char delayed_const_str[] = "delayed";
    constexpr char delayed_const_uppercase_str[] = "DELAYED";

    constexpr char null_const_uppercase_str[] = "NULL";

    constexpr char measure_all_value_sizes_after_this_instruction_fn_name_str[] =
        "measure_all_value_sizes_after_this_instruction()";

    constexpr char wavepack_type_value_name_str[] = "wavepack_type";

    constexpr char number_of_waves_in_level_value_name_str[] = "number_of_waves_in_level_";


    constexpr char data_size_value_name_str[] = "data_size";
}

#endif // !WPK_COMPILATOR_STRINGS_HPP
