#include <vector>
#include <map>

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	// Forward
	enum class GridPtType: unsigned int;
	class GridPt;
	class GridPtOut;
	class IdxSetKey;
	class IdxSet;

	// Line of 4 grid pts
	struct Line4 {
		// p1,p2 are in
		std::shared_ptr<GridPt> p1,p2;

		// Type of p0,p3
		GridPtType p0type, p3type;

		// If p0,p3 are in
		std::shared_ptr<GridPt> p0in, p3in;

		// If p0,p3 are out
		std::shared_ptr<GridPtOut> p0out, p3out;

		// Print
		std::string print() const;
	};

	// Neighborhood of points
	struct Nbr4 {
		std::map<IdxSetKey, GridPtType> types;
		std::map<IdxSetKey, std::shared_ptr<GridPt>> in;
		std::map<IdxSetKey, std::shared_ptr<GridPtOut>> out;

		// Get lines of 4 in some dim
		std::vector<Line4> get_grid_pt_lines_4(int line_dim, int dim_grid) const;
		void _iterate_other_idxs_make_line(IdxSet &idxs, int dim, int dim_grid, int line_dim, std::vector<Line4> &ret) const;
	};

	class Dim;

	class DataPt {

	private:

		class Impl;
		std::unique_ptr<Impl> _impl;

	public:

		/********************
		Constructor
		********************/

		DataPt(std::vector<double> abscissas, double ordinate, std::map<IdxSetKey, std::shared_ptr<GridPt>> nbr2, Nbr4 nbr4);
		DataPt(const DataPt& other);
		DataPt(DataPt&& other);
		DataPt& operator=(const DataPt &other);
		DataPt& operator=(DataPt &&other);
		~DataPt();

		/********************
		Accessors
		********************/

		// Abcissas
		double get_abscissa(int dim) const;
		std::vector<double> get_abscissas() const;

		// Ordinate
		double get_ordinate() const;

		// Get frac abscissa (between 0 and 1, between nearest grid points)
		double get_frac_abscissa(int dim) const;
		std::vector<double> get_frac_abscissas() const;

		// Get surrounding grid point
		// Length of idxs = _dim_grid
		// Each idx = 0,1,2, or 3
		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_nbr2() const;
		Nbr4 get_nbr4() const;
		GridPtType get_nbr4_type(IdxSetKey idxs) const;
		std::shared_ptr<GridPt> get_nbr4_inside(IdxSetKey idxs) const;
		std::shared_ptr<GridPtOut> get_nbr4_outside(IdxSetKey idxs) const;

		// Make lines in dim....
		std::vector<Line4> get_grid_pt_lines_4(int line_dim) const;
	};

};