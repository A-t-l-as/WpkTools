#ifndef WPK_COMPILATOR_H
#define WPK_COMPILATOR_H

#include "Compilator/CompilatorBase.hpp"
#include "Compilator/CompilatorStrings.hpp"
#include "File/TextFile.hpp"
#include "WpkCompilatorStrings.hpp"
#include "Utils/StringUtils.hpp"

class WpkCompilator : public CompilatorBase
{
public:							
    WpkCompilator(
                    BinFile& arg_output_file_buffer,
                    HashMapsBase& arg_hash_maps,
                    bool& arg_error_handler
                ) :

        CompilatorBase(arg_output_file_buffer, arg_hash_maps, arg_error_handler, true),

        m_measure_error_signal(false),
        m_data_size_offset(0),
        m_actual_level_index(-1),

        temp_name_uppercased(std::string())
	{}
	
	void CompileFile(const std::filesystem::path& arg_input_path) override
	{

        TextFile input_file;
        input_file.LoadFrom(arg_input_path);

		input_file.Informations();
		//input_file.ExitWhenFileDontExist();
        this->r_error_handler = !(input_file.GetIsOpened());
	
        if ((input_file.Empty() == false) && (this->r_error_handler == Globals::success_code))
		{
            input_file.DeleteComments();

            input_file.CleanCppFileBufferFromContrChars();

			CompileCppFileToBin(input_file.GetCleanFileBuffer());


            //Poprawka dla measure_all_value_sizes_after_this_instruction();
            if (this->m_measure_error_signal == true)
            {
                uint32_t data_size =
                static_cast<uint32_t>
                (this->r_output_file_buffer.GetFileLength() - (this->m_data_size_offset + sizeof(data_size)));

                CONSOLE_OUT.PrintLn(data_size);

                this->r_output_file_buffer.ChangeValue(m_data_size_offset, data_size);

                this->m_measure_error_signal = false;
            }

            for (int j = 0; j < this->c_number_of_levels; ++j)
            {
                if (this->m_arr_of_signals_for_levels[j] == true)
                {
                    this->r_output_file_buffer.ChangeValue
                        ( m_arr_of_offsets_for_number_of_waves[j], m_arr_of_number_of_waves[j]);

                    this->m_arr_of_signals_for_levels[j] = false;
                }
            }

		}
	}


private:
    bool m_measure_error_signal;
    size_t m_data_size_offset;

    constexpr const unsigned int static c_number_of_levels = 3;
    bool m_arr_of_signals_for_levels[c_number_of_levels]            = { 0 };
    size_t m_arr_of_offsets_for_number_of_waves[c_number_of_levels] = { 0 };
    uint32_t m_arr_of_number_of_waves[c_number_of_levels]           = { 0 };

    int32_t m_actual_level_index;

    std::string temp_name_uppercased;

protected:

    bool TryResolveSpecialValue
    (
        std::string& arg_instruction,
        std::string& arg_value_type,
        Enums::ENCompilatorValueTypes& arg_en_value_type,
        std::string& arg_name,
        std::string& arg_value
    )
    override
    {
        // measure_all_value_sizes_after_this_instruction();
        if (
            arg_en_value_type == Enums::ENCompilatorValueTypes::e_uint32_t
            &&
            arg_name == "data_size"
            &&
            arg_value == WpkCompilatorStrings::measure_all_value_sizes_after_this_instruction_fn_name_str
            )
        {
            arg_value = "0";
            this->m_measure_error_signal = true;
            this->m_data_size_offset = this->r_output_file_buffer.GetFileLength();
            return true;
        }

        for (int i = 0 ; i < this->c_number_of_levels ; ++i)
        {
            if (
                arg_en_value_type == Enums::ENCompilatorValueTypes::e_uint32_t
                &&
                arg_name == ("number_of_waves_in_level_" + std::to_string(i))
                &&
                arg_value == CompilatorStrings::count_fn_str
                )
            {
                arg_value = "0";
                this->m_arr_of_signals_for_levels[i] = true;
                this->m_arr_of_offsets_for_number_of_waves[i] = this->r_output_file_buffer.GetFileLength();
                return true;
            }
        }

        return false;
    }


    bool SpecialAction0(
        std::string& arg_instruction,
        std::string& arg_value_type,
        Enums::ENCompilatorValueTypes& arg_en_value_type,
        std::string& arg_name,
        std::string& arg_value
        ) override
    {

        // SLOT 0 : --------------------
        // {

        temp_name_uppercased = StringUtils::GetToUpper(arg_name);

        for (int i = 0; i < this->c_number_of_levels; ++i)
        {
            const std::string temp_level_string =
                std::string(WpkCompilatorStrings::level_struct_name_upper_str) + '_' + std::to_string(i);

            if (
                arg_en_value_type == Enums::ENCompilatorValueTypes::e_struct
                &&
                temp_name_uppercased.starts_with(temp_level_string)
                )
            {
                DEBUG_PRINT("LEVEL ");
                DEBUG_PRINT(i);
                DEBUG_PRINT(" DETECTED!\n");

                this->m_actual_level_index = i;
            }
        }

        // }
        // ---------------------------

        return false;
    }

    bool SpecialAction1(
        std::string& arg_instruction,
        std::string& arg_value_type,
        Enums::ENCompilatorValueTypes& arg_en_value_type,
        std::string& arg_name,
        std::string& arg_value
        ) override
    {
        // SLOT 1 : ------------------------------

        DEBUG_PRINT("TEMP UPPERCASED = |"); DEBUG_PRINT(temp_name_uppercased);  DEBUG_PRINT("\n");
        DEBUG_PRINT("::wave_struct_name = |");
        DEBUG_PRINT(WpkCompilatorStrings::wave_struct_name_upper_str); DEBUG_PRINT("|\n");

        if (temp_name_uppercased.starts_with(WpkCompilatorStrings::wave_struct_name_upper_str) )
        {
            ++this->m_arr_of_number_of_waves[this->m_actual_level_index];
        }

        temp_name_uppercased = std::string();

        // ----------------------------------------

        return false;
    }

};



#endif // !WPK_COMPILATOR_H
