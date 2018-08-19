#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#endif

#ifndef MAP_H
#define MAP_H
#include <map>
#endif

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	// Forward
	enum class GridPtType: unsigned int;
	class GridPt;
	class GridPtOut;
	class IdxSetKey;

	// Line of 4 grid pts
	struct GridPtLine4 {
		// p1,p2 are in
		std::shared_ptr<GridPt> p1,p2;

		// Type of p0,p3
		GridPtType p0type, p3type;

		// If p0,p3 are in
		std::shared_ptr<GridPt> p0in, p3in;

		// If p0,p3 are out
		std::shared_ptr<GridPtOut> p0out, p3out;
	};

	// Neighborhood of points
	struct Nbr4 {
		std::map<IdxSetKey, GridPtType> types;
		std::map<IdxSetKey, std::shared_ptr<GridPt>> in;
		std::map<IdxSetKey, std::shared_ptr<GridPtOut>> out;
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

		DataPt(std::vector<double> abscissas, double ordinate, Nbr4 nbr4, std::vector<std::shared_ptr<Dim>> dims);
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

		// Get surrounding grid point
		// Length of idxs = _grid_dim
		// Each idx = 0,1,2, or 3
		Nbr4 get_nbr4() const;
		GridPtType get_nbr4_type(IdxSetKey idxs) const;
		std::shared_ptr<GridPt> get_nbr4_inside(IdxSetKey idxs) const;
		std::shared_ptr<GridPtOut> get_nbr4_outside(IdxSetKey idxs) const;

		// Make lines in dim....
		std::vector<GridPtLine4> get_grid_pt_lines_4(int line_dim) const;
	};

};