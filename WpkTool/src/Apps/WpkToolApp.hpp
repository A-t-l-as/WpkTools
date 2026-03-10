#ifndef WPK_TOOLAPP_HPP
#define WPK_TOOLAPP_HPP

#include "System/HybridApp.hpp"
#include "WpkExporterApp.hpp"
#include "WpkImporterApp.hpp"

class WpkToolApp : public HybridApp
{
public:
    WpkToolApp()
    {}

protected:

    void ShowStartScreen() override
    {
        CONSOLE_OUT.PrintLn("******************", Mess::endl,
                            "|WpkTool by ATLAS|", Mess::endl,
                            "********2026******");
    }

    void RunExporter(int my_argc, char* my_argv[]) override
    {
        WpkExporterApp().Run(my_argc, my_argv);
    }

    void RunImporter(int my_argc, char* my_argv[]) override
    {
        WpkImporterApp().Run(my_argc, my_argv);
    }

};


#endif // !WPK_TOOLAPP_HPP
