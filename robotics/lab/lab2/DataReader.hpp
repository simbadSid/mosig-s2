#include <iostream>
#include <fstream>
#include <string>
#include <cv.h>
#include <highgui.h>

/*Beams number*/
#define BN 725
/*Number of frames in the DataSet*/
#define FRAMESNUMBER 100
/*Error macros for data reading*/
#define NOERROR 1
#define ENDDATASET 2
#define ERROR 0

//using namespace std;
//using namespace cv;

class DataReader{
    private:
        int fileIndex;
        /*Directory name of current data set*/
        std::string dataSetName;
        std::string FileName;
        
   public:

        /*These matrices allocate the length values retrieved from laser 
        sensor (cm)/radians/X coordinates/Y coordinates respectively, for a 
        sliding window of size SW.*/
        // each time, the method readData is called these 4 arrays are updated
        float dataLaser[BN]; //angle in degree
        float dataLaserR[BN]; //range in centimeters
        float dataLaserX[BN]; //position on X-axis
        float dataLaserY[BN]; //position on Y-axis
		
        // Variables that you have to use
        float background[BN]; // used to store the background
        int detection[BN]; // used to store the beam where a motion has been detected
        int cluster[BN]; // used to store the index of the cluster the beam belongs to
        int xmin, xmax, ymin, ymax; // used to store the coordinates of the bounding box
        int x, y; // used to store the coordinates of the center of gravity of the bounding box
       
        // The methods you have to implement
        int initBackground();
        int detectMotion(int threshold);
        int buildCluster(int threshold);
        int updateBackground();

        int previous_detection[BN]; // used to store the previous detection

	/*openCV Image to show the data*/
        IplImage* image;
        
	/*Contructor function needs the dataset directory name*/
        DataReader(std::string fileName);
        ~DataReader();
        
	/*Each time this function is called, the next sliding window 
        is stored in the four matrices. It returns NOERROR if no error ocurred; 
        ENDDATASET when the file index reachs the end of the data set; 
        and ERROR in other case.*/
        int readData();
        
	/*Function to initialize the draw window. You have to called just once
        before start to use drawing/displaying functions.*/
        int initWindow();
        
	/*Draws the set points stored in dataLaserX and dataLaserY at time t from
        current sliding window.*/
        int displayLaserData();
        
	/*Print the set points stored in dataLaser at time t from
        current sliding window.*/
	int printLaserData();
	int printLaserData(int beam);

	/*These funtions display an rectangle and a point in the 
        actual draw window. All coordinate paramaters have to be transformed 
        before send these to drawing functions.*/
        void drawRectangle(float xl, float yl, float xr, float yr, CvScalar color);
        void drawPoint(float x, float y, CvScalar color);
        
	/*Set image to black.*/
        void clearImage();
        
	/*Translates and rotates the coordinates x and y to be friendly displayed in
        the opencv coordinate system.*/
        void transformCoordinates(float x,float y, float *nx, float *ny);
        
        int printMotion();
        int displayMotion();
        int displayCluster();

        /*Save actual image to a file. File extension determines file format.*/
        void saveCurrentImage();

    int detectMotionLR();
    int detectMotionRL();

};
