
echo "- Start test with float"
BUILD/homog2d_test_f >BUILD/stderr_f
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with double"
BUILD/homog2d_test_d >BUILD/stderr_d
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with long double"
BUILD/homog2d_test_l >BUILD/stderr_l
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

