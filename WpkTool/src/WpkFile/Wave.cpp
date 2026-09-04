#include "Wave.hpp"
#include "../WpkCompilator/WpkCompilatorStrings.hpp"
#include "Compilator/CompilatorValueTypes.hpp"
#include "Utils/CppWriter.hpp"

using namespace std;

void Wave::ParseFrom(const string& uppercased_instruction, const string& original_instruction, size_t line_number)
{
    if ( uppercased_instruction.starts_with(WpkCompilatorStrings::wave_struct_name_with_space_str) )
    {
        this->m_line_number = line_number;

        string m_help_str = 
        original_instruction.substr
        (
            WpkCompilatorStrings::wave_struct_name_with_space_len,
            original_instruction.length() - WpkCompilatorStrings::wave_struct_name_with_space_len
        );

        int32_t temp_values[size_of_temp_values] = { 0 };
        string wave_path_token;

        DEBUG_PRINT("WAVE TYPE = ", std::to_string(this->m_wavepack_type), Mess::endl);


        if (this->m_wavepack_type == WpkCompilatorGlobals::new_wpk_format_type)
        {
            std::istringstream iss(m_help_str);
            bool parsed = false;

            // Proba parsowania z "delayed"
            if (iss
                >> temp_values[0]
                >> temp_values[1]
                >> temp_values[2]
                >> temp_values[3]
                >> temp_values[4]
                >> temp_values[5])
            {
                string next_token;
                if (iss >> next_token)
                {
                    if (next_token == WpkCompilatorStrings::delayed_const_str ||
                        next_token == WpkCompilatorStrings::delayed_const_uppercase_str)
                    {
                        if (iss >> wave_path_token)
                        {
                            temp_values[6] = WpkCompilatorGlobals::delayed_const_value;
                            parsed = true;
                        }
                    }
                    else
                    {
                        // next_token jest już ścieżką (brak "delayed")
                        wave_path_token = next_token;
                        parsed = true;
                    }
                }
            }

            if (!parsed)
            {
                this->m_error = true;
            }
        }

        if (this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type)
        {
            std::istringstream iss(m_help_str);

            if (!(iss >> temp_values[0] >> wave_path_token))
            {
                this->m_error = true;
            }
        }

        this->m_wave_path = wave_path_token;

        /*
        // Old code with scanf

        constexpr unsigned int size_of_c_help = 255;
        char c_help[size_of_c_help] = { '\0' };

        int32_t temp_values[size_of_temp_values] = { 0 };

        DEBUG_PRINT("WAVE TYPE = ", std::to_string(this->m_wavepack_type), Mess::endl);

        if (this->m_wavepack_type == WpkCompilatorGlobals::new_wpk_format_type)
        {
            if( sscanf_s
                (
                m_help_str.c_str(),
                "%d %d %d %d %d %d delayed %s",
                &temp_values[0],
                &temp_values[1],
                &temp_values[2],
                &temp_values[3],
                &temp_values[4],
                &temp_values[5],
                c_help,
                size_of_c_help
                ) < 7
            )
            {
                if (sscanf_s
                    (
                    m_help_str.c_str(),
                    "%d %d %d %d %d %d %s",
                    &temp_values[0],
                    &temp_values[1],
                    &temp_values[2],
                    &temp_values[3],
                    &temp_values[4],
                    &temp_values[5],
                    c_help,
                    size_of_c_help
                    ) < 7
                )
                {
                    this->m_error = true;
                }
            }
            else
            {
                temp_values[6] = WpkCompilatorGlobals::delayed_const_value;
            }
        }


        if (this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type)
        {
            if(sscanf_s
                (
                m_help_str.c_str(), 
                "%d %s", 
                &temp_values[0], 
                c_help, 
                size_of_c_help
                ) < 2
            )
            {
                this->m_error = true;
            }
        }

        for (int i = 0; i < size_of_c_help; ++i)
        {
            if (c_help[i] == '\0') { break; }
            this->m_wave_path += c_help[i];
        }

        */

        this->m_volume         = static_cast<uint32_t>(temp_values[0]);
        this->m_distance_min_a = static_cast<uint32_t>(temp_values[1]);
        this->m_distance_max_a = static_cast<uint32_t>(temp_values[2]);

        this->m_sound_flags   = static_cast<uint8_t>(temp_values[    this->a_size]);
        this->m_play_priority = static_cast<uint8_t>(temp_values[1 + this->a_size]);
   
        this->m_const_value_0 = static_cast<uint32_t>(temp_values[    this->a_size + this->b_size]);
        this->m_delay = static_cast<uint32_t>(temp_values[1 + this->a_size + this->b_size]);
        this->m_const_value_2 = static_cast<uint32_t>(temp_values[2 + this->a_size + this->b_size]);



        DEBUG_PRINT(
                    "WAVE:\n{\n",
                    std::to_string(this->m_a[0]), Mess::endl,
                    std::to_string(this->m_a[1]), Mess::endl,
                    std::to_string(this->m_a[2]), Mess::endl,
                    std::to_string(this->m_b[0]), Mess::endl,
                    std::to_string(this->m_b[1]), Mess::endl,
                    std::to_string(this->m_c[0]), Mess::endl,
                    std::to_string(this->m_c[1]), Mess::endl,
                    this->m_wave_path, Mess::endl,
                    "}\n\n"
                    );
    }

}

