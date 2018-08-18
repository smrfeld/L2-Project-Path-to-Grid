#ifndef STRING_H
#define STRING_H
#include <string>
#endif

#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
#endif

/************************************
* Namespace for L2PG
************************************/

namespace L2PG {

	/****************************************
	Projector
	****************************************/

	class GridPt;
	class IdxSet;

	class Projector {

	private:

		class Impl;
		std::unique_ptr<Impl> _impl;

	public:

		/********************
		Constructor
		********************/

		Projector(int dim_grid, std::vector<int> no_pts_dim, int no_pts_path);
		Projector(const Projector& other);
		Projector(Projector&& other);
		Projector& operator=(const Projector &other);
		Projector& operator=(Projector &&other);
		~Projector();

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

		std::shared_ptr<GridPt> get_grid_point(std::vector<int> grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(int grid_idx) const;

		/********************
		Get neighbors
		********************/

		std::vector<std::shared_ptr<GridPt>> get_neighbors(std::shared_ptr<GridPt> grid_pt) const;
		std::vector<std::shared_ptr<GridPt>> get_neighbors(std::vector<int> grid_idxs) const;
		std::vector<std::shared_ptr<GridPt>> get_neighbors(IdxSet grid_idxs) const;
		std::vector<std::shared_ptr<GridPt>> get_neighbors(int grid_idx) const;

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

};