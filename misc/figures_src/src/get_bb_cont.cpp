// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

std::vector<img::Color> v_colors;

/// This one is enabled only for points, and will draw the extremum points
template<
	typename T,
	typename std::enable_if<
		std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value,T
	>::type* = nullptr
>
void DrawExtremePoints( img::Image<img::SvgImage>& img, const T& vec )
{
	auto style1 = DrawParams().setPointStyle(PtStyle::Dot).setThickness(3).setColor(0,250,0);
	auto style2 = DrawParams().setPointStyle(PtStyle::Dot).setThickness(3).setColor(0,0,250);
	getTmPoint( vec ).draw( img, style1 );
	getRmPoint( vec ).draw( img, style2 );
	getLmPoint( vec ).draw( img, style1 );
	getBmPoint( vec ).draw( img, style2 );
}

/// This one is enabled for all other types, and will do nothing
template<
	typename T,
	typename std::enable_if<
		!std::is_same<typename T::value_type,Point2d_<typename T::value_type::FType>>::value,T
	>::type* = nullptr
>
void DrawExtremePoints( img::Image<img::SvgImage>&, const T& )
{}

template<typename T>
void process( img::Image<img::SvgImage>& im, const T& vec, std::string fname )
{
	auto lamb_func = [&v_colors](int i)   // lambda, needed to fetch color from index
	{
		return img::DrawParams().setColor( v_colors[i] );
	};
	std::function<img::DrawParams(int)> f(lamb_func);

	im.clear();
	draw( im, vec, f );

	auto bb = getBB( vec );
	bb.draw( im, DrawParams().setColor(250,50,20) );
	DrawExtremePoints( im, vec );
	im.write( fname + ".svg" );
}

int main()
{
	std::vector<Segment>   v_segs;
	std::vector<Point2d>   v_pts;
	std::vector<Point2d>   v_polypts;
	std::vector<CPolyline> v_poly;
	auto nb_pts_poly_max = 5;
	auto nb_pts_poly_min = 2;
	size_t polySize = 1.*std::rand()/RAND_MAX * nb_pts_poly_max + nb_pts_poly_min;

	int xmax = 300;
	int ymax = 250;
	int xmin = 40;
	int seg_max = 50;
	int seg_min = 10;
	std::srand( std::time(nullptr) );
	int nbSegs = 40;

	v_colors = genRandomColors( 2 * nbSegs );

	for( int i=0; i<nbSegs; i++ )
	{
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * ymax + xmin;
		v_pts.emplace_back( Point2d(x1,y1) );

		auto x2 = x1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		auto y2 = y1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		v_pts.emplace_back( Point2d(x2,y2) );
		v_segs.emplace_back( Segment( x1,y1,x2,y2) );

		v_polypts.emplace_back( Point2d(x1,y1) );
		if( v_polypts.size() == polySize )
		{
			v_poly.emplace_back( CPolyline(v_polypts) );
			polySize = 1.*std::rand()/RAND_MAX * nb_pts_poly_max + nb_pts_poly_min;
			v_polypts.clear();
		}
	}
	auto k = 1.5;
	img::Image<img::SvgImage> im( k*xmax, k*ymax );
//	img2.setSize( k*xmax, k*ymax );

	std::vector<FRect> v_rects;
	for( const auto& seg: v_segs )
		v_rects.emplace_back( seg.getPts() );

	std::vector<Circle> v_circles;
	for( const auto& seg: v_segs )
	{
		auto ppts = seg.getPts();
		v_circles.emplace_back( ppts.first, ppts.second );
	}

	std::vector<Ellipse> v_ell;
	for( const auto& seg: v_segs )
	{
		auto a     = 1.*std::rand()/RAND_MAX * 40 + 5;
		auto b     = 1.*std::rand()/RAND_MAX * 10 + 5;
		auto angle = M_PI*std::rand()/RAND_MAX;

		Ellipse ell( seg.getPts().first, a, b, angle );
		v_ell.push_back( ell );
	}

	process( im, v_rects,   "bb_Rects" );
	process( im, v_pts,     "bb_Points" );
	process( im, v_segs,    "bb_Segs" );
	process( im, v_circles, "bb_Circles" );
	process( im, v_poly,    "bb_Poly" );
	process( im, v_ell,     "bb_Ellipses" );
}

