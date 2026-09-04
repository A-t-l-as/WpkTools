#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WpkJsonEditor
=============

Prosty GUI (tkinter, tylko biblioteka standardowa) do edycji plików JSON
eksportowanych przez WpkTool (WpkFile::ExportTo(..., "json")).

Struktura pliku JSON (na podstawie WaveBase::to_json / WavePackBase::to_json):

{
    "wavepack": "<nazwa>",
    "type": 0 | 1,                 // 0 = old_wpk_format_type, 1 = new_wpk_format_type
    "disableLevel2": 0 | 1,
    "levels": [                    // zawsze 3 poziomy (c_number_of_levels)
        [ <Wave>, <Wave>, ... ],   // level 0
        [ <Wave>, <Wave>, ... ],   // level 1
        [ <Wave>, <Wave>, ... ]    // level 2
    ]
}

Wave:
{
    "volume": int,
    "distanceMinA": int,
    "distanceMaxA": int,
    "soundFlags": int,
    "playPriority": int,
    "constValue0": int,
    "delay": int,
    "constValue2": int,
    "wavePath": "<ścieżka>"
}

Uruchomienie:
    python3 wpk_json_editor.py [ścieżka_do_pliku.json]
"""

import json
import os
import sys
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

NUM_LEVELS = 3

WAVE_FIELDS = [
    ("volume",        "Volume"),
    ("distanceMinA",  "Distance Min A"),
    ("distanceMaxA",  "Distance Max A"),
    ("soundFlags",    "Sound Flags"),
    ("playPriority",  "Play Priority"),
    ("constValue0",   "Const Value 0"),
    ("delay",         "Delay"),          # w oknie edycji renderowane jako checkbox (0/1)
    ("constValue2",   "Const Value 2"),
    ("wavePath",      "Wave Path"),
]

WAVE_INT_FIELDS = {
    "volume", "distanceMinA", "distanceMaxA",
    "soundFlags", "playPriority",
    "constValue0", "delay", "constValue2",
}

COLUMNS = [f[0] for f in WAVE_FIELDS]
HEADINGS = [f[1] for f in WAVE_FIELDS]

DEFAULT_WAVE = {
    "volume": 0,
    "distanceMinA": 0,
    "distanceMaxA": 0,
    "soundFlags": 0,
    "playPriority": 0,
    "constValue0": 0,
    "delay": 0,
    "constValue2": 0,
    "wavePath": "",
}


def empty_wavepack():
    return {
        "wavepack": "NewWavePack",
        "type": 1,
        "disableLevel2": 0,
        "levels": [[], [], []],
    }


class WaveEditDialog(tk.Toplevel):
    """Okno modalne do dodawania / edycji pojedynczego Wave."""

    def __init__(self, parent, title="Edytuj Wave", wave=None):
        super().__init__(parent)
        self.title(title)
        self.resizable(False, False)
        self.result = None
        self.transient(parent)
        self.grab_set()

        wave = wave or DEFAULT_WAVE
        self.vars = {}

        frame = ttk.Frame(self, padding=12)
        frame.grid(row=0, column=0, sticky="nsew")

        # 'delay' w Wave.cpp jest ustawiane tylko na 0 (brak słowa "delayed")
        # albo na 1 (delayed_const_value, gdy w tekście jest "delayed") - to
        # de facto flaga logiczna, więc pokazujemy checkbox zamiast pola liczbowego,
        # zachowując oryginalną kolejność pól (między Const Value 0 i Const Value 2).
        for row, (key, label) in enumerate(WAVE_FIELDS):
            ttk.Label(frame, text=label + ":").grid(row=row, column=0, sticky="w", pady=2)

            if key == "delay":
                current_delay = wave.get("delay", DEFAULT_WAVE["delay"])
                try:
                    delay_initial = bool(int(current_delay))
                except (TypeError, ValueError):
                    delay_initial = bool(current_delay)
                self.delay_var = tk.BooleanVar(value=delay_initial)
                ttk.Checkbutton(
                    frame,
                    text="Delayed (delay = 1)",
                    variable=self.delay_var,
                ).grid(row=row, column=1, sticky="w", pady=2, padx=(6, 0))
                continue

            var = tk.StringVar(value=str(wave.get(key, DEFAULT_WAVE[key])))
            entry = ttk.Entry(frame, textvariable=var, width=32)
            entry.grid(row=row, column=1, sticky="ew", pady=2, padx=(6, 0))
            self.vars[key] = var

        frame.columnconfigure(1, weight=1)

        btn_frame = ttk.Frame(self, padding=(12, 0, 12, 12))
        btn_frame.grid(row=1, column=0, sticky="e")

        ttk.Button(btn_frame, text="Anuluj", command=self._on_cancel).pack(side="right", padx=(6, 0))
        ttk.Button(btn_frame, text="OK", command=self._on_ok).pack(side="right")

        self.bind("<Return>", lambda e: self._on_ok())
        self.bind("<Escape>", lambda e: self._on_cancel())

        self.wait_window(self)

    def _on_ok(self):
        result = {}
        for key, label in WAVE_FIELDS:
            if key == "delay":
                continue
            raw = self.vars[key].get().strip()
            if key in WAVE_INT_FIELDS:
                try:
                    result[key] = int(raw)
                except ValueError:
                    messagebox.showerror(
                        "Błąd wartości",
                        f"Pole '{label}' musi być liczbą całkowitą (podano: '{raw}').",
                        parent=self,
                    )
                    return
            else:
                result[key] = raw

        result["delay"] = 1 if self.delay_var.get() else 0

        self.result = result
        self.destroy()

    def _on_cancel(self):
        self.result = None
        self.destroy()


class WpkJsonEditor(tk.Tk):
    def __init__(self, initial_path=None):
        super().__init__()
        self.title("WpkTool JSON Editor")
        self.geometry("880x560")
        self.minsize(700, 420)

        self.current_path = None
        self.data = empty_wavepack()
        self.dirty = False

        self._build_menu()
        self._build_header()
        self._build_levels_notebook()
        self._build_statusbar()

        self.protocol("WM_DELETE_WINDOW", self._on_close)

        if initial_path:
            self._load_file(initial_path)
        else:
            self._refresh_all()

    # ------------------------------------------------------------------ UI

    def _build_menu(self):
        menubar = tk.Menu(self)

        file_menu = tk.Menu(menubar, tearoff=0)
        file_menu.add_command(label="Nowy", command=self._new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Otwórz...", command=self._open_file, accelerator="Ctrl+O")
        file_menu.add_command(label="Zapisz", command=self._save_file, accelerator="Ctrl+S")
        file_menu.add_command(label="Zapisz jako...", command=self._save_file_as, accelerator="Ctrl+Shift+S")
        file_menu.add_separator()
        file_menu.add_command(label="Zakończ", command=self._on_close)
        menubar.add_cascade(label="Plik", menu=file_menu)

        self.config(menu=menubar)

        self.bind_all("<Control-n>", lambda e: self._new_file())
        self.bind_all("<Control-o>", lambda e: self._open_file())
        self.bind_all("<Control-s>", lambda e: self._save_file())
        self.bind_all("<Control-S>", lambda e: self._save_file_as())

    def _build_header(self):
        header = ttk.Frame(self, padding=10)
        header.pack(side="top", fill="x")

        ttk.Label(header, text="WavePack name:").grid(row=0, column=0, sticky="w")
        self.name_var = tk.StringVar()
        self.name_var.trace_add("write", lambda *_: self._mark_dirty())
        ttk.Entry(header, textvariable=self.name_var, width=30).grid(
            row=0, column=1, sticky="w", padx=(6, 20)
        )

        ttk.Label(header, text="Type:").grid(row=0, column=2, sticky="w")
        self.type_var = tk.StringVar()
        self.type_combo = ttk.Combobox(
            header,
            textvariable=self.type_var,
            values=["0 - old_wpk_format_type", "1 - new_wpk_format_type"],
            state="readonly",
            width=24,
        )
        self.type_combo.grid(row=0, column=3, sticky="w", padx=(6, 20))
        self.type_combo.bind("<<ComboboxSelected>>", lambda e: self._mark_dirty())

        self.disable_level2_var = tk.BooleanVar()
        ttk.Checkbutton(
            header,
            text="Disable level 2",
            variable=self.disable_level2_var,
            command=self._mark_dirty,
        ).grid(row=0, column=4, sticky="w")

    def _build_levels_notebook(self):
        self.notebook = ttk.Notebook(self)
        self.notebook.pack(side="top", fill="both", expand=True, padx=10, pady=(0, 10))

        self.trees = []
        for level in range(NUM_LEVELS):
            tab = ttk.Frame(self.notebook)
            self.notebook.add(tab, text=f"Level {level}")

            tree_frame = ttk.Frame(tab)
            tree_frame.pack(side="top", fill="both", expand=True, padx=8, pady=8)

            tree = ttk.Treeview(tree_frame, columns=COLUMNS, show="headings", selectmode="browse")
            for col, heading in zip(COLUMNS, HEADINGS):
                tree.heading(col, text=heading)
                width = 160 if col == "wavePath" else 95
                tree.column(col, width=width, anchor="w", stretch=(col == "wavePath"))

            vsb = ttk.Scrollbar(tree_frame, orient="vertical", command=tree.yview)
            tree.configure(yscrollcommand=vsb.set)
            tree.pack(side="left", fill="both", expand=True)
            vsb.pack(side="right", fill="y")

            tree.bind("<Double-1>", lambda e, lvl=level: self._on_tree_double_click(e, lvl))

            btn_bar = ttk.Frame(tab)
            btn_bar.pack(side="bottom", fill="x", padx=8, pady=(0, 8))

            ttk.Button(btn_bar, text="Dodaj", command=lambda lvl=level: self._add_wave(lvl)).pack(
                side="left"
            )
            ttk.Button(btn_bar, text="Edytuj", command=lambda lvl=level: self._edit_wave(lvl)).pack(
                side="left", padx=(6, 0)
            )
            ttk.Button(btn_bar, text="Usuń", command=lambda lvl=level: self._delete_wave(lvl)).pack(
                side="left", padx=(6, 0)
            )
            ttk.Button(btn_bar, text="W górę", command=lambda lvl=level: self._move_wave(lvl, -1)).pack(
                side="left", padx=(18, 0)
            )
            ttk.Button(btn_bar, text="W dół", command=lambda lvl=level: self._move_wave(lvl, 1)).pack(
                side="left", padx=(6, 0)
            )

            self.trees.append(tree)

    def _build_statusbar(self):
        self.status_var = tk.StringVar(value="Gotowy.")
        status = ttk.Label(self, textvariable=self.status_var, anchor="w", relief="sunken", padding=(6, 2))
        status.pack(side="bottom", fill="x")

    # ------------------------------------------------------------- helpers

    def _set_status(self, text):
        self.status_var.set(text)

    def _mark_dirty(self):
        self.dirty = True
        self._update_title()

    def _update_title(self):
        name = self.current_path if self.current_path else "(bez nazwy)"
        star = "*" if self.dirty else ""
        self.title(f"WpkTool JSON Editor - {name}{star}")

    def _on_tree_double_click(self, event, level):
        tree = self.trees[level]
        row_id = tree.identify_row(event.y)
        col_id = tree.identify_column(event.x)
        if not row_id:
            return

        col_index = int(col_id.replace("#", "")) - 1
        if 0 <= col_index < len(COLUMNS) and COLUMNS[col_index] == "delay":
            idx = tree.index(row_id)
            wave = self.data["levels"][level][idx]
            wave["delay"] = 0 if int(wave.get("delay", 0)) else 1
            self._refresh_tree(level)
            tree.selection_set(tree.get_children()[idx])
            self._mark_dirty()
            self._set_status(
                f"Wave #{idx} (Level {level}): delay = {wave['delay']} "
                f"({'Tak' if wave['delay'] else 'Nie'})."
            )
            return

        self._edit_wave(level)

    def _selected_index(self, level):
        tree = self.trees[level]
        sel = tree.selection()
        if not sel:
            return None
        return tree.index(sel[0])

    def _refresh_tree(self, level):
        tree = self.trees[level]
        tree.delete(*tree.get_children())
        for wave in self.data["levels"][level]:
            values = []
            for col in COLUMNS:
                raw = wave.get(col, DEFAULT_WAVE.get(col, ""))
                if col == "delay":
                    raw = "Tak" if int(raw) else "Nie"
                values.append(raw)
            tree.insert("", "end", values=values)

    def _refresh_header(self):
        self.name_var.set(self.data.get("wavepack", ""))
        wtype = int(self.data.get("type", 1))
        self.type_combo.current(wtype if wtype in (0, 1) else 1)
        self.disable_level2_var.set(bool(self.data.get("disableLevel2", 0)))

    def _refresh_all(self):
        self._refresh_header()
        for level in range(NUM_LEVELS):
            self._refresh_tree(level)
        self._update_title()

    def _sync_header_into_data(self):
        self.data["wavepack"] = self.name_var.get()
        self.data["type"] = 0 if self.type_var.get().startswith("0") else 1
        self.data["disableLevel2"] = 1 if self.disable_level2_var.get() else 0

    # -------------------------------------------------------- wave actions

    def _add_wave(self, level):
        dialog = WaveEditDialog(self, title=f"Dodaj Wave (Level {level})")
        if dialog.result is not None:
            self.data["levels"][level].append(dialog.result)
            self._refresh_tree(level)
            self._mark_dirty()
            self._set_status(f"Dodano Wave do Level {level}.")

    def _edit_wave(self, level):
        idx = self._selected_index(level)
        if idx is None:
            messagebox.showinfo("Edytuj Wave", "Najpierw zaznacz element na liście.")
            return
        wave = self.data["levels"][level][idx]
        dialog = WaveEditDialog(self, title=f"Edytuj Wave (Level {level})", wave=wave)
        if dialog.result is not None:
            self.data["levels"][level][idx] = dialog.result
            self._refresh_tree(level)
            self._mark_dirty()
            self._set_status(f"Zaktualizowano Wave #{idx} w Level {level}.")

    def _delete_wave(self, level):
        idx = self._selected_index(level)
        if idx is None:
            messagebox.showinfo("Usuń Wave", "Najpierw zaznacz element na liście.")
            return
        if messagebox.askyesno("Usuń Wave", "Czy na pewno usunąć zaznaczony Wave?"):
            del self.data["levels"][level][idx]
            self._refresh_tree(level)
            self._mark_dirty()
            self._set_status(f"Usunięto Wave #{idx} z Level {level}.")

    def _move_wave(self, level, direction):
        idx = self._selected_index(level)
        if idx is None:
            return
        waves = self.data["levels"][level]
        new_idx = idx + direction
        if 0 <= new_idx < len(waves):
            waves[idx], waves[new_idx] = waves[new_idx], waves[idx]
            self._refresh_tree(level)
            tree = self.trees[level]
            tree.selection_set(tree.get_children()[new_idx])
            self._mark_dirty()

    # -------------------------------------------------------- file actions

    def _new_file(self):
        if not self._confirm_discard_changes():
            return
        self.current_path = None
        self.data = empty_wavepack()
        self.dirty = False
        self._refresh_all()
        self._set_status("Utworzono nowy, pusty WavePack.")

    def _open_file(self):
        if not self._confirm_discard_changes():
            return
        path = filedialog.askopenfilename(
            title="Otwórz plik JSON WavePacka",
            filetypes=[("Pliki JSON", "*.json"), ("Wszystkie pliki", "*.*")],
        )
        if path:
            self._load_file(path)

    def _load_file(self, path):
        try:
            with open(path, "r", encoding="utf-8") as f:
                raw = json.load(f)
        except (OSError, json.JSONDecodeError) as exc:
            messagebox.showerror("Błąd wczytywania", f"Nie udało się wczytać pliku:\n{exc}")
            return

        data = self._normalize_loaded_data(raw)
        if data is None:
            return

        self.data = data
        self.current_path = path
        self.dirty = False
        self._refresh_all()
        self._set_status(f"Wczytano: {path}")

    def _normalize_loaded_data(self, raw):
        """Waliduje/uzupełnia strukturę wczytaną z JSON do oczekiwanego kształtu."""
        if not isinstance(raw, dict):
            messagebox.showerror("Błąd formatu", "Plik JSON nie zawiera obiektu WavePack.")
            return None

        data = {
            "wavepack": raw.get("wavepack", ""),
            "type": int(raw.get("type", 1)),
            "disableLevel2": int(raw.get("disableLevel2", 0)),
            "levels": [[], [], []],
        }

        levels_raw = raw.get("levels", [])
        if not isinstance(levels_raw, list):
            messagebox.showerror("Błąd formatu", "Pole 'levels' powinno być tablicą.")
            return None

        for i in range(NUM_LEVELS):
            level_list = levels_raw[i] if i < len(levels_raw) else []
            if not isinstance(level_list, list):
                level_list = []
            cleaned = []
            for wave_raw in level_list:
                if not isinstance(wave_raw, dict):
                    continue
                wave = dict(DEFAULT_WAVE)
                for key in DEFAULT_WAVE:
                    if key in wave_raw:
                        wave[key] = wave_raw[key]
                cleaned.append(wave)
            data["levels"][i] = cleaned

        return data

    def _save_file(self):
        if self.current_path is None:
            self._save_file_as()
            return
        self._write_to(self.current_path)

    def _save_file_as(self):
        path = filedialog.asksaveasfilename(
            title="Zapisz plik JSON WavePacka",
            defaultextension=".json",
            filetypes=[("Pliki JSON", "*.json"), ("Wszystkie pliki", "*.*")],
            initialfile=(self.name_var.get() or "wavepack") + ".json",
        )
        if path:
            self._write_to(path)

    def _write_to(self, path):
        self._sync_header_into_data()
        try:
            with open(path, "w", encoding="utf-8") as f:
                json.dump(self.data, f, indent=4, ensure_ascii=False)
        except OSError as exc:
            messagebox.showerror("Błąd zapisu", f"Nie udało się zapisać pliku:\n{exc}")
            return

        self.current_path = path
        self.dirty = False
        self._update_title()
        self._set_status(f"Zapisano: {path}")

    def _confirm_discard_changes(self):
        if not self.dirty:
            return True
        answer = messagebox.askyesnocancel(
            "Niezapisane zmiany",
            "Masz niezapisane zmiany. Czy chcesz je zapisać przed kontynuowaniem?",
        )
        if answer is None:
            return False
        if answer is True:
            self._save_file()
            return not self.dirty
        return True

    def _on_close(self):
        if self._confirm_discard_changes():
            self.destroy()


def main():
    initial_path = sys.argv[1] if len(sys.argv) > 1 else None
    if initial_path and not os.path.exists(initial_path):
        print(f"Ostrzeżenie: plik '{initial_path}' nie istnieje. Otwieram edytor z pustym WavePackiem.")
        initial_path = None

    app = WpkJsonEditor(initial_path)
    app.mainloop()


if __name__ == "__main__":
    main()
