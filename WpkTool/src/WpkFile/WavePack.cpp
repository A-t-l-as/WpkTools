#include "WavePack.hpp"
#include "Utils/Globals.hpp"
#include "Utils/TxtUtils.hpp"
#include "../WpkCompilator/WpkCompilatorStrings.hpp"
#include "Utils/CppWriter.hpp"
#include "Compilator/CompilatorStrings.hpp"
#include "Compilator/CompilatorValueTypes.hpp"

using namespace std;

void WavePack::ParseFrom(const string& uppercased_instruction, const string& original_instruction, size_t line_number)
{
    if (//error detection:
        CheckWaves(
            uppercased_instruction,
            this->m_number_of_waves_in_level_checksum[this->m_level_index],
            this->m_level_mode
        )
        ==
        Globals::error_code
        )
    {
        this->m_waves_error[this->m_level_index] = true;
    }

    if (
        TU::GetOneValue<uint32_t>
        (
            uppercased_instruction,
            WpkCompilatorStrings::type_instruction_name_upper_str,
            this->m_wavepack_type
        )
        == 
        Globals::success_code
        )
    {
        this->m_type_instruction_line_number = line_number;
    }

    if (
        TU::GetOneValue<uint32_t>
        (
            uppercased_instruction,
            WpkCompilatorStrings::disable_level_2_instruction_name_upper_str,
            this->m_special_hh_and_ww3_case
        )
        ==
        Globals::success_code
        )
    {
        this->m_disable_level_2_instruction_line_number = line_number;
    }


    if (
        ParseLevelHeader
        (
            uppercased_instruction,
            this->m_level_index,
            this->m_number_of_waves_in_level_checksum,
            this->m_level_mode
        )
        ==
        Globals::success_code
        )
    {
        this->m_level_instruction_line_number[this->m_level_index] = line_number;
    }
                        
    ParseWaves(uppercased_instruction,
                 original_instruction,
                 this->m_wavepack_type,
                 this->m_wave_vector_level[this->m_level_index],
                 this->m_number_of_waves_in_level_checksum[this->m_level_index],
                 this->m_level_mode,
                 line_number);

}

void WavePack::ReadFrom(BinFile& bin_file)
{
    const uint8_t correct_format[] = {'W', 'P', 'K', '\0'};

    bin_file.ReadArray(this->m_format, this->format_size);

    if( memcmp(this->m_format, correct_format, this->format_size) != 0 )
    {
        throw std::runtime_error("The file you are trying to read WavePack from is not a WPK file (magic bytes).");
    }

    bin_file.ReadValue(this->m_wavepack_type);

    bin_file.ReadValue(this->m_number_of_waves_in_level[0]);
    bin_file.ReadValue(this->m_number_of_waves_in_level[1]);

    if (this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type)
    {
        uint32_t temp_data_size_value;
        bin_file.ReadValue(temp_data_size_value);

        if (temp_data_size_value == bin_file.GetFileLength() - bin_file.GetCurrentOffset() )
        {
            this->m_special_hh_and_ww3_case = true;
        }

        bin_file.MoveOverBy( - static_cast<int64_t>( sizeof(temp_data_size_value) ) );
    }

    if(this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type && this->m_special_hh_and_ww3_case == false)
    {
        bin_file.ReadValue(this->m_number_of_waves_in_level[2]);
    }

    bin_file.MoveOverBy(sizeof(uint32_t));

    for(uint32_t i = 0 ; i < this->c_number_of_levels ; ++i)
        ReadWaveVector(bin_file, this->m_wave_vector_level[i], this->m_number_of_waves_in_level[i]);
}



void WavePack::WriteTo(BinFile& output_bin_file)
{
    output_bin_file.WriteValue(this->m_format);
    output_bin_file.WriteValue(this->m_wavepack_type);

    output_bin_file.WriteValue(static_cast<uint32_t>(this->m_wave_vector_level[0].size()) );
    output_bin_file.WriteValue(static_cast<uint32_t>(this->m_wave_vector_level[1].size()) );

    if ( 
         this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type
         && 
         this->m_special_hh_and_ww3_case == static_cast<uint32_t>(false) 
       )
    {
        output_bin_file.WriteValue(static_cast<uint32_t>(this->m_wave_vector_level[2].size()) );
    }

    output_bin_file.WriteValue( this->GetDataSize() );

    for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        for (Wave& single_wave : m_wave_vector_level[i])
        {
            single_wave.WriteTo(output_bin_file);
        }
    }
}

