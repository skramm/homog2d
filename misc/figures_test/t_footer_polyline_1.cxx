// file t_footer_polyline.cxx
// used in makefile target test_fig

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );

// grid
	int g=220, g0=g/2;
	auto colgrid  = img::DrawParams().setColor(g,g,g);
	auto colgrid0 = img::DrawParams().setColor(g0,g0,g0).setThickness(2);
	for (int i=0; i<6; i++ )
	{
		Line2d li( LineDir::H, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img2, colgrid0 );
		else
			li2.draw( img2, colgrid );
	}
	for (int i=0; i<7; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img2, colgrid0 );
		else
			li2.draw( img2, colgrid );
	}

	auto poly1 = H*pl;
	auto dp = img::DrawParams().setColor(250,0,20).showPoints(true).setPointStyle(img::PtStyle::Times).showIndex(true);
	poly1.draw( img2, dp );

#ifdef TESTFIG_HAS_POLY2
	auto poly2 = H*pl2;
	poly2.draw( img2, img::DrawParams().setColor(20,250,20) );
#endif

	img2.write( std::string(argv[0])+ ".svg" );
}

