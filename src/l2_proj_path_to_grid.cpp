#include "../include/l2_proj_path_to_grid.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Projector implementation
	****************************************/

	// Grid point
	typedef std::vector<double> Pt;

	class Projector::Impl {
	private:

		// Filenames
		std::string _fname_path, _fname_grid_points;

		// Dimension of the grid
		int _dim_grid;

		// Length of the path
		int _no_pts_path;

		// Size of the grid
		int _no_pts_grid;

		// Grid points
		std::vector<Pt> _grid_pts;

		// Path abcissas
		std::vector<Pt> _path_abcissas;

		// Path ordinates
		std::vector<double> _path_ordinates;

		// Clear grid/abcissas/ordinates
		void _clear_grid();
		void _clear_abcissas();
		void _clear_ordinates();

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(int dim_grid, int no_pts_grid, int no_pts_path);
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
		void read_grid_points(std::string fname_grid_points);

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

	Projector::Impl::Impl(int dim_grid, int no_pts_grid, int no_pts_path) {
		// Store
		_dim_grid = dim_grid;
		_no_pts_grid = no_pts_grid;
		_no_pts_path = no_pts_path;

		// Init
		_fname_path = "";
		_fname_grid_points = "";

		// Grid
		for (auto i=0; i<_no_pts_grid; i++) {
			_grid_pts.push_back(Pt());
			for (auto j=0; j<_dim_grid; j++) {
				_grid_pts[i].push_back(0.0);
			};
		};

		// Path
		for (auto i=0; i<_no_pts_path; i++) {
			_path_abcissas.push_back(Pt());
			for (auto j=0; j<_dim_grid; j++) {
				_path_abcissas[i].push_back(0.0);
			};
			_path_ordinates.push_back(0.0);
		};
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
		_no_pts_path = other._no_pts_path;
		_no_pts_grid = other._no_pts_grid;
		_grid_pts = other._grid_pts;
		_path_abcissas = other._path_abcissas;
		_path_ordinates = other._path_ordinates;
	};
	void Projector::Impl::_move(Impl& other)
	{
		_fname_path = other._fname_path;
		_fname_grid_points = other._fname_grid_points;
		_dim_grid = other._dim_grid;
		_no_pts_path = other._no_pts_path;
		_no_pts_grid = other._no_pts_grid;
		_grid_pts = other._grid_pts;
		_path_abcissas = other._path_abcissas;
		_path_ordinates = other._path_ordinates;

		// Reset other
		other._fname_path = "";
		other._fname_grid_points = "";
		other._dim_grid = 0;
		other._no_pts_path = 0;
		other._no_pts_grid = 0;
		other._grid_pts.clear();
		other._path_abcissas.clear();
		other._path_ordinates.clear();
	};

	/********************
	Clear grid/abcissas/ordinates
	********************/

	void Projector::Impl::_clear_grid() {
		for (auto i=0; i<_no_pts_grid; i++) {
			for (auto j=0; j<_dim_grid; j++) {
				_grid_pts[i][j] = 0.0;
			};
		};
	};
	void Projector::Impl::_clear_abcissas() {
		for (auto i=0; i<_no_pts_grid; i++) {
			for (auto j=0; j<_dim_grid; j++) {
				_path_abcissas[i][j] = 0.0;
			};
		};
	};
	void Projector::Impl::_clear_ordinates() {
		for (auto i=0; i<_no_pts_grid; i++) {
			_path_ordinates[i] = 0.0;
		};
	};

	/********************
	Files
	********************/

	std::string Projector::Impl::get_fname_path() const {
		return _fname_path;
	};
	void Projector::Impl::read_path(std::string fname_path) {
		// Set
		_fname_path = fname_path;

		// Read

		// Clear old
		_clear_abcissas();
		_clear_ordinates();

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
			if (i_line > _no_pts_path-1) {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< Line no: " << i_line << " is greater than the no pts specified for the path: " << _no_pts_path-1 << std::endl;
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
			};
			// Ordinate
			iss >> y;
			if (y == "") {
				std::cerr << ">>> Error: Projector::Impl::read_path <<< No pts specified seems incorrect - should be: " << _dim_grid << " abcissas and an ordinate." << std::endl;
				exit(EXIT_FAILURE);
			};

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
		if (i_line != _no_pts_path) {
			std::cerr << ">>> Error: Projector::Impl::read_path <<< Read: " << i_line << " lines but there were supposed to be: " << _no_pts_path << std::endl;
			exit(EXIT_FAILURE);
		};
	};

	std::string Projector::Impl::get_fname_grid_points() const {
		return _fname_grid_points;
	};
	void Projector::Impl::read_grid_points(std::string fname_grid_points) {
		// Set
		_fname_grid_points = fname_grid_points;

		// Read

		// Clear old
		_clear_grid();

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

		// Line, etc
		std::string line;
		std::istringstream iss;
		int i_line=0;
		while (getline(f,line)) {

			// Skip empty
			if (line == "") { continue; };

			// Check line no
			if (i_line > _no_pts_grid-1) {
				std::cerr << ">>> Error: Projector::Impl::read_grid_points <<< Line no: " << i_line << " is greater than the no pts specified for the grid: " << _no_pts_grid-1 << std::endl;
				exit(EXIT_FAILURE);
			};

			// Get
			iss = std::istringstream(line);

			// Grid pts
			for (auto i=0; i<_dim_grid; i++) {
				iss >> x[i];
				if (x[i] == "") {
					std::cerr << ">>> Error: Projector::Impl::read_grid_points <<< No pts specified seems incorrect - should be: " << _dim_grid << " values." << std::endl;
					exit(EXIT_FAILURE);
				};
			};

			// Put into vecs
			for (auto i=0; i<_dim_grid; i++) {
				_grid_pts[i_line][i] = atof(x[i].c_str());
			};

			// Next line
			i_line++;			

			// Reset line string		
			for (auto i=0; i<_dim_grid; i++) {
				x[i] = "";
			};
		};

		// Check line no
		if (i_line != _no_pts_grid) {
			std::cerr << ">>> Error: Projector::Impl::read_grid_points <<< Read: " << i_line << " lines but there were supposed to be: " << _no_pts_grid << std::endl;
			exit(EXIT_FAILURE);
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

	Projector::Projector(int dim_grid, int no_pts_grid, int no_pts_path) : _impl(new Impl(dim_grid, no_pts_grid, no_pts_path)) {};
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
	void Projector::read_grid_points(std::string fname_grid_points) {
		_impl->read_grid_points(fname_grid_points);
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