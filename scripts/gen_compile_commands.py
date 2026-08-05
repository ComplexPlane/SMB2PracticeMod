#!/usr/bin/env python3

from pathlib import Path
import json


def main():
    cmds = []
    repo_dir = Path(__file__).parent.parent.resolve()
    src_dir = repo_dir / "src"
    include_dir = repo_dir / "src"
    for src_path in src_dir.glob("**/*.c*"):
        compiler = "/usr/bin/cc" if src_path.suffix == ".c" else "/usr/bin/c++"
        cxx_version = "c23" if src_path.suffix == ".c" else "gnu++20"
        command = (
            f"{compiler} "
            f"-I {include_dir} "
            f"-m32 -std={cxx_version} "
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
