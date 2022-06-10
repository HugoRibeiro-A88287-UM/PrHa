/**
 * @file main.c
 * @author PrHa
 * @brief PrHa Main Code
 * @version 0.1
 * @date 2022-03-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h> /* For mode constants */
#include <linux/types.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h> /* For O_* constants */  
#include <stdint.h>

extern "C" {
#include"../inc/utilits.h"

}

#include "../inc/licensePlate.hpp"
#include "../inc/licensePlate_Ha.hpp"
#include "../inc/fifo.hpp"

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <ctime>
#include <chrono>

using namespace std::chrono;
using namespace std;
using namespace cv;

//FIFO1 Variables
Mat images[FIFOLEN];
fifoPhoto_t imagesFifo;

//Hw Plates
//FIFO1 Variables
Mat imagesHw[FIFOLEN];
fifoPhoto_t imagesFifoHw;


//Thread Priority
enum mainProcessPrio {getPhotoPrio = 2, plateRecognitionPrio, textRecognitionPrio};

//Threads Functions Prototypes
void *t_getPhoto(void *arg);
void *t_plateRecognition(void *arg);
//void *t_textRecognition(void *arg);

/**
 * @brief Parent Process Signal Handler
 * 
 * @param signo Received Signal
 */
static void signalHandler(int signo)
{		
    switch (signo)
    {
        case SIGINT:
        case SIGTERM:
            
            exit(1);        
            break;

        default:
            break;
    }

    return;
}


int main(int count, char *args[])
{

    //Create Fifos
    fifoPhoto_init(&imagesFifo,images,FIFOLEN);

    //Create Fifos
    fifoPhoto_init(&imagesFifoHw,imagesHw,FIFOLEN);

    init_cascade();
    initPlateDetect();

    /*Threads Creation*/

    pthread_attr_t thread_attr;
    struct sched_param thread_param;

    pthread_t getPhoto_id, plateRecognition_id;
    //pthread_t textRecognition_id;
    pthread_t updatePlate_id, plateValidation_id;

    pthread_attr_init(&thread_attr);
    pthread_attr_getschedparam(&thread_attr, &thread_param);

    //Setup the thread priority and trys to create it. If successed the program continues. 
    setupThread(getPhotoPrio, &thread_attr, &thread_param);
    checkFail(  pthread_create(&getPhoto_id, &thread_attr, t_getPhoto, NULL)  ); 

    setupThread(plateRecognitionPrio, &thread_attr, &thread_param);
    checkFail(  pthread_create(&plateRecognition_id, &thread_attr, t_plateRecognition, NULL)  );

//    setupThread(textRecognitionPrio, &thread_attr, &thread_param);
//    checkFail(  pthread_create(&textRecognition_id, &thread_attr, t_textRecognition, NULL)  );

    pthread_join(getPhoto_id,NULL);
    pthread_join(plateRecognition_id,NULL);
//    pthread_join(textRecognition_id,NULL);

    pthread_exit(NULL);

    return 0;

}


void *t_getPhoto(void *arg)
{

   Mat img;
   Mat imgHw;
   string path = "./ExamplePhotos/";
   string pathHw = "./ExamplePhotos/";
   uint16_t counter = 0;
   uint16_t counterHw = 0;

   printf("t_getPhoto Thread is Ready \n");

   while (1)
   {
       sleep(5);


       if(( get_FifoPhotoBuffSize(imagesFifo) == FIFOLEN ) || counter > 14)
       {
           /*Ignore, FIFO is FULL*/
           cout << "The End" << endl;
           sleep(10);
       }
       else
       {

           string image_path = path + to_string(counter) + ".jpg";

           img = imread(image_path, IMREAD_COLOR);

           //cout << "Pushing Image" << endl << img << endl;

           fifoPhoto_push(&imagesFifo,img);

           counter++;

       }

       if(( get_FifoPhotoBuffSize(imagesFifoHw) == FIFOLEN ) || (counterHw > 14))
	  {
		  /*Ignore, FIFO is FULL*/
	  }
	  else
	  {

		  string image_path_Hw = pathHw + to_string(counterHw) + ".jpg";

		  imgHw = imread(image_path_Hw, IMREAD_COLOR);

		  //cout << "Pushing Image" << endl << img << endl;

		  fifoPhoto_push(&imagesFifoHw,imgHw);

		  counterHw++;

	  }

       //sleep(10);

   }


}

void *t_plateDetection(void *arg)
{
   Mat receivedImage;
   Mat receivedImageHw;
   Mat plateImage;
   bool fifoReturn = true;
   bool fifoReturnHw = true;
   //time_t start,stop;

   printf("t_plateDetection is ready \n");

   while (1)
   {



       while( ( fifoReturn = ( fifoPhoto_pop(&imagesFifo,&receivedImage) == -ENODATA ) )
    		   || ( fifoReturnHw = ( fifoPhoto_pop(&imagesFifoHw,&receivedImageHw) == -ENODATA) ) )
       { /*Waits for an image*/
           sleep(1);
       }


       if( ( get_Fifo16BuffSize(platesFifo) == FIFOLEN ) || fifoReturn )
       {
           /*Ignore, FIFO is FULL*/ cout << "SH";
       }
       else
       {

           int detectPlateReturn = detectPlate(receivedImage, (platesFifo.writeIndex & (platesFifo.buff_len-1) ) );

       }

      if( ( get_Fifo16BuffSize(platesFifoHw) == FIFOLEN ) || fifoReturnHw )
	  {
		  /*Ignore, FIFO is FULL*/
	  }
	  else
	  {

    	   plateDetect(receivedImageHw, (platesFifoHw.writeIndex & (platesFifoHw.buff_len-1) ) );

	  }


       sleep(1);

   }


}

//void *t_textRecognition(void *arg)
//{
//    int16_t receivedPlate;
//    uint16_t counter = 1;
//
//    printf("t_textRecognition is ready! \n");
//
//    while (1)
//    {
//
//        while( (receivedPlate = fifo16_pop(&platesFifo) ) == -ENODATA )
//        { /*Waits for an image*/
//            sleep(1);
//        }
//
//
//
//        string plateString = read_license_plates(receivedPlate);
//
//        if(plateString == "ERROR")
//        {
//            /*Plate String not founded*/
//        }
//        else
//        {
//            cout << to_string(counter++) + " Plate detected : " << plateString << endl;
//        }
//
//        sleep(1);
//
//    }
//
//
//}

