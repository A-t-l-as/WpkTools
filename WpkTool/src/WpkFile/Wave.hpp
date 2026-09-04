#ifndef WAVE_HPP
#define WAVE_HPP

#include "../WpkCompilator/WpkCompilatorGlobals.hpp"
#include <cstdint>
#include <string>
#include "File/BinFile.hpp"
#include "Console/Console.hpp"

#include "WaveBase.hpp"

class Wave : public WaveBase
{
public:

    Wave(uint32_t arg_wavepack_type = WpkCompilatorGlobals::new_wpk_format_type) :
        WaveBase(),

        m_error(false),
        m_line_number(0),
        m_wavepack_type(arg_wavepack_type)
    {}

    ~Wave()
    {}

    void ParseFrom
    (
        const std::string& uppercased_instruction,
        const std::string& original_instruction,
        size_t line_number
    );

    void ReadFrom(BinFile& bin_file)
    {
        switch (this->m_wavepack_type)
        {
            case WpkCompilatorGlobals::old_wpk_format_type:
                bin_file.ReadValue(this->m_volume);
                this->m_wave_path = bin_file.ReadStringWithoutLen();
            break;

            case WpkCompilatorGlobals::new_wpk_format_type:
                bin_file.ReadValue(this->m_volume);
                bin_file.ReadValue(this->m_distance_min_a);
                bin_file.ReadValue(this->m_distance_max_a);

                bin_file.ReadValue(this->m_sound_flags);
                bin_file.ReadValue(this->m_play_priority);

                bin_file.ReadValue(this->m_const_value_0);
                bin_file.ReadValue(this->m_delay);
                bin_file.ReadValue(this->m_const_value_2);

                this->m_wave_path = bin_file.ReadStringWithoutLen();
            break;

            default:
            break;
        }
    }


    void WriteTo(BinFile& output_bin_file)
    {
        switch (this->m_wavepack_type)
        {
            case WpkCompilatorGlobals::old_wpk_format_type:
                output_bin_file.WriteValue(this->m_volume);
                output_bin_file.WriteString(this->m_wave_path);
                output_bin_file.WriteValue('\0');
            break;

            case WpkCompilatorGlobals::new_wpk_format_type:
                output_bin_file.WriteValue(this->m_volume);
                output_bin_file.WriteValue(this->m_distance_min_a);
                output_bin_file.WriteValue(this->m_distance_max_a);


                output_bin_file.WriteValue(this->m_sound_flags);
                output_bin_file.WriteValue(this->m_play_priority);

                output_bin_file.WriteValue(this->m_const_value_0);
                output_bin_file.WriteValue(this->m_delay);
                output_bin_file.WriteValue(this->m_const_value_2);

                output_bin_file.WriteString(this->m_wave_path);
                output_bin_file.WriteValue('\0');
            break;

            default:
            break;
        }
    }

    bool GetError() const { return this->m_error; }

    uint32_t GetSizeOf() const
    {
        uint32_t result = 0;

        if (this->m_wavepack_type == WpkCompilatorGlobals::new_wpk_format_type)
        {
            result = a_size * sizeof(uint32_t) +
                     b_size * sizeof(uint8_t) +
                     c_size * sizeof(uint32_t) +
                     static_cast<uint32_t>(this->m_wave_path.length()) +
                     sizeof(char); // \0
        }


        if (this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type)
        {
            result = sizeof(m_volume) +
                     static_cast<uint32_t>(this->m_wave_path.length()) +
                     sizeof(char); // \0
        }

        DEBUG_PRINT("SIZE OF WAVE = ", result, Mess::endl);

        return result;
    }

    std::size_t GetLineNumber() const { return this->m_line_number;  }

    std::string ToOldFormatString() const;
    std::string ToNewFormatString() const;


private:
    bool m_error;
    size_t m_line_number;
    uint32_t m_wavepack_type;

};




#endif // WAVE_HPP
