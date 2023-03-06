// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

template<typename T>
void process( img::Image<img::SvgImage>& im, const T& poly, std::string fn, bool doFill=false )
{
	im.clear();
	if( !doFill )
		poly.draw( im, DrawParams().setColor(250,0,20) );
	else
		poly.draw( im, DrawParams().setColor(250,0,20).setAttrString("fill=\"rgb(150,200,150)\"") );
	poly.getBB().draw( im, DrawParams().setColor(150,150,120) );

	auto style1 = DrawParams().setPointStyle(PtStyle::Dot).setThickness(2).setColor(0,250,0);
	auto style2 = DrawParams().setPointStyle(PtStyle::Dot).setThickness(2).setColor(0,0,250);
	getTmPoint( poly ).draw( im, style1 );
	getRmPoint( poly ).draw( im, style2 );
	getLmPoint( poly ).draw( im, style1 );
	getBmPoint( poly ).draw( im, style2 );
	im.write( fn );
}

int main()
{
	std::vector<Point2d> vpts{
		{4.5,4.2},
		{7,3},
		{6,2},
		{7.5, 1.},
		{4, 0},
		{5.5, 2.5},
		{4., 1.2},
		{3., 2.2},
		{3.6, 4.1},
		{1.6, 5.4}
		,{4.5, 6.2}
	};
	OPolyline opl(vpts);

	auto H = Homogr().setScale(30).addTranslation(10,30);
	opl = H * opl;
	CPolyline cpl(opl);

	img::Image<img::SvgImage> im( 350,250 );

	process( im, opl, "polyline1a.svg" );
	process( im, opl, "polyline1a_f.svg", true );
	process( im, cpl, "polyline1b.svg" );
	process( im, cpl, "polyline1b_f.svg", true );
}
