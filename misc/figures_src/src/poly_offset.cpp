// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig


#include "fig_src.header"



int main()
{
	std::vector<Point2d> vpts1{ {1,1},{3,4},{1,3} };

	CPolyline pol(vpts1);

	auto H = Homogr().setScale(30).addTranslation(50,30);
	img::Image<img::SvgImage> im( 550,250 );

	auto pol2 = H*pol;
	draw( im, pol2 );
	std::cout << "main: pol2=" << pol2 << '\n';
	auto poff = pol2.buildOffset( 30 );
	im.write( "poly_offset.svg" );
}
