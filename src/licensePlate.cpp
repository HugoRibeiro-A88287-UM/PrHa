/**
 * @file licensePlate.cpp
 * @author PIgate
 * @brief License Plate recognition module implementation
 * @version 0.1
 * @date 2022-02-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "fcntl.h"
#include "unistd.h"
#include <iostream>
#include <string>


#include "../inc/licensePlate.hpp"


CascadeClassifier plate_cascade;


int init_cascade()
{
    //Get the license plate cascade name 
    string license_plate_cascade_name = "/usr/share/opencv4/haarcascades/cascade.xml"; 
    if( !plate_cascade.load( license_plate_cascade_name ) )
    {
        cout << "--(!)Error loading face cascade\n";
        return -EXIT_FAILURE;
    };
    return EXIT_SUCCESS;
}



Mat take_picture()
{
    //imread funtion take a photo with the selected device
    Mat frame = imread(CAMERA);
 
    return frame;
}



int detectPlate( Mat frame, int len )
{
    Mat frame_gray;
    std::vector<Rect> license_plates;

    /*turns image into gray scale*/
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    
	auto start = std::chrono::steady_clock::now();

	plate_cascade.detectMultiScale( frame_gray, license_plates );

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


    cout << "Software -> "<< elapsed.count() << " microseconds "<< endl;


    string plate;
    
    //If Plate not found
    if(license_plates.size() == 0)
        return -EXIT_FAILURE;

    //for each found license plate, run read_license_plates
    for ( size_t i = 0; i < license_plates.size(); i++ )
    {
        //draw a rectangle arround the license plate on the original image
        Point corner_1(license_plates[i].x, license_plates[i].y);
        Point corner_4(license_plates[i].x + license_plates[i].width, license_plates[i].y + license_plates[i].height);
        rectangle(frame, corner_1, corner_4, Scalar( 255, 0, 255 ), 1, LINE_8, 0);
        Mat plate_pic = frame_gray( license_plates[i] );

        //create a .jpg file of the croped license plate
        string imageName = "./" + to_string(len) + ".jpg";

        imwrite(imageName.c_str(), plate_pic);

    }
   
   return len;
}



string read_license_plates(uint16_t plate)
{
    char text_plate[9] = {'\0'};
    string plate_leters;
    /*run tesseract 
    outputs into output.txt file
    PSM -page segmentation mode = 8 -> Treat the image as a single word 
    OEM - OCR engine mode = 2 -> tesseract + LSTM 
    --dpi -> image resolution estimation
    --config file contains pattern information, whitelisted characters and blacklisted characters */
    string callTerminal = "tesseract ./" + to_string(plate) + ".jpg ./output --psm 8 --oem 2 --dpi 300 ./config";
    system(callTerminal.c_str());

    //read tesseract output from file
    int fd0 = open("output.txt", O_RDONLY);
     if(fd0 == -1)
    {
        perror("Cannot Open Output.txt \n");
        return "ERROR";
    }
    read(fd0,text_plate, 7);
    close(fd0);
    
    //convert to string
    plate_leters = text_plate;
        //remove first recognized character, corresponds to the country indication on the plate, 6 is the usefull number of characters in a PT license plate
    if((plate_leters.length()) > 6)
    {
        plate_leters.erase(0,1);
    }
    if(plate_leters.length() != 6)
    {
        return "ERROR";
    }
    
    return plate_leters;
}
