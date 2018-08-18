#include "../include/L2ProjPathToGrid_bits/data_pt.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt_out.hpp"
#include "../include/L2ProjPathToGrid_bits/grid_pt.hpp"

#include <iostream>

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Implementation header
	****************************************/

	class DataPt::Impl {

	private:

		// Dimension of grid
		int _grid_dim;

		// Abscissa
		std::vector<double> _abscissas;
		
		// Ordinate
		double _ordinate;

		// Surrounding 4 pts in each dim
		std::map<IdxSet, GridPtType> _surrounding_type_map;
		std::map<IdxSet, GridPt> _surrounding_inside_map;
		std::map<IdxSet, GridPtOut> _surrounding_outside_map;

		// Check if index set is valid
		void _check_idx_set_valid(IdxSet idxs);

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(std::vector<double> abscissas, double ordinate, std::map<IdxSet, GridPtType> surrounding_type_map, std::map<IdxSet, GridPt> surrounding_inside_map, std::map<IdxSet, GridPtOut> surrounding_outside_map);
		Impl(const Impl& other);
		Impl(Impl&& other);
		Impl& operator=(const Impl &other);
		Impl& operator=(Impl &&other);
		~Impl();

		/********************
		Accessors
		********************/

		// Abcissas
		double get_abscissa(int dim) const;
		std::vector<double> get_abscissas() const;

		// Ordinate
		double get_ordinate() const;

		// Get surrounding grid point
		// Length of idxs = _grid_dim
		// Each idx = 0,1,2, or 3
		GridPtType get_surrounding_grid_pt_type(IdxSet idxs) const;
		GridPt get_surrounding_grid_pt_inside(IdxSet idxs) const;
		GridPtOut get_surrounding_grid_pt_outside(IdxSet idxs) const;

		// Make lines in dim....
		std::vector<GridPtLine2> get_grid_pt_lines_2(int line_dim) const;
		std::vector<GridPtLine4> get_grid_pt_lines_4(int line_dim) const;

	};































	/****************************************
	Implementation
	****************************************/

	DataPt::Impl::Impl(std::vector<double> abscissas, double ordinate, std::map<IdxSet, GridPtType> surrounding_type_map, std::map<IdxSet, GridPt> surrounding_inside_map, std::map<IdxSet, GridPtOut> surrounding_outside_map) {
		// Store
		_abscissas = abscissas;
		_grid_dim = _abscissas.size();
		_ordinate = ordinate;
		_surrounding_type_map = surrounding_type_map;
		_surrounding_inside_map = surrounding_inside_map;
		_surrounding_outside_map = surrounding_outside_map;
	};
	DataPt::Impl::Impl(const Impl& other) {
		_copy(other);
	};
	DataPt::Impl::Impl(Impl&& other) {
		_move(other);
	};
    DataPt::Impl& DataPt::Impl::operator=(const Impl& other) {
		if (this != &other) {
			_clean_up();
			_copy(other);
		};
		return *this;
    };
    DataPt::Impl& DataPt::Impl::operator=(Impl&& other) {
		if (this != &other) {
			_clean_up();
			_move(other);
		};
		return *this;
    };
	DataPt::Impl::~Impl()
	{
		_clean_up();
	};

	/********************
	Helpers for constructors
	********************/

	void DataPt::Impl::_clean_up()
	{
		// Nothing...
	};
	void DataPt::Impl::_copy(const Impl& other)
	{	
		_grid_dim = other._grid_dim;
		_abscissas = other._abscissas;
		_ordinate = other._ordinate;
		_surrounding_type_map = other._surrounding_type_map;
		_surrounding_inside_map = other._surrounding_inside_map;
		_surrounding_outside_map = other._surrounding_outside_map;
	};
	void DataPt::Impl::_move(Impl& other)
	{
		_copy(other);

		// Reset other
		other._grid_dim = 0;
		other._abscissas.clear();
		other._ordinate = 0.0;
		other._surrounding_type_map.clear();
		other._surrounding_inside_map.clear();
		other._surrounding_outside_map.clear();
	};

	/********************
	Check if index set is valid
	********************/

	void DataPt::Impl::_check_idx_set_valid(IdxSet idxs) {
		// Check size
		if (idxs.size() != _grid_dim) {
			std::cerr << ">>> Error: DataPt::Impl::_check_idx_set_valid <<< IdxSet size must be grid dimensionality: " << _grid_dim << std::endl;
			exit(EXIT_FAILURE);
		};
		// Check each
		for (auto i=0; i<idxs.size(); i++) {
			if (idxs[i] < 0 || idxs[i] > 3) {
				std::cerr << ">>> Error: DataPt::Impl::_check_idx_set_valid <<< indexes must be between 0 and 3 (inclusive)." << std::endl;
				exit(EXIT_FAILURE);
			};
		};
	};

	/********************
	Access
	********************/

	// Abscissa
	double DataPt::Impl::get_abscissa(int dim) const {
		return _abscissas[dim];
	};
	std::vector<double> DataPt::Impl::get_abscissas() const {
		return _abscissas;
	};

	// Ordinate
	double DataPt::Impl::get_ordinate() const {
		return _ordinate;
	};

	// Get surrounding grid point
	// Length of idxs = _grid_dim
	// Each idx = 0,1,2, or 3
	GridPtType DataPt::Impl::get_surrounding_grid_pt_type(IdxSet idxs) const {
		return _surrounding_type_map.at(idxs);
	};
	GridPt DataPt::Impl::get_surrounding_grid_pt_inside(IdxSet idxs) const {
		return _surrounding_inside_map.at(idxs);
	};
	GridPtOut DataPt::Impl::get_surrounding_grid_pt_outside(IdxSet idxs) const {
		return _surrounding_outside_map.at(idxs);
	};

	// Make lines in dim....
	std::vector<GridPtLine2> DataPt::Impl::get_grid_pt_lines_2(int line_dim) const {
		return {};
	};
	std::vector<GridPtLine4> DataPt::Impl::get_grid_pt_lines_4(int line_dim) const {
		return {};
	};

































	/****************************************
	Impl forwards
	****************************************/

	/********************
	Constructor
	********************/

	DataPt::DataPt(std::vector<double> abscissas, double ordinate, std::map<IdxSet, GridPtType> surrounding_type_map, std::map<IdxSet, GridPt> surrounding_inside_map, std::map<IdxSet, GridPtOut> surrounding_outside_map) : _impl(new Impl(abscissas, ordinate, surrounding_type_map, surrounding_inside_map, surrounding_outside_map)) {};
	DataPt::DataPt(const DataPt& other) : _impl(new Impl(*other._impl)) {};
	DataPt::DataPt(DataPt&& other) : _impl(std::move(other._impl)) {};
	DataPt& DataPt::operator=(const DataPt &other) {
        _impl.reset(new Impl(*other._impl));
        return *this; 
	};
	DataPt& DataPt::operator=(DataPt &&other) {
        _impl = std::move(other._impl);
        return *this; 
	};
	DataPt::~DataPt() = default;

	/********************
	Accessors
	********************/

	// Abcissas
	double DataPt::get_abscissa(int dim) const {
		return _impl->get_abscissa(dim);
	};
	std::vector<double> DataPt::get_abscissas() const {
		return _impl->get_abscissas();
	};

	// Ordinate
	double DataPt::get_ordinate() const {
		return _impl->get_ordinate();
	};

	// Get surrounding grid point
	// Length of idxs = _grid_dim
	// Each idx = 0,1,2, or 3
	GridPtType DataPt::get_surrounding_grid_pt_type(IdxSet idxs) const {
		return _impl->get_surrounding_grid_pt_type(idxs);
	};
	GridPt DataPt::get_surrounding_grid_pt_inside(IdxSet idxs) const {
		return _impl->get_surrounding_grid_pt_inside(idxs);
	};
	GridPtOut DataPt::get_surrounding_grid_pt_outside(IdxSet idxs) const {
		return _impl->get_surrounding_grid_pt_outside(idxs);
	};

	// Make lines in dim....
	std::vector<GridPtLine2> DataPt::get_grid_pt_lines_2(int line_dim) const {
		return _impl->get_grid_pt_lines_2(line_dim);
	};
	std::vector<GridPtLine4> DataPt::get_grid_pt_lines_4(int line_dim) const {
		return _impl->get_grid_pt_lines_4(line_dim);
	};


};