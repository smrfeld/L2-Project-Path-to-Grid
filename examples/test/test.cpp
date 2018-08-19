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
	IdxSet idx_set(idxs);
	shared_ptr<GridPt> grid_pt = proj.get_grid_point(idx_set);
	cout << grid_pt->print_abscissa() << endl;

	// Get linear idx
	cout << "> Linear idx of (1,3): " << IdxSetKey(IdxSet({1,3}),dims).get_linear() << endl;
	cout << "> Linear idx of (-1,0): " << IdxSetKey(IdxSet({-1,0}),dims).get_linear() << endl;

	// Get surrounding 2 pts
	cout << "> Getting surrounding 2 (0.02,0.74):" << endl;
	vector<double> abcissas({0.02,0.74});
	map<IdxSetKey, shared_ptr<GridPt>> surr = proj.get_surrounding_2(abcissas);
	for (auto const &pr: surr) {
		// Print idx
		cout << pr.first << " = " << pr.second->print_abscissa() << endl;
	};

	// Get surrounding 4 pts
	cout << "> Getting surrounding 4 (0.32,0.74):" << endl;
	Nbr4 surr4 = proj.get_surrounding_4(abcissas);
	for (auto const &pr: surr4.types) {
		// Print idx
		if (pr.second == GridPtType::INSIDE) {
			cout << pr.first << " = INSIDE = ";
			cout << surr4.in[pr.first]->print_abscissa() << endl;
		} else {
			cout << pr.first << " = OUTSIDE = ";
			cout << surr4.out[pr.first]->print_abscissa() << endl;
		};
	};

	// Get surrounding 4 pts
	cout << "> Getting surrounding 4 (0.02,0.74):" << endl;
	abcissas = {0.02,0.74};
	surr4 = proj.get_surrounding_4(abcissas);
	for (auto const &pr: surr4.types) {
		// Print idx
		if (pr.second == GridPtType::INSIDE) {
			cout << pr.first << " = INSIDE = ";
			cout << surr4.in[pr.first]->print_abscissa() << endl;
		} else {
			cout << pr.first << " = OUTSIDE = ";
			cout << surr4.out[pr.first]->print_abscissa() << endl;
		};
	};

	// Read path
	cout << "> Reading path" << endl;
	proj.read_path("path.txt");

	// Write solution
	cout << "> Writing solution" << endl;
	proj.write_solution("solution.txt");


	return 0;

};