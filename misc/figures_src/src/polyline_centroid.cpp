// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

#include "fig_src.header"

template<typename IMG,typename T>
void process( IMG& im, const T& poly )
{
//	im.clear();

	auto style1 = DrawParams().setPointStyle(PtStyle::Diam).setThickness(1).setColor(250,0,0);
	auto style2 = DrawParams().setColor(0,0,250);
	auto style3 = DrawParams().setColor(0,250,0);
	auto pc = poly.centroid();
	for( const auto& pt: poly.getPts() )
		draw( im, Segment(pt,pc), style3 );
	draw( im, pc,  style1 );

	draw( im, poly, style2 );
//	im.write( fn );
}

int main()
{
	img::Image<img::SvgImage> im( 450,250 );

	{
		std::vector<Point2d> vpts{
			{25,20},
			{160,63},
			{85,156}
		};
		CPolyline cpl(vpts);
		process( im, cpl  );
	}
	{
		std::vector<Point2d> vpts{
			{25,30},
			{145,63},
			{105,146},
			{45,89}
		};
		CPolyline cpl(vpts);
		cpl.translate( 160, -10 );
		process( im, cpl  );
	}

	{
		std::vector<Point2d> vpts{
			{25,30},
			{80,63},
			{123,22},
			{95,146},
			{15,89},
			{54,79}
		};
		CPolyline cpl(vpts);
		cpl.translate( 100, 80 );
		process( im, cpl  );
	}

	{
		std::vector<Point2d> vpts{
			{0,0},
			{100,3},
			{100,20},
			{20,20},
			{22,60},
			{80,65},
			{83,88},
			{10,88}
		};
		CPolyline cpl(vpts);
		cpl.translate( 320, 10 );
		process( im, cpl  );
	}

	{
		std::vector<Point2d> vpts{
			{0,0},
			{100,3},
			{100,100},
			{70,120},
			{79,30},
			{0,30}
		};
		CPolyline cpl(vpts);
		cpl.translate( 320, 120 );
		process( im, cpl  );
	}

	im.write( "polyline_centroid.svg" );

//	process( im, opl, "polyline1a_f.svg", true );
//	process( im, cpl, "polyline1b.svg" );
//	process( im, cpl, "polyline1b_f.svg", true );
}
