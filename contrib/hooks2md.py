import argparse
from contextlib import contextmanager
import csv
import datetime
import os
import tkinter.filedialog as tkFileDialog
from dataclasses import dataclass
from typing import Any

GITHUB_SHA = os.environ.get("GITHUB_SHA", "unknown")
GITHUB_REPO_URL = os.environ.get("GITHUB_REPO_URL")

ap = argparse.ArgumentParser(description="Generate a Markdown file with reversed classes stats from hooks.csv")
ap.add_argument("--input", default=None, help="Path to the hooks.csv file (if not provided, a file dialog will be shown to select the input file)")
ap.add_argument("--output", default=None, help="Path to the output Markdown file (if not provided, a file dialog will be shown to select the output location)")
args = ap.parse_args()

if args.input is None:
    args.input = tkFileDialog.askopenfilename(title='Please select the hooks.csv file')
if args.output is None:
    args.output = tkFileDialog.asksaveasfilename(title='Please select the output MD file location', defaultextension=".md")

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
    klass_info : dict[str, Klass] = {}
    with open(args.input, "r", encoding='utf8') as hooksf:
        for r in csv.DictReader(hooksf):
            name: str = r["class"]
            klass_info.setdefault(name, Klass(name, 0, 0)).process_row(r)

    not_at_all: list[Klass] = []
    partially: list[Klass]  = []
    completely: list[Klass] = []
    for klass in klass_info.values():
        if klass.num_not_reversed == 0:
            completely.append(klass)
        elif klass.num_reversed == 0:
            not_at_all.append(klass)
        else:
            partially.append(klass)
    num_total_klass = len(partially) + len(completely) + len(not_at_all)

    with open(args.output, "w", encoding="utf8", newline='\n') as outf:
        outf.write("# Reversed Classes progress\n")
        outf.write("This file is updated automatically every time the hooks.csv file is updated (which happens every time there are changes to hooks made by a commit), and shows the current progress of reversed classes in the project.\n\n")
        outf.write(f"Last update was at {datetime.datetime.now(datetime.timezone.utc).strftime('%b %d, %Y at %H:%M:%S')} UTC triggered by [{GITHUB_SHA}]({GITHUB_REPO_URL}/commit/{GITHUB_SHA}) \n")
        outf.write("\n")

        outf.write("## Disclaimer\n")
        outf.write(
            "The percentages and the number of classes shown here may not be "
            "completely accurate, because not all classes and functions "
            "are documented yet.\n"
        )

        #outf.write("## Brief stats\n")
        #outf.write(f"- #Functions: {sum(k.num_fn for k in klass_info.values())}<br />\n")
        #outf.write(f"- #Classes: {len(klass_info)}<br />\n")
        
        outf.write(f"## Stats ({sum(k.num_fn for k in klass_info.values())} functions, {len(klass_info)} classes)\n")

        def write_header(title: str, klasses: list[Klass]):
            outf.write("\n")
            outf.write(f"#### {title} ({len(klasses)}/{num_total_klass}) [{len(klasses) / num_total_klass:.0%}]\n")
            outf.write("\n")

        @contextmanager
        def class_list_spoiler():
            outf.write("<details>\n")
            outf.write("<summary>See list of classes</summary>")
            yield
            outf.write("\n</details>\n")

        write_header("Completely reversed classes", completely)
        with class_list_spoiler():
            for klass in completely:
                outf.write(f"- {klass.name} ({klass.num_fn})<br />\n")

        write_header("Partially reversed classes", partially)
        with class_list_spoiler():
            for klass in partially:
                outf.write(f"- {klass.name} ({klass.num_reversed}/{klass.num_fn}) [{1 - klass.num_not_reversed / klass.num_fn:.0%}]<br />\n")

        write_header("Not-at-all reversed classes", not_at_all)
        with class_list_spoiler():
            for klass in not_at_all:
                outf.write(f"- {klass.name} ({klass.num_fn})<br />\n")

if __name__ == '__main__':
    main()