void WavePack::DebugTheObject()
{
    bool error = false;

    if (this->m_special_hh_and_ww3_case > static_cast<uint32_t>(true))
    {
        CONSOLE_OUT.PrintLn(Mess::endl,
                            "-- Error: ----------------------------------------------------------------------------",
                            Mess::endl,
                            " The instruction named DisableLevel2 should take the value 0 or 1..."                  ,
                            Mess::endl,
                            " Wavepack name: " , this->m_wavepack_name                                              ,
                            Mess::endl,
                            " Line: " , this->m_disable_level_2_instruction_line_number                             ,
                            Mess::endl,
                            "--------------------------------------------------------------------------------------",
                            Mess::endl);


        this->m_special_hh_and_ww3_case = static_cast<uint32_t>(static_cast<bool>(this->m_special_hh_and_ww3_case));

        error = true;
    }

    for (uint32_t i = 0; i < this->c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        if (this->m_waves_error[i] == true)
        {
            CONSOLE_OUT.PrintLn(Mess::endl,
                                "-- Error: ----------------------------------------------------------------------------" ,
                                Mess::endl,
                                " The number of Waves in the Level header does not match the actual number of Waves."    ,
                                Mess::endl,
                                " Wavepack name: " , this->m_wavepack_name                                               ,
                                Mess::endl,
                                " Line: " , this->m_level_instruction_line_number[i]                                     ,
                                Mess::endl,
                                "--------------------------------------------------------------------------------------" ,
                                Mess::endl);

            error = true;
        }
    }

    for (uint32_t i = 0; i < c_number_of_levels - this->m_special_hh_and_ww3_case; ++i)
    {
        size_t j = 0;
        for (const Wave& single_wave : m_wave_vector_level[i])
        {
            if ( single_wave.GetError() )
            {
                CONSOLE_OUT.PrintLn(Mess::endl,
                                    "-- Error: ----------------------------------------------------------------------------" ,
                                    Mess::endl,
                                    " Wave " , j , " in level " , i , " has an error!"                                       ,
                                    Mess::endl,
                                    " Wavepack name: " , this->m_wavepack_name                                               ,
                                    Mess::endl,
                                    " Line: " , single_wave.GetLineNumber()                                                  ,
                                    Mess::endl,
                                    "--------------------------------------------------------------------------------------" ,
                                    Mess::endl);

                error = true;
            }
            ++j;
        }
    }

    if (error == false)
    {
        CONSOLE_OUT.PrintLn( this->m_wavepack_name, " ..... OK");
    }
    else
    {
        CONSOLE_OUT.PrintLn( "The file has errors and cannot be compiled... ");
        exit(EXIT_FAILURE);
    }

}

void WavePack::ShowInformations() const
{
    uint32_t temp_number_of_levels = 0;

    if  (
            this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type
            &&
            this->m_special_hh_and_ww3_case == static_cast<uint32_t>( false )
        )
    {
        temp_number_of_levels = this->c_number_of_levels;
    }
    else
    {
        temp_number_of_levels = this->c_number_of_levels - 1;
    }


    CONSOLE_OUT.PrintLn("Format: ", std::string( reinterpret_cast<const char*>(this->m_format) ), Mess::endl,
                "Wavepack type: " , this->m_wavepack_type , Mess::endl,
                "Size of data: " , this->GetDataSize() );


    for (uint32_t i = 0; i < temp_number_of_levels; ++i)
        CONSOLE_OUT.PrintLn("Number of waves in level", i, ": ", this->m_number_of_waves_in_level[i]);

}


