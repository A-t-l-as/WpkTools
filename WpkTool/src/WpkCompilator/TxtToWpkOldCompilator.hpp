#ifndef TXT_TO_WPK_OLD_COMPILATOR_HPP
#define TXT_TO_WPK_OLD_COMPILATOR_HPP

#include <cstdint>
#include <string>
#include <filesystem>
#include "File/TextFile.hpp"
#include "Utils/Globals.hpp"
#include "../WpkFile/WavePack.hpp"
#include "../WpkFile/WholeWpkFilesContainer.hpp"
#include "WpkCompilatorGlobals.hpp"
#include "WpkCompilatorStrings.hpp"
#include "Utils/TxtUtils.hpp"
#include "Utils/StringUtils.hpp"

class TxtToWpkOldCompilator
{
public:
    TxtToWpkOldCompilator(WholeWpkFilesContainer& arg_wpk_files, bool& arg_error_handler) :
        r_wavepacks( arg_wpk_files.GetWavePacksRef() ),
        r_error_handler( arg_error_handler ),

        m_inside_obj_type( std::string() ),
        m_inside_obj_name( std::string() ),

        bracket_counter( WpkCompilatorGlobals::initialize_signal_value ),
        m_line_number( 0 )
    {}

    void CompileFile(const std::filesystem::path& arg_input_path)
    {
        TextFile input_file;
        input_file.LoadFrom(arg_input_path);

        input_file.Informations();
        //input_file.ExitWhenFileDontExist();
        this->r_error_handler = !(input_file.GetIsOpened());

        if ((input_file.Empty() == false) && (this->r_error_handler == Globals::success_code))
        {
            input_file.DeleteComments();
            input_file.CleanTxtFileBufferFromMultiSpaces(true);

            CompileTxtFileToBinBuffers(input_file.GetCleanFileBuffer());
        }
    }

private:

    bool& r_error_handler;

    size_t m_line_number;

    std::vector<WavePack>& r_wavepacks;

    WavePack m_single_wavepack = {};

    int64_t bracket_counter;

    std::string m_inside_obj_type;
    std::string m_inside_obj_name;


    void CompileTxtFileToBinBuffers(const std::vector<char>& arg_clean_file)
    {
        std::string_view file_view(arg_clean_file.data(), arg_clean_file.size());

        std::size_t start = 0;
        while(true)
        {
            std::size_t end = file_view.find(';', start);

            if (end == std::string_view::npos)
                break;

            std::string instruction( file_view.substr(start, end - start ) );

            ++this->m_line_number;

            TU::PrepareString(instruction, "{");
            TU::PrepareString(instruction, "}");

            DEBUG_PRINT("----\n\"", instruction, "\"\n");

            InterpretTxtWavePacksInstructions( StringUtils::GetToUpper(instruction), instruction);

            DEBUG_PRINT("----\n");

            start = end + 1;
        }

    }

    template<class T>
    void CompileObject(
        const std::string& uppercased_instruction,
        const std::string& original_instruction,
        T& single_object,
        std::vector<T>& objects_vec
        )
    {
        if (uppercased_instruction == "{" && bracket_counter == WpkCompilatorGlobals::initialize_signal_value)
        {
            ++bracket_counter;
            DEBUG_PRINT("Initialize...\n");
            single_object = T(m_inside_obj_name);
        }

        if (uppercased_instruction == "{")
        {
            ++bracket_counter;
        }

        if (bracket_counter > 0)
        {
            single_object.ParseFrom(uppercased_instruction, original_instruction, this->m_line_number);
        }

        if (uppercased_instruction == "}")
        {

            --bracket_counter;
        }

        if (uppercased_instruction == "}" && bracket_counter == WpkCompilatorGlobals::push_back_signal_value)
        {
            DEBUG_PRINT("DEBUGGING OBJECT!\n");

            single_object.DebugTheObject();

            DEBUG_PRINT("PushBack\n");
            objects_vec.push_back(single_object);
            single_object = T();
            bracket_counter = WpkCompilatorGlobals::initialize_signal_value;

            m_inside_obj_type = std::string();
            m_inside_obj_name = std::string();
        }

    }

    void InterpretTxtWavePacksInstructions(const std::string& uppercased_instruction, const std::string& original_instruction)
    {
        TU::GetObjType(
            uppercased_instruction,
            original_instruction,
            m_inside_obj_type,
            m_inside_obj_name,
            WpkCompilatorStrings::wavepack_namespace_name_with_space_upper_str);

        if (m_inside_obj_type == WpkCompilatorStrings::wavepack_namespace_name_upper_str)
        {
            DEBUG_PRINT("ITS WAVEPACK!!!\n");

            CompileObject<WavePack>(uppercased_instruction,
                                    original_instruction,
                                    this->m_single_wavepack,
                                    this->r_wavepacks );
        }
    }

};

#endif // !TXT_TO_WPK_OLD_COMPILATOR_HPP




