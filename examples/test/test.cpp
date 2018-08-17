#include "l2_proj_path_to_grid.hpp"

using namespace std;
using namespace L2PG;

int main() {

	auto proj = Projector(2,121,20);

	proj.read_grid_points("grid_points.txt");

	return 0;

};