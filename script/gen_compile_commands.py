#!/usr/bin/env python3

from pathlib import Path
import json


def main():
    cmds = []
    repo_dir = Path(__file__).parent.parent.resolve()
    src_dir = repo_dir / "rel"
    include_dir = repo_dir / "rel" / "include"
    for src_path in src_dir.iterdir():
        if src_path.suffix not in [".cpp"]:
            continue

        command = (
            "/usr/bin/c++ "
            f"-I {include_dir} "
            "-m32 -std=gnu++20 "
            f"-c {src_path} -o {src_path.with_suffix('.o')}"
        )
        cmds.append(
            {
                "directory": str(repo_dir),
                "command": command,
                "file": str(src_path),
            }
        )

    with open(repo_dir / "compile_commands.json", "w") as f:
        json.dump(cmds, f, indent=4)


if __name__ == "__main__":
    main()
