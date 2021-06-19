In this folder, some code that MUST NOT build.
Checking is implemented through the makefile target "nobuild"

Below, file and illegal function that is checked for

file:
no_build_1.cxx : li1.distTo(li2)
no_build_2.cxx : li.isInsideRectangle(pt1,pt2);
no_build_3.cxx : pt.getOrthogonalLine( GivenCoord::X, 2 );
no_build_4.cxx : pt.isParallelTo( li );
no_build_5.cxx : pt.getAngle( li )
