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

    int Run(int argc, char* argv[]) override
    {
        std::filesystem::path input_file_path = ResolveInputPath(argc, argv);
        const Formats::ENFormat file_type = ClassifyExtension(input_file_path);

        if (file_type == Formats::ENFormat::invalid)
        {
            CONSOLE_OUT.PrintLn(
                "The specified input file is not a .cpp, .txt or .json file. The program terminates with an error.");
            exit(EXIT_FAILURE);
        }

        if (!std::filesystem::exists(input_file_path))
        {
            CONSOLE_OUT.PrintLn("The file with the path ", input_file_path.string(), " does not exist!");
            exit(EXIT_FAILURE);
        }

        bool success = file_type == Formats::ENFormat::txt
                           ? CompileTxt(input_file_path)
                           : CompileCppOrJson(input_file_path, file_type);

        ReportResult(input_file_path, success);
        return EXIT_SUCCESS;
    }


private:

    std::filesystem::path ResolveInputPath(int argc, char* argv[])
    {
        if (argc == c_correct_number_of_args)
        {
            std::filesystem::path path = argv[2];
            CONSOLE_OUT.PrintLn("FileName = ", path);
            return path;
        }

        std::filesystem::path path;
        CONSOLE_OUT.Print("Enter the name of the input file:");
        CONSOLE_IN.Scan(path);
        return path;
    }

    Formats::ENFormat ClassifyExtension(const std::filesystem::path& path)
    {
        const std::string ext = path.extension().string();
        if (ext == Formats::cpp_format_str_with_dot)  return Formats::ENFormat::cpp;
        if (ext == Formats::txt_format_str_with_dot)  return Formats::ENFormat::txt;
        if (ext == Formats::json_format_str_with_dot) return Formats::ENFormat::json;
        return Formats::ENFormat::invalid;
    }

    bool CompileCppOrJson(const std::filesystem::path& input_file_path, Formats::ENFormat type)
    {
        WpkFile output_wpk_file;
        try
        {
            if (type == Formats::ENFormat::cpp)
                output_wpk_file.ParseFromCpp(input_file_path);
            else // Json
                output_wpk_file.ParseFromJson(input_file_path);
        }
        catch (const std::invalid_argument& e)
        {
            CONSOLE_OUT.PrintLn(e.what());
            exit(EXIT_FAILURE);
        }
        catch (const std::runtime_error& e)
        {
            CONSOLE_OUT.PrintLn(e.what());
            exit(EXIT_FAILURE);
        }

        const bool success = output_wpk_file.GetErrorHandler();

        const std::filesystem::path output_bin_file_path =
            input_file_path.parent_path()
            /
            (output_wpk_file.GetWavePackName() + Formats::wpk_format_str_with_dot);

        DEBUG_PRINT("ParentPath: ", input_file_path.parent_path().string(), Mess::endl);
        DEBUG_PRINT("output_file_path: ", output_bin_file_path.string(), Mess::endl);
        CONSOLE_OUT.PrintLn("Output wavepack file:", output_bin_file_path.string());

        output_wpk_file.SaveTo(output_bin_file_path);
        return success;
    }


    bool CompileTxt(const std::filesystem::path& input_file_path)
    {
        WholeWpkFilesContainer container;
        bool success = Globals::success_code;

        TxtToWpkOldCompilator(container, success).CompileFile(input_file_path);

        if (success == Globals::success_code)
            container.SaveToDirectory(input_file_path.parent_path());

        return success;
    }

    void ReportResult(const std::filesystem::path& input_file_path, bool success)
    {
        if (success == Globals::success_code)
        {
            CONSOLE_OUT.PrintLn(Mess::endl, "The ", input_file_path.string(), " was compiled successfully...");
        }
        else
        {
            CONSOLE_OUT.PrintLn("An error occurred during compilation ", input_file_path,
                                ". The compiler has been terminated.");
            exit(EXIT_FAILURE);
        }
    }

};

#endif // !WPK_IMPORTER_APP_HPP
