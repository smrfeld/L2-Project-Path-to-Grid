#include "../include/L2ProjPathToGrid_bits/projector.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt_out.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt.hpp"
#include "../include/L2ProjPathToGrid_bits/data_pt.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include "cmath"

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

		std::string get_fname_path() const;
		void read_path(std::string fname_path);

		std::string get_fname_grid_points() const;
		void read_grid(std::string fname_grid_points);

		/********************
		Get grid points
		********************/

		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(IdxSetKey key) const;

		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSet grid_idxs) const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSetKey key) const;

		/********************
		Get grid points surrounding a point
		********************/

		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_surrounding_2(std::vector<double> abscissas) const;
		Nbr4 get_surrounding_4(std::vector<double> abscissas) const;

		/********************
		Project
		********************/

		void project();

		/********************
		Get solution
		********************/

		std::vector<double> get_solution() const;

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

		/*

		// Clear old
		_data_pts.clear();

		// Open
		std::ifstream f;
		f.open(_fname_path);

		// Make sure we found it
		if (!f.is_open()) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< could not open file: " << _fname_path << std::endl;
			exit(EXIT_FAILURE);
		};

		// Strings for the pts
		std::string y="";
		std::vector<std::string> x;
		for (auto i=0; i<_dim_grid; i++) {
			x.push_back("");
		};

		// Line, etc
		std::string line;
		std::istringstream iss;
		int i_line=0;
		while (getline(f,line)) {

			// Skip empty
			if (line == "") { continue; };

			// Check line no
			if (i_line > _no_pts_data-1) {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< Line no: " << i_line << " is greater than the no pts specified for the path: " << _no_pts_data-1 << std::endl;
				exit(EXIT_FAILURE);
			};

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
			};
			// Ordinate
			iss >> y;
			if (y == "") {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abscissas and an ordinate." << std::endl;
				exit(EXIT_FAILURE);
			};
			// std::cout << y << std::endl;

			// Put into vecs
			for (auto i=0; i<_dim_grid; i++) {
				_path_abscissas[i_line][i] = atof(x[i].c_str());
			};
			_path_ordinates[i_line] = atof(y.c_str());

			// Next line
			i_line++;			

			// Reset line string		
			for (auto i=0; i<_dim_grid; i++) {
				x[i] = "";
			};
			y = "";
		};

		// Check line no
		if (i_line != _no_pts_data) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< Read: " << i_line << " lines but there were supposed to be: " << _no_pts_data << std::endl;
			exit(EXIT_FAILURE);
		};

		*/
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
			_grid_pts[IdxSetKey(grid_pt_idxs,GridPtType::INSIDE,_dims)] = std::make_shared<GridPt>(grid_pt_idxs,abscissas);
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
			IdxSet idxs_of_dims_outside(_dim_grid);
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
			/*
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				std::cout << grid_pt_idxs[dim2] << " ";
			};
			std::cout << std::endl;
			*/

			// Make the abscissa
			std::cout << "---" << std::endl;
			std::vector<double> abscissas;
			for (auto dim2=0; dim2<_dim_grid; dim2++) {
				abscissas.push_back(_dims[dim2]->get_pt_at_idx(grid_pt_idxs[dim2]));
				std::cout << grid_pt_idxs[dim2] << " " << abscissas.back() << std::endl;
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
			_grid_pts_out[IdxSetKey(grid_pt_idxs,GridPtType::OUTSIDE,_dims)] = std::make_shared<GridPtOut>(grid_pt_idxs,abscissas,p1,p2);
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
			std::cout << "Dims outside: ";
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

	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSet grid_idxs) const {
		return _grid_pts.at(IdxSetKey(grid_idxs,GridPtType::INSIDE,_dims));
	};
	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSetKey key) const {
		return _grid_pts.at(key);
	};

	std::shared_ptr<GridPtOut> Projector::Impl::get_grid_point_outside(IdxSet grid_idxs) const {
		return _grid_pts_out.at(IdxSetKey(grid_idxs,GridPtType::OUTSIDE,_dims));
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
			map[IdxSetKey(idxs_local,GridPtType::INSIDE,_dims)] = get_grid_point(idxs_grid_pt);
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
			if (inside) {
				nbr4.types[IdxSetKey(idxs_local,GridPtType::INSIDE,_dims)] = GridPtType::INSIDE;
				nbr4.in[IdxSetKey(idxs_local,GridPtType::INSIDE,_dims)] = get_grid_point(idxs_grid_pt);
			} else {
				nbr4.types[IdxSetKey(idxs_local,GridPtType::OUTSIDE,_dims)] = GridPtType::OUTSIDE;
				nbr4.out[IdxSetKey(idxs_local,GridPtType::OUTSIDE,_dims)] = get_grid_point_outside(idxs_grid_pt);
			};
		};
	};

	/********************
	Project
	********************/

	void Projector::Impl::project() {

	};

	/********************
	Get solution
	********************/

	std::vector<double> Projector::Impl::get_solution() const {
		return {};
	};

	/********************
	Write
	********************/

	void Projector::Impl::write_solution(std::string fname) const {

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

	std::shared_ptr<GridPt> Projector::get_grid_point(std::vector<int> grid_idxs) const {
		return get_grid_point(IdxSet(grid_idxs));
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSet grid_idxs) const {
		return _impl->get_grid_point(grid_idxs);
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSetKey key) const {
		return _impl->get_grid_point(key);
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
	Project
	********************/

	void Projector::project() {
		_impl->project();
	};

	/********************
	Get solution
	********************/

	std::vector<double> Projector::get_solution() const {
		return _impl->get_solution();
	};

	/********************
	Write
	********************/

	void Projector::write_solution(std::string fname) const {
		_impl->write_solution(fname);
	};
};