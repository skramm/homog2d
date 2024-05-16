// sample test file, used to make sure everything is correctly inlined

/**
\file
\brief used to check build of multiple files with MS compiler, see msvc.yml
*/

#include "mylib.hpp"
void DummyTest::foo()
{
	pt.set(3,4);
}
