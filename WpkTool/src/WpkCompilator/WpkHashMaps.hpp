#ifndef WPK_HASH_MAPS_HPP
#define WPK_HASH_MAPS_HPP

#include "Compilator/HashMapsBase.hpp"
#include "WpkCompilatorStrings.hpp"
#include "WpkCompilatorGlobals.hpp"

class WpkHashMaps : public HashMapsBase
{
public:
    WpkHashMaps() : HashMapsBase()
    {}

    void InitializeHashMapsOfConsts() override
    {
        m_list_of_32_bit_consts[WpkCompilatorStrings::null_const_uppercase_str]    = WpkCompilatorGlobals::null_const_value;
        m_list_of_32_bit_consts[WpkCompilatorStrings::delayed_const_str]           = WpkCompilatorGlobals::delayed_const_value;
        m_list_of_32_bit_consts[WpkCompilatorStrings::delayed_const_uppercase_str] = WpkCompilatorGlobals::delayed_const_value;
    }

    void InitializeHashMapOfMasks() override
    {}


};


#endif // !WPK_HASH_MAPS_HPP
