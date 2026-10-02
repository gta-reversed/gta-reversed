# pylint: disable=missing-module-docstring,missing-class-docstring,missing-function-docstring

import argparse
import datetime
import json
import os
from contextlib import contextmanager
from typing import TypedDict

GITHUB_SHA = os.environ.get("GITHUB_SHA")
GITHUB_SHA_SHORT = GITHUB_SHA[:8] if GITHUB_SHA else None
GITHUB_REPO_URL = os.environ.get("GITHUB_REPO_URL")
GITHUB_COMMIT_URL = f'{GITHUB_REPO_URL}/commit/{GITHUB_SHA}' if GITHUB_SHA and GITHUB_REPO_URL else None

ap = argparse.ArgumentParser(
    description="Generate a Markdown file with reversed categories stats from hooks.csv"
)
ap.add_argument(
    "--input",
    default=None,
    help="Path to the hooks.csv file (if not provided, a file dialog will be shown to select the input file)",
)
ap.add_argument(
    "--output",
    default=None,
    help="Path to the output Markdown file (if not provided, a file dialog will be shown to select the output location)",
)
args = ap.parse_args()

if args.input is None or args.output is None:
    try:
        import tkinter.filedialog as tkFileDialog
    except ImportError as exc:
        raise ImportError(
            "tkinter is required for file dialogs but is not available. Please provide --input and --output arguments."
        ) from exc

    if args.input is None:
        args.input = tkFileDialog.askopenfilename(
            title="Please select the hooks.csv file"
        )
    if args.output is None:
        args.output = tkFileDialog.asksaveasfilename(
            title="Please select the output MD file location", defaultextension=".md"
        )


class HookCategoryItem(TypedDict):
    Category: str
    Name: str
    IsReversed: bool


class HookCategory:
    name: str
    num_reversed: int = 0
    num_not_reversed: int = 0

    def __init__(self, name: str):
        self.name = name

    def process_item(self, item: HookCategoryItem):
        if item.get("IsReversed", False):
            self.num_reversed += 1
        else:
            self.num_not_reversed += 1

    @property
    def num_fn(self):
        return self.num_not_reversed + self.num_reversed

    @property
    def is_completely_reversed(self):
        return self.num_not_reversed == 0


def main() -> None:
    category_info: dict[str, HookCategory] = {}
    with open(args.input, "r", encoding="utf8") as hooksf:
        r: HookCategoryItem
        for r in json.load(hooksf):
            category_info.setdefault(
                r["Category"], HookCategory(r["Category"])
            ).process_item(r)

    not_at_all: list[HookCategory] = []
    partially: list[HookCategory] = []
    completely: list[HookCategory] = []
    for cat in category_info.values():
        if cat.num_not_reversed == 0:
            completely.append(cat)
        elif cat.num_reversed == 0:
            not_at_all.append(cat)
        else:
            partially.append(cat)
    num_total_categories = len(partially) + len(completely) + len(not_at_all)

    with open(args.output, "w", encoding="utf8", newline="\n") as outf:
        outf.write("# Reimplementation progress\n")
        outf.write(
            "This file is updated automatically every time the hooks.csv file is updated (which happens every time there are changes to hooks made by a commit), and shows the current progress of reversed categories in the project.\n\n"
        )
        outf.write(
            f"Last update was at {datetime.datetime.now(datetime.timezone.utc).strftime('%b %d, %Y at %H:%M:%S')} UTC\n"
        )
        if GITHUB_SHA:
            if GITHUB_COMMIT_URL:
                outf.write(f'(Triggered by commit [{GITHUB_SHA_SHORT}]({GITHUB_REPO_URL}/commit/{GITHUB_SHA}))\n')
            else:
                outf.write(f'(Triggered by commit {GITHUB_SHA_SHORT})\n')

        outf.write("\n")

        outf.write("## Disclaimer\n")
        outf.write(
            "The percentages and the number of categories shown here may not be "
            "completely accurate, because not all categories and functions "
            "are documented yet.\n"
            "\n"
        )

        outf.write(
            f"## Stats ({sum(k.num_fn for k in category_info.values())} functions, {len(category_info)} categories)\n"
        )

        def write_header(title: str, klasses: list[HookCategory]):
            outf.write("\n")
            outf.write(
                f"#### {title} ({len(klasses)}/{num_total_categories}) [{len(klasses) / num_total_categories:.0%}]\n"
            )
            outf.write("\n")

        @contextmanager
        def class_list_spoiler():
            outf.write("<details>\n")
            outf.write("<summary>See list of categories</summary>\n")
            yield
            outf.write("\n</details>\n")

        write_header("Completely reversed categories", completely)
        with class_list_spoiler():
            for cat in completely:
                outf.write(f"- {cat.name} ({cat.num_fn})<br />\n")

        write_header("Partially reversed categories", partially)
        with class_list_spoiler():
            for cat in partially:
                outf.write(
                    f"- {cat.name} ({cat.num_reversed}/{cat.num_fn}) [{1 - cat.num_not_reversed / cat.num_fn:.0%}]<br />\n"
                )

        write_header("Not-at-all reversed categories", not_at_all)
        with class_list_spoiler():
            for cat in not_at_all:
                outf.write(f"- {cat.name} ({cat.num_fn})<br />\n")


if __name__ == "__main__":
    main()
