#include "../include/L2ProjPathToGrid_bits/projector.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt_out.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt.hpp"
#include "../include/L2ProjPathToGrid_bits/data_pt.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include "cmath"

// Armadillo
#include <armadillo>

#define DIAG_PROJ 0

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Dimension
	****************************************/

	Dim::Dim(double start_pt, double end_pt, int no_pts) {
		_start_pt = start_pt;
		_end_pt = end_pt;
		_no_pts = no_pts;
		_spacing = (_end_pt - _start_pt) / (_no_pts - 1.0);
		for (int i=0; i<_no_pts; i++) {
			_pts.push_back(_start_pt + i*_spacing);
		};
	};

	// Accessors
	int Dim::get_no_pts() const {
		return _no_pts;
	};
	double Dim::get_start_pt() const {
		return _start_pt;
	};
	double Dim::get_end_pt() const {
		return _end_pt;
	};
	double Dim::get_spacing() const {
		return _spacing;
	};

	double Dim::get_pt_at_idx(int idx) const {
		if (idx >= 0 && idx <= _no_pts-1) {
			return _pts[idx];
		} else {
			return _start_pt + idx*_spacing;
		};
	};
	std::vector<double> Dim::get_pts() const {
		return _pts;
	};

	std::pair<bool,std::pair<int,int>> Dim::get_surrounding_idxs(double pt) const {
		if (pt < _start_pt || pt > _end_pt) {
			// Outside
			return std::make_pair(false,std::make_pair(0,0));
		};

		int idx;
		if (pt == _end_pt) {
			// At end
			idx = _no_pts-2;
		} else {
			idx = int((pt - _start_pt) / _spacing);
		};

		return std::make_pair(true,std::make_pair(idx,idx+1));
	};

	/****************************************
	Projector implementation
	****************************************/

	class Projector::Impl {
	private:

		// Dimension of the grid
		int _dim_grid;

		// No pts in each dim of the grid
		std::vector<std::shared_ptr<Dim>> _dims;		

		// Size of the grid
		int _no_pts_grid;

		// Grid points
		// std::vector<std::shared_ptr<GridPt>> _grid_pts;
		std::map<IdxSetKey, std::shared_ptr<GridPt>> _grid_pts;

		// Outside grid points
		// std::vector<std::shared_ptr<GridPtOut>> _grid_pts_out;
		std::map<IdxSetKey, std::shared_ptr<GridPtOut>> _grid_pts_out;

		// Length of the data
		int _no_pts_data;

		// Data
		std::vector<std::shared_ptr<DataPt>> _data_pts;

		/********************
		Make grid
		********************/

		void _make_grid_pts();

		void _iterate_make_grid_pt(IdxSet &grid_pt_idxs, int dim);

		/********************
		Make outside grid points
		********************/

		void _make_outside_grid_pts();

		void _iterate_make_grid_pt_outside(IdxSet &grid_pt_idxs, IdxSet &idxs_of_dims_outside, int dim);
		void _iterate_which_dim_are_outside(IdxSet &idxs_of_dims_outside, int idx, int no_dim_outside);

		/********************
		Get surrounding
		********************/

		void _iterate_get_surrounding_2(IdxSet &idxs_local, IdxSet &idxs_lower, IdxSet &idxs_upper, std::map<IdxSetKey, std::shared_ptr<GridPt>> &map, int dim) const;

		void _iterate_get_surrounding_4(IdxSet &idxs_local, IdxSet &idxs_0, IdxSet &idxs_1, IdxSet &idxs_2, IdxSet &idxs_3, Nbr4 &nbr4, int dim) const;

		/********************
		Form the coeffs
		********************/

		// Start with coeff_p = 1.0
		// Dim to interpolate in starts at _grid_dim-1
		void _iterate_form_coeffs(std::map<IdxSetKey, double> &coeffs_store, std::vector<double> &frac_abscissas, int dim_to_interpolate_in, IdxSet &idxs_p, double coeff_p);

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(std::vector<std::shared_ptr<Dim>> dims);
		Impl(const Impl& other);
		Impl(Impl&& other);
		Impl& operator=(const Impl &other);
		Impl& operator=(Impl &&other);
		~Impl();

		/********************
		Get dims
		********************/

		int get_no_dims() const;
		std::vector<std::shared_ptr<Dim>> get_dims() const;

		/********************
		Files
		********************/

		void read_path(std::string fname_path);

		/********************
		Get grid points
		********************/

		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_grid_points() const;
		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(IdxSetKey key) const;

		std::map<IdxSetKey, std::shared_ptr<GridPtOut>> get_grid_points_outside() const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSet grid_idxs) const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSetKey key) const;

		/********************
		Get grid points surrounding a point
		********************/

		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_surrounding_2(std::vector<double> abscissas) const;
		Nbr4 get_surrounding_4(std::vector<double> abscissas) const;

		/********************
		Get data points
		********************/

		std::vector<std::shared_ptr<DataPt>> get_data_points() const;

		/********************
		Project
		********************/

		void project();

		/********************
		Write
		********************/

		void write_solution(std::string fname) const;

	};





















	/****************************************
	Implementation
	****************************************/

	Projector::Impl::Impl(std::vector<std::shared_ptr<Dim>> dims) {
		// Store
		_dim_grid = dims.size();
		_dims = dims;
		_no_pts_grid = 1;
		for (auto const &dim: _dims) {
			_no_pts_grid *= dim->get_no_pts();
		};

		// Make grid
		_make_grid_pts();
		_make_outside_grid_pts();

		// Init
		_no_pts_data = 0;
	};
	Projector::Impl::Impl(const Impl& other) {
		_copy(other);
	};
	Projector::Impl::Impl(Impl&& other) {
		_move(other);
	};
    Projector::Impl& Projector::Impl::operator=(const Impl& other) {
		if (this != &other) {
			_clean_up();
			_copy(other);
		};
		return *this;
    };
    Projector::Impl& Projector::Impl::operator=(Impl&& other) {
		if (this != &other) {
			_clean_up();
			_move(other);
		};
		return *this;
    };
	Projector::Impl::~Impl()
	{
		_clean_up();
	};

	/********************
	Helpers for constructors
	********************/

	void Projector::Impl::_clean_up()
	{
		// Nothing...
	};
	void Projector::Impl::_copy(const Impl& other)
	{
		_dim_grid = other._dim_grid;
		_dims = other._dims;
		_no_pts_grid = other._no_pts_grid;
		_grid_pts = other._grid_pts;
		_grid_pts_out = other._grid_pts_out;
		_no_pts_data = other._no_pts_data;
		_data_pts = other._data_pts;
	};
	void Projector::Impl::_move(Impl& other)
	{
		_copy(other);

		// Reset other
		other._dim_grid = 0;
		other._dims.clear();
		other._no_pts_grid = 0;
		other._grid_pts.clear();
		other._grid_pts_out.clear();
		other._no_pts_data = 0;
		other._data_pts.clear();
	};

	/********************
	Get dims
	********************/

	int Projector::Impl::get_no_dims() const {
		return _dims.size();
	};
	std::vector<std::shared_ptr<Dim>> Projector::Impl::get_dims() const {
		return _dims;
	};

	/********************
	Files
	********************/

	void Projector::Impl::read_path(std::string fname_path) {

		// Check that a grid exists
		if (_grid_pts.size() == 0) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< must read a grid first!" << std::endl;
			exit(EXIT_FAILURE);
		};

		// Read

		// Clear old
		_data_pts.clear();

		// Open
		std::ifstream f;
		f.open(fname_path);

		// Make sure we found it
		if (!f.is_open()) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< could not open file: " << fname_path << std::endl;
			exit(EXIT_FAILURE);
		};

		// Strings for the pts
		std::string y="";
		std::vector<std::string> x;
		for (auto i=0; i<_dim_grid; i++) {
			x.push_back("");
		};

		// Abscissa vector
		std::vector<double> abscissas;
		for (auto i=0; i<_dim_grid; i++) {
			abscissas.push_back(0.0);
		};

		// Line, etc
		Nbr4 nbr4;
		std::map<IdxSetKey, std::shared_ptr<GridPt>> nbr2;
		std::string line;
		std::istringstream iss;
		int i_line=0;
		while (getline(f,line)) {

			// Skip empty
			if (line == "") { continue; };

			// Get
			iss = std::istringstream(line);

			// Abscissas
			for (auto i=0; i<_dim_grid; i++) {
				iss >> x[i];
				if (x[i] == "") {
					std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abscissas and an ordinate." << std::endl;
					exit(EXIT_FAILURE);
				};

				// std::cout << x[i] << " ";

				abscissas[i] = atof(x[i].c_str());
			};
			// Ordinate
			iss >> y;
			if (y == "") {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abscissas and an ordinate." << std::endl;
				exit(EXIT_FAILURE);
			};
			// std::cout << y << std::endl;

			// Get the neighborhood of this pt
			nbr4 = get_surrounding_4(abscissas);

			// Get surrounding 2
			nbr2 = get_surrounding_2(abscissas);

			// Put into vec
			_data_pts.push_back(std::make_shared<DataPt>(abscissas,atof(y.c_str()),nbr2,nbr4));

			// Next line
			i_line++;			

			// Reset line string		
			for (auto i=0; i<_dim_grid; i++) {
				x[i] = "";
			};
			y = "";
		};

		// Set line nos
		_no_pts_data = _data_pts.size();
	};

	/********************
	Make grid
	********************/

	void Projector::Impl::_make_grid_pts() {
		// Clear old
		_grid_pts.clear();

		// Iterate
		IdxSet grid_pt_idxs(_dim_grid);
		_iterate_make_grid_pt(grid_pt_idxs,0);
	};

	void Projector::Impl::_iterate_make_grid_pt(IdxSet &grid_pt_idxs, int dim) {
		if (dim != _dim_grid) {
			// Deeper!
			for (grid_pt_idxs[dim]=0; grid_pt_idxs[dim]<_dims[dim]->get_no_pts(); grid_pt_idxs[dim]++) {
				_iterate_make_grid_pt(grid_pt_idxs, dim+1);
			};
		} else {
			// Do something

			// Make the abscissa
			std::vector<double> abscissas;
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				abscissas.push_back(_dims[dim2]->get_pt_at_idx(grid_pt_idxs[dim2]));
			};

			// Make the grid pt
			_grid_pts[IdxSetKey(grid_pt_idxs,_dims)] = std::make_shared<GridPt>(grid_pt_idxs,abscissas);
		};
	};

	/********************
	Make outside grid points
	********************/

	void Projector::Impl::_make_outside_grid_pts() {
		// Different kinds of outside points:
		// Only 1 dim is outside
		// 2 dims are outside
		// etc

		// Iterate over how many dimensions are outside for each grid point
		// Min = 1
		// Max = _dim_grid
		for (int no_outside=1; no_outside <= _dim_grid; no_outside++) {
			// The idxs of the grid point that are outside
			IdxSet idxs_of_dims_outside(no_outside);
			// Iterate
			_iterate_which_dim_are_outside(idxs_of_dims_outside, 0, no_outside);
		};	
	};

	void Projector::Impl::_iterate_make_grid_pt_outside(IdxSet &grid_pt_idxs, IdxSet &idxs_of_dims_outside, int dim) {

		if (dim != _dim_grid) {
			// Further down!

			// Check if this dim is outside
			if (!idxs_of_dims_outside.find(dim)) {
				// Inside
				// Loop all interior pts
				for (grid_pt_idxs[dim]=0; grid_pt_idxs[dim]<_dims[dim]->get_no_pts(); grid_pt_idxs[dim]++) {
					_iterate_make_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
				};
			} else {
				// Outside
				// Loop just -1 and _dims[dim]->get_no_pts()
				grid_pt_idxs[dim] = -1;
				_iterate_make_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
				grid_pt_idxs[dim] = _dims[dim]->get_no_pts();
				_iterate_make_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
			};

		} else {
			// Do something

			// Print the outside grid pt idxs
			// std::cout << grid_pt_idxs << std::endl;

			// Make the abscissa
			std::vector<double> abscissas;
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				abscissas.push_back(_dims[dim2]->get_pt_at_idx(grid_pt_idxs[dim2]));
			};

			// Find the two pts
			IdxSet p1_idxs(_dim_grid), p2_idxs(_dim_grid);
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				if (grid_pt_idxs[dim2] == -1) {
					p1_idxs[dim2] = grid_pt_idxs[dim2] + 1;
					p2_idxs[dim2] = grid_pt_idxs[dim2] + 2;
				} else if (grid_pt_idxs[dim2] == _dims[dim2]->get_no_pts()) {
					p1_idxs[dim2] = grid_pt_idxs[dim2] - 1;
					p2_idxs[dim2] = grid_pt_idxs[dim2] - 2;
				} else {
					p1_idxs[dim2] = grid_pt_idxs[dim2];
					p2_idxs[dim2] = grid_pt_idxs[dim2];
				};
			};
			std::shared_ptr<GridPt> p1 = get_grid_point(p1_idxs);
			std::shared_ptr<GridPt> p2 = get_grid_point(p2_idxs);

			// Make the outside grid point
			_grid_pts_out[IdxSetKey(grid_pt_idxs,_dims)] = std::make_shared<GridPtOut>(grid_pt_idxs,abscissas,p1,p2);
			// std::cout << "Made outside pt: " << grid_pt_idxs << " = " << _grid_pts_out[IdxSetKey(grid_pt_idxs,GridPtType::OUTSIDE,_dims)]->print_abscissa() << std::endl;
		};
	};

	void Projector::Impl::_iterate_which_dim_are_outside(IdxSet &idxs_of_dims_outside, int idx, int no_dim_outside) {
		if (idx != no_dim_outside) {
			// Further down!

			int idxStart;
			if (idx == 0) {
				idxStart = 0;
			} else {
				idxStart = idxs_of_dims_outside[idx-1]+1; // no duplicates in for loop
				// This makes for loops like:
				// for (i=0; ...) 
				//    for (j = i+1; ....)
				//       ...
			};

			for (idxs_of_dims_outside[idx]=idxStart; idxs_of_dims_outside[idx]<_dim_grid; idxs_of_dims_outside[idx]++) {
				_iterate_which_dim_are_outside(idxs_of_dims_outside,idx+1,no_dim_outside);
			};

		} else {
			// Do something
			
			// Print which dims are outside
			/*
			std::cout << ">>> Dims outside: ";
			for (auto j_outside=0; j_outside<idxs_of_dims_outside.size(); j_outside++) {
				std::cout << idxs_of_dims_outside[j_outside] << " ";
			};
			std::cout << "" << std::endl;
			*/

			// Go over all possible grid points satisfying having this many outside points
			IdxSet grid_pt_idxs(_dim_grid);
			_iterate_make_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,0);
		};
	};

	/********************
	Get grid point
	********************/

	std::map<IdxSetKey, std::shared_ptr<GridPt>> Projector::Impl::get_grid_points() const {
		return _grid_pts;
	};
	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSet grid_idxs) const {
		return _grid_pts.at(IdxSetKey(grid_idxs,_dims));
	};
	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSetKey key) const {
		return _grid_pts.at(key);
	};

	std::map<IdxSetKey, std::shared_ptr<GridPtOut>> Projector::Impl::get_grid_points_outside() const {
		return _grid_pts_out;
	};
	std::shared_ptr<GridPtOut> Projector::Impl::get_grid_point_outside(IdxSet grid_idxs) const {
		return _grid_pts_out.at(IdxSetKey(grid_idxs,_dims));
	};
	std::shared_ptr<GridPtOut> Projector::Impl::get_grid_point_outside(IdxSetKey key) const {
		return _grid_pts_out.at(key);
	};

	/********************
	Get grid points surrounding a point
	********************/

	std::map<IdxSetKey, std::shared_ptr<GridPt>> Projector::Impl::get_surrounding_2(std::vector<double> abscissas) const {
		// Check size
		if (abscissas.size() != _dim_grid) {
			std::cerr << ">>> Error:Projector::Impl::get_surrounding_2 <<< Abscissa size should equal grid size." << std::endl;
			exit(EXIT_FAILURE);
		};

		// Get bounding idxs
		IdxSet idxs_lower(_dim_grid), idxs_upper(_dim_grid);
		std::pair<bool,std::pair<int,int>> bounds;
		for (auto dim=0; dim<_dim_grid; dim++) {
			bounds = _dims[dim]->get_surrounding_idxs(abscissas[dim]);
			if (!bounds.first) {
				// Outside grid
				std::cerr << ">>> Error:Projector::Impl::get_surrounding_2 <<< Abscissa in dim: " << dim << " value: " << abscissas[dim] << " is outside the grid: " << _dims[dim]->get_start_pt() << " to: " << _dims[dim]->get_end_pt() << std::endl;
				exit(EXIT_FAILURE);
			};

			idxs_lower[dim] = bounds.second.first;
			idxs_upper[dim] = bounds.second.second;
		};

		// Iterate to fill out the map
		IdxSet idxs_local(_dim_grid);
		std::map<IdxSetKey, std::shared_ptr<GridPt>> ret;
		_iterate_get_surrounding_2(idxs_local,idxs_lower,idxs_upper,ret,0);

		return ret;
	};

	void Projector::Impl::_iterate_get_surrounding_2(IdxSet &idxs_local, IdxSet &idxs_lower, IdxSet &idxs_upper, std::map<IdxSetKey, std::shared_ptr<GridPt>> &map, int dim) const {
		if (dim != _dim_grid) {
			// Deeper!
			// Can be lower (=0) or higher (=+1) in this dim
			idxs_local[dim] = 0;
			_iterate_get_surrounding_2(idxs_local,idxs_lower,idxs_upper,map,dim+1);
			idxs_local[dim] = 1;
			_iterate_get_surrounding_2(idxs_local,idxs_lower,idxs_upper,map,dim+1);

		} else {
			// Do something

			// Get grid point idxs
			IdxSet idxs_grid_pt(_dim_grid);
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				if (idxs_local[dim2] == 0) {
					idxs_grid_pt[dim2] = idxs_lower[dim2];
				} else if (idxs_local[dim2] == 1) {
					idxs_grid_pt[dim2] = idxs_upper[dim2];
				};
			};

			// Add to map
			map[IdxSetKey(idxs_local,2)] = get_grid_point(idxs_grid_pt);
		};
	};

	Nbr4 Projector::Impl::get_surrounding_4(std::vector<double> abscissas) const {
		// Check size
		if (abscissas.size() != _dim_grid) {
			std::cerr << ">>> Error:Projector::Impl::get_surrounding_4 <<< Abscissa size should equal grid size." << std::endl;
			exit(EXIT_FAILURE);
		};

		// Get bounding idxs
		IdxSet idxs_0(_dim_grid), idxs_1(_dim_grid), idxs_2(_dim_grid), idxs_3(_dim_grid);
		std::pair<bool,std::pair<int,int>> bounds;
		for (auto dim=0; dim<_dim_grid; dim++) {
			bounds = _dims[dim]->get_surrounding_idxs(abscissas[dim]);
			if (!bounds.first) {
				// Outside grid
				std::cerr << ">>> Error:Projector::Impl::get_surrounding_4 <<< Abscissa in dim: " << dim << " value: " << abscissas[dim] << " is outside the grid: " << _dims[dim]->get_start_pt() << " to: " << _dims[dim]->get_end_pt() << std::endl;
				exit(EXIT_FAILURE);
			};

			idxs_1[dim] = bounds.second.first;
			idxs_2[dim] = bounds.second.second;
			idxs_0[dim] = idxs_1[dim]-1;
			idxs_3[dim] = idxs_2[dim]+1;
		};

		// Iterate to fill out the map
		IdxSet idxs_local(_dim_grid);
		Nbr4 ret;
		_iterate_get_surrounding_4(idxs_local,idxs_0,idxs_1,idxs_2,idxs_3,ret,0);

		return ret;
	};	

	void Projector::Impl::_iterate_get_surrounding_4(IdxSet &idxs_local, IdxSet &idxs_0, IdxSet &idxs_1, IdxSet &idxs_2, IdxSet &idxs_3, Nbr4 &nbr4, int dim) const {
		if (dim != _dim_grid) {
			// Deeper!
			// Can be lower (=0,1) or higher (=2,3) in this dim
			idxs_local[dim] = 0;
			_iterate_get_surrounding_4(idxs_local,idxs_0,idxs_1,idxs_2,idxs_3,nbr4,dim+1);
			idxs_local[dim] = 1;
			_iterate_get_surrounding_4(idxs_local,idxs_0,idxs_1,idxs_2,idxs_3,nbr4,dim+1);
			idxs_local[dim] = 2;
			_iterate_get_surrounding_4(idxs_local,idxs_0,idxs_1,idxs_2,idxs_3,nbr4,dim+1);
			idxs_local[dim] = 3;
			_iterate_get_surrounding_4(idxs_local,idxs_0,idxs_1,idxs_2,idxs_3,nbr4,dim+1);

		} else {
			// Do something

			// Get grid point idxs
			IdxSet idxs_grid_pt(_dim_grid);
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				if (idxs_local[dim2] == 0) {
					idxs_grid_pt[dim2] = idxs_0[dim2];
				} else if (idxs_local[dim2] == 1) {
					idxs_grid_pt[dim2] = idxs_1[dim2];
				} else if (idxs_local[dim2] == 2) {
					idxs_grid_pt[dim2] = idxs_2[dim2];
				} else if (idxs_local[dim2] == 3) {
					idxs_grid_pt[dim2] = idxs_3[dim2];
				};
			};

			// Check: is it inside or out?
			bool inside=true;
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				if (idxs_grid_pt[dim2] < 0 || idxs_grid_pt[dim2] > _dims[dim2]->get_no_pts()-1) {
					// Out
					inside = false;
					break;
				};
			};

			// Add to nbr4
			IdxSetKey key(idxs_local,4);
			if (inside) {
				nbr4.types[key] = GridPtType::INSIDE;
				nbr4.in[key] = get_grid_point(idxs_grid_pt);
			} else {
				nbr4.types[key] = GridPtType::OUTSIDE;
				nbr4.out[key] = get_grid_point_outside(idxs_grid_pt);
			};
		};
	};

	/********************
	Get data points
	********************/

	std::vector<std::shared_ptr<DataPt>> Projector::Impl::get_data_points() const {
		return _data_pts;
	};

	/********************
	Project
	********************/

	void Projector::Impl::project() {

		if (DIAG_PROJ) {
			std::cout << ">>> Projector::Impl::project <<<" << std::endl;
		};

		// Entries in the vec/matrix
		std::map<IdxSetKey, std::map<IdxSetKey,double> > a_matr;
		std::map<IdxSetKey, double> b_vec;

		// Global coeff map
		std::map<IdxSetKey, double> global_coeffs;

		// Declarations
		std::vector<double> abscissas,abscissas_next,frac_abscissas;
		double path;
		std::map<IdxSetKey, double> local_coeffs;
		std::shared_ptr<DataPt> data_pt, data_pt_next;
		Nbr4 nbr4;
		double coeff;
		IdxSet idxs_local(_dim_grid);
		IdxSet idxs_global(_dim_grid);
		IdxSet idxs_dep_1(_dim_grid),idxs_dep_2(_dim_grid);
		std::vector<IdxSetKey> keys;
		double xk,xl;

		// Go through all data points except the last
		for (auto i_pt=0; i_pt<_data_pts.size()-1; i_pt++) {
			if (DIAG_PROJ) {
				std::cout << ">>> Projector::Impl::project <<< Doing pt: " << i_pt << " / " << _data_pts.size()-2 << std::endl;
			};

			data_pt = _data_pts[i_pt];
			data_pt_next = _data_pts[i_pt+1];

			// Get abscissas
			abscissas = data_pt->get_abscissas();
			abscissas_next = data_pt_next->get_abscissas();

			// Calculate path element
			path = 0.0;
			for (auto dim=0; dim<_dim_grid; dim++) {
				path += pow(abscissas_next[dim]-abscissas[dim],2);
			};
			path = sqrt(path);

			if (DIAG_PROJ) {
				std::cout << ">>> Projector::Impl::project <<<     Calculated path length: " << path << std::endl;
			};

			// Get frac abscissas
			frac_abscissas = data_pt->get_frac_abscissas();

			// Iterate such that local_coeffs holds the local map of coeffs for the surrounding nbr4
			local_coeffs.clear();
			_iterate_form_coeffs(local_coeffs,frac_abscissas,_dim_grid-1,idxs_local,1.0);

			if (DIAG_PROJ) {
				std::cout << ">>> Projector::Impl::project <<<     Local coeffs:" << std::endl;
				for (auto &pr: local_coeffs) {
					std::cout << "                             " << pr.first << " " << pr.second << std::endl;
				};
			};

			// Now grab the nbr4
			nbr4 = data_pt->get_nbr4();

			// Go through each point; add INSIDE to the global map
			// Skip OUTSIDE
			global_coeffs.clear();
			for (auto &pr: nbr4.types) {
				if (pr.second == GridPtType::INSIDE) {
					// Get global idxs
					idxs_global = nbr4.in[pr.first]->get_idxs();

					// Get coeff
					coeff = local_coeffs[pr.first];

					// std::cout << "Getting coeff of local idx: " << pr.first << " : " << coeff << std::endl;

					// Add to global map
					global_coeffs[IdxSetKey(idxs_global,_dims)] = coeff;
				};
			};

			// Corrections for OUTSIDE pts
			for (auto &pr: nbr4.types) {
				if (pr.second == GridPtType::OUTSIDE) {
					// Need to correct the coeffs

					// Get the coeff from the local map
					coeff = local_coeffs[pr.first];

					// Get the idxs of the two dep pts
					idxs_dep_1 = nbr4.out[pr.first]->get_dep_p1()->get_idxs();
					idxs_dep_2 = nbr4.out[pr.first]->get_dep_p2()->get_idxs();

					// Adjust coeffs
					global_coeffs[IdxSetKey(idxs_dep_1,_dims)] += 2.0 * coeff;
					global_coeffs[IdxSetKey(idxs_dep_2,_dims)] -= coeff;
				};
			};

			if (DIAG_PROJ) {
				std::cout << ">>> Projector::Impl::project <<<     Got coeffs for all neighborhood pts" << std::endl;
			};

			// All keys in global_coeffs
			keys.clear();
			for (auto &pr: global_coeffs) {
				keys.push_back(pr.first);	
			};

			if (DIAG_PROJ) {
				std::cout << ">>> Projector::Impl::project <<<     Adding to matrix,vector...." << std::endl;
			};

			// Now add to the matrix/vector
			for (auto k=0; k<keys.size(); k++) {
				IdxSetKey idx_k = keys[k];
				xk = global_coeffs[idx_k];

				// Add to vec
				auto it = b_vec.find(idx_k);
				if (it == b_vec.end()) {
					b_vec[idx_k] = xk * data_pt->get_ordinate() * path;
				} else {
					b_vec[idx_k] += xk * data_pt->get_ordinate() * path;
				};

				for (auto l=k; l<keys.size(); l++) {
					IdxSetKey idx_l = keys[l];
					xl = global_coeffs[idx_l];

					// Add to matr
					auto it1 = a_matr.find(idx_k);
					if (it1 == a_matr.end()) {
						a_matr[idx_k][idx_l] = xk * xl * path;
						if (k!=l) {
							a_matr[idx_l][idx_k] = xk * xl * path;
						};
					} else {
						auto it2 = it1->second.find(idx_k);
						if (it2 == it1->second.end()) {
							a_matr[idx_k][idx_l] = xk * xl * path;
							if (k!=l) {
								a_matr[idx_l][idx_k] = xk * xl * path;
							};
						} else {
							a_matr[idx_k][idx_l] += xk * xl * path;
							if (k!=l) {
								a_matr[idx_l][idx_k] += xk * xl * path;
							};
						};
					};
				};
			};
		};

		if (DIAG_PROJ) {
			std::cout << ">>> Projector::Impl::project <<< Done looping through data." << std::endl;
			std::cout << ">>> Projector::Impl::project <<< The A matrix size is: " << a_matr.size() << std::endl;
			std::cout << ">>> Projector::Impl::project <<< The b vector size is: " << b_vec.size() << std::endl;
			std::cout << ">>> Projector::Impl::project <<< Now constructing the armadillo objects!" << std::endl;
		};

		// Get size
		int n = a_matr.size();
		if (a_matr.size() != b_vec.size()) {
			std::cerr << ">>> Error: Projector::Impl::project <<< b vec size = " << b_vec.size() << " but A matrix size = " << a_matr.size() << std::endl;
			exit(EXIT_FAILURE);
		};

		// arma::sp_mat A(n,n);
		arma::mat A(n,n);
		arma::vec b(n),x(n);

		// Grab all the keys
		std::vector<IdxSetKey> keys2;
		for (auto &pr: b_vec) {
			keys2.push_back(pr.first);
		};

		// Make the matrix,vector
		for (auto i=0; i<keys2.size(); i++) {

			// vec
			b(i) = b_vec[keys2[i]];

			for (auto j=0; j<keys2.size(); j++) {

				// matr
				A(i,j) = a_matr[keys2[i]][keys2[j]];

			};
		};

		if (DIAG_PROJ) {
			A.print("A:");
			b.print("b:");
			std::cout << ">>> Projector::Impl::project <<< Solving..." << std::endl;
		};

		// Solve
		// x = spsolve(A, b, "lapack");
		x = solve(A,b);

		if (DIAG_PROJ) {
			std::cout << ">>> Projector::Impl::project <<< Done!" << std::endl;
			x.print("x:");
			std::cout << ">>> Projector::Impl::project <<< Next: setting in grid" << std::endl;
		};

		// Set solution in the grid
		for (auto &pr: _grid_pts) {
			auto it = std::find(keys2.begin(), keys2.end(), pr.first);
			if (it == keys2.end()) {
				// Not found -> 0
				pr.second->set_ordinate(0.0);
			} else {
				// Found; set from x
				int i = it - keys2.begin();
				pr.second->set_ordinate(x(i));
			};
		};

		if (DIAG_PROJ) {
			std::cout << ">>> Projector::Impl::project <<< Finished!" << std::endl;
		};
	};

	void Projector::Impl::_iterate_form_coeffs(std::map<IdxSetKey, double> &coeffs_store, std::vector<double> &frac_abscissas, int dim_to_interpolate_in, IdxSet &idxs_p, double coeff_p) {

		if (dim_to_interpolate_in != -1) {
			// Deeper!

			// Grab the pt in this dim
			double x = frac_abscissas[dim_to_interpolate_in];

			// p0
			idxs_p[dim_to_interpolate_in] = 0;		
			_iterate_form_coeffs( coeffs_store, frac_abscissas, dim_to_interpolate_in-1, idxs_p, coeff_p*(-0.5*x+pow(x,2)-0.5*pow(x,3)) );

			// p1
			idxs_p[dim_to_interpolate_in] = 1;		
			_iterate_form_coeffs( coeffs_store, frac_abscissas, dim_to_interpolate_in-1, idxs_p, coeff_p*(1.0-2.5*pow(x,2)+1.5*pow(x,3)) );

			// p2
			idxs_p[dim_to_interpolate_in] = 2;		
			_iterate_form_coeffs( coeffs_store, frac_abscissas, dim_to_interpolate_in-1, idxs_p, coeff_p*(0.5*x + 2.0 * pow(x,2) -1.5*pow(x,3)) );

			// p3
			idxs_p[dim_to_interpolate_in] = 3;		
			_iterate_form_coeffs( coeffs_store, frac_abscissas, dim_to_interpolate_in-1, idxs_p, coeff_p*(-0.5*pow(x,2)+0.5*pow(x,3)) );

		} else {
			// Do something

			// Print the idx and coeff
			/*
			std::cout << idxs_p << " coeff: " << coeff_p << std::endl;
			*/

			// Add to the map
			coeffs_store[IdxSetKey(idxs_p,4)] = coeff_p;
		};
	};

	/********************
	Write
	********************/

	void Projector::Impl::write_solution(std::string fname) const {
		std::ofstream f;

		// Open
		f.open(fname);

		// Make sure we found it
		if (!f.is_open()) {
			std::cerr << ">>> Error: Projector::Impl::write_solution <<< could not write to file: " << fname << std::endl;
			exit(EXIT_FAILURE);
		};

		// Go through all grid pts
		std::vector<double> abscissas;
		double ordinate;
		int i_count = 0;
		for (auto &pr: _grid_pts) {

			// Get
			abscissas = pr.second->get_abscissas();
			ordinate = pr.second->get_ordinate();

			// Write
			for (auto dim=0; dim<_dim_grid; dim++) {
				f << abscissas[dim] << " ";
			};
			f << ordinate;

			if (i_count != _grid_pts.size()-1) {
				f << "\n";
			};
			i_count++;
		};

		// Close
		f.close();
	};






























	/****************************************
	Impl forwards
	****************************************/

	/********************
	Constructor
	********************/

	Projector::Projector(std::vector<std::shared_ptr<Dim>> dims) : _impl(new Impl(dims)) {};
	Projector::Projector(const Projector& other) : _impl(new Impl(*other._impl)) {};
	Projector::Projector(Projector&& other) : _impl(std::move(other._impl)) {};
	Projector& Projector::operator=(const Projector &other) {
        _impl.reset(new Impl(*other._impl));
        return *this; 
	};
	Projector& Projector::operator=(Projector &&other) {
        _impl = std::move(other._impl);
        return *this; 
	};
	Projector::~Projector() = default;

	/********************
	Get dims
	********************/

	int Projector::get_no_dims() const {
		return _impl->get_no_dims();
	};
	std::vector<std::shared_ptr<Dim>> Projector::get_dims() const {
		return _impl->get_dims();
	};

	/********************
	Files
	********************/

	void Projector::read_path(std::string fname_path) {
		_impl->read_path(fname_path);
	};

	/********************
	Get grid pts
	********************/

	std::map<IdxSetKey, std::shared_ptr<GridPt>> Projector::get_grid_points() const {
		return _impl->get_grid_points();
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(std::vector<int> grid_idxs) const {
		return get_grid_point(IdxSet(grid_idxs));
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSet grid_idxs) const {
		return _impl->get_grid_point(grid_idxs);
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSetKey key) const {
		return _impl->get_grid_point(key);
	};

	std::map<IdxSetKey, std::shared_ptr<GridPtOut>> Projector::get_grid_points_outside() const {
		return _impl->get_grid_points_outside();
	};
	std::shared_ptr<GridPtOut> Projector::get_grid_point_outside(std::vector<int> grid_idxs) const {
		return _impl->get_grid_point_outside(IdxSet(grid_idxs));
	};
	std::shared_ptr<GridPtOut> Projector::get_grid_point_outside(IdxSet grid_idxs) const {
		return _impl->get_grid_point_outside(grid_idxs);
	};
	std::shared_ptr<GridPtOut> Projector::get_grid_point_outside(IdxSetKey key) const {
		return _impl->get_grid_point_outside(key);
	};

	/********************
	Get grid points surrounding a point
	********************/

	std::map<IdxSetKey, std::shared_ptr<GridPt>> Projector::get_surrounding_2(std::vector<double> abscissas) const {
		return _impl->get_surrounding_2(abscissas);
	};
	Nbr4 Projector::get_surrounding_4(std::vector<double> abscissas) const {
		return _impl->get_surrounding_4(abscissas);
	};

	/********************
	Get data points
	********************/

	std::vector<std::shared_ptr<DataPt>> Projector::get_data_points() const {
		return _impl->get_data_points();
	};

	/********************
	Project
	********************/

	void Projector::project() {
		_impl->project();
	};

	/********************
	Write
	********************/

	void Projector::write_solution(std::string fname) const {
		_impl->write_solution(fname);
	};
};