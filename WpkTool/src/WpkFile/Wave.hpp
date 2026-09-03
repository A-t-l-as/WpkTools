#ifndef WAVE_HPP
#define WAVE_HPP

#include "../WpkCompilator/WpkCompilatorGlobals.hpp"
#include <cstdint>
#include <string>
#include "File/BinFile.hpp"
#include "Console/Console.hpp"

class Wave
{
public:
    Wave(uint32_t arg_wavepack_type = WpkCompilatorGlobals::new_wpk_format_type) :
        m_wave_path(std::string()),
        m_wavepack_type(arg_wavepack_type),

        m_error(false),
        m_line_number(0),
        m_volume(0),
        m_distance_min_a(0),
        m_distance_max_a(0),

        m_sound_flags(0),
        m_play_priority(0),

        m_const_value_0(0),
        m_delay(0),
        m_const_value_2(0)
    {}

    ~Wave()
    {}

    void ParseFrom(const std::string& uppercased_instruction, const std::string& original_instruction, size_t line_number);

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
            result = a_size +
                     b_size +
                     c_size +
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

    constexpr const unsigned int static a_size = 3;
    constexpr const unsigned int static b_size = 2;
    constexpr const unsigned int static c_size = 3;
    constexpr const size_t       static size_of_temp_values = a_size + b_size + c_size;

    uint32_t m_volume; // volume
    uint32_t m_distance_min_a; //distanceMinA
    uint32_t m_distance_max_a; //distanceMaxA

    uint8_t  m_sound_flags; //soundFlags
    uint8_t  m_play_priority; //playPriority

    uint32_t m_const_value_0;
    uint32_t m_delay;
    uint32_t m_const_value_2;

    std::string m_wave_path;
};




#endif // WAVE_HPP
