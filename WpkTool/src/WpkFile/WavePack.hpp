#ifndef WAVE_PACK_HPP
#define WAVE_PACK_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Wave.hpp"
#include "../WpkCompilator/WpkCompilatorGlobals.hpp"
#include "WavePackBase.hpp"

class WavePack : public WavePackBase
{
public:

    WavePack(std::string arg_wavepack_name = std::string()) :
        WavePackBase(arg_wavepack_name),

        m_level_index(0),
        m_level_mode(false),

        m_type_instruction_line_number(0),
        m_disable_level_2_instruction_line_number(0)
    {}

    ~WavePack()
    {}


    void ParseFrom(const std::string& uppercased_instruction, const std::string& original_instruction, std::size_t line_number);

    void ReadFrom(BinFile& bin_file);

    void WriteTo(BinFile& output_bin_file);

    void DebugTheObject();

    void ShowInformations() const;

    std::string ToOldFormatString();
    std::string ToNewFormatString();

    std::string GetWavePackName() const { return this->m_wavepack_name; }

    uint32_t GetDataSize() const
    {
        uint32_t temp_data_size = 0;
        for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
        {
            for (const Wave& single_wave : m_wave_vector_level[i])
            {
                temp_data_size += single_wave.GetSizeOf();
            }
        }

        return temp_data_size;
    }

public:

    friend void to_json(ordered_json& j, const WavePack& wp)
    {
        to_json(j, static_cast<const WavePackBase&>(wp));
    }

    friend void from_json(const ordered_json& j, WavePack& wp)
    {
        from_json(j, static_cast<WavePackBase&>(wp));
        wp.RecalculateWaveCounts();
    }

private:
    constexpr const unsigned int static format_size = 4;
    uint8_t m_format[format_size] = { 'W', 'P', 'K', '\0' };

    uint32_t m_number_of_waves_in_level[WavePackBase::c_number_of_levels] = { 0 };
    uint32_t m_number_of_waves_in_level_checksum[WavePackBase::c_number_of_levels] = {0};

    uint32_t m_level_index;

    bool m_level_mode;

    bool m_waves_error[c_number_of_levels] = { false };

    size_t m_type_instruction_line_number;
    size_t m_disable_level_2_instruction_line_number;
    size_t m_level_instruction_line_number[c_number_of_levels] = { 0 };

    void ReadWaveVector( BinFile& bin_file, std::vector<Wave>& arg_wave_vector, uint32_t arg_number_of_waves)
    {
        if (arg_number_of_waves != 0)
        {
            for (uint32_t j = 0; j < arg_number_of_waves; ++j)
            {
                Wave single_wave(this->m_wavepack_type);
                single_wave.ReadFrom(bin_file);
                arg_wave_vector.push_back( std::move(single_wave) );
            }
        }
    }

    void ExportWaveVector(const std::vector<Wave>& arg_wave_vector,
                         uint32_t arg_number_of_waves,
                         uint32_t arg_level_number,
                         std::stringstream& ss,
                         const WpkCompilatorGlobals::ENStringFormatMode& arg_string_format_mode);


    bool ParseLevelHeader(  const std::string& arg_line,
                            uint32_t& arg_level_index,
                            uint32_t arg_number_of_waves_in_level_checksum[],
                            bool& arg_level_mode);

    void ParseWaves(const std::string& uppercased_instruction,
                  const std::string& original_instruction,
                  uint32_t& arg_wavepack_type,
                  std::vector<Wave>& arg_wave_vector_level,
                  uint32_t& arg_number_of_waves_in_level_checksum,
                  bool& arg_level_mode,
                  size_t line_number);

    bool CheckWaves(const std::string& uppercased_instruction,
                    uint32_t arg_number_of_waves_in_level_checksum,
                    bool arg_level_mode);


private:

    void RecalculateWaveCounts()
    {
        for (uint32_t i = 0; i < c_number_of_levels; ++i)
        {
            this->m_number_of_waves_in_level[i] =
                static_cast<uint32_t>(this->m_wave_vector_level[i].size());

            this->m_number_of_waves_in_level_checksum[i] =
                this->m_number_of_waves_in_level[i];
        }
    }

};



#endif // WAVE_PACK_HPP end
