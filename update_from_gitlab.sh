#!/bin/bash
#
# after running this once:
# git remote add upstream ttps://git.libretro.com/libretro/doublecherryGB-libretro
#
# these two lines will update to the latest from gitlab source
git fetch upstream
git merge upstream/master