import csv
from pathlib import Path
import tkinter as tk
import tkinter.filedialog as tkFileDialog
from dataclasses import dataclass

hooks_csv_path = tkFileDialog.askopenfilename(title='Please select the hooks.csv file')
output_md  = tkFileDialog.asksaveasfilename(title='Please select the output MD file location', defaultextension=".md")

@dataclass
class RevKlassInfo:
    num_reversed : int
    num_not_reversed : int

    def process_row(self, r):
        if bool(int(r['reversed'])):
            self.num_reversed += 1
        else:
            self.num_not_reversed += 1

# Open csv and process it

klass_info : dict[str, RevKlassInfo] = {}
with open(hooks_csv_path, "r", encoding='utf8') as hooksf:
    for r in csv.DictReader(hooksf):
        klass_info.setdefault(r['class'], RevKlassInfo(0, 0)).process_row(r)

with open(output_md, "w", encoding="utf8", newline='\n') as outf:
    outf.write("# Reversed Classes\n")

    partially  = []
    completely = []
    for klass, v in klass_info.items():
        (completely if v.num_not_reversed == 0 else partially).append(klass)

    outf.write("\n")
    outf.write(f"## Completely Reversed ({len(completely)} classes)\n")
    outf.write("\n")
    
    for klass in completely:
        info = klass_info[klass]
        outf.write(f"- {klass} ({info.num_reversed + info.num_not_reversed})\n")

    outf.write("\n")
    outf.write(f"## Partially Reversed ({len(partially)} classes)\n")
    outf.write("\n")

    for klass in partially:
        info = klass_info[klass]
        outf.write(f"- {klass} ({info.num_reversed}/{info.num_reversed + info.num_not_reversed})\n")
