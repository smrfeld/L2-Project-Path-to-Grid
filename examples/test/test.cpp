#include <L2ProjPathToGrid>

#include <iostream>

using namespace std;
using namespace L2PG;

int main() {

	// Dims
	auto d1 = make_shared<Dim>(0.0,1.0,11);
	auto d2 = make_shared<Dim>(0.0,1.0,11);
	vector<shared_ptr<Dim>> dims({d1,d2});

	// Projector
	cout << "> Making projector" << endl;
	auto proj = Projector(dims);

	// Get grid point
	cout << "> Get grid point (1,3):" << endl;
	vector<int> idxs({1,3});
	IdxSet idx_set(dims,idxs);
	shared_ptr<GridPt> grid_pt = proj.get_grid_point(idx_set);
	cout << grid_pt->print_abscissa() << endl;

	// Get linear idx
	cout << "> Linear idx: " << idx_set.get_linear() << endl;

	// Get surrounding pts
	cout << "> Getting surrounding (0.32,0.74):" << endl;
	vector<double> abcissas({0.32,0.74});
	map<IdxSet, shared_ptr<GridPt>> surr = proj.get_surrounding_2(abcissas);
	for (auto const &pr: surr) {
		// Print idx
		cout << pr.first << " = " << pr.second->print_abscissa() << endl;
	};

	// Read path
	// cout << "> Reading path" << endl;
	// proj.read_path("path.txt");

	return 0;

};