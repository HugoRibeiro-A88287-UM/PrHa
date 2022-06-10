/**
 * @file licensePlate_Ha.cpp
 * @author PrHa
 * @brief License Plate recognition module include
 * @version 0.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */


#include "../inc/licensePlate_Ha.hpp"

std::unique_ptr< PlateDetect > modelPointer;


bool initPlateDetect(void){

	modelPointer = vitis::ai::PlateDetect::create("plate_detect", true);

	if(modelPointer == nullptr)
		return false;

	return true;
}

void plateDetect(Mat image, int length){

	auto start = std::chrono::steady_clock::now();

	vitis::ai::PlateDetectResult result = modelPointer->run(image);

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    cout << "Hardware -> "<< elapsed.count() << " microseconds "<< endl<<endl;

	auto rect = cv::Rect{
	        (int)(result.box.x * image.cols), (int)(result.box.y * image.rows),
	        (int)(result.box.width * image.cols), (int)(result.box.height * image.rows)};

	Mat croped = image(rect);

	string name_out = "./imgHA_" + to_string(length) + ".jpg";
	cv::imwrite(name_out.c_str(),croped);

}


