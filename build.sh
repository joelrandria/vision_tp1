#!/bin/bash

g++ -o executable -I"/Shared/TP_VTDVR/LIRIS-VISION" -I"/usr/include/opencv" source_code.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_flann
