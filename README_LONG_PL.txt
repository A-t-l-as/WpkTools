Do czego służy ta paczka programów?
***********************************
Programy w tej paczce służą do dekompilacji i kompilacji plików .wpk. WpkExporter jest następcą programu WPKtoTXT (napisanego przez Atlasa), który eksportował pliki .wpk do postaci .txt. 
WpkExporter różni się tym, że wprowadza obsługę innych wersji .wpk z innych gier studia Reality Pump. Program umożliwia również eskport do formatu .cpp.

WpkImporter natomiast jest następcą programu Wpconvert.exe stworzonego przez Reality Pump dołączanego do Earth 2160 sdk. 
WpkImporter został został napisany w taki sposób by naśladował zachowanie Wpconvert jednak dodaje nową funckję kompilacji plików cpp.

-> WpkImporter i WpkExporter zostały napisane w 100% przez Atlasa.

WpkExporter
***********

Ten program służy do konwersjii plików .wpk czyli wavepack do .txt lub .cpp.
Włączamy program i wpisujemy nazwę pliku .wpk, następnie program zapyta się jaki ma być format wyjściowy.
Wybieramy txt lub cpp wpisując jeden z tych formatów i klikamy enter. Program stworzy gotowy plik cpp lub txt.

Program obsługuje pliki .wpk z następujących gier:
-> Earth 2150: Escape from the Blue Planet,
-> Earth 2150: The Moon Project,
-> Earth 2150: Lost Souls,
-> Heli Heroes,
-> World War II: Panzer Claws/Frontline Attack: War Over Europe,
-> World War III: Black Gold,
-> KnightShift/Polanie 2/Once Upon a Knight,
-> KnightShift II Curse of Souls/Polanie 3,
-> Earth 2160,

Program działa również w trybie ARGC&ARGV:

cmd:
WpkExporter.exe <ścieżka_do_pliku_wpk> <cpp/txt>

powershell:
.\WpkExporter.exe <ścieżka_do_pliku_wpk> <cpp/txt>

Przykład:
WpkExporter.exe D:\folder\plik.wpk cpp

-> Możemy od razu zdekompilować wszystkie pliki .wpk w katalogu do .txt za pomocą pliku: _EXPORT_ALL_TO_TXT.bat lub _EXPORT_ALL_TO_TXT.ps1
-> Możemy też od razu zdekompilować wszystkie pliki .wpk w katalogu do .cpp za pomocą pliku: _EXPORT_ALL_TO_CPP.bat lub _EXPORT_ALL_TO_CPP.ps1

----------------------------------------------------------------------------------------------------------------------------

Edycja plików .txt lub .cpp
***************************
W zależności od gry, z której mamy plik .wpk otrzymamy troche inną strukturę wyjściową pliku cpp lub txt.

Edycja txt:
***********

Przykład txt dla KnightShift:

wavepack TALK_AMAZONE_COMMAND
{
	Level0 9
	{
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_2.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_3.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_4.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_5.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_6.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_7.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_8.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_9.wav
	}
	Level1 9
	{
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_2.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_3.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_4.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_5.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_6.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_7.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_8.wav
		Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_9.wav
	}
}

Przykład txt dla ww3 black gold oraz heli heroes:

wavepack RUS_M07
{
	Type 0 <------------------------ jeżeli chcemy zachować ten format wpk dla tej gry
	DisableLevel2 1 <--------------- to tych dwóch wartości nie zmieniamy,
	Level0 1
	{
		Wave 100 MESSAGE\RUS\RUS_M07.WAV
        ... <--- tutaj sobie dopisujemy wpisy Wave
	}
    Level1 ... <---- możemy stworzyć dodatkowy level Wavów
    {
        ...
    }
}

-> Instrukcja Type oznacza typ wavepacka.
-> DisableLevel2 to instrukcja służąca do zablokowania Level2. Czyli to taka instrukcja, która umożliwia utworzenie formatu .wpk
dla gry Earth 2150.
-> Jeżeli chcemy zachować format z danej gry, to tych dwóch instrukcji nie dotykamy.
-> Wavepacki najlepiej jest edytować na przykładzie jakiegoś innego wavepacka.
-> W jednym pliku txt można zdefiniować wiele wavepacków co ukazuje plik Sample.txt w katalogu z WpkImporterem.
Zdefiniowanie wielu wavepacków w jednym pliku txt tworzy wiele plików wpk i jest szybsze niż uruchamianie każdego pliku txt
osobno i kompilowanie go.
-> Minusem pliku txt jest to, że trzeba mocno trzymać się składni.
-> Jeżeli użyto wcięcia przez tab to też używamy wcięcia przez tab.
-> Staramy się tworzyć pliki na wzór i trzymać estetykę.
-> Ilość wavów w danym Levelu powinna się zgadzać z podaną ilością obok instrukcji Level.
-> Program ma wbudowany debugger, który wyrzuca errory co należy poprawić. Jednak ten debugger wszystkiego nie przewidzi, więc po
prostu nie róbcie błędów.
-> Najlepiej jest używać jakiegoś edytora tekstu z możliwością zobaczenia wszystkich znaków. Przykładem takiego programu jest Notepad++
i tam jest opcja Show All Characters.

Edycja cpp
**********

Przykład pliku cpp:

uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace ww3_black_gold_test
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\RUS_M07.WAV"; };
	};
};

-> Plik cpp charakteryzuje się tym, że mamy możliwość edycji dosłownie każdej wartości w pliku.
-> Plik cpp może przechowywać tylko jeden wavepack.
-> Jeżeli chcemy edytować dany wavepack z danej gry i zachować jego format tak aby działał w danej grze to
edytujemy tylko struct Level oraz kopiujemy linijkę struct Wave{...} i wklejamy niżej.
-> Wszystkie dane powyżej namespace to takie, których nie trzeba edytować.
-> 32bituniquekey c_1 to typ wartości uint32_t, która może przyjąć stałe: NULL czyli 0, albo DELAYED czyli 1.

Przykład prawidłowo zedytowanego pliku cpp:

uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;
uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace ww3_black_gold_test
{
	struct Level_0
	{
		struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\RUS_M07.WAV"; };
        struct Wave{ uint32_t a_0 = 95;	    string wave_path = "MESSAGE\RUS\RUS_M08.WAV"; };
        struct Wave{ uint32_t a_0 = 10;	    string wave_path = "MESSAGE\RUS\RUS_M09.WAV"; };
	};

    struct Level_1
    {
        struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\test_M01.WAV"; };
        struct Wave{ uint32_t a_0 = 2;	    string wave_path = "MESSAGE\RUS\test_M02.WAV"; };
        struct Wave{ uint32_t a_0 = 34;	    string wave_path = "MESSAGE\RUS\test_M03.WAV"; };
        struct Wave{ uint32_t a_0 = 55;	    string wave_path = "MESSAGE\RUS\test_M04.WAV"; };
        struct Wave{ uint32_t a_0 = 100;	string wave_path = "MESSAGE\RUS\test_M05.WAV"; };
    };

};

-> Jak widzimy przy struct Level nie wpisuje ilości wavów, ponieważ WpkImporter sam policzy podczas kompilacji ilość wavów i wpisze
w pole number_of_waves_in_level_<index>. Program wykrywa funkcje count() oraz measure_all_value_sizes_after_this_instruction() przez co nie musimy się
martwić o liczenie wavów.


----------------------------------------------------------------------------------------------------------------------------

WpkImporter
***********

Ten program służy do konwersji plików .txt i .cpp na .wpk czyli wavepack.
Po uruchomieniu programu zapyta nas o nazwę pliku wejściowego. Wpisujemy nazwę pliku o formacie txt lub cpp i klikamy enter.
Program stworzy gotowy plik lub pliki .wpk.

Edycja struktury txt jest prosta. Wystarczy przejrzeć plik Sample.txt.
Przy napisie level0 jest liczba i ona oznacza liczbę wpisów wave na tym poziomie.
Czyli jeżeli będę mieć 9 wpisów o wave to muszę wpisać 9. Jeżeli nic nie mam na level1 to mogę go kompletnie usunąć
ale level0 musi być zawsze.

W jednym pliku txt może być wiele wavepacków, natomiast w jednym pliku cpp może być tylko jeden wavepack.

-> Program kompiluje wszystkie pliki .txt w katalogu przez plik _CONVERT_ALL_TXT.bat lub _CONVERT_ALL_TXT.ps1.
-> Program kompiluje wszystkie pliki .cpp w katalogu przez plik _CONVERT_ALL_CPP.bat lub _CONVERT_ALL_CPP.ps1.

Program działa również w trybie ARGC&ARGV:

cmd:
WpkImporter.exe <ścieżka_do_pliku_txt_lub_cpp>

powershell:
.\WpkImporter.exe <ścieżka_do_pliku_txt_lub_cpp>

Przykład:
WpkImporter.exe D:\folder\all_wpks.txt

----------------------------------------------------------------------------------------------------------------------------

Programy były testowane na:
***************************
-> 566 plikach .wpk (czyli wszystkich) z gry Earth 2150: Escape from the Blue Planet - wynik testu 100%,
-> 289 plikach .wpk (czyli wszystkich) z gry Earth 2150: The Moon Project - wynik testu 100%,
-> 289 plikach .wpk (czyli wszystkich) z gry Earth 2150: Lost Souls - wynik testu 100%,
-> 70 plikach .wpk (czyli wszystkich) z gry Heli Heroes - wynik testu 100%,
-> 218 plikach .wpk (czyli wszystkich) z gry World War II: Panzer Claws/Frontline Attack: War Over Europe - wynik testu 100%,
-> 261 plikach .wpk (czyli wszystkich) z gry World War III: Black Gold - wynik testu 100%,
-> 424 plikach .wpk (czyli wszystkich) z gry KnightShift/Polanie 2/Once Upon a Knight - wynik testu 100%,
-> 1065 plikach .wpk (czyli wszystkich) z gry KnightShift II Curse of Souls/Polanie 3 - wynik testu 100%,
-> 961 plikach .wpk (czyli wszystkich) z gry Earth 2160 - wynik testu 100%,

Programy zostały stworzone przez Atlasa.

