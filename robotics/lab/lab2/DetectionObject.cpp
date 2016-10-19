#include <iostream>
/*Libraries needed*/
#include "DataReader.hpp"

int main(int argc, char* argv[]){
    std::string setName(argv[1]);
   	DataReader dataObj(setName);
    
   	/*Window initialization*/
   	dataObj.initWindow();
   	dataObj.clearImage();

 	// read the first data to determine the background
	dataObj.readData();//read the next laser data
   	dataObj.initBackground();//define the background
 	dataObj.displayLaserData();//display the current laser data on the graphical window
   
   	//Displaying the dataset (each sliding window; each
    while(dataObj.readData() == NOERROR) {

        int threshold = 5;//the threshold is choosen at 5cms
        dataObj.clearImage();// Clear the graphical window
        dataObj.displayLaserData();//display the current laser data on the graphical window
        //dataObj.updateBackground();

        // Detection
        dataObj.detectMotion(threshold);//perform the detection of motion for each beam
        dataObj.buildCluster(threshold);//perform the clustering of detected beams to form the object
        dataObj.printMotion();//display the beams corresponding to a dynamic object on the terminal window
        dataObj.displayMotion();//display the beams corresponding to a dynamic object on the graphical window
        dataObj.displayCluster();//display the cluster corresponding to the dynamic object on the graphical window
        dataObj.saveCurrentImage();//save the current image on the disk

		/*delay*/
        int key = cvWaitKey(0);

        //Added, otherwise there will be no difference!
        dataObj.updateBackground();

	}
    
     return 0;

}
