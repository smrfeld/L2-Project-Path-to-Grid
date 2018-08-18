#include "l2_proj_path_to_grid.hpp"

#include <iostream>

using namespace std;
using namespace L2PG;

int main() {

	// Projector
	cout << "> Making projector" << endl;
	int dim_grid = 2; // Dim of the grid
	std::vector<int> no_pts_dim = {11,11}; // 11x11 grid
	int no_pts_path = 20; // data length
	auto proj = Projector(dim_grid,no_pts_dim,no_pts_path);

	// Read grid
	cout << "> Reading grid" << endl;
	proj.read_grid("grid.txt");

	// Read path
	cout << "> Reading path" << endl;
	proj.read_path("path.txt");

	// Get grid point
	cout << "> Get grid point (1,3):" << endl;
	auto grid_pt = proj.get_grid_point({1,3});
	for (auto const &x: grid_pt) {
		cout << x << " ";
	};
	cout << endl;

	// Get idxs of grid idx
	cout << "> Get grid idx of 26:" << endl;
	auto grid_idxs = proj.get_idxs(26);
	for (auto const &i: grid_idxs) {
		cout << i << " ";
	};
	cout << endl;



	return 0;

};