# Opencv binding problems

[Manual main page](homog2d_manual.md)

__Please read this if you have Opencv installed on your system, but fail to build the Opencv binding demo.__


The [Opencv](https://opencv.org/releases/) binding capabilities for this library are demonstrated with a short graphical demo, included here.
Building the app using the provided Makefile (`$ make demo`) relies on the `pkg-config` utility.

Recently, with Opencv4 (and probably also with latest 3.x releases), the Opencv team seems to have dropped support of pkg-config
(see https://github.com/opencv/opencv/issues/13154).

The demo program has been tested using Opencv 3.4, build from source.
The trick is to activate the pkg-config option when building Opencv with Cmake.
I used:
```
cmake .. -DOPENCV_GENERATE_PKGCONFIG=ON -DWITH_FFMPEG=ON
make
sudo make install
```
However, while this indeed generates the `opencv.pc` file (at least with Opencv3.4), it does not install it on your system.
Thus the makefile for demo_opencv fails.
You need to copy it from the opencv build folder to the pkg-config usual location.
From the OpenCv 'build' folder, something like this should be sufficient:
```
$ sudo cp unix-install/opencv.pc /usr/share/pkgconfig/
```

FWIW, in case it does not get generated with your Opencv release, here is the content of that file for release 3.4.14.
Name it `opencv.pc` and copy it to `/usr/share/pkgconfig/`.

```
# Package Information for pkg-config

prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir_old=${prefix}/include/opencv
includedir_new=${prefix}/include

Name: OpenCV
Description: Open Source Computer Vision Library
Version: 3.4.14
Libs: -L${exec_prefix}/lib -lopencv_dnn -lopencv_highgui -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_videoio -lopencv_imgcodecs -lopencv_features2d -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core
Libs.private: -ldl -lm -lpthread -lrt
Cflags: -I${includedir_old} -I${includedir_new}
```


