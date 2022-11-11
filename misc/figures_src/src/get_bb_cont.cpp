// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

//img::Image<cv::Mat>       img1;
img::Image<img::SvgImage> img2;
std::vector<img::Color>   v_colors;

std::vector<img::Color> genRandomColors( size_t nb )
{
	std::vector<img::Color> vcol( nb );
	int k_col  = 200;
	int k_min  = 15;

	for( size_t i=0; i<nb; i++ )
	{
		auto colR = 1.0*rand() / RAND_MAX * k_col + k_min;
		auto colG = 1.0*rand() / RAND_MAX * k_col + k_min;
		auto colB = 1.0*rand() / RAND_MAX * k_col + k_min;
		vcol[i] = img::Color(colR,colG,colB);
	}
	return vcol;
}

template<typename T>
void process( const T& vec, std::string fname )
{
	auto lamb_func = [&v_colors](int i)   // lambda, needed to fetch color from index
	{
		return img::DrawParams().setColor( v_colors[i] );
	};
	std::function<img::DrawParams(int)> f(lamb_func);

//	img1.clear();
	img2.clear();
//	draw( img1, vec, f );
	draw( img2, vec, f );

	auto bb = getBB( vec );
//	bb.draw( img1, DrawParams().setColor(250,50,20) );
	bb.draw( img2, DrawParams().setColor(250,50,20) );

//	img1.write( fname + ".png" );
	img2.write( fname + ".svg" );
}

int main()
{
	std::vector<Segment> vsegs;
	std::vector<Point2d> vpts;
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
	srand( time(0) );
	int nbSegs = 40;

	v_colors = genRandomColors( 2 * nbSegs );

	for( int i=0; i<nbSegs; i++ )
	{
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * ymax + xmin;
		vpts.emplace_back( Point2d(x1,y1) );

		auto x2 = x1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		auto y2 = y1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		vpts.emplace_back( Point2d(x2,y2) );
		vsegs.emplace_back( Segment( x1,y1,x2,y2) );

		v_polypts.emplace_back( Point2d(x1,y1) );
		if( v_polypts.size() == polySize )
		{
			v_poly.emplace_back( CPolyline(v_polypts) );
			polySize = 1.*std::rand()/RAND_MAX * nb_pts_poly_max + nb_pts_poly_min;
			v_polypts.clear();
		}
	}
	auto k = 1.5;

//	img1.setSize( k*xmax, k*ymax );
	img2.setSize( k*xmax, k*ymax );

	std::vector<FRect> vrects;
	for( const auto& seg: vsegs )
		vrects.emplace_back( seg.getPts() );

	std::vector<Circle> vcircles;
	for( const auto& seg: vsegs )
	{
		auto ppts = seg.getPts();
		vcircles.emplace_back( ppts.first, ppts.second );
	}

	process( vrects,   "bb_Rects" );
	process( vpts,     "bb_Points" );
	process( vsegs,    "bb_Segs" );
	process( vcircles, "bb_Circles" );
	process( v_poly,   "bb_Poly" );
}
