#!/usr/bin/env bash

# Just some cut-and-paste subtree commands. Maybe one day it'll be a real script.

git subtree add --prefix dep/ttyd-tools https://github.com/PistonMiner/ttyd-tools master --squash
git subtree pull --prefix dep/ttyd-tools https://github.com/PistonMiner/ttyd-tools master --squash
git subtree push --prefix dep/ttyd-tools git@github.com-complexplane:ComplexPlane/ttyd-tools.git elf2rel-cmake

git subtree add --prefix dep/tinyalloc https://github.com/thi-ng/tinyalloc.git master --squash
