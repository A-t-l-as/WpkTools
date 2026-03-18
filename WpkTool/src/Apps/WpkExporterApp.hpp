#ifndef WPK_EXPORTER_APP_HPP
#define WPK_EXPORTER_APP_HPP

#include <string>
#include <filesystem>
#include "Console/Console.hpp"
#include "System/IApp.hpp"
#include "Utils/Formats.hpp"
#include "../WpkFile/WpkFile.hpp"


class WpkExporterApp : public IApp
{
public:
    constexpr const unsigned int static c_correct_number_of_args = 4;

    WpkExporterApp()
    {}

    int Run(int my_argc, char* my_argv[]) override
    {
        std::filesystem::path input_file_path;
        std::string output_format = {};

        if (my_argc == c_correct_number_of_args)
        {
            input_file_path = my_argv[2];
            output_format = my_argv[3];
            ArgcAndArgvExecution( my_argv[0], output_format );
        }
        else
        {
            StandardProgramExecution(input_file_path, output_format);
        }

        if (input_file_path.extension().string() != Formats::wpk_format_str_with_dot)
        {
            CONSOLE_OUT.PrintLn("The specified file is not a .wpk file. The program terminates with an error.");
            exit(EXIT_FAILURE);
        }

        if(!std::filesystem::exists(input_file_path))
        {
            CONSOLE_OUT.PrintLn("The file with the path ", input_file_path.string(), " does not exist!");
            exit(EXIT_FAILURE);
        }

        WpkFile input_wpk_file;

        try
        {
            input_wpk_file.LoadFrom(input_file_path);
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

        DEBUG_PRINT("wavepack_name: ", input_wpk_file.GetWavePackName(), Mess::endl);

        CONSOLE_OUT.PrintLn("Input file path: ", input_file_path.string());

        const std::filesystem::path output_file_path =
            input_file_path.parent_path() / (input_wpk_file.GetWavePackName() + "." + output_format);

        DEBUG_PRINT("ParentPath: ", input_file_path.parent_path().string(), Mess::endl);

        CONSOLE_OUT.PrintLn("Output file path: ", output_file_path.string());

        input_wpk_file.ExportTo(output_file_path, output_format);

        CONSOLE_OUT.PrintLn("DONE!");

        return EXIT_SUCCESS;
    }



private:

    void StandardProgramExecution(std::filesystem::path& input_file_path, std::string& output_format)
    {
        const std::vector<std::string> correct_formats = { Formats::txt_format_str, Formats::cpp_format_str };

        CONSOLE_OUT.PrintLn("Enter the name of the input file:");
        CONSOLE_IN.Scan(input_file_path);

        CONSOLE_OUT.PrintLn("Choose format [", Formats::txt_format_str, "/", Formats::cpp_format_str, "]:");

        do
        {
            CONSOLE_IN.Scan(output_format);

            if (!Misc::CheckThatElementExistInVec(output_format, correct_formats))
            {
                CONSOLE_OUT.PrintLn("Choose from the given formats...");
            }

        } while (!Misc::CheckThatElementExistInVec(output_format, correct_formats));
    }


    void ArgcAndArgvExecution(const std::string& program_name, std::string& output_format)
    {
        const std::vector<std::string> correct_formats =
        {
            Formats::txt_format_str,
            '-' + std::string(Formats::txt_format_str),
            Formats::cpp_format_str,
            '-' + std::string(Formats::cpp_format_str)
        };

        if (!Misc::CheckThatElementExistInVec(output_format, correct_formats))
        {
            CONSOLE_OUT.PrintLn("You have entered an incorrect output format.", Mess::endl,
                                "Use: ", program_name, "-e <wpk_file> <",
                                correct_formats[0], ",", correct_formats[1], ",",
                                correct_formats[2], ",", correct_formats[3], ">");

            exit(EXIT_FAILURE);
        }

        if (output_format == correct_formats[1])
        {
            output_format = Formats::txt_format_str;
        }

        if (output_format == correct_formats[3])
        {
            output_format = Formats::cpp_format_str;
        }
    }



};


#endif // WPKEXPORTERAPP_HPP
