/**
\file
\brief Sierpinski triangle, recursively
*/

#define HOMOG2D_USE_OPENCV

//#define HOMOG2D_DEBUGMODE

#include "../../homog2d.hpp"

using namespace h2d;

using DrawType = std::pair<CPolyline,img::Color>;
auto max_color = 200.;

/// Each polyline in the input vector \c v_pl will be replaced by 3
void recurseTriangles(
	std::vector<CPolyline>& v_pl,       ///< the set of triangles that will be split
	std::vector<DrawType>&  v_all,      ///< what we actually draw
	int                     max_depth,
	Homogr&                 H,
	img::Image<cv::Mat>&    im          ///< output image
)
{
	static int depth;
	depth++;
	H.addTranslation(-50,-50).addRotation( 5.*M_PI/180. ).addTranslation(50,50).addScale(1.1,1.1);
	std::cout << "depth=" << depth << " nb pl input=" << v_pl.size() << '\n';
	if( depth == max_depth )
		return;
	std::vector<CPolyline> v_pl_new;
	int r = std::rand()*max_color/RAND_MAX;
	int g = std::rand()*max_color/RAND_MAX;
	int b = std::rand()*max_color/RAND_MAX;
	for( const auto& pl: v_pl )	{
		auto v_segs = pl.getSegs();
		auto v_mid = getCenters( v_segs );
		auto v_pts  = pl.getPts();
		auto pl1 = CPolyline( std::vector<Point2d>{ v_pts[0], v_mid[0], v_mid[2] } );
		auto pl2 = CPolyline( std::vector<Point2d>{ v_pts[1], v_mid[1], v_mid[0] } );
		auto pl3 = CPolyline( std::vector<Point2d>{ v_pts[2], v_mid[2], v_mid[1] } );

		auto pld = CPolyline( v_mid );
		v_all.push_back( std::make_pair(pld,img::Color(r,g,b) ) );

		v_pl_new.push_back(pl1);
		v_pl_new.push_back(pl2);
		v_pl_new.push_back(pl3);
	}
	im.clear();
	for( const auto& e: v_all )
	{
		auto e2 = H*e.first;
		draw( im, e2, img::DrawParams().setColor( e.second ) );
	}

	std::ostringstream oss;
	oss << "showcase10_" << std::setfill('0') << std::setw(2) << depth << ".png";
	im.write( oss.str() );

	recurseTriangles( v_pl_new, v_all, max_depth, H, im );
}

int main()
{
	Homogr H;
	std::srand( std::time(nullptr) ); // to get random colors
	auto x0 = 10;
	auto y0 = 10;
	auto l = 500;
	auto max_depth = 10;
	std::vector<Point2d> vpts{
		{x0,y0}, {x0+l,y0}, {x0+l/2.,y0+l/2.*sqrt(3.)}
	};
	CPolyline pl(vpts);                  // build initial equilateral triangle
	std::vector<CPolyline> v_pl{pl};
	std::vector<DrawType> v_draw(1);
	v_draw[0] = std::make_pair( pl, img::Color(250,0,20) );

	img::Image<cv::Mat> im( 600, 600 );
	pl.draw( im, img::DrawParams().setColor(250,0,20) );
	im.write( "showcase10_00.png" );

	recurseTriangles( v_pl, v_draw, max_depth, H, im );
}
