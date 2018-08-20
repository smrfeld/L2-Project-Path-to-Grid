# L2 Project Path to Grid

L2 project a path to a regular grid in arbitrary dimension, using cubic splines in the grid.

## Install

Use the makefile: `make` and `make install`.

## Compilation

Link against the library like this: `g++ -std=c++14 -lL2ProjPathToGrid test.cpp -o test.o`

## Example

See `examples/test` directory.

Generate a path of data:

![Path](examples/test/figures/path.png "Path")

After running `project()`, the grid obtained is:

![Grid](examples/test/figures/grid.png "Grid")

Interpolating in this grid at a higher resolution (see Mathematica notebook `analyze`):d

![GridResampled](examples/test/figures/grid_resampled.png "Grid Resampled")

Along the path, the result is identical to the original path:

![PathInterp](examples/test/figures/path_interp.png "Path Interpolated")