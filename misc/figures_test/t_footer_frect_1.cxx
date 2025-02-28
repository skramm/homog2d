// file t_footer_frect_1.cxx
// used in makefile target test-fig

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );
	drawGrid( img2, H );

	auto r1b = H*r1;
	auto r2b = H*r2;

	img2.drawText( "r1", getBB(r1b).getPts().second, img::DrawParams().setColor(250,0,0) );
	img2.drawText( "r2", getBB(r2b).getPts().second, img::DrawParams().setColor(0,0,250) );

	r1b.draw( img2, img::DrawParams().setColor(250,0,20) );
	r2b.draw( img2, img::DrawParams().setColor(20,0,250) );

	img2.write( std::string(argv[0])+ ".svg" );
}

