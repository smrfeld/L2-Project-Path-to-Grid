#include <L2ProjPathToGrid>

#include <iostream>

using namespace std;
using namespace L2PG;

int main() {

	/****************************************
	Setup
	****************************************/

	// Dims
	auto d1 = make_shared<Dim>(0.0,1.0,21);
	auto d2 = make_shared<Dim>(0.0,1.0,21);
	vector<shared_ptr<Dim>> dims({d1,d2});

	// Projector
	cout << "> Making projector" << endl;
	auto proj = Projector(dims);

	/****************************************
	Get a grid point
	****************************************/

	/*
	cout << "> Get grid point (1,3):" << endl;
	vector<int> idxs({1,3});
	IdxSet idx_set(idxs);
	shared_ptr<GridPt> grid_pt = proj.get_grid_point(idx_set);
	cout << grid_pt->print_abscissa() << endl;
	*/

	/****************************************
	Each grid point has a unique linear idx
	****************************************/

	/*
	cout << "> Linear idx of (1,3): " << IdxSetKey(IdxSet({1,3}),dims).get_linear() << endl;
	cout << "> Linear idx of (-1,0): " << IdxSetKey(IdxSet({-1,0}),dims).get_linear() << endl;
	*/

	/****************************************
	Get nearest 2 grid points surrounding some arbitrary point
	****************************************/

	/*
	cout << "> Getting surrounding 2 (0.02,0.74):" << endl;
	vector<double> abcissas({0.02,0.74});
	map<IdxSetKey, shared_ptr<GridPt>> surr = proj.get_surrounding_2(abcissas);
	for (auto const &pr: surr) {
		// Print idx
		cout << pr.first << " = " << pr.second->print_abscissa() << endl;
	};
	*/

	/****************************************
	Get nearest 4 grid points surrounding some arbitrary point
	****************************************/

	/*
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
	*/

	/*
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
	*/

	/****************************************
	Read a path
	****************************************/

	cout << "> Reading path" << endl;
	proj.read_path("path.txt");

	/****************************************
	Get the data point and explore properties
	****************************************/

	cout << "> Getting data" << endl;
	std::vector<std::shared_ptr<DataPt>> data_pts = proj.get_data_points();
	cout << data_pts.size() << endl;

	cout << "> Get neighborhood (nearest 4) of a point" << endl;
	Nbr4 nbr4 = data_pts[0]->get_nbr4();

	cout << "> Making lines in the neighborhood in dimension 1:" << endl;
	int line_dim = 1;
	std::vector<Line4> lines = nbr4.get_grid_pt_lines_4(line_dim,dims.size());
	for (auto &line: lines) {
		cout << line.print() << endl;
	};

	/****************************************
	Project
	****************************************/

	// Try projector
	cout << "> Projecting:" << endl;
	proj.project();

	/****************************************
	Write solution
	****************************************/

	// Write solution
	cout << "> Writing solution" << endl;
	proj.write_solution("solution.txt");

	return 0;

};