std::string WavePack::ToOldFormatString()
{
    stringstream ss;

    ss << WpkCompilatorStrings::wavepack_namespace_name_str << ' ' << this->m_wavepack_name << endl
       << '{' << endl;

    if(this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type)
    {
        ss << '\t' << WpkCompilatorStrings::type_instruction_name_str << ' ' << this->m_wavepack_type << endl;
        ss << '\t'
           << WpkCompilatorStrings::disable_level_2_instruction_name_str << ' ' << this->m_special_hh_and_ww3_case << endl;
    }

    for(uint32_t i = 0 ; i < this->c_number_of_levels; ++i)
        ExportWaveVector(
                        this->m_wave_vector_level[i],
                        this->m_number_of_waves_in_level[i],
                        i,
                        ss,
                        WpkCompilatorGlobals::StringFormatMode::OldStringFormat
                        );

    ss << '}' << endl;

    return ss.str();
}


string WavePack::ToNewFormatString()
{
    stringstream ss;

    for (int i = 0; i < format_size; ++i)
    {
        CW::WriteTrivialValue(ss, WpkCompilatorStrings::format_value_name_str + to_string(i), this->m_format[i], false);

        if (isalpha(this->m_format[i]))
            ss << " // " << this->m_format[i];

        ss << endl;
    }

    ss << endl;

    CW::WriteTrivialValue(ss, WpkCompilatorStrings::wavepack_type_value_name_str, this->m_wavepack_type); ss << endl;

    uint32_t temp_number_of_levels = 0;

    if (this->m_wavepack_type == WpkCompilatorGlobals::old_wpk_format_type && this->m_special_hh_and_ww3_case == false)
    {
        temp_number_of_levels = this->c_number_of_levels;
    }
    else
    {
        temp_number_of_levels = this->c_number_of_levels - 1;
    }

    for (uint32_t i = 0; i < temp_number_of_levels; ++i)
    {
        ss << CW::ConvertTrivialTypeToString(this->m_number_of_waves_in_level[i])
        << ' ' << WpkCompilatorStrings::number_of_waves_in_level_value_name_str << i << " = "
        << CompilatorStrings::count_fn_str << ';'
        << endl;
    }

    ss  << endl
       << CW::ConvertTrivialTypeToString( this->GetDataSize() )
        << ' '
        << WpkCompilatorStrings::data_size_value_name_str
        << " = "
        << WpkCompilatorStrings::measure_all_value_sizes_after_this_instruction_fn_name_str
        << ';'
        << endl
        << endl

        << CompilatorValueTypes::c_namespace_type_str << ' ' << m_wavepack_name << endl
        << '{' << endl;


    for(uint32_t i = 0 ; i < temp_number_of_levels; ++i)
        ExportWaveVector(
                        this->m_wave_vector_level[i],
                        this->m_number_of_waves_in_level[i],
                        i,
                        ss,
                        WpkCompilatorGlobals::StringFormatMode::NewStringFormat
                        );

    ss << "};" << endl;

    return ss.str();
}


void WavePack::ExportWaveVector( const std::vector<Wave>& arg_wave_vector,
                                uint32_t arg_number_of_waves,
                                uint32_t arg_level_number,
                                std::stringstream& arg_ss,
                                const WpkCompilatorGlobals::StringFormatMode& arg_string_format_mode)
{
    if (arg_number_of_waves != 0)
    {
        switch (arg_string_format_mode)
        {
            case WpkCompilatorGlobals::StringFormatMode::OldStringFormat:
                arg_ss  << '\t' << WpkCompilatorStrings::level_struct_name_str
                        << arg_level_number << " " << arg_number_of_waves << endl
                        << "\t{" << endl;
            break;

            case WpkCompilatorGlobals::StringFormatMode::NewStringFormat:
                arg_ss  << '\t' << CompilatorValueTypes::c_struct_type_str
                        << ' '  << WpkCompilatorStrings::level_struct_name_str << '_' << arg_level_number << endl
                        << "\t{" << endl;
            break;

            default:
            break;
        }

        for (const auto& single_wave : arg_wave_vector)
        {

            arg_ss << "\t\t";

            switch (arg_string_format_mode)
            {
                case WpkCompilatorGlobals::StringFormatMode::OldStringFormat:
                    arg_ss << single_wave.ToOldFormatString();
                break;

                case WpkCompilatorGlobals::StringFormatMode::NewStringFormat:
                    arg_ss << single_wave.ToNewFormatString();
                break;

                default:
                break;
            }

            arg_ss << endl;
        }


        switch (arg_string_format_mode)
        {
            case WpkCompilatorGlobals::StringFormatMode::OldStringFormat:
                arg_ss << "\t}" << endl;
            break;

            case WpkCompilatorGlobals::StringFormatMode::NewStringFormat:
                arg_ss << "\t};" << endl;
            break;

            default:
            break;
        }

    }
}



