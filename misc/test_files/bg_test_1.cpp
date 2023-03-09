/**
\file
\brief  test of importing a polygon from Boost Geometry

\note 20230216: preliminar feature, will be expanded )

run with `$ make test_bg_1`
*/

#define HOMOG2D_USE_BOOSTGEOM
#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

int main()
{
	namespace bg = boost::geometry;

// build a boost geometry polygon
	using Bg_point_t1 = bg::model::point<double, 2, bg::cs::cartesian>;
	using Bg_point_t2 = bg::model::d2::point_xy<double>;

	using Bg_cpoly_t1 = bg::model::polygon<Bg_point_t1,true,true>;
	using Bg_opoly_t1 = bg::model::polygon<Bg_point_t1,true,false>;

	Bg_cpoly_t1 cpoly1{{ {0.0, 0.0}, {0.0, 5.0}, {5.0, 5.0}, {5.0, 0.0}, {0.0, 0.0} }};
	Bg_opoly_t1 opoly1{{ {0.0, 0.0}, {0.0, 5.0}, {5.0, 5.0}, {5.0, 0.0} }};

	h2d::CPolyline p1a( cpoly1 );
	h2d::OPolyline p1b( opoly1 );
	std::cout << "p1a=" << p1a << "p1b=" << p1b << '\n';

	using Bg_cpolygon_t2 = bg::model::polygon<Bg_point_t2,true,true>;
	using Bg_opolygon_t2 = bg::model::polygon<Bg_point_t2,true,false>;

	Bg_cpolygon_t2 cpoly2{{ {0.0, 0.0}, {0.0, 5.0}, {5.0, 5.0}, {5.0, 0.0}, {0.0, 0.0} }};
	Bg_opolygon_t2 opoly2{{ {0.0, 0.0}, {0.0, 5.0}, {5.0, 5.0}, {5.0, 0.0} }};

	h2d::CPolyline p2a( cpoly2 );
	h2d::OPolyline p2b( opoly2 );
	std::cout << "p2a=" << p2a << "p2b=" << p2b << '\n';

	Bg_point_t1 ptb1(3,4);
	Bg_point_t2 ptb2(8,9);
	h2d::Point2d p1(ptb1);
	h2d::Point2d p2(ptb2);
	std::cout << "p1=" << p1 << " p2=" << p2 << '\n';

	p1.set(ptb2);
	p2.set(ptb1);
	std::cout << "p1=" << p1 << " p2=" << p2 << '\n';

	p1 = ptb2;
	p2 = ptb1;

// convert to bg type
	Bg_point_t1 bpt1a = p1.getPt<Bg_point_t1>();
	Bg_point_t1 bpt1b = h2d::getPt<Bg_point_t1>(p2);
}
