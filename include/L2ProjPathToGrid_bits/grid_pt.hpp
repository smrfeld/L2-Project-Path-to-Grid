#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#endif

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Index set
	****************************************/

	// Forward
	class Dim;

	struct IdxSet {
		// Idxs
		std::vector<int> idxs;
		std::vector<int> max_idxs; // exlusive

		// Constructors
		IdxSet();
		IdxSet(int no_idxs, int max_idx);
		IdxSet(std::vector<std::shared_ptr<Dim>> dims);
		IdxSet(std::vector<std::shared_ptr<Dim>> dims, std::vector<int> idxs);

		// Accessors
	    int operator[](int i) const;
		int & operator[](int i);

		// Size
		int size() const;

		// Linear idx
		int get_linear() const;

		// Set from linear
		void set_from_linear(int idx_linear);
	};
	// Comparator
	bool operator <(const IdxSet& x, const IdxSet& y);
	bool operator ==(const IdxSet& x, const IdxSet& y);

	/****************************************
	Interior grid pt
	****************************************/
 
 	// Two types: in the grid and outside
	enum class GridPtType: unsigned int { INSIDE, OUTSIDE };

	class GridPt {

	private:

		class Impl;
		std::unique_ptr<Impl> _impl;

	public:

		/********************
		Constructor
		********************/

		GridPt(IdxSet idxs, std::vector<double> abscissas);
		GridPt(const GridPt& other);
		GridPt(GridPt&& other);
		GridPt& operator=(const GridPt &other);
		GridPt& operator=(GridPt &&other);
		~GridPt();

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
	};
};