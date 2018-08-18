#include <L2ProjPathToGrid>

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
	// cout << "> Reading path" << endl;
	// proj.read_path("path.txt");

	// Get grid point
	cout << "> Get grid point (1,3):" << endl;
	shared_ptr<GridPt> grid_pt = proj.get_grid_point({1,3});
	for (auto dim=0; dim<dim_grid; dim++) {
		cout << grid_pt->get_abscissa(dim) << " ";
	};
	cout << endl;

	// Get idxs of grid idx
	cout << "> Get grid idx of 26:" << endl;
	IdxSet grid_idxs = proj.get_idxs(26);
	for (auto i=0; i<grid_idxs.size(); i++) {
		cout << grid_idxs[i] << " ";
	};
	cout << endl;


	return 0;

};