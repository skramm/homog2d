// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

#include "fig_src.header"

Line2d l1, l2;
Point2d ptA, ptB, ptC, ptD;
int a = 180;
int b = 80;

template<typename IM>
void drawInitial( IM& im, std::array<Point2d,4> pts, Point2d pt3 )
{
	draw( im, pts );
	drawText( im, "A", ptA );
	drawText( im, "B", ptB );
	drawText( im, "C", ptC );
	drawText( im, "D", ptD );

	Segment s0( ptA, ptB );
//	auto dist = s0.length();
	Line2d l0 = s0.getLine();

	draw( im, l0, DrawParams().setColor(250,a,a) );
	draw( im, s0, img::DrawParams().setColor(250,0,0) );

	l1 = l0.getOrthogLine( pt3 );
	draw( im, l1, img::DrawParams().setColor(250,a,a) );

	draw(im, pt3 );
}

template<typename IM, typename PTARR>
void process2(
	IM& im,
	PTARR pts,
	Point2d ptx,
	std::string id1,
	std::string id2,
	Point2d pt4,
	Point2d pt3
)
{
	im.clear();
	drawInitial(im, pts, ptx );

	l2 = ptx * pt4;
	draw( im, l2, DrawParams().setColor(b,b,250) );
	im.write( "build_square_" + id1 + id2 + "_1.svg" );

	auto l3a = l2.getOrthogLine( ptA );
	auto l3b = l2.getOrthogLine( ptB );
	draw( im, l3a, DrawParams().setColor(b,250,b) );
draw( im, l3b, DrawParams().setColor(b,250,b) );
	im.write( "build_square_" + id1 + id2 + "_2.svg" );

	auto l4 = l3a.getOrthogLine( pt3 );
	draw( im, l4, DrawParams().setColor(b,250,b) );
	im.write( "build_square_" + id1 + id2 + "_3.svg" );
}

template<typename IM, typename PTARR, typename PPTS>
void process1(
	IM& im,
	PTARR pts,
	Point2d pt1,
	Point2d pt2,
	std::string id,
	PPTS ppts       ///< pair of points on the orthogonal line
)
{
//    draw( im, l1, img::DrawParams().setColor(250,a,a) );

//	auto d1 = ppts.first.distTo( l0 );
//	auto d2 = ppts.second.distTo( l0 );

	auto ptx = ppts.first;
	process2( im, pts, ptx, id, "1", pt1, pt2 );

	ptx = ppts.second;
	process2( im, pts, ptx, id, "3", pt1, pt2 );
}

int main()
{
	ptA= Point2d(100,135);
	ptB=Point2d(280,180);
	ptC.set(265,80);
	ptD.set(130,310);
	std::array<Point2d,4> pts = { ptA, ptB, ptC, ptD };

	img::Image<img::SvgImage> im( 400,400 );
	Segment s0( ptA, ptB );
	auto dist = s0.length();
	Line2d l0 = s0.getLine();

//	draw( im, l0, DrawParams().setColor(250,a,a) );
//	draw( im, s0, img::DrawParams().setColor(250,0,0) );
//	im.write( "build_square_0.svg" );

	l1 = l0.getOrthogLine( ptC );
	auto ppts = l1.getPoints( ptC, dist );
	process1( im, pts, ptD, ptC, "C", ppts );

	l1 = l0.getOrthogLine( ptD );
	ppts = l1.getPoints( ptD, dist );
	process1( im, pts, ptC, ptD, "D", ppts );
}
