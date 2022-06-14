#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <chrono>

#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../inc/platerecog.hpp"
#include <vitis/ai/platenum.hpp>
#include <vitis/ai/platedetect.hpp>
#include <vitis/ai/demo.hpp>

using namespace std::chrono;
using namespace std;
using namespace cv;
using namespace vitis::ai;


cv::Mat process_result(cv::Mat &m1, const vitis::ai::PlateRecogResult &result,
                       bool is_jpeg) {

		if( result.box.score > 0.9){
			  cout << "Plate Color: " << result.plate_color << endl           //
				   << "Plate Number: " << result.plate_number << endl         //
			  //<< "Time: "<< elapsed.count() << " microseconds "<< endl		   //

			  << endl;
		}

  cv::Mat image;
  cv::resize(m1, image, cv::Size{result.width, result.height});

  auto rect = cv::Rect{(int)(result.box.x * image.cols),
                       (int)(result.box.y * image.rows),
                       (int)(result.box.width * image.cols),
                       (int)(result.box.height * image.rows)};
  cv::putText(image, result.plate_number, cv::Point(result.box.x*image.cols , result.box.y*image.rows), 1, 1, cv::Scalar(0, 0, 255),1,1,false);
  cv::rectangle(image, rect, cv::Scalar(0, 0, 255));

  return image;
}

int main(int argc, char *argv[]) {

	int arrayLen = 6;
	char* array[arrayLen]
	        = { "PrHa", "plate_detect", "plate_num", "0", "-t" ,"8" };

	std::string model = array[1];
	std::string model1 = array[2];

	return vitis::ai::main_for_video_demo(
		arrayLen,array,
		  [model,model1] {
			return vitis::ai::PlateRecog::create(model,model1);
		  },
		  process_result, 3);


	return 0;
}



/* int main(int argc, char *argv[]){

	auto det = PlateRecog::create("plate_detect", "plate_num", true);

	for(int i = 0 ; i < 7 ; i++){

		  string image_path = "ExamplePhotosAsian/" + to_string(i) + ".jpg";
		  Mat image = imread(image_path, IMREAD_COLOR);



		  auto start = std::chrono::steady_clock::now();

		auto result = det->run(image);

        //time(&stop);
        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		std::cout << "Score: " << result.box.score << endl                    //
				  << "Plate Color: " << result.plate_color << endl           //
				  << "Plate Number: " << result.plate_number << endl         //
				  << "Time: "<< elapsed.count() << " microseconds "<< endl		   //
				  << endl;

		auto rect =
			cv::Rect{cv::Point{(int)(result.box.x), (int)(result.box.y)},
					 cv::Size{(int)(result.box.width), (int)(result.box.height)}};

	  	Mat croped = image(rect);

	  	string name_out = "./" + to_string(i) + ".jpg";
	  	string name_out2 = "./Test" + to_string(i) + ".jpg";
	  	cv::imwrite(name_out.c_str(),croped);
	  	cv::imwrite(name_out2.c_str(),imread("/dev/ptyaf"));



	  }

  return 0;

}*/