bool WavePack::ParseLevelHeader(const string& arg_line,
                                uint32_t& arg_level_index,
                                uint32_t arg_number_of_waves_in_level_checksum[],
                                bool& arg_level_mode)
{
    const uint32_t number_of_levels = 3;

    for (uint32_t i = 0; i < number_of_levels; ++i)
    {
        string temp_level_type_name = WpkCompilatorStrings::level_struct_name_upper_str + to_string(i);
        const size_t temp_level_type_name_len = temp_level_type_name.length();

        if ( arg_line.starts_with(temp_level_type_name) )
        {
            DEBUG_PRINT("ITS LEVEL HEADER!\n");

            string m_help_str = arg_line.substr(temp_level_type_name_len, arg_line.length() - temp_level_type_name_len);

            //sscanf_s
            //    (
            //        m_help_str.c_str(),
            //        "%d",
            //       &arg_number_of_waves_in_level_checksum[i]
            //        );

            arg_number_of_waves_in_level_checksum[i] = static_cast<uint32_t>( stoul(m_help_str) );

            DEBUG_PRINT("arg_number_of_waves_in_level_checksum = ", arg_number_of_waves_in_level_checksum[i], Mess::endl);

            arg_level_index = i;

            arg_level_mode = true;

            return Globals::success_code;
        }
    }

    return Globals::error_code;
}

void WavePack::ParseWaves(  const string& uppercased_instruction,
                            const string& original_instruction,
                            uint32_t& arg_wavepack_type,
                            vector<Wave>& arg_wave_vector_level,
                            uint32_t& arg_number_of_waves_in_level_checksum,
                            bool& arg_level_mode,
                            size_t line_number)
{
    if ( !uppercased_instruction.starts_with(WpkCompilatorStrings::level_struct_name_upper_str)  &&
        uppercased_instruction != "{" &&
        uppercased_instruction != "}" &&
        arg_number_of_waves_in_level_checksum > 0 &&
        arg_level_mode == true)
    {
        if (uppercased_instruction.find(" ") != string::npos)
        {
            DEBUG_PRINT("TU::GetWaves\n");

            Wave temporary_single_wave(arg_wavepack_type);
            temporary_single_wave.ParseFrom(uppercased_instruction, original_instruction, line_number);

            arg_wave_vector_level.push_back(temporary_single_wave);


            --arg_number_of_waves_in_level_checksum;
        }
    }

    if (arg_level_mode == true && arg_number_of_waves_in_level_checksum == 0)
    {
        arg_level_mode = false;
    }

}


bool WavePack::CheckWaves(  const std::string& uppercased_instruction,
                            uint32_t arg_number_of_waves_in_level_checksum,
                            bool arg_level_mode)
// success = everything is okay
// failure = error detected
{
    if (uppercased_instruction.starts_with(WpkCompilatorStrings::wave_struct_name_upper_str) &&
        uppercased_instruction != "{" &&
        uppercased_instruction != "}" &&
        arg_number_of_waves_in_level_checksum == 0 &&
        arg_level_mode == false)
    {
        return Globals::error_code;
    }


    if (uppercased_instruction == "}" &&
        arg_number_of_waves_in_level_checksum > 0 &&
        arg_level_mode == true)
    {
        return Globals::error_code;
    }

    return Globals::success_code;
}



