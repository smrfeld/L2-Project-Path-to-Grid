#ifndef STRING_H
#define STRING_H
#include <string>
#endif

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

	/****************************************
	Dimension
	****************************************/

	class Dim {

	private:

		double _start_pt;
		double _end_pt;
		double _spacing;
		int _no_pts;
		std::vector<double> _pts;

	public:

		Dim(double start_pt, double end_pt, int no_pts);

		// Accessors
		int get_no_pts() const;
		double get_start_pt() const;
		double get_end_pt() const;
		double get_spacing() const;

		double get_pt_at_idx(int idx) const;
		std::vector<double> get_pts() const;

		std::pair<bool,std::pair<int,int>> get_surrounding_idxs(double pt) const;
	};

	/****************************************
	Projector
	****************************************/

	class GridPt;
	class GridPtOut;
	class IdxSet;
	class IdxSetKey;
	struct Nbr4;
	class DataPt;

	class Projector {

	private:

		class Impl;
		std::unique_ptr<Impl> _impl;

	public:

		/********************
		Constructor
		********************/

		Projector(std::vector<std::shared_ptr<Dim>> dims);
		Projector(const Projector& other);
		Projector(Projector&& other);
		Projector& operator=(const Projector &other);
		Projector& operator=(Projector &&other);
		~Projector();

		/********************
		Get dims
		********************/

		int get_no_dims() const;
		std::vector<std::shared_ptr<Dim>> get_dims() const;

		/********************
		Files
		********************/

		void read_path(std::string fname_path);

		/********************
		Get grid points
		********************/

		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_grid_points() const;
		std::shared_ptr<GridPt> get_grid_point(std::vector<int> grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(IdxSet grid_idxs) const;
		std::shared_ptr<GridPt> get_grid_point(IdxSetKey key) const;

		std::map<IdxSetKey, std::shared_ptr<GridPtOut>> get_grid_points_outside() const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(std::vector<int> grid_idxs) const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSet grid_idxs) const;
		std::shared_ptr<GridPtOut> get_grid_point_outside(IdxSetKey key) const;

		/********************
		Get grid points surrounding a point
		********************/

		std::map<IdxSetKey, std::shared_ptr<GridPt>> get_surrounding_2(std::vector<double> abscissas) const;
		Nbr4 get_surrounding_4(std::vector<double> abscissas) const;

		/********************
		Get data points
		********************/

		std::vector<std::shared_ptr<DataPt>> get_data_points() const;

		/********************
		Project
		********************/

		void project();

		/********************
		Write
		********************/

		void write_solution(std::string fname) const;

	};

};