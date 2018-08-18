#include <L2ProjPathToGrid>

#include <iostream>

using namespace std;
using namespace L2PG;

int main() {

	// Dims
	auto d1 = make_shared<Dim>(0.0,1.0,11);
	auto d2 = make_shared<Dim>(0.0,1.0,11);
	std::vector<std::shared_ptr<Dim>> dims({d1,d2});

	// Projector
	cout << "> Making projector" << endl;
	auto proj = Projector(dims);

	// Get grid point
	cout << "> Get grid point (1,3):" << endl;
	std::vector<int> idxs({1,3});
	IdxSet idx_set(dims,idxs);
	shared_ptr<GridPt> grid_pt = proj.get_grid_point(idx_set);
	for (auto dim=0; dim<2; dim++) {
		cout << grid_pt->get_abscissa(dim) << " ";
	};
	cout << endl;

	// Get linear idx
	cout << "> Linear idx: " << idx_set.get_linear() << endl;
	
	// Read path
	// cout << "> Reading path" << endl;
	// proj.read_path("path.txt");

	return 0;

};