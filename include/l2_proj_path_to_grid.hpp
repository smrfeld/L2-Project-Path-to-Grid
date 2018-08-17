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

	class Projector {

	private:

		class Impl;
		std::unique_ptr<Impl> _impl;

	public:

		/********************
		Constructor
		********************/

		Projector(int dim_grid, int no_pts_grid, int no_pts_path);
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

};