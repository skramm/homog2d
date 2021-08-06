#!/bin/bash
# gitlog.sh

# run from root folder

set -x

# generate log file
#git log --pretty=format:'%cn %ai' > BUILD/git_log_2.dat
git log --pretty=format:'%ai' > BUILD/git_log_2.dat

# run gnuplot script
misc/plot_gitlog.plt

