no-pkg:
	g++ -std=c++11 -I/usr/local/include/opencv4/opencv2 -I/usr/local/include/opencv4 -L/usr/local/lib  -lopencv_calib3d   -lopencv_core  -lopencv_dnn  -lopencv_features2d  -lopencv_flann  -lopencv_gapi  -lopencv_highgui  -lopencv_imgcodecs  -lopencv_imgproc  -lopencv_ml  -lopencv_objdetect  -lopencv_photo  -lopencv_stitching  -lopencv_videoio  -lopencv_video `pkg-config --cflags --libs libcurl` `pkg-config --cflags --libs jsoncpp` src/date.cpp src/http_request.cpp src/image.cpp src/main.cpp -o main

all:
	g++ -std=c++11 `pkg-config --cflags --libs opencv4` `pkg-config --cflags --libs libcurl` `pkg-config --cflags --libs jsoncpp` src/date.cpp src/http_request.cpp src/image.cpp src/main.cpp -o main

clean:
	rm -f main