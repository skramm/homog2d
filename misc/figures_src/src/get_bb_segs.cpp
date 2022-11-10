// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

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

int main()
{
	std::vector<Segment> vsegs;
	int xmax = 300;
	int xmin = 40;
	int seg_max = 50;
	int seg_min = 10;
	srand( time(0) );
	int nbSegs = 60;
	for( int i=0; i<nbSegs; i++ )
	{
		auto x1 = 1.*std::rand()/RAND_MAX * xmax + xmin;
		auto y1 = 1.*std::rand()/RAND_MAX * xmax + xmin;

		auto x2 = x1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		auto y2 = y1 + ((2.*std::rand()/RAND_MAX)-1.) * seg_max + seg_min;
		vsegs.push_back( Segment( x1,y1,x2,y2) );
	}
	auto k = 1.3;

	img::Image<cv::Mat>       img1( k*xmax, k*xmax );
	img::Image<img::SvgImage> img2( k*xmax, k*xmax );

//	img1.clear();
//	img2.clear();
	auto bbpts = getBB( vsegs );
	bbpts.draw( img1, DrawParams().setColor(250,50,20) );
	bbpts.draw( img2, DrawParams().setColor(250,50,20) );

	auto vcol = genRandomColors( nbSegs );
	auto lamb_func = [&](int i)   // lambda, needed to fetch color from index
	{
		return img::DrawParams().setColor( vcol[i] );
	};
	std::function<img::DrawParams(int)> f(lamb_func);

	draw( img1, vsegs, f );
	draw( img2, vsegs, f );
	img1.write( "bbSegs.png" );
	img2.write( "bbSegs.svg" );
}
