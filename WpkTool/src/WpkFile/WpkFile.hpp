#ifndef WPK_FILE_HPP
#define WPK_FILE_HPP

#include <stdexcept>
#include "File/BinFile.hpp"
#include "File/TextFile.hpp"
#include "WavePack.hpp"
#include "Utils/Formats.hpp"
#include "../WpkCompilator/WpkCompilator.hpp"
#include "../WpkCompilator/WpkHashMaps.hpp"
#include "WavePack.hpp"


class WpkFile
{
public:

    WpkFile() :
        m_wavepack( WavePack() ),
        m_error_handler(Globals::success_code)
    {}


    WpkFile(const WavePack& arg_wavepack) :
        m_wavepack(arg_wavepack),
        m_error_handler(Globals::success_code)
    {}

    void ParseFromCpp(const std::filesystem::path& arg_input_file_path)
    {
        if(!std::filesystem::exists(arg_input_file_path))
            throw std::invalid_argument("The file with the path "
                                        + arg_input_file_path.string() + " does not exist!");


        this->m_wavepack = WavePack( arg_input_file_path.stem().string()  );

        DEBUG_PRINT("wavepack_name: ", GetWavePackName(), Mess::endl);

        BinFile bin_file;

        WpkHashMaps wpk_hash_maps;
        wpk_hash_maps.Init();

        WpkCompilator(bin_file, wpk_hash_maps, this->m_error_handler).CompileFile(arg_input_file_path);

        if(this->m_error_handler == Globals::success_code)
        {
            try
            {
                this->m_wavepack.ReadFrom(bin_file);
            }
            catch(const std::out_of_range& e_out_of_range)
            {
                throw std::runtime_error
                    ("The parsed " + arg_input_file_path.string()
                                         + " file was empty or contained invalid information!");
            }

            this->m_wavepack.ShowInformations();
        }
    }

    void ParseFromJson(const std::filesystem::path& arg_input_file_path)
    {
        if(!std::filesystem::exists(arg_input_file_path))
            throw std::invalid_argument("The file with the path "
                                        + arg_input_file_path.string() + " does not exist!");

        if(!std::filesystem::is_regular_file(arg_input_file_path))
            throw std::invalid_argument("The path "
                                        + arg_input_file_path.string() + " is not a regular file!");


        this->m_wavepack = WavePack( arg_input_file_path.stem().string()  );
        DEBUG_PRINT("wavepack_name: ", GetWavePackName(), Mess::endl);


        std::ifstream input_file(arg_input_file_path);
        if(!input_file)
            throw std::runtime_error("Unable to open the file: " + arg_input_file_path.string());

        ordered_json j;
        input_file >> j;

        this->m_wavepack = j.get<WavePack>();
        this->m_wavepack.ShowInformations();
    }

    void LoadFrom(const std::filesystem::path& arg_input_file_path)
    {
        if(!std::filesystem::exists(arg_input_file_path))
            throw std::invalid_argument("The file with the path "
                                        + arg_input_file_path.string() + " does not exist!");


        this->m_wavepack = WavePack(  arg_input_file_path.stem().string()  );

        BinFile input_wpk_bin_file;
        input_wpk_bin_file.LoadFrom(arg_input_file_path);

        try
        {
            this->m_wavepack.ReadFrom(input_wpk_bin_file);
        }
        catch(const std::out_of_range& e_out_of_range)
        {
            throw std::runtime_error
                ("The loaded " + arg_input_file_path.string()
                                     + " file is empty or contains invalid information!");
        }

        this->m_wavepack.ShowInformations();
    }

    void SaveTo(const std::filesystem::path& arg_output_file_path)
    {
        if(this->m_error_handler == Globals::success_code)
        {
            BinFile output_bin_file;
            this->m_wavepack.WriteTo(output_bin_file);
            output_bin_file.SaveTo(arg_output_file_path);
        }
    }

    void ExportTo( const std::filesystem::path& arg_output_file_path,  const std::string& output_file_format)
    {
        TextFile output_text_file;

        if (output_file_format == Formats::txt_format_str)
        {
            output_text_file << m_wavepack.ToOldFormatString();
        }

        if (output_file_format == Formats::cpp_format_str)
        {
            output_text_file << m_wavepack.ToNewFormatString();
        }

        if (output_file_format == Formats::json_format_str)
        {
            ordered_json wavepack_in_json = this->m_wavepack;

            output_text_file << wavepack_in_json.dump(4);
        }

        output_text_file.SaveOnlyStringStream(arg_output_file_path);
    }


    std::string GetWavePackName() const { return this->m_wavepack.GetWavePackName(); }

    bool GetErrorHandler() const { return this->m_error_handler; }

private:
    WavePack m_wavepack;
    bool m_error_handler;
};


#endif // WPK_FILE_HPP
