In this folder, some code that MUST NOT build.
Checking is implemented through the makefile target "nobuild"

Below, file and illegal code that is checked for

file:
no_build_01.cxx : li1.distTo(li2)
no_build_02.cxx : li.isInside(pt1,pt2);
no_build_03.cxx : pt.getOrthogonalLine( GivenCoord::X, 2 );
no_build_04.cxx : pt.isParallelTo( li );
no_build_05.cxx : point.getAngle( line )
no_build_06.cxx : build a homography matrix using a point
no_build_07.cxx : build a homography matrix using a pair of points
no_build_08.cxx : intersection of a point and a OPolyline
no_build_09.cxx : intersection of a point and a CPolyline
no_build_10.cxx : homography applied to a FRect does not produce a FRect
no_build_11.cxx : cannot build an OPolyline from a closed one
no_build_12.cxx : cannot build an OPolyline from FRect


no_build_16.cxx : cannot build an OPolyline from a non-container object
no_build_17.cxx : cannot build an CPolyline from a non-container object

no_build_18.cxx : a point cannot be inside a line
no_build_19.cxx : a point cannot be inside a segment

