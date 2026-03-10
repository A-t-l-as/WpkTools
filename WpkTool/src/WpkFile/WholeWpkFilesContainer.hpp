#ifndef WHOLE_WPK_FILES_CONTAINER_HPP
#define WHOLE_WPK_FILES_CONTAINER_HPP

#include "Utils/Formats.hpp"
#include "WpkFile.hpp"

class WholeWpkFilesContainer
{
public:
    void SaveToDirectory(const std::filesystem::path& arg_dir_path)
    {
        CONSOLE_OUT.PrintLn(Mess::endl,
                            "Created wavepack output files:", Mess::endl,
                            "------------------------------");

        size_t i = 1;
        for (const WavePack& single_wavepack : m_wavepacks)
        {
            const std::filesystem::path output_bin_file_path =
                arg_dir_path / (single_wavepack.GetWavePackName() + Formats::wpk_format_str_with_dot);

            CONSOLE_OUT.PrintLn(i, ". ", output_bin_file_path.string());

            WpkFile(single_wavepack).SaveTo(output_bin_file_path);

            ++i;
        }

    }

    std::vector<WavePack>& GetWavePacksRef()
    {
        return this->m_wavepacks;
    }

    std::vector<WpkFile> GetWpkFiles() const
    {
        std::vector<WpkFile> result;

        for(const WavePack& single_wavepack : m_wavepacks)
            result.push_back( WpkFile(single_wavepack) );

        return result;
    }

private:
    std::vector<WavePack> m_wavepacks = {};


};


#endif // !WHOLE_WPK_FILES_CONTAINER_HPP
