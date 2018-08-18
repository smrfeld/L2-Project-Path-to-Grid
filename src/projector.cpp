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
	Projector implementation
	****************************************/

	class Projector::Impl {
	private:

		// Filenames
		std::string _fname_path, _fname_grid_points;

		// Dimension of the grid
		int _dim_grid;

		// No pts in each dim of the grid
		std::vector<int> _no_pts_dim;

		// Size of the grid
		int _no_pts_grid;

		// Grid points
		std::vector<std::shared_ptr<GridPt>> _grid_pts;

		// Length of the data
		int _no_pts_data;

		// Data
		std::vector<std::shared_ptr<DataPt>> _data_pts;

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(int dim_grid, std::vector<int> no_pts_dim, int no_pts_path);
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
		Get values
		********************/

		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(int grid_idx) const;

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

	Projector::Impl::Impl(int dim_grid, std::vector<int> no_pts_dim, int no_pts_path) {
		// Check dim
		if (no_pts_dim.size() != dim_grid) {
			std::cout << ">>> Error: Projector::Impl::Impl <<< Must specify no pts for each dimension." << std::endl;
			exit(EXIT_FAILURE);
		};

		// Store
		_dim_grid = dim_grid;
		_no_pts_data = no_pts_path;
		_no_pts_dim = no_pts_dim;
		_no_pts_grid = 1;
		for (auto const &i: _no_pts_dim) {
			_no_pts_grid *= i;
		};

		// Init
		_fname_path = "";
		_fname_grid_points = "";
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
		_fname_path = other._fname_path;
		_fname_grid_points = other._fname_grid_points;
		_dim_grid = other._dim_grid;
		_no_pts_data = other._no_pts_data;
		_no_pts_grid = other._no_pts_grid;
		_grid_pts = other._grid_pts;
		_data_pts = other._data_pts;
	};
	void Projector::Impl::_move(Impl& other)
	{
		_copy(other);

		// Reset other
		other._fname_path = "";
		other._fname_grid_points = "";
		other._dim_grid = 0;
		other._no_pts_data = 0;
		other._no_pts_grid = 0;
		other._grid_pts.clear();
		other._data_pts.clear();
	};

	/********************
	Files
	********************/

	std::string Projector::Impl::get_fname_path() const {
		return _fname_path;
	};
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

	std::string Projector::Impl::get_fname_grid_points() const {
		return _fname_grid_points;
	};
	void Projector::Impl::read_grid(std::string fname_grid_points) {
		// Set
		_fname_grid_points = fname_grid_points;

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
			_grid_pts.push_back(std::make_shared<GridPt>(idxs, abcissas));

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
			if (grid_idxs[dim] > _no_pts_dim[dim]-1) {
				std::cerr << ">>> Error: Projector::Impl::get_grid_point <<< Index specified: " << grid_idxs[dim] << " in dim: " << dim << " should be less than: " << _no_pts_dim[dim]-1 << std::endl;
				exit(EXIT_FAILURE);
			};
		};

		// Find gridpoint by index
		int grid_idx=0,add=0;
		for (auto dim=0; dim<_dim_grid; dim++) {
			add = grid_idxs[dim];
			for (auto dim2=dim+1; dim2<_dim_grid; dim2++) {
				add *= _no_pts_dim[dim2];
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
				pwr *= _no_pts_dim[dim2];
			};

			// Add
			grid_idxs[dim] = int(grid_idx_working/pwr);

			// Remove
			grid_idx_working -= grid_idxs.idxs.back()*pwr;
		};

		return grid_idxs;
	};

	/********************
	Get values
	********************/

	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(IdxSet grid_idxs) const {
		return _grid_pts[get_idx(grid_idxs)];
	};
	std::shared_ptr<GridPt> Projector::Impl::get_grid_point(int grid_idx) const {
		return _grid_pts[grid_idx];
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

	Projector::Projector(int dim_grid, std::vector<int> no_pts_dim, int no_pts_path) : _impl(new Impl(dim_grid, no_pts_dim, no_pts_path)) {};
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
	Get values
	********************/

	std::shared_ptr<GridPt> Projector::get_grid_point(IdxSet grid_idxs) const {
		return _impl->get_grid_point(grid_idxs);
	};
	std::shared_ptr<GridPt> Projector::get_grid_point(int grid_idx) const {
		return _impl->get_grid_point(grid_idx);
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