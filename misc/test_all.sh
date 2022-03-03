
echo "- Start test with float"
BUILD/homog2d_test_f 1>BUILD/stdout_f 2>BUILD/stderr_f
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with double"
BUILD/homog2d_test_d 1>BUILD/stdout_d 2>BUILD/stderr_d
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

echo "- Start test with long double"
BUILD/homog2d_test_l 1>BUILD/stdout_l 2>BUILD/stderr_l
if [ $? = 0 ]; then echo "Success"; else echo "Failure !"; fi

