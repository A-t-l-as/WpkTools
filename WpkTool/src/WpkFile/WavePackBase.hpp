#ifndef WAVE_PACK_BASE_HPP
#define WAVE_PACK_BASE_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Wave.hpp"
#include "../WpkCompilator/WpkCompilatorGlobals.hpp"

class WavePackBase
{
public:

    WavePackBase(std::string arg_wavepack_name = std::string()) :
        m_wavepack_name(arg_wavepack_name),
        m_wavepack_type(WpkCompilatorGlobals::new_wpk_format_type),
        m_special_hh_and_ww3_case(static_cast<uint32_t>(false))
    {}

    ~WavePackBase()
    {}

    friend void to_json(ordered_json& j, const WavePackBase& wp)
    {
        j = ordered_json
            {
                {WpkJsonStrings::wavepack_value_name_str, wp.m_wavepack_name},
                {WpkJsonStrings::type_value_name_str, wp.m_wavepack_type},
                {WpkJsonStrings::disable_level_2_value_name_str, wp.m_special_hh_and_ww3_case},
                {WpkJsonStrings::levels_value_name_str, wp.m_wave_vector_level}
            };
    }

    friend void from_json(const ordered_json& j, WavePackBase& wp)
    {
        j.at(WpkJsonStrings::wavepack_value_name_str).get_to(wp.m_wavepack_name);
        j.at(WpkJsonStrings::type_value_name_str).get_to(wp.m_wavepack_type);
        j.at(WpkJsonStrings::disable_level_2_value_name_str).get_to(wp.m_special_hh_and_ww3_case);
        j.at(WpkJsonStrings::levels_value_name_str).get_to(wp.m_wave_vector_level);
    }

protected:
    std::string m_wavepack_name;
    uint32_t m_wavepack_type;
    uint32_t m_special_hh_and_ww3_case;

    constexpr const unsigned int static c_number_of_levels = 3;
    std::vector<Wave> m_wave_vector_level[c_number_of_levels] = {};

};



#endif // WAVE_PACK_BASE_HPP end
