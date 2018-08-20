#include <L2ProjPathToGrid>

#include <iostream>
#include <sstream>

using namespace std;
using namespace L2PG;

std::string title(std::string text) {
	std::ostringstream s;
	s << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	s << "> " << text << endl;
	s << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	return s.str();
};

int main() {

	/****************************************
	Setup
	****************************************/

	cout << title("Making projector") << endl;

	// Dims
	auto d1 = make_shared<Dim>(0.0,1.0,41);
	auto d2 = make_shared<Dim>(0.0,1.0,41);
	vector<shared_ptr<Dim>> dims({d1,d2});

	// Projector
	auto proj = Projector(dims);

	/****************************************
	Get a grid point
	****************************************/

	cout << title("Get grid point (1,3):") << endl;

	vector<int> idxs({1,3});
	IdxSet idx_set(idxs);
	shared_ptr<GridPt> grid_pt = proj.get_grid_point(idx_set);
	cout << grid_pt->print_abscissa() << endl;

	/****************************************
	Each grid point has a unique linear idx
	****************************************/

	cout << title("Linear idx of (1,3):") << endl;

	cout << IdxSetKey(IdxSet({1,3}),dims).get_linear() << endl;

	cout << title("Linear idx of (-1,0):") << endl;

	cout << IdxSetKey(IdxSet({-1,0}),dims).get_linear() << endl;

	/****************************************
	Get nearest 2 grid points surrounding some arbitrary point
	****************************************/

	cout << title("Getting surrounding 2 (0.02,0.74):") << endl;

	vector<double> abcissas({0.02,0.74});
	map<IdxSetKey, shared_ptr<GridPt>> surr = proj.get_surrounding_2(abcissas);
	for (auto const &pr: surr) {
		// Print idx
		cout << pr.first << " = " << pr.second->print_abscissa() << endl;
	};

	/****************************************
	Get nearest 4 grid points surrounding some arbitrary point
	****************************************/

	cout << title("Getting surrounding 4 (0.32,0.74):") << endl;

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

	cout << title("Getting surrounding 4 (0.02,0.74):") << endl;

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

	/****************************************
	Read a path
	****************************************/

	cout << title("Read path") << endl;

	proj.read_path("path.txt");

	/****************************************
	Get the data point and explore properties
	****************************************/

	cout << title("Get data") << endl;

	std::vector<std::shared_ptr<DataPt>> data_pts = proj.get_data_points();
	cout << data_pts.size() << endl;

	cout << title("Get neighborhood (nearest 4) of a point") << endl;

	Nbr4 nbr4 = data_pts[0]->get_nbr4();

	cout << title("Making lines in the neighborhood in dimension 1") << endl;

	int line_dim = 1;
	std::vector<Line4> lines = nbr4.get_grid_pt_lines_4(line_dim,dims.size());
	for (auto &line: lines) {
		cout << line.print() << endl;
	};

	/****************************************
	Project
	****************************************/

	cout << title("Project") << endl;

	proj.project();

	/****************************************
	Write solution
	****************************************/

	cout << title("Writing solution") << endl;

	proj.write_solution("solution.txt");

	return 0;

};