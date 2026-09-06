# WpkTools

**WpkTool** - narzędzie do dekompilacji i kompilacji plików `.wpk` (wavepack) z gier studia Reality Pump.

Wersja: **2.0.0**

---

## PL Polski

### O programie

Od wersji 2.0.0 dawne osobne programy **WpkExporter** i **WpkImporter** zostały połączone w jeden plik wykonywalny - **`WpkTool.exe`**. Tryb pracy (eksport / import) wybiera się teraz za pomocą przełączników uruchomieniowych, a nie osobnych programów.

Kompilacja programu wymaga skompilowanej statycznej biblioteki `KsUtils`:
[Link do LibKsUtils](https://github.com/A-t-l-as/LibKsUtils)

Program obsługuje pliki `.wpk` z następujących gier:
- Earth 2150: Escape from the Blue Planet
- Earth 2150: The Moon Project
- Earth 2150: Lost Souls
- Heli Heroes
- World War II: Panzer Claws / Frontline Attack: War Over Europe
- World War III: Black Gold
- KnightShift / Polanie 2 / Once Upon a Knight
- KnightShift II Curse of Souls / Polanie 3
- Earth 2160

WpkTool został napisany w 100% przez Atlasa.

### Formaty

Program obsługuje trzy formaty pośrednie, zarówno przy eksporcie, jak i przy imporcie:

| Format | Opis |
| --- | --- |
| `txt` | Format tekstowy, znany z dawnego WPKtoTXT/WpkExportera. Może zawierać wiele wavepacków w jednym pliku. |
| `cpp` | Format w stylu C++, pozwala edytować dosłownie każdą wartość. Jeden plik = jeden wavepack. |
| `json` | **Nowość w 2.0.0.** Ustrukturyzowany format czytelny dla człowieka i łatwy do edycji w dowolnym edytorze tekstu bez pilnowania wcięć/składni jak w `.txt`. |

### Użycie — przełączniki

Program uruchamiany bez argumentów albo z nieprawidłowymi argumentami przechodzi w **tryb interaktywny** - sam odpytuje w konsoli o kolejne parametry i opcje (nazwę pliku, format itd.).

**Eksport** (`.wpk` -> `txt`/`cpp`/`json`):

```
WpkTool.exe -e <ścieżka_do_pliku_wpk> <txt/cpp/json>
```

Przykład:

```
WpkTool.exe -e D:\folder\plik.wpk json
```

**Import / kompilacja** (`txt`/`cpp`/`json` -> `.wpk`):

```
WpkTool.exe -c <ścieżka_do_pliku_źródłowego>
```

Format pliku źródłowego (w tym `.json`) jest rozpoznawany automatycznie na podstawie rozszerzenia pliku — nie trzeba go podawać osobno.

Przykład:

```
WpkTool.exe -c D:\folder\all_wpks.txt
WpkTool.exe -c D:\folder\SND_HIT_STONEEXPLODE.json
```

### Masowa konwersja katalogu

W katalogu z programem znajdują się gotowe skrypty do konwersji wszystkich plików danego typu w folderze:

**Eksport:**
- `_EXPORT_ALL_TO_TXT.bat` / `_EXPORT_ALL_TO_TXT.ps1`
- `_EXPORT_ALL_TO_CPP.bat` / `_EXPORT_ALL_TO_CPP.ps1`
- `_EXPORT_ALL_TO_JSON.bat` / `_EXPORT_ALL_TO_JSON.ps1` *(nowość w 2.0.0)*

**Import:**
- `_CONVERT_ALL_TXT.bat` / `_CONVERT_ALL_TXT.ps1`
- `_CONVERT_ALL_CPP.bat` / `_CONVERT_ALL_CPP.ps1`
- `_CONVERT_ALL_JSON.bat` / `_CONVERT_ALL_JSON.ps1` *(nowość w 2.0.0)*

Kompilowanie/eksportowanie wielu plików naraz jednym skryptem jest szybsze niż uruchamianie programu osobno dla każdego pliku.

### Edycja plików

W zależności od gry, z której pochodzi plik `.wpk`, otrzymamy nieco inną strukturę pliku wynikowego. Poniżej opis każdego z trzech formatów.

#### Format `txt`

Przykład dla KnightShift:

```
wavepack TALK_AMAZONE_COMMAND
{
    Level0 9
    {
        Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
        Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_2.wav
        ...
    }
    Level1 9
    {
        ...
    }
}
```

Przykład dla WW3 Black Gold / Heli Heroes:

```
wavepack RUS_M07
{
    Type 0                  <-- jeżeli chcemy zachować ten format wpk dla tej gry
    DisableLevel2 1         <-- tych dwóch wartości nie zmieniamy
    Level0 1
    {
        Wave 100 MESSAGE\RUS\RUS_M07.WAV
        ...
    }
    Level1 ...
    {
        ...
    }
}
```

- `Type` oznacza typ wavepacka.
- `DisableLevel2` blokuje Level2 — ta instrukcja umożliwia zbudowanie formatu `.wpk` dla gry Earth 2150.
- Jeśli chcemy zachować format z danej gry, tych dwóch instrukcji nie ruszamy.
- Wavepacki najlepiej edytować na wzór już istniejącego pliku danej gry.
- W jednym pliku `.txt` można zdefiniować wiele wavepacków (patrz plik `Sample.txt` dołączony do programu) — to szybsze niż kompilowanie każdego osobno.
- Liczba wpisów `Wave` w danym Levelu musi się zgadzać z liczbą podaną obok instrukcji `Level`.
- Program ma wbudowany debugger wychwytujący błędy składni, ale nie wszystkie — trzeba zachować ostrożność, szczególnie przy wcięciach (tabulator vs spacje).
- Polecany edytor: Notepad++ z włączoną opcją "Show All Characters".

#### Format `cpp`

Przykład:

```cpp
uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;

uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace EXP_M60_U
{
	struct Level_0
	{
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U1.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U2.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U3.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U4.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U5.wav"; };
	};
};
```

- Format `.cpp` pozwala edytować dosłownie każdą wartość pliku.
- Jeden plik `.cpp` przechowuje tylko **jeden** wavepack.
- Aby zachować oryginalny format danej gry, edytujemy tylko zawartość `struct Level_X` — kopiujemy linię `struct Wave{...}` i wklejamy poniżej dla kolejnych dźwięków.
- Wszystko powyżej `namespace` to wartości nagłówkowe, których zwykle nie trzeba ruszać.
- Nie trzeba samodzielnie liczyć liczby wavów — program wykrywa funkcje `count()` i `measure_all_value_sizes_after_this_instruction()` i sam wylicza te wartości podczas kompilacji.

#### Format `json` *(nowość w 2.0.0)*

Ten sam wavepack co powyżej może być zapisany jako czytelny, ustrukturyzowany JSON - bez konieczności pilnowania wcięć czy nawiasów klamrowych jak w `.txt`. Przykład (na podstawie realnego pliku eksportowego):

```json
{
    "wavepack": "SND_HIT_STONEEXPLODE",
    "type": 1,
    "disableLevel2": 0,
    "levels": [
        [
            {
                "volume": 90,
                "distanceMinA": 10,
                "distanceMaxA": 35,
                "soundFlags": 0,
                "playPriority": 0,
                "constValue0": 0,
                "delay": 0,
                "constValue2": 0,
                "wavePath": "ARTIFACTS\\SND_HIT_STONEEXP_1.wav"
            },
            {
                "volume": 90,
                "distanceMinA": 10,
                "distanceMaxA": 35,
                "soundFlags": 0,
                "playPriority": 0,
                "constValue0": 0,
                "delay": 0,
                "constValue2": 0,
                "wavePath": "ARTIFACTS\\SND_HIT_STONEEXP_2.wav"
            }
        ],
        [],
        []
    ]
}
```

Opis pól:

- `wavepack` — nazwa wavepacka (odpowiednik nagłówka `wavepack ...` z formatu `.txt`).
- `type` — typ wavepacka, odpowiednik instrukcji `Type` z formatu `.txt`.
- `disableLevel2` — odpowiednik instrukcji `DisableLevel2` (`0`/`1`).
- `levels` — tablica trzech poziomów (`Level0`, `Level1`, `Level2` — w tej kolejności), z których każdy jest tablicą obiektów dźwięków. Pusta tablica `[]` oznacza brak dźwięków na danym poziomie.
- Każdy obiekt dźwięku odpowiada jednemu wpisowi `Wave` z formatu `.txt` — pola `volume`, `distanceMinA`, `distanceMaxA`, `soundFlags`, `playPriority`, `constValue0`, `delay`, `constValue2` to nazwane odpowiedniki kolejnych parametrów liczbowych/flag znanych z linii `Wave`, a `wavePath` to ścieżka do pliku `.wav`.

Jeśli chcemy zachować format zgodny z konkretną grą, nie zmieniamy pól `type` i `disableLevel2`, tak samo jak w przypadku instrukcji `Type`/`DisableLevel2` w `.txt`.

### WpkJsonEditor.py — GUI do edycji plików JSON

Do paczki dołączony jest dodatkowy skrypt **`WpkJsonEditor.py`** - proste GUI (Tkinter, wyłącznie biblioteka standardowa Pythona) do wygodnej edycji plików `.json` wyeksportowanych przez `WpkTool.exe`, bez ręcznego grzebania w kodzie JSON.

**Wymagania:** Python 3 z modułem `tkinter` (dostarczanym domyślnie z większością instalacji Pythona na Windows).

**Uruchomienie:**

```
python3 WpkJsonEditor.py [ścieżka_do_pliku.json]
```

Podanie ścieżki jako argumentu od razu otwiera dany plik; bez argumentu program startuje z pustym, nowym wavepackiem.

**Możliwości:**

- Edycja nagłówka wavepacka: nazwa, `Type` (`0 - old_wpk_format_type` / `1 - new_wpk_format_type`) oraz `Disable level 2`.
- Trzy zakładki (`Level 0` / `Level 1` / `Level 2`), każda z listą wpisów `Wave` w formie tabeli.
- Dodawanie, edycja i usuwanie pojedynczych wpisów `Wave` w oknie dialogowym (z walidacją pól liczbowych).
- Zmiana kolejności wpisów w danym poziomie przyciskami "W górę" / "W dół".
- Pole `delay` prezentowane jest jako checkbox (`Delayed`, 0/1) — a w tabeli poziomu można je przełączyć jednym dwuklikiem wprost na tej kolumnie, bez otwierania okna edycji.
- Wczytywanie plików JSON z walidacją i uzupełnianiem brakujących pól wartościami domyślnymi (np. gdy plik ma mniej niż 3 poziomy).
- Zapis (`Ctrl+S`) i zapis jako (`Ctrl+Shift+S`), nowy plik (`Ctrl+N`), otwieranie (`Ctrl+O`) — z ostrzeżeniem o niezapisanych zmianach przy zamykaniu lub wczytywaniu innego pliku.

### Testy

Program (silnik eksportu/importu) był testowany na wszystkich plikach `.wpk` z poniższych gier z wynikiem 100%:

| Gra | Liczba przetestowanych plików `.wpk` |
| --- | --- |
| Earth 2150: Escape from the Blue Planet | 566 |
| Earth 2150: The Moon Project | 289 |
| Earth 2150: Lost Souls | 289 |
| Heli Heroes | 70 |
| World War II: Panzer Claws / Frontline Attack: War Over Europe | 218 |
| World War III: Black Gold | 261 |
| KnightShift / Polanie 2 / Once Upon a Knight | 424 |
| KnightShift II Curse of Souls / Polanie 3 | 1065 |
| Earth 2160 | 961 |

---

## GB English

### About

Starting from version 2.0.0, the former separate programs **WpkExporter** and **WpkImporter** have been merged into a single executable - **`WpkTool.exe`**. The mode of operation (export / import) is now selected via command-line switches instead of running separate programs.

Compiling the program requires the compiled static library `KsUtils`:
[LibKsUtils link](https://github.com/A-t-l-as/LibKsUtils)

The program supports `.wpk` files from the following games:
- Earth 2150: Escape from the Blue Planet
- Earth 2150: The Moon Project
- Earth 2150: Lost Souls
- Heli Heroes
- World War II: Panzer Claws / Frontline Attack: War Over Europe
- World War III: Black Gold
- KnightShift / Polanie 2 / Once Upon a Knight
- KnightShift II Curse of Souls / Polanie 3
- Earth 2160

WpkTool was written entirely by Atlas.

### Formats

The program supports three intermediate formats, for both export and import:

| Format | Description |
| --- | --- |
| `txt` | Text format, known from the old WPKtoTXT/WpkExporter. Can hold multiple wavepacks in a single file. |
| `cpp` | C++-style format, allows editing literally every value. One file = one wavepack. |
| `json` | **New in 2.0.0.** A structured, human-readable format that's easy to edit in any text editor without worrying about indentation/syntax like in `.txt`. |

### Usage — switches

Running the program with no arguments, or with invalid arguments, starts an **interactive mode** - it will prompt you in the console for the required parameters and options (file name, format, etc.).

**Export** (`.wpk` -> `txt`/`cpp`/`json`):

```
WpkTool.exe -e <path_to_wpk_file> <txt/cpp/json>
```

Example:

```
WpkTool.exe -e D:\folder\file.wpk json
```

**Import / compile** (`txt`/`cpp`/`json` -> `.wpk`):

```
WpkTool.exe -c <path_to_source_file>
```

The source file format (including `.json`) is detected automatically from the file extension — you don't need to specify it separately.

Example:

```
WpkTool.exe -c D:\folder\all_wpks.txt
WpkTool.exe -c D:\folder\SND_HIT_STONEEXPLODE.json
```

### Bulk directory conversion

The program's folder includes ready-made scripts to convert every file of a given type in a directory:

**Export:**
- `_EXPORT_ALL_TO_TXT.bat` / `_EXPORT_ALL_TO_TXT.ps1`
- `_EXPORT_ALL_TO_CPP.bat` / `_EXPORT_ALL_TO_CPP.ps1`
- `_EXPORT_ALL_TO_JSON.bat` / `_EXPORT_ALL_TO_JSON.ps1` *(new in 2.0.0)*

**Import:**
- `_CONVERT_ALL_TXT.bat` / `_CONVERT_ALL_TXT.ps1`
- `_CONVERT_ALL_CPP.bat` / `_CONVERT_ALL_CPP.ps1`
- `_CONVERT_ALL_JSON.bat` / `_CONVERT_ALL_JSON.ps1` *(new in 2.0.0)*

Converting many files at once with a script is faster than running the program separately for each file.

### Editing files

Depending on which game a `.wpk` file comes from, the resulting file structure will differ slightly. Below is a description of each of the three formats.

#### `txt` format

Example for KnightShift:

```
wavepack TALK_AMAZONE_COMMAND
{
    Level0 9
    {
        Wave 100 30 35 255 255 0 delayed TALK\AMAZONE\1_19_B_1.wav
        ...
    }
}
```

Example for WW3 Black Gold / Heli Heroes:

```
wavepack RUS_M07
{
    Type 0                  <-- keep this if you want to preserve this game's wpk format
    DisableLevel2 1         <-- do not change these two values
    Level0 1
    {
        Wave 100 MESSAGE\RUS\RUS_M07.WAV
    }
}
```

- `Type` denotes the wavepack type.
- `DisableLevel2` disables Level2 — this instruction is what allows building the Earth 2150 `.wpk` format.
- If you want to keep a given game's format, don't touch these two instructions.
- It's best to edit wavepacks based on an existing example from the same game.
- A single `.txt` file can define multiple wavepacks (see the bundled `Sample.txt`) — this is faster than compiling each one separately.
- The number of `Wave` entries in a Level must match the count declared next to the `Level` instruction.
- The program has a built-in debugger that catches syntax errors, but it won't catch everything — be careful, especially with indentation (tabs vs. spaces).
- Recommended editor: Notepad++ with "Show All Characters" enabled.

#### `cpp` format

Example:

```cpp
uint8_t format_0 = 87; // W
uint8_t format_1 = 80; // P
uint8_t format_2 = 75; // K
uint8_t format_3 = 0;

uint32_t wavepack_type = 0;

uint32_t number_of_waves_in_level_0 = count();
uint32_t number_of_waves_in_level_1 = count();

uint32_t data_size = measure_all_value_sizes_after_this_instruction();

namespace EXP_M60_U
{
	struct Level_0
	{
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U1.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U2.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U3.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U4.wav"; };
		struct Wave{ uint32_t volume = 20;	char wave_path[] = "EXPL\EXP_M60_U5.wav"; };
	};
};
```

- The `.cpp` format lets you edit literally every value in the file.
- A single `.cpp` file can only hold **one** wavepack.
- To keep a game's original format, only edit the contents of `struct Level_X` — copy the `struct Wave{...}` line and paste it below for additional sounds.
- Everything above `namespace` is header data you generally don't need to touch.
- You don't need to count the number of waves yourself — the program detects the `count()` and `measure_all_value_sizes_after_this_instruction()` functions and computes these values automatically during compilation.

#### `json` format *(new in 2.0.0)*

The same wavepack as above can be stored as a readable, structured JSON file — no need to track indentation or curly braces as in `.txt`. Example (from a real exported file):

```json
{
    "wavepack": "SND_HIT_STONEEXPLODE",
    "type": 1,
    "disableLevel2": 0,
    "levels": [
        [
            {
                "volume": 90,
                "distanceMinA": 10,
                "distanceMaxA": 35,
                "soundFlags": 0,
                "playPriority": 0,
                "constValue0": 0,
                "delay": 0,
                "constValue2": 0,
                "wavePath": "ARTIFACTS\\SND_HIT_STONEEXP_1.wav"
            }
        ],
        [],
        []
    ]
}
```

Field reference:

- `wavepack` — the wavepack's name (equivalent to the `wavepack ...` header in `.txt`).
- `type` — the wavepack type, equivalent to the `Type` instruction in `.txt`.
- `disableLevel2` — equivalent to the `DisableLevel2` instruction (`0`/`1`).
- `levels` — an array of three levels (`Level0`, `Level1`, `Level2`, in that order), each an array of sound objects. An empty array `[]` means no sounds at that level.
- Each sound object corresponds to one `Wave` entry from the `.txt` format — the `volume`, `distanceMinA`, `distanceMaxA`, `soundFlags`, `playPriority`, `constValue0`, `delay`, `constValue2` fields are named equivalents of the numeric parameters/flags known from the `Wave` line, and `wavePath` is the path to the `.wav` file.

If you want to preserve a specific game's format, don't change the `type` and `disableLevel2` fields — just like with the `Type`/`DisableLevel2` instructions in `.txt`.

### WpkJsonEditor.py — a JSON editing GUI

The package also includes an additional script, **`WpkJsonEditor.py`** - a simple GUI (Tkinter, Python standard library only) for conveniently editing `.json` files exported by `WpkTool.exe`, without having to hand-edit the JSON.

**Requirements:** Python 3 with the `tkinter` module (bundled by default with most Python installations on Windows).

**Running it:**

```
python3 WpkJsonEditor.py [path_to_file.json]
```

Passing a path as an argument opens that file immediately; without an argument, the editor starts with a new, empty wavepack.

**Features:**

- Edit the wavepack header: name, `Type` (`0 - old_wpk_format_type` / `1 - new_wpk_format_type`), and `Disable level 2`.
- Three tabs (`Level 0` / `Level 1` / `Level 2`), each showing its `Wave` entries as a table.
- Add, edit, and delete individual `Wave` entries via a dialog window (with validation of numeric fields).
- Reorder entries within a level using the "Up" / "Down" buttons.
- The `delay` field is shown as a checkbox (`Delayed`, 0/1) - and can also be toggled directly in the level's table with a single double-click on that column, without opening the edit dialog.
- Loading JSON files includes validation and fills in missing fields with default values (e.g. if a file has fewer than 3 levels).
- Save (`Ctrl+S`) and Save As (`Ctrl+Shift+S`), New file (`Ctrl+N`), Open (`Ctrl+O`) — with a warning about unsaved changes when closing or loading another file.

### Testing

The export/import engine was tested on all `.wpk` files from the games below, with a 100% success rate:

| Game | Number of `.wpk` files tested |
| --- | --- |
| Earth 2150: Escape from the Blue Planet | 566 |
| Earth 2150: The Moon Project | 289 |
| Earth 2150: Lost Souls | 289 |
| Heli Heroes | 70 |
| World War II: Panzer Claws / Frontline Attack: War Over Europe | 218 |
| World War III: Black Gold | 261 |
| KnightShift / Polanie 2 / Once Upon a Knight | 424 |
| KnightShift II Curse of Souls / Polanie 3 | 1065 |
| Earth 2160 | 961 |

WpkTool was written entirely by Atlas.
