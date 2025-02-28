// file t_footer_polyline.cxx
// used in makefile target test-fig

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );
	drawGrid( img2, H );

	auto poly1 = H*pl;
	auto dp = img::DrawParams().setColor(250,0,20).showPoints(true).setPointStyle(img::PtStyle::Times).showIndex(true);
	poly1.draw( img2, dp );

#ifdef TESTFIG_HAS_POLY2
	auto poly2 = H*pl2;
	poly2.draw( img2, img::DrawParams().setColor(20,250,20) );
#endif

	img2.write( std::string(argv[0])+ ".svg" );
}