std::string Wave::ToOldFormatString() const
{
    stringstream ss;

    switch (this->m_wavepack_type)
    {
        case WpkCompilatorGlobals::old_wpk_format_type:

            ss  << WpkCompilatorStrings::wave_struct_name_str
                << " "
                << this->m_volume
                << " "
                << this->m_wave_path;
        break;

        case WpkCompilatorGlobals::new_wpk_format_type:

            ss  << WpkCompilatorStrings::wave_struct_name_str
                << " "
                << this->m_volume
                << " "
                << this->m_distance_min_a
                << " "
                << this->m_distance_max_a
                << " "
                << static_cast<uint32_t>(this->m_sound_flags)
                << " "
                << static_cast<uint32_t>(this->m_play_priority)
                << " "
                << this->m_const_value_0
                << " ";

                if (this->m_delay == WpkCompilatorGlobals::delayed_const_value)
                {
                    ss << WpkCompilatorStrings::delayed_const_str << ' ';
                }

            ss << this->m_wave_path;

        break;

        default:
        break;
    }

    return ss.str();
}

string Wave::ToNewFormatString() const
{
    stringstream ss;

    const uint32_t temp_const_values_array[Wave::c_size] =
        { m_const_value_0, m_delay, m_const_value_2 };

    switch (this->m_wavepack_type)
    {
        case WpkCompilatorGlobals::old_wpk_format_type:

            ss  << CompilatorValueTypes::c_struct_type_str << " " << WpkCompilatorStrings::wave_struct_name_str << "{ ";

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::volume_value_name_str, this->m_volume, false);
            ss  << '\t';

            ss  << CompilatorValueTypes::c_char_type_str << " "
                << WpkCompilatorStrings::wave_path_name_str << "[] = \"" << this->m_wave_path << "\"; };";

        break;

        case WpkCompilatorGlobals::new_wpk_format_type:

            ss << CompilatorValueTypes::c_struct_type_str << " " << WpkCompilatorStrings::wave_struct_name_str << "{ ";

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::volume_value_name_str, this->m_volume, false);
            ss << '\t';

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::distance_min_a_value_name_str, this->m_distance_min_a, false);
            ss << '\t';

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::distance_max_a_value_name_str, this->m_distance_max_a, false);
            ss << '\t';

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::sound_flags_value_name_str, this->m_sound_flags, false);
            ss << '\t';

            CW::WriteTrivialValue(ss, WpkCompilatorStrings::play_priority_value_name_str, this->m_play_priority, false);
            ss << '\t';

            for (int i = 0; i < this->c_size; ++i)
            {
                if (i == 1)
                {
                    ss << CompilatorValueTypes::c_32bituniquekey_type_str
                       << " "
                       << WpkCompilatorStrings::delay_const_str
                       << " = ";


                    switch (temp_const_values_array[i])
                    {
                        case WpkCompilatorGlobals::null_const_value:
                            ss << WpkCompilatorStrings::null_const_uppercase_str;
                        break;

                        case WpkCompilatorGlobals::delayed_const_value:
                            ss << WpkCompilatorStrings::delayed_const_uppercase_str;
                        break;

                        default:
                            ss << temp_const_values_array[i];
                        break;
                    }

                    ss << ";\t";

                    continue;
                }

                CW::WriteTrivialValue(ss, "c_" + to_string(i), temp_const_values_array[i], false);
                ss << '\t';
            }

            ss << CompilatorValueTypes::c_char_type_str << " "
               << WpkCompilatorStrings::wave_path_name_str <<
                "[] = \"" << this->m_wave_path << "\"; };\t   ";
        break;

        default:
        break;
    }

    return ss.str();
}
