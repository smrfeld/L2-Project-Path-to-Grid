#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#endif

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

 	// Two types: in the grid and outside
	enum class GridPtType: unsigned int { INSIDE, OUTSIDE };

	/****************************************
	Index set
	****************************************/

	class IdxSet {

	private:
		
		// Idxs
		std::vector<int> _idxs;

	public:

		/********************
		Constructor
		********************/

		IdxSet();
		IdxSet(int no_idxs);
		IdxSet(std::vector<int> idxs);

		/********************
		Accessors
		********************/

		int operator [](int idx) const;
		int & operator [](int idx);

		int size() const;

		bool find(int val);

		std::string print() const;
	};

	// Printing
    std::ostream& operator<< (std::ostream& stream, const IdxSet& idxs);

	// Forward
	class Dim;

	class IdxSetKey {

	private:

		// Inside or outside
		GridPtType _type;

		// Idxs
		IdxSet _idxs;
		std::vector<int> _no_idxs_possible;

	public:

		/********************
		Constructor
		********************/

		IdxSetKey(IdxSet idxs, int no_idxs_possible);
		IdxSetKey(IdxSet idxs, std::vector<std::shared_ptr<Dim>> dims);
		/*
		IdxSet(const IdxSet& other);
		IdxSet(IdxSet&& other);
		IdxSet& operator=(const IdxSet &other);
		IdxSet& operator=(IdxSet &&other);
		~IdxSet();
		*/

		/********************
		Accessors
		********************/

		// Type
		GridPtType get_type() const;

		// Linear idx
		int get_linear() const;

		// Set from linear
		void set_from_linear(int idx_linear);

		std::string print() const;
	};

	// Printing
    std::ostream& operator<< (std::ostream& stream, const IdxSetKey& idxs);

	// Comparator
	bool operator <(const IdxSetKey& x, const IdxSetKey& y);
	bool operator ==(const IdxSetKey& x, const IdxSetKey& y);

	/****************************************
	Interior grid pt
	****************************************/

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

		/********************
		Print
		********************/

		std::string print_abscissa() const;
	};
};