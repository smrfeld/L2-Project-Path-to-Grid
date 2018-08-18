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
		return _pts[idx];
	};
	std::vector<double> Dim::get_pts() const {
		return _pts;
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
		std::map<IdxSet, std::shared_ptr<GridPt>> _grid_pts;

		// Outside grid points
		// std::vector<std::shared_ptr<GridPtOut>> _grid_pts_out;
		std::map<IdxSet, std::shared_ptr<GridPtOut>> _grid_pts_out;

		// Length of the data
		int _no_pts_data;

		// Data
		std::vector<std::shared_ptr<DataPt>> _data_pts;

		/********************
		Make grid
		********************/

		void _make_grid();

		void _iterate_grid_pt(IdxSet &grid_pt_idxs, int dim);

		/********************
		Make outside grid points
		********************/

		void _make_outside_grid_pts();

		void _iterate_grid_pt_outside(IdxSet &grid_pt_idxs, IdxSet &idxs_of_dims_outside, int dim);
		void _iterate_which_dim_are_outside(IdxSet &idxs_of_dims_outside, int idx, int no_dim_outside);

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
		Files
		********************/

		std::string get_fname_path() const;
		void read_path(std::string fname_path);

		std::string get_fname_grid_points() const;
		void read_grid(std::string fname_grid_points);

		/********************
		Get indexes
		********************/

		// Input: idx in each dim
		int get_idx(IdxSet grid_idxs) const;
		// Input: idx in the actual 1D vector
		IdxSet get_idxs(int grid_idx) const; 

		/********************
		Get grid points
		********************/

		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;

		/********************
		Get neighbors
		********************/

		std::vector<std::shared_ptr<GridPt>> get_neighbors(IdxSet grid_idxs) const;

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
		_make_grid();

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
	Files
	********************/

	void Projector::Impl::read_path(std::string fname_path) {

		// Check that a grid exists
		if (_fname_grid_points == "") {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< must read a grid first!" << std::endl;
			exit(EXIT_FAILURE);
		};

		// Set
		_fname_path = fname_path;

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
					std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abcissas and an ordinate." << std::endl;
					exit(EXIT_FAILURE);
				};

				// std::cout << x[i] << " ";
			};
			// Ordinate
			iss >> y;
			if (y == "") {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abcissas and an ordinate." << std::endl;
				exit(EXIT_FAILURE);
			};
			// std::cout << y << std::endl;

			// Put into vecs
			for (auto i=0; i<_dim_grid; i++) {
				_path_abcissas[i_line][i] = atof(x[i].c_str());
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

	void Projector::Impl::_make_grid() {
		// Read

		// Clear old
		_grid_pts.clear();

		// Open
		std::ifstream f;
		f.open(_fname_grid_points);

		// Make sure we found it
		if (!f.is_open()) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< could not open file: " << _fname_grid_points << std::endl;
			exit(EXIT_FAILURE);
		};

		// Strings for the pts
		std::vector<std::string> x;
		for (auto i=0; i<_dim_grid; i++) {
			x.push_back("");
		};

		// For making the grid pts
		IdxSet idxs(_dim_grid);
		std::vector<double> abcissas;
		for (auto i=0; i<_dim_grid; i++) {
			abcissas.push_back(0.0);
		};

		// Line, etc
		std::string line;
		std::istringstream iss;
		int i_line=0;
		while (getline(f,line)) {

			// Skip empty
			if (line == "") { continue; };

			// Check line no
			if (i_line > _no_pts_grid-1) {
				std::cerr << ">>> Error: Projector::Impl::read_grid <<< Line no: " << i_line << " is greater than the no pts specified for the grid: " << _no_pts_grid-1 << std::endl;
				exit(EXIT_FAILURE);
			};

			// Get
			iss = std::istringstream(line);

			// Grid pts
			for (auto i=0; i<_dim_grid; i++) {
				// String
				iss >> x[i];
				if (x[i] == "") {
					std::cerr << ">>> Error: Projector::Impl::read_grid <<< No pts specified seems incorrect - should be: " << _dim_grid << " values." << std::endl;
					exit(EXIT_FAILURE);
				};
				// Val
				abcissas[i] = atof(x[i].c_str());
			};

			// Get idx set
			idxs = get_idxs(i_line);

			// Put into vec
			_grid_pts[idxs] = std::make_shared<GridPt>(idxs, abcissas);

			// Next line
			i_line++;			

			// Reset line string		
			for (auto i=0; i<_dim_grid; i++) {
				x[i] = "";
			};
		};

		// Check line no
		if (i_line != _no_pts_grid) {
			std::cerr << ">>> Error: Projector::Impl::read_grid <<< Read: " << i_line << " lines but there were supposed to be: " << _no_pts_grid << std::endl;
			exit(EXIT_FAILURE);
		};

		// Next: make the outside grid points
		_make_outside_grid();
	};

	/********************
	Make outside grid points
	********************/

	void Projector::Impl::_make_outside_grid() {
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

	void Projector::Impl::_iterate_grid_pt_outside(IdxSet &grid_pt_idxs, IdxSet &idxs_of_dims_outside, int dim) {

		if (dim != _dim_grid) {
			// Further down!

			// Check if this dim is outside
			auto it = std::find(idxs_of_dims_outside.idxs.begin(),idxs_of_dims_outside.idxs.end(),dim);
			if (it == idxs_of_dims_outside.idxs.end()) {
				// Inside
				// Loop all interior pts
				for (grid_pt_idxs[dim]=0; grid_pt_idxs[dim]<_dims[dim]->get_no_pts(); grid_pt_idxs[dim]++) {
					_iterate_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
				};
			} else {
				// Outside
				// Loop just -1 and _dims[dim]->get_no_pts()
				grid_pt_idxs[dim] = -1;
				_iterate_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
				grid_pt_idxs[dim] = _dims[dim]->get_no_pts();
				_iterate_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,dim+1);
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

			// Determine the abscissa from the idxs


			// Make the outside grid point

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
			_iterate_grid_pt_outside(grid_pt_idxs,idxs_of_dims_outside,0);
		};
	};

	/********************
	Get indexes
	********************/

	// Input: idx in each dim
	int Projector::Impl::get_idx(IdxSet grid_idxs) const {
		// Check that no idxs = no dim
		if (grid_idxs.size() != _dim_grid) {
			std::cerr << ">>> Error: Projector::Impl::get_grid_point <<< No indexes should be equal to dimension of the grid: " << _dim_grid << std::endl;
			exit(EXIT_FAILURE);
		};

		// Check that each index does not go above the max dim
		for (auto dim=0; dim<grid_idxs.size(); dim++) {
			if (grid_idxs[dim] > _dims[dim]->get_no_pts()-1) {
				std::cerr << ">>> Error: Projector::Impl::get_grid_point <<< Index specified: " << grid_idxs[dim] << " in dim: " << dim << " should be less than: " << _dims[dim]->get_no_pts()-1 << std::endl;
				exit(EXIT_FAILURE);
			};
		};

		// Find gridpoint by index
		int grid_idx=0,add=0;
		for (auto dim=0; dim<_dim_grid; dim++) {
			add = grid_idxs[dim];
			for (auto dim2=dim+1; dim2<_dim_grid; dim2++) {
				add *= _dims[dim2]->get_no_pts();
			};
			grid_idx += add;
		};

		return grid_idx;
	};

	// Input: idx in the actual 1D vector
	IdxSet Projector::Impl::get_idxs(int grid_idx) const {
		// Check that idx is not too great
		if (grid_idx > _no_pts_grid-1) {
			std::cerr << ">>> Error: Projector::Impl::get_idxs <<< Max idx is: " << _no_pts_grid-1 << " but specified was: " << grid_idx << std::endl;
			exit(EXIT_FAILURE);
		};

		// Determine the idxs
		IdxSet grid_idxs(_dim_grid);
		int pwr;
		int grid_idx_working = grid_idx;
		for (auto dim=0; dim<_dim_grid; dim++) {
			pwr = 1;
			for (auto dim2=dim+1; dim2<_dim_grid; dim2++) {
				pwr *= _dims[dim2]->get_no_pts();
			};

			// Add
			grid_idxs[dim] = int(grid_idx_working/pwr);

			// Remove
			grid_idx_working -= grid_idxs.idxs.back()*pwr;
		};

		return grid_idxs;
	};

	/********************
	Get grid point
	********************/

	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSet grid_idxs) const {
		return _grid_pts.at(grid_idxs);
	};

	/********************
	Get neighbors
	********************/

	std::vector<std::shared_ptr<GridPt>> Projector::Impl::get_neighbors(IdxSet grid_idxs) const {
		IdxSet id = grid_idxs;
		std::vector<std::shared_ptr<GridPt>> ret;
		for (auto dim=0; dim<_dim_grid; dim++) {
			// Before and after!
			id[0] -= 1;
		};
		return ret;
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
	Files
	********************/

	std::string Projector::get_fname_path() const {
		return _impl->get_fname_path();
	};
	void Projector::read_path(std::string fname_path) {
		_impl->read_path(fname_path);
	};

	std::string Projector::get_fname_grid_points() const {
		return _impl->get_fname_grid_points();
	};
	void Projector::read_grid(std::string fname_grid_points) {
		_impl->read_grid(fname_grid_points);
	};

	/********************
	Get indexes
	********************/

	// Input: idx in each dim
	int Projector::get_idx(IdxSet grid_idxs) const {
		return _impl->get_idx(grid_idxs);
	};
	// Input: idx in the actual 1D vector
	IdxSet Projector::get_idxs(int grid_idx) const {
		return _impl->get_idxs(grid_idx);
	};

	/********************
	Get grid pts
	********************/

	std::shared_ptr<GridPt> Projector::get_grid_point(std::vector<int> grid_idxs) const {
		return get_grid_point(IdxSet(grid_idxs));
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSet grid_idxs) const {
		return get_grid_point(grid_idxs);
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(int grid_idx) const {
		return _impl->get_grid_point(get_idxs(grid_idx));
	};

	/********************
	Get neighbors
	********************/

	std::vector<std::shared_ptr<GridPt>> Projector::get_neighbors(std::shared_ptr<GridPt> grid_pt) const {
		return get_neighbors(grid_pt->get_idxs());
	};
	std::vector<std::shared_ptr<GridPt>> Projector::get_neighbors(std::vector<int> grid_idxs) const {
		return get_neighbors(IdxSet(grid_idxs));
	};
	std::vector<std::shared_ptr<GridPt>> Projector::get_neighbors(IdxSet grid_idxs) const {
		return _impl->get_neighbors(grid_idxs);
	};
	std::vector<std::shared_ptr<GridPt>> Projector::get_neighbors(int grid_idx) const {
		return get_neighbors(get_idxs(grid_idx));
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