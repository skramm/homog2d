// file t_footer_polyline.cxx
// used in makefile target test-fig

	OPolyline psrc_o( src );
	CPolyline psrc_c( src );

	std::ofstream fhtml_O{ "polysplit_O.html", std::ios::app };
	std::ofstream fhtml_C{ "polysplit_C.html", std::ios::app };

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 40,40 );


	auto fname = std::regex_replace( std::string(argv[0]), std::regex("/bin/"), "/out/" );

	processSource( psrc_o, li, H, fname, "O" );
	processSource( psrc_c, li, H, fname, "C" );


	processAll( psrc_o, fname, "O", vv_pts_O, li, H, fhtml_O );
	processAll( psrc_c, fname, "C", vv_pts_C, li, H, fhtml_C );
}

template<typename PTYPE,typename HO>
void
processSource( const PTYPE& poly, const Line2d& li, const HO& H, std::string fname, std::string OC )
{
	auto dp = img::DrawParams().setThickness(3);

	img::Image<img::SvgImage> im_src(400,300);
	drawGrid( im_src, H );
	(H*poly).draw( im_src, dp.setColor(250,0,0) );

	(H*li).draw( im_src, dp.setColor(0,250,0).setThickness(2) );
	im_src.write( fname+"_"+OC+".svg" );
}


template<typename TYPE,typename VVPTS>
void drawExpectedSplit( TYPE, const VVPTS& vv_pts, const Homogr& H, img::DrawParams dp, string fname, string t, std::ofstream& fhtml )
{
	if( !vv_pts.size() )
	{
		fhtml << "<p>NIL</p>\n";
		return;
	}

// we need to store them so we can sort them
	std::vector<TYPE> v_exp;
	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		const auto& v_pts = vv_pts[i];
		v_exp.push_back( TYPE(v_pts) );
	}
	std::sort( v_exp.begin(), v_exp.end() );

	fhtml << "<table border='1'><tr>\n";
	for( size_t i=0;i<vv_pts.size(); i++ )
		fhtml << "<td>" << v_exp[i] << "</td>\n";
	fhtml << "</tr><tr>\n";

	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		(H*v_exp[i]).draw( im, dp );
		char t2 = (v_exp[i].isClosed() ? 'c' : 'o');

		auto fn1 = fname+t+t2+'_'+std::to_string(i)+".svg";
		im.write( fn1 );
		fhtml << "<td><img src='"<< fn1 << "'></td>\n";
	}
	fhtml << "</tr></table>\n";
}



template<typename POLYSET,typename DP>
void
drawReal( POLYSET pset, const Homogr& H, DP dp, std::string fname, std::ofstream& fhtml )
{
	std::sort( pset.begin(), pset.end() );
	fhtml << "<table border='1'><tr>\n";
	for( size_t i=0;i<pset.size(); i++ )
		fhtml << "<td>" << pset[i] << "</td>\n";
	fhtml << "</tr><tr>\n";

	for( size_t i=0;i<pset.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		const auto& poly = H*pset[i];
		poly.draw( im, dp );
		auto fn = fname+std::to_string(i)+".svg";
		std::cout << "drawSplit(): name=" << fn << '\n';
		im.write( fn );
		fhtml << "<td><img src='"<< fn << "'></td>\n";
	}
	fhtml << "</tr></table>\n";

}

template<typename TYPE,typename POLY,typename DP>
void drawRealSplit( TYPE, const POLY& poly, const Line2d& li, const Homogr& H, DP dp, std::string fname, std::string suffix, std::ofstream& fhtml )
{
	auto dummy = TYPE();
	if( dummy.isClosed() )
		drawReal( poly.splitC( li ), H, dp, fname + suffix, fhtml );
	else
		drawReal( poly.splitO( li ), H, dp, fname + suffix, fhtml );
}

template<typename POLY,typename VVPTS>
void
processAll(
	const POLY&    poly,   ///< the polyline we want to split (open or closed)
	std::string    name,   ///< source filename
	std::string    OC,     ///< "O" or "C" (source polyline type)
	const VVPTS&   vv_pts, ///< expected set of points
	const Line2d&  li,     ///< line used for the split
	const Homogr&  H,      ///< Homography, needed to draw the grid
	std::ofstream& fhtml   ///< output HTML file
)
{
	auto fn = name + "_" + OC;
	auto dp = img::DrawParams().setColor(250,0,0).showPoints(true).setPointSize(7).setThickness(2);


	fhtml<< "<h2>Source polyline: " << fn << " (open output)</h2>\n"
		<< "<img src='" << fn+".svg" << "'><br>\n";


	fhtml << "<h3>1 - Open output</h3>\n";
	fhtml << "<h4>1.1 - Expected</h4>\n";
	drawExpectedSplit( OPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
	fhtml << "<h4>1.2 - Generated</h4>\n";
	drawRealSplit( OPolyline(), poly, li, H, dp, name, "_real_"+OC+"_o", fhtml );
	fhtml << "<hr>\n";

	fhtml<< "<h2>Source polyline: " << fn << " (closed output)</h2>\n"
		<< "<img src='" << fn+".svg" << "'><br>\n";

	fhtml << "<h3>2 - Closed output</h3>\n";
	fhtml << "<h4>2.1 - Expected</h4>\n";
	drawExpectedSplit( CPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
	fhtml << "<h4>2.2 - Generated</h4>\n";
	drawRealSplit( CPolyline(), poly, li, H, dp, name, "_real_"+OC+"_c", fhtml );
	fhtml << "<hr>\n";
}


