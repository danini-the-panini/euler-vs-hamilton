Euler vs Hamilton
=================

Round 1 - Fight!

An investigation on the differences in numerical stability between the use of Euler angles and Hamilton quaternions for rotation

Usage
-----

Make sure you have [glfw](http://www.glfw.org/) and [glew](http://glew.sourceforge.net/) installed.

Run `make` from within this folder.

Run `./main`. Command line options are available, namely `-o FILE` and `-i FILE` to write or read recorded user input from or to `FILE` respectively.

Accumulated error and computation time (in seconds) are output to `STDOUT`. The first four columns represent error, the last four columns represent computation time. Columns are listed in the order of Rotation Matrix, Orthonormalised Rotation Matrix, Quaternion, Orthonormalised Quaternion.

Extra Scripts
-------------

The script `calculate_performance.rb` takes in the input from a run and outputs the average number of milliseconds spent per frame for each implementation.

The script `calculate_wins.rb` takes in the input from a run and outputs the number of frames in which each implementation was "winning", i.e. exhibited the least error.

The scripts plot_stability and plot_timing can be fed into [gnuplot]() to generate plots of error and timing respectively. Use a plot script as follows:

    gnuplot -e '-e "IN_FILE"' > OUT_FILE

Some example input and corresponding output can be found under `tests`. Please note that output will vary between executions and platforms.
