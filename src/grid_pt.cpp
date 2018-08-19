#include "../include/L2ProjPathToGrid_bits/grid_pt.hpp"
#include "../include/L2ProjPathToGrid_bits/projector.hpp"

#include <iostream>
#include <sstream>

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Index set
	****************************************/

	// Constructors
	IdxSet::IdxSet() {};
	IdxSet::IdxSet(int no_idxs) {
		for (auto i=0; i<no_idxs; i++) {
			_idxs.push_back(0);
		};
	};
	IdxSet::IdxSet(std::vector<int> idxs) {
		_idxs = idxs;
	};

	// Accessors
	int IdxSet::operator [](int idx) const {
		return _idxs[idx];
	};
	int & IdxSet::operator [](int idx) {
		return _idxs[idx];
	};

	/*
    int IdxSet::get(int idx) const { 
    	return _idxs[idx];
    };
    void IdxSet::set(int idx, int val) { 
    	// Check
		if (!_outside) {
			if (val > _no_idxs_possible[idx]-1 || val < 0) {
				std::cout << ">>> Error: IdxSet::set_from_linear <<< Val: " << val << " must be between: " << 0 << " and: " << _no_idxs_possible[idx]-1 << std::endl;
				exit(EXIT_FAILURE);
			};
		} else {
			if (val > _no_idxs_possible[idx] || val < -1) {
				std::cout << ">>> Error: IdxSet::set_from_linear <<< Val: " << val << " must be between: " << -1 << " and: " << _no_idxs_possible[idx] << std::endl;
				exit(EXIT_FAILURE);
			};		
		};

		_idxs[idx] = val;
	};
	*/

	// Size
	int IdxSet::size() const {
		return _idxs.size();
	};

	/*
    int IdxSet::get_no_idxs_possible(int idx) const { 
    	return _no_idxs_possible[idx];
    };

	bool IdxSet::is_outside() const {
		return _outside;
	};
	*/

	// Find
	bool IdxSet::find(int val) {
		auto it = std::find(_idxs.begin(), _idxs.end(), val);
		if (it == _idxs.end()) {
			return false;
		} else {
			return true;
		};
	};

 	// Print
 	std::string IdxSet::print() const {
 		std::ostringstream stream;
		stream << "(";
		for (auto i=0; i<_idxs.size(); i++) {
			stream << _idxs[i];
			if (i != _idxs.size()-1) {
				stream << " ";
			};
		};
		stream << ")";
	    return stream.str();
 	};

	// Printing
	std::ostream& operator<<(std::ostream& stream, const IdxSet& idxs) {
		stream << idxs.print();
		return stream;
	 };

















	/****************************************
	IdxSetKey
	****************************************/

	/********************
	Constructor
	********************/

	IdxSetKey::IdxSetKey(IdxSet idxs, int no_idxs_possible) {
		for (auto i=0; i<idxs.size(); i++) {
			_no_idxs_possible.push_back(no_idxs_possible);
		};
		_outside = false;

		// Set vals
		_idxs = idxs;
	};
	IdxSetKey::IdxSetKey(IdxSet idxs, GridPtType type, std::vector<std::shared_ptr<Dim>> dims) {
		for (auto i=0; i<dims.size(); i++) {
			if (type == GridPtType::INSIDE) {
				_no_idxs_possible.push_back(dims[i]->get_no_pts());
			} else {
				_no_idxs_possible.push_back(dims[i]->get_no_pts()+2);
			};
		};

		if (type == GridPtType::INSIDE) {
			_outside = false;
		} else {
			_outside = true;
		};

		// Set vals
		_idxs = idxs;
	};

	// Input: idx in each dim
	int IdxSetKey::get_linear() const {
		int ret=0,add=0;
		for (auto i=0; i<_idxs.size(); i++) {
			add = _idxs[i];

			// Add 1 for outside because starts at -1
			if (_outside) {
				add += 1;
			};

			for (auto j=i+1; j<_idxs.size(); j++) {
				add *= _no_idxs_possible[j];
			};
			ret += add;
		};
		return ret;
	};

	// Set from linear
	void IdxSetKey::set_from_linear(int idx_linear) {
		// Determine the idxs
		int pwr;
		int idx_working = idx_linear;
		for (auto dim=0; dim<_no_idxs_possible.size(); dim++) {
			pwr = 1;
			for (auto dim2=dim+1; dim2<_no_idxs_possible.size(); dim2++) {
				pwr *= _no_idxs_possible[dim2];
			};

			// Add
			_idxs[dim] = int(idx_working/pwr);

			// Remove
			idx_working -= _idxs[dim]*pwr;

			// Remove 1 for outside because starts at -1
			if (_outside) {
				_idxs[dim] -= 1;
			};
		};
	};
 
 	// Print
 	std::string IdxSetKey::print() const {
 		return _idxs.print();
 	};

	// Printing
	std::ostream& operator<<(std::ostream& stream, const IdxSetKey& idxs) {
		stream << idxs.print();
		return stream;
	 };

	// Comparator
	bool operator <(const IdxSetKey& x, const IdxSetKey& y) {
		return x.get_linear() < y.get_linear();
	};
	bool operator ==(const IdxSetKey& x, const IdxSetKey& y) {
		return x.get_linear() == y.get_linear();
	};




















	
	/****************************************
	Implementation header
	****************************************/

	class GridPt::Impl {

	private:

		// Abscissa values
		std::vector<double> _abcissas;

		// Ordinate
		double _ordinate;

		// Indexes
		IdxSet _idxs;

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(IdxSet idxs, std::vector<double> abscissas);
		Impl(const Impl& other);
		Impl(Impl&& other);
		Impl& operator=(const Impl &other);
		Impl& operator=(Impl &&other);
		~Impl();

		/********************
		Access
		********************/

		// Abscissa
		double get_abscissa(int dim) const;
		std::vector<double> get_abscissas() const;

		// Ordinate
		double get_ordinate() const;
		void set_ordinate(double val);

		// Idxs
		int get_idx(int dim) const;
		IdxSet get_idxs() const;

		/********************
		Print
		********************/

		std::string print_abscissa() const;

	};






















	/****************************************
	Implementation
	****************************************/

	GridPt::Impl::Impl(IdxSet idxs, std::vector<double> abscissas) {
		// Check lengths
		if (idxs.size() != abscissas.size()) {
			std::cout << ">>> Error: GridPt::Impl::Impl <<< Sizes must match." << std::endl;
			exit(EXIT_FAILURE);
		};

		// Store
		_idxs = idxs;
		_abcissas = abscissas;

		// Init
		_ordinate = 0.0;
	};
	GridPt::Impl::Impl(const Impl& other) {
		_copy(other);
	};
	GridPt::Impl::Impl(Impl&& other) {
		_move(other);
	};
    GridPt::Impl& GridPt::Impl::operator=(const Impl& other) {
		if (this != &other) {
			_clean_up();
			_copy(other);
		};
		return *this;
    };
    GridPt::Impl& GridPt::Impl::operator=(Impl&& other) {
		if (this != &other) {
			_clean_up();
			_move(other);
		};
		return *this;
    };
	GridPt::Impl::~Impl()
	{
		_clean_up();
	};

	/********************
	Helpers for constructors
	********************/

	void GridPt::Impl::_clean_up()
	{
		// Nothing...
	};
	void GridPt::Impl::_copy(const Impl& other)
	{
		_abcissas = other._abcissas;
		_ordinate = other._ordinate;
		_idxs = other._idxs;
	};
	void GridPt::Impl::_move(Impl& other)
	{
		_abcissas = other._abcissas;
		_ordinate = other._ordinate;
		_idxs = other._idxs;

		// Reset other
		other._abcissas.clear();
		other._ordinate = 0.0;
		other._idxs = IdxSet();
	};

	/********************
	Access
	********************/

	// Abscissa
	double GridPt::Impl::get_abscissa(int dim) const {
		return _abcissas[dim];
	};
	std::vector<double> GridPt::Impl::get_abscissas() const {
		return _abcissas;
	};

	// Ordinate
	double GridPt::Impl::get_ordinate() const {
		return _ordinate;
	};
	void GridPt::Impl::set_ordinate(double val) {
		_ordinate = val;
	};

	// Idxs
	int GridPt::Impl::get_idx(int dim) const {
		return _idxs[dim];
	};
	IdxSet GridPt::Impl::get_idxs() const {
		return _idxs;
	};

	/********************
	Print
	********************/

	std::string GridPt::Impl::print_abscissa() const {
		std::ostringstream s;
		s << "(";
		for (auto dim=0; dim<_abcissas.size(); dim++) {
			s << _abcissas[dim];
			if (dim != _abcissas.size()-1) {
				s << " ";
			};
		};
		s << ")";
		return s.str();
	};





















	/****************************************
	Impl forwards
	****************************************/

	/********************
	Constructor
	********************/

	GridPt::GridPt(IdxSet idxs, std::vector<double> abscissas) : _impl(new Impl(idxs, abscissas)) {};
	GridPt::GridPt(const GridPt& other) : _impl(new Impl(*other._impl)) {};
	GridPt::GridPt(GridPt&& other) : _impl(std::move(other._impl)) {};
	GridPt& GridPt::operator=(const GridPt &other) {
        _impl.reset(new Impl(*other._impl));
        return *this; 
	};
	GridPt& GridPt::operator=(GridPt &&other) {
        _impl = std::move(other._impl);
        return *this; 
	};
	GridPt::~GridPt() = default;

	/********************
	Access
	********************/

	// Abscissa
	double GridPt::get_abscissa(int dim) const {
		return _impl->get_abscissa(dim);
	};
	std::vector<double> GridPt::get_abscissas() const {
		return _impl->get_abscissas();
	};

	// Ordinate
	double GridPt::get_ordinate() const {
		return _impl->get_ordinate();
	};
	void GridPt::set_ordinate(double val) {
		_impl->set_ordinate(val);
	};

	// Idxs
	int GridPt::get_idx(int dim) const {
		return _impl->get_idx(dim);
	};
	IdxSet GridPt::get_idxs() const {
		return _impl->get_idxs();
	};

	/********************
	Print
	********************/

	std::string GridPt::print_abscissa() const {
		return _impl->print_abscissa();
	};

};