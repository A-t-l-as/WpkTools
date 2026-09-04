#ifndef WAVE_BASE_HPP
#define WAVE_BASE_HPP

#include <cstdint>
#include <string>
#include "../Other/json.hpp"
#include "../WpkCompilator/WpkJsonStrings.hpp"

using ordered_json = nlohmann::ordered_json;

class WaveBase
{
public:
    WaveBase() :
        m_volume(0),
        m_distance_min_a(0),
        m_distance_max_a(0),

        m_sound_flags(0),
        m_play_priority(0),

        m_const_value_0(0),
        m_delay(0),
        m_const_value_2(0),

        m_wave_path(std::string())
    {}

    ~WaveBase()
    {}

    friend void to_json(ordered_json& j, const WaveBase& w)
    {
        j = ordered_json
            {
                {WpkJsonStrings::volume_value_name_str, w.m_volume},
                {WpkJsonStrings::distance_min_a_value_name_str, w.m_distance_min_a},
                {WpkJsonStrings::distance_max_a_value_name_str, w.m_distance_max_a},

                {WpkJsonStrings::sound_flags_value_name_str, w.m_sound_flags},
                {WpkJsonStrings::play_priority_value_name_str, w.m_play_priority},

                {WpkJsonStrings::const_value_0_name_str, w.m_const_value_0},
                {WpkJsonStrings::delay_value_name_str, w.m_delay},
                {WpkJsonStrings::const_value_2_name_str, w.m_const_value_2},

                {WpkJsonStrings::wave_path_value_name_str, w.m_wave_path}
            };
    }

    friend void from_json(const ordered_json& j, WaveBase& w)
    {
        j.at(WpkJsonStrings::volume_value_name_str).get_to(w.m_volume);
        j.at(WpkJsonStrings::distance_min_a_value_name_str).get_to(w.m_distance_min_a);
        j.at(WpkJsonStrings::distance_max_a_value_name_str).get_to(w.m_distance_max_a);

        j.at(WpkJsonStrings::sound_flags_value_name_str).get_to(w.m_sound_flags);
        j.at(WpkJsonStrings::play_priority_value_name_str).get_to(w.m_play_priority);

        j.at(WpkJsonStrings::const_value_0_name_str).get_to(w.m_const_value_0);
        j.at(WpkJsonStrings::delay_value_name_str).get_to(w.m_delay);
        j.at(WpkJsonStrings::const_value_2_name_str).get_to(w.m_const_value_2);

        j.at(WpkJsonStrings::wave_path_value_name_str).get_to(w.m_wave_path);
    }

protected:

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


#endif // WAVE_BASE_HPP end
