#ifndef WPK_IMPORTER_APP_HPP
#define WPK_IMPORTER_APP_HPP

#include <string>
#include <filesystem>
#include "Console/Console.hpp"
#include "Utils/Formats.hpp"

#include "../WpkCompilator/TxtToWpkOldCompilator.hpp"

#include "../WpkFile/WpkFile.hpp"

#include "System/IApp.hpp"


class WpkImporterApp : public IApp
{
public:

    constexpr const unsigned int static c_correct_number_of_args = 3;

    WpkImporterApp()
    {}

    int Run(int my_argc, char* my_argv[]) override
    {
        std::filesystem::path input_file_path;

        if (my_argc == c_correct_number_of_args)
        {
            input_file_path = my_argv[2];
            CONSOLE_OUT.PrintLn("FileName = ", input_file_path);
        }
        else
        {
            StandardProgramExecution(input_file_path);
        }

        const std::string input_file_extension_with_dot = input_file_path.extension().string(); // rozszerzenie pliku z kropka

        if (
            input_file_extension_with_dot != Formats::cpp_format_str_with_dot
            &&
            input_file_extension_with_dot != Formats::txt_format_str_with_dot
           )
        {
            CONSOLE_OUT.PrintLn("The specified input file is not a .cpp file or a .txt file. The program terminates with an error.");
            exit(EXIT_FAILURE);
        }

        if(!std::filesystem::exists(input_file_path))
        {
            CONSOLE_OUT.PrintLn("The file with the path ", input_file_path.string(), " does not exist!");
            exit(EXIT_FAILURE);
        }

        bool error_handler = Globals::success_code;

        if (input_file_extension_with_dot == Formats::cpp_format_str_with_dot)
        {
            WpkFile output_wpk_file;

            try
            {
                output_wpk_file.ParseFrom(input_file_path);
            }
            catch(const std::invalid_argument& e)
            {
                CONSOLE_OUT.PrintLn(e.what());
                exit(EXIT_FAILURE);
            }
            catch(const std::runtime_error& r_e)
            {
                CONSOLE_OUT.PrintLn(r_e.what());
                exit(EXIT_FAILURE);
            }

            error_handler = output_wpk_file.GetErrorHandler();

            const std::filesystem::path output_bin_file_path =
                input_file_path.parent_path() / ( output_wpk_file.GetWavePackName() + Formats::wpk_format_str_with_dot );

            DEBUG_PRINT("ParentPath: ", input_file_path.parent_path().string(), Mess::endl);
            DEBUG_PRINT("output_file_path: ", output_bin_file_path.string(), Mess::endl);

            CONSOLE_OUT.PrintLn("Output wavepack file:", output_bin_file_path.string());
            output_wpk_file.SaveTo(output_bin_file_path);
        }

        if (input_file_extension_with_dot == Formats::txt_format_str_with_dot)
        {
            WholeWpkFilesContainer whole_wpk_files_container;

            TxtToWpkOldCompilator(whole_wpk_files_container, error_handler).CompileFile(input_file_path);

            if(error_handler == Globals::success_code)
            {
                whole_wpk_files_container.SaveToDirectory( input_file_path.parent_path() );
            }
        }

        if(error_handler == Globals::success_code)
        {
            CONSOLE_OUT.PrintLn(Mess::endl, "The ", input_file_path.string(), " was compiled successfully...");
        }
        else
        {
            CONSOLE_OUT.PrintLn("An error occurred during compilation ", input_file_path, ". The compiler has been terminated.");
            exit(EXIT_FAILURE);
        }

        return EXIT_SUCCESS;
    }


private:

    void StandardProgramExecution(std::filesystem::path& input_file_path)
    {
        CONSOLE_OUT.Print("Enter the name of the input file:"); CONSOLE_IN.Scan(input_file_path);
    }

};

#endif // !WPK_IMPORTER_APP_HPP
