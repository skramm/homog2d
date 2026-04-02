// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc-fig

/// ref: http://kirkmcd.princeton.edu/examples/4point.pdf

#include "fig_src.header"

//Line2d l1, l2;
Point2d ptA, ptB, ptC, ptD;
//int a = 180;
///int b = 80;

int main()
{
	int a = 70;
	int b = 140;

	auto colr1 = DrawParams().setColor(250,a,a);
	auto colg1 = DrawParams().setColor(a,250,a);
	auto colr2 = DrawParams().setColor(250,b,b);
	auto colg2 = DrawParams().setColor(b,250,b);
	auto colb1 = DrawParams().setColor(a,a,250);
	auto colb2 = DrawParams().setColor(b,b,250);


	ptA= Point2d(85,115);
	ptB=Point2d(265,160);
	ptC.set(295,205);
	ptD.set(70,300);
	std::array<Point2d,4> pts = { ptA, ptB, ptC, ptD };

	img::Image<img::SvgImage> im( 400,400 );
	draw( im, pts );
	drawText( im, "A", ptA );
	drawText( im, "B", ptB );
	drawText( im, "C", ptC );
	drawText( im, "D", ptD );

	Circle cAB( ptA, ptB );
	Circle cCD( ptC, ptD );
	draw( im, cAB, colr1 );
	draw( im, cCD, colg1 );

	auto sAB = Segment(ptA,ptB);
	auto sCD = Segment(ptC,ptD);

	auto ptE = cAB.getCenter();
	auto ptG = cCD.getCenter();
	draw( im, ptE );
	draw( im, ptG );
	draw( im, sAB, colr1 );
	draw( im, sCD,colg1 );
	drawText( im, "E", ptE );
	drawText( im, "G", ptG );

	im.write( "build_square2_0.svg" );

	auto liE = sAB.getLine().getOrthogLine(ptE);
	auto liG = sCD.getLine().getOrthogLine(ptG);
	draw( im, liE, colr2 );
	draw( im, liG, colg2 );

	auto pptsE = liE.getPoints( ptE, sAB.length()/2. );
	auto pptsG = liG.getPoints( ptG, sCD.length()/2. );
	draw( im, pptsE );
	draw( im, pptsG );

	im.write( "build_square2_1.svg" );

// find the points F and H that are the closest to other center
	auto ptF = pptsE.first;
	if( dist( pptsE.second, ptG) < dist( ptF, ptG ) )
		ptF = pptsE.second;

	auto ptH = pptsG.first;
	if( dist( pptsG.second, ptE) < dist( ptH, ptE ) )
		ptH = pptsG.second;

	drawText( im, "F", ptF );
	drawText( im, "H", ptH );
	auto liFH = ptF * ptH;
	draw( im, liFH, colb1 );
	im.write( "build_square2_3.svg" );

	Point2d ptI, ptK;

	auto itI = cAB.intersects( liFH );
	if( itI() )
	{
		auto ppts = itI.get();
		draw( im, ppts, colb1 );
		auto d1 = dist( ppts.first,  ptG );
		auto d2 = dist( ppts.second, ptG );
		ptI =(  d1>d2 ? ppts.first : ppts.second );
		drawText( im, "I", ptI );
	}

	auto itK = cCD.intersects( liFH );
	if( itK() )
	{
		auto ppts = itK.get();
		draw( im, ppts, colb1 );
		auto d1 = dist( ppts.first,  ptG );
		auto d2 = dist( ppts.second, ptG );
		ptK =(  d1>d2 ? ppts.first : ppts.second );
		drawText( im, "K", ptK );
	}

	im.write( "build_square2_4.svg" );

	auto liAI = ptI * ptA;
	auto liBI = ptI * ptB;
	auto liCK = ptK * ptC;
	auto liDK = ptK * ptD;

	draw( im, liAI, colb2 );
	draw( im, liBI, colb2 );
	draw( im, liDK, colb2 );
	draw( im, liCK, colb2 );
	im.write( "build_square2_5.svg" );

}

