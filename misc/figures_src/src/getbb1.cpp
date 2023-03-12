// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	auto H = Homogr().setScale(30).addTranslation(10,30);

	img::Image<img::SvgImage> im( 450,200 );

	Circle c( 1.4, 1., 1.5 );
	auto c2 = H*c;

	getBB(c2).draw( im );
	c2.draw( im, DrawParams().setColor(250,0,20) );

	CPolyline p(
		std::vector<Point2d>{
			{4,1},{6,.2},{4.9,1.7},{6.2,2.4},{5,3.8}
		}
	);
	auto p2 = H*p;
	p2.getBB().draw( im );
	p2.draw( im, DrawParams().setColor(250,0,20) );

	Ellipse e( 10.,2., 2., 1.2, 25*M_PI/180. );
	auto e2 = H*e;
	getBB(e2).draw(  im, DrawParams().setColor(10,0,250) );
	getOBB(e2).draw( im, DrawParams().setColor(10,250,0) );
	e2.draw( im, DrawParams().setColor(250,0,20) );

	im.write( "getbb1a.svg" );
	im.clear();

	FRect r( 6.8,0.1, 7.3,2.7 );
	auto r2= H*r;

	auto style1 = DrawParams().setColor(40,200,40);
	getBB( c2,p2).draw( im, DrawParams().setColor(40,40,200) );
	getBB( r2,p2).draw( im, style1 );
	getBB( r2,e2).draw( im, DrawParams().setColor(200,40,40) );

//	getBB( p2,r2).draw( im, DrawParams().setColor(40,40,200) );
//	getBB( p2,e).draw( im, DrawParams().setColor(200,40,40) );

	r2.draw( im, DrawParams().setColor(30,0,20) );
	e2.draw( im, DrawParams().setColor(30,0,20) );
	p2.draw( im, DrawParams().setColor(30,0,20) );
	c2.draw( im, DrawParams().setColor(30,0,20) );
	im.write( "getbb1b.svg" );
}
