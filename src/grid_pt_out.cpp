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

	class GridPtOut::Impl {

	private:

		// Idxs
		IdxSet _idxs;

		// Abscissa values
		std::vector<double> _abcissas;

		// Dependencies
		std::shared_ptr<GridPt> _p1,_p2;

		// Constructor helpers
		void _clean_up();
		void _copy(const Impl& other);
		void _move(Impl& other);

	public:

		/********************
		Constructor
		********************/

		Impl(IdxSet idxs, std::vector<double> abscissas, std::shared_ptr<GridPt> p1, std::shared_ptr<GridPt> p2);
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

		// Idxs
		int get_idx(int dim) const;
		IdxSet get_idxs() const;

		// Get dependent points
		std::shared_ptr<GridPt> get_dep_p1() const;
		std::shared_ptr<GridPt> get_dep_p2() const;

	};






















	/****************************************
	Implementation
	****************************************/

	GridPtOut::Impl::Impl(IdxSet idxs, std::vector<double> abscissas, std::shared_ptr<GridPt> p1, std::shared_ptr<GridPt> p2) {
		// Check lengths
		if (idxs.size() != abscissas.size()) {
			std::cout << ">>> Error: GridPt::Impl::Impl <<< Sizes must match." << std::endl;
			exit(EXIT_FAILURE);
		};

		// Store
		_idxs = idxs;
		_abcissas = abscissas;
		_p1 = p1;
		_p2 = p2;
	};
	GridPtOut::Impl::Impl(const Impl& other) {
		_copy(other);
	};
	GridPtOut::Impl::Impl(Impl&& other) {
		_move(other);
	};
    GridPtOut::Impl& GridPtOut::Impl::operator=(const Impl& other) {
		if (this != &other) {
			_clean_up();
			_copy(other);
		};
		return *this;
    };
    GridPtOut::Impl& GridPtOut::Impl::operator=(Impl&& other) {
		if (this != &other) {
			_clean_up();
			_move(other);
		};
		return *this;
    };
	GridPtOut::Impl::~Impl()
	{
		_clean_up();
	};

	/********************
	Helpers for constructors
	********************/

	void GridPtOut::Impl::_clean_up()
	{
		// Nothing...
	};
	void GridPtOut::Impl::_copy(const Impl& other)
	{
		_idxs = other._idxs;
		_abcissas = other._abcissas;
		_p1 = other._p1;
		_p2 = other._p2;
	};
	void GridPtOut::Impl::_move(Impl& other)
	{
		_copy(other);

		// Reset other
		other._idxs = IdxSet();
		other._abcissas.clear();
		other._p1 = nullptr;
		other._p2 = nullptr;
	};

	/********************
	Access
	********************/

	// Abscissa
	double GridPtOut::Impl::get_abscissa(int dim) const {
		return _abcissas[dim];
	};
	std::vector<double> GridPtOut::Impl::get_abscissas() const {
		return _abcissas;
	};

	// Ordinate
	double GridPtOut::Impl::get_ordinate() const {
		return 2.0 * _p1->get_ordinate() - _p2->get_ordinate(); 
	};

	// Idxs
	int GridPtOut::Impl::get_idx(int dim) const {
		return _idxs[dim];
	};
	IdxSet GridPtOut::Impl::get_idxs() const {
		return _idxs;
	};

	// Get dependent points
	std::shared_ptr<GridPt> GridPtOut::Impl::get_dep_p1() const {
		return _p1;
	};
	std::shared_ptr<GridPt> GridPtOut::Impl::get_dep_p2() const {
		return _p2;
	};























	/****************************************
	Impl forwards
	****************************************/

	/********************
	Constructor
	********************/

	GridPtOut::GridPtOut(IdxSet idxs, std::vector<double> abscissas, std::shared_ptr<GridPt> p1, std::shared_ptr<GridPt> p2) : _impl(new Impl(idxs,abscissas, p1, p2)) {};
	GridPtOut::GridPtOut(const GridPtOut& other) : _impl(new Impl(*other._impl)) {};
	GridPtOut::GridPtOut(GridPtOut&& other) : _impl(std::move(other._impl)) {};
	GridPtOut& GridPtOut::operator=(const GridPtOut &other) {
        _impl.reset(new Impl(*other._impl));
        return *this; 
	};
	GridPtOut& GridPtOut::operator=(GridPtOut &&other) {
        _impl = std::move(other._impl);
        return *this; 
	};
	GridPtOut::~GridPtOut() = default;

	/********************
	Access
	********************/

	// Abscissa
	double GridPtOut::get_abscissa(int dim) const {
		return _impl->get_abscissa(dim);
	};
	std::vector<double> GridPtOut::get_abscissas() const {
		return _impl->get_abscissas();
	};

	// Ordinate
	double GridPtOut::get_ordinate() const {
		return _impl->get_ordinate();
	};

	// Idxs
	int GridPtOut::get_idx(int dim) const {
		return _impl->get_idx(dim);
	};
	IdxSet GridPtOut::get_idxs() const {
		return _impl->get_idxs();
	};

	// Get dependent points
	std::shared_ptr<GridPt> GridPtOut::get_dep_p1() const {
		return _impl->get_dep_p1();
	};
	std::shared_ptr<GridPt> GridPtOut::get_dep_p2() const {
		return _impl->get_dep_p2();
	};

};