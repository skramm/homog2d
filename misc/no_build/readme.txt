In this folder, some code that MUST NOT build.
Checking is implemented through the makefile target "nobuild"

Below, file and illegal code that is checked for

file:
no_build_1.cxx : li1.distTo(li2)
no_build_2.cxx : li.isInside(pt1,pt2);
no_build_3.cxx : pt.getOrthogonalLine( GivenCoord::X, 2 );
no_build_4.cxx : pt.isParallelTo( li );
no_build_5.cxx : point.getAngle( line )
no_build_6.cxx : build a homography matrix using a point
no_build_7.cxx : build a homography matrix using a pair of points
no_build_8.cxx : intersection of a point and a polyline
no_build_9.cxx :
