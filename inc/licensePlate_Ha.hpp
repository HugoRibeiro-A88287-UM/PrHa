/**
 * @file licensePlate_Ha.hpp
 * @author PrHa
 * @brief License Plate recognition module include
 * @version 0.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */


#include <iostream>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vitis/ai/platedetect.hpp>
#include <vitis/ai/profiling.hpp>

using namespace std;
using namespace vitis::ai;
using namespace cv;




bool initPlateDetect(void);

void plateDetect(Mat image, int length);
