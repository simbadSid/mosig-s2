#include <iostream>
/*Libraries needed*/
#include "DataReader.hpp"
#include "Kalman.hpp"

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
   
    // First detection to initialize the Kalman filter
    int threshold = 5;//the threshold is choosen at 5cms
    dataObj.readData();//read the next laser data
    dataObj.detectMotion(threshold);//perform the detection of motion for each beam
    dataObj.buildCluster(threshold);//perform the clustering of detected beams to form the object
    dataObj.printMotion();//display the beams corresponding to a dynamic object on the terminal window
    dataObj.displayMotion();//display the beams corresponding to a dynamic object on the graphical window
    dataObj.displayCluster();//display the cluster corresponding to the dynamic object on the graphical window
    dataObj.saveCurrentImage();

    float init_mean, init_std, init_q, init_r;

    Kalman y(init_mean, init_std, init_q, init_r);// Kalman filter used to estimate the position of the y axis
    std::cout << "mean(I) = " << y.mean << " std(I) = " << y.std << std::endl << std::endl;

    int current_position;
    int previous_position;
    int action;

    // Displaying the dataset (each sliding window; each frame)
	while(dataObj.readData() == NOERROR) {

        dataObj.clearImage();// Clear the graphical window
        dataObj.displayLaserData();//display the current laser data on the graphical window
        dataObj.updateBackground();

        // prediction phase
        // action = ... the action used to perform prediction
        std::cout << "action = " << action << std::endl;
        y.prediction(action);
        std::cout << "mean(P) = " << y.mean << " std(P) = " << y.std << std::endl;

        // Detection
        dataObj.detectMotion(threshold);//perform the detection of motion for each beam
        dataObj.buildCluster(threshold);//perform the clustering of detected beams to form the object
        //dataObj.printMotion();//display the beams corresponding to a dynamic object on the terminal window
        dataObj.displayMotion();//display the beams corresponding to a dynamic object on the graphical window
        dataObj.displayCluster();//display the cluster corresponding to the dynamic object on the graphical window
        dataObj.saveCurrentImage();

        // estimation phase
        int o;
        std::cout << "observation = " << o << std::endl;
        y.estimation(o);
        std::cout << "mean(E) = " << y.mean << " std(E) = " << y.std << std::endl << std::endl;

		/*delay*/
        int key = cvWaitKey(0);
	}
    
     return 0;

}
