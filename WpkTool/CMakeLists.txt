cmake_minimum_required(VERSION 3.16)

project(WpkTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(WpkTool
    src/main.cpp
    src/Apps/WpkExporterApp.hpp src/Apps/WpkImporterApp.hpp src/Apps/WpkToolApp.hpp src/WpkCompilator/TxtToWpkOldCompilator.hpp   src/WpkCompilator/WpkCompilator.hpp src/WpkCompilator/WpkCompilatorGlobals.hpp src/WpkCompilator/WpkCompilatorStrings.hpp src/WpkCompilator/WpkHashMaps.hpp src/WpkFile/Wave.cpp src/WpkFile/Wave.hpp src/WpkFile/WavePack.cpp src/WpkFile/WavePack.hpp src/WpkFile/WholeWpkFilesContainer.hpp src/WpkFile/WpkFile.hpp
    src/Other/json.hpp
    src/WpkCompilator/WpkJsonStrings.hpp
    src/WpkFile/WaveBase.hpp
    src/WpkFile/WavePackBase.hpp


)

target_include_directories(WpkTool PRIVATE
    ../KsUtils
    ../KsUtils/src
)

target_link_libraries(WpkTool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../KsUtils/build/Desktop_Qt_6_9_3_MinGW_64_bit-Release/libKsUtils.a
)

include(GNUInstallDirs)
install(TARGETS WpkTool
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
