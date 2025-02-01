import csv
import datetime
import tkinter.filedialog as tkFileDialog
from dataclasses import dataclass
from typing import Any

@dataclass
class Klass:
    name: str
    num_reversed: int
    num_not_reversed: int

    def process_row(self, row: dict[str, Any]):
        if bool(int(row['reversed'])):
            self.num_reversed += 1
        else:
            self.num_not_reversed += 1

    @property
    def num_fn(self):
        return self.num_not_reversed + self.num_reversed
    
    @property
    def is_completely_reversed(self):
        return self.num_not_reversed == 0

def main():
    hooks_csv_path = tkFileDialog.askopenfilename(title='Please select the hooks.csv file')
    output_md  = tkFileDialog.asksaveasfilename(title='Please select the output MD file location', defaultextension=".md")

    klass_info : dict[str, Klass] = {}
    with open(hooks_csv_path, "r", encoding='utf8') as hooksf:
        for r in csv.DictReader(hooksf):
            name: str = r["class"]
            klass_info.setdefault(name, Klass(name, 0, 0)).process_row(r)

    with open(output_md, "w", encoding="utf8", newline='\n') as outf:
        partially: list[Klass]  = []
        completely: list[Klass] = []
        for klass in klass_info.values():
            (completely if klass.is_completely_reversed else partially).append(klass)
        num_total_klass = len(partially) + len(completely)

        outf.write(f"# Reversed Classes [As of {datetime.datetime.now(datetime.timezone.utc).strftime('%b %d, %Y, %H:%M:%S')} UTC]\n")

        def write_header(title: str, klasses: list[Klass]):
            outf.write("\n")
            outf.write(f"## {title} ({len(klasses)}/{num_total_klass}) [{len(klasses) / num_total_klass:.0%}]\n")
            outf.write("\n")

        write_header("Completely Reversed classes", completely)
        for klass in completely:
            outf.write(f"- {klass.name} ({klass.num_reversed})\n")

        write_header("Partially Reversed classes", partially)
        for klass in partially:
            outf.write(f"- {klass.name} ({klass.num_reversed}/{klass.num_fn}) [{1 - klass.num_not_reversed / klass.num_fn:.0%}]\n")

if __name__ == '__main__':
    main()
