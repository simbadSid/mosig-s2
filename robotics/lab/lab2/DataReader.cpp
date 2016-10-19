#include "DataReader.hpp"

int DataReader::initBackground(){
	int i;
	for (i=0;i<BN;i++){
		background[i] = dataLaserR[i];
	}
    return 1;

}

int DataReader::updateBackground() {
	initBackground();
    return 1;

}

int DataReader::detectMotion(int threshold) {
	int i;
		for (i=0;i<BN;i++){
			detection[i] = 0;
			if ((background[i] - dataLaserR[i])> threshold){
				detection[i] =1;
			}
		}
    return 1;

}

int DataReader::buildCluster(int threshold) {
	/*int i,x,y;
	int nbcluster = 1;
	int lastBeam;
	while (detection[i] != 1){
		i++;
	}
	lastBeam = dataLaser[i];
	for (x=0;x<BN;x++){
		cluster[x] = lastBeam;
	}
	for (y=0; y<BN; y++){
		if (detection[y] == 1){
			if ((dataLaser[y] - lastBeam)<threshold){
				cluster[y] = nbcluster;
			}
			else{
				nbcluster = lastBeam;
			}
			lastBeam = dataLaser[y];
		}
	}*/
    return NOERROR;

}

int DataReader::detectMotionLR() {
    int i,j,k;

    j = 0;

   while ( i<BN ) {
       int a = 0;
       int b = 0;
       int c =0;

        i = j;
         while( !detection[i] && previous_detection[i] ) {
                i++;
                a=1;
            }

     while( detection[i] && previous_detection[i] ) {
                i++;
                b=1;
            }
     while( detection[i] && !previous_detection[i] ) {
                i++;
                c=1;
            }

     if ( ( a && b && c ) || ( a && b ) || ( b && c ) ) {
        std::cout << "Deplacement Gauche -> droite " << j << " " << i << std::endl;
        j = i;
    }

    j++;
       }

        return 1;
}

int DataReader::detectMotionRL() {
    int i,j,k;

    j = 0;

   while ( i<BN ) {
       int a = 0;
       int b = 0;
       int c =0;

        i = j;
         while( detection[i] && !previous_detection[i] ) {
                i++;
                a=1;
            }

     while( detection[i] && previous_detection[i] ) {
                i++;
                b=1;
            }
     while( !detection[i] && previous_detection[i] ) {
                i++;
                c=1;
            }

     if ( ( a && b && c ) || ( a && b ) || ( b && c ) ) {
        std::cout << "Deplacement Droite -> gauche " << j << " " << i << std::endl;
        j = i;
    }

    j++;
       }

        return 1;

}

DataReader::DataReader(std::string setName){
    /*Index file initialization*/
    fileIndex = 0;
    dataSetName = setName;
}

int DataReader::initWindow(){

    /*Initialazing the window and main image.*/
    cvNamedWindow("myWindow", CV_WINDOW_AUTOSIZE);
    image = cvCreateImage(cvSize(630,630),IPL_DEPTH_32F,3);
}

int DataReader::readData(){
    std::ifstream inFile;
    char tempChar[255];
    std::string fileNameTemp(""), lineTemp("");
    
    fileIndex ++;

    /*If we reach the end of the data set*/
    /*if (fileIndex == FRAMESNUMBER)
        return ENDDATASET;*/
    
    /*We read the next frame */
    std::ostringstream convert("");
    convert << fileIndex;
    if (fileIndex < 10) {
    	fileNameTemp = dataSetName + std::string("data_000") + convert.str() + std::string(".csv");
	FileName = std::string("result_000") + convert.str() + std::string(".jpg");
	}
    else if (fileIndex < 100) {
    	fileNameTemp = dataSetName + std::string("data_00") + convert.str() + std::string(".csv");
	FileName = std::string("result_00") + convert.str() + std::string(".jpg");
    	}
    else {
    	fileNameTemp = dataSetName + std::string("data_0") + convert.str() + std::string(".csv");
        FileName = std::string("result_0") + convert.str() + std::string(".jpg");
        }

    inFile.open(fileNameTemp.c_str(), std::ifstream::in);
    if (!inFile.good()){
    	std::cout << "ERROR: Input file doesn't exist." << std::endl;
      return ERROR;
    } else {            
    	for (int j=0 ; j<BN ; j++){
    		inFile.getline(tempChar,255);
         lineTemp = std::string(tempChar);
         replace( lineTemp.begin(), lineTemp.end(), ',', ' ' ); 
         std::istringstream tempStream(lineTemp);
			tempStream >> dataLaser[j]  >> dataLaserR[j] >> dataLaser[j] >> dataLaserX[j] >> dataLaserY[j];
         /*to cm*/
         dataLaserR[j] /= 10;
    		dataLaserX[j] = dataLaserR[j] * cos(dataLaser[j]);
         dataLaserY[j] = dataLaserR[j] * sin(dataLaser[j]);
			
			// to degree
			dataLaser[j] *= 180;
			dataLaser[j] /= 3.14159;
		}
	}
	inFile.close();       
    
    return NOERROR;
}

int DataReader::displayLaserData(){
    float nxl,nyl,rxl,ryl;
    
    transformCoordinates(0,0,&nxl,&nyl);
    transformCoordinates(40,0,&rxl,&ryl);
    cvLine(image, cvPoint(nxl,nyl), cvPoint(rxl,ryl), CV_RGB(217,217,217),1,4);

    transformCoordinates(0,0, &nxl,&nyl);
    transformCoordinates(0,40, &rxl,&ryl);
    cvLine(image, cvPoint(nxl,nyl), cvPoint(rxl,ryl), CV_RGB(217,217,217),1,4);

    transformCoordinates(0,0,&nxl,&nyl);
    cvCircle(image,cvPoint(nxl,nyl),5,CV_RGB(0,255,0));    
    //transformCoordinates(-20,-10, &nxl,&nyl);
    //transformCoordinates(0,10, &rxl,&ryl);
    //cvRectangle(image,cvPoint(nxl,nyl), cvPoint(rxl,ryl), CV_RGB(217,217,217),2);

    CvFont f;
    cvInitFont(&f,CV_FONT_HERSHEY_SIMPLEX,0.5,0.5,1,1,1);
    transformCoordinates(490,10, &nxl,&nyl);
    transformCoordinates(3,300, &rxl,&ryl);
    cvPutText(image,"+x",cvPoint(nxl,nyl),&f,CV_RGB(217,217,217));
    cvPutText(image,"+y",cvPoint(rxl,ryl),&f,CV_RGB(217,217,217));
    
    for (int i=0;i < BN; i++){
        /*Before display every point, we have to transform its coordinates*/
        transformCoordinates(dataLaserX[i],dataLaserY[i],&nxl,&nyl);
        cvRectangle(image, cvPoint(nxl,nyl), cvPoint(nxl,nyl), CV_RGB(255,255,255), 1);
    }
    cvShowImage("myWindow", image);
    return NOERROR;
}

int DataReader::printLaserData(){

	for (int j=0;j<BN;j++)
		std::cout << "beam " << j << ", "
			<< dataLaserR[j] << " degre, " 
			<< dataLaser[j] << "cms, X=" 
			<< dataLaserX[j] << ", Y=" 
		  	<< dataLaserY[j] << std::endl;
}
int DataReader::printLaserData(int beam){

	std::cout << "beam " << beam << ", "
		<< dataLaserR[beam] << " degre, " 
		<< dataLaser[beam] << "cms, X=" 
		<< dataLaserX[beam] << ", Y=" 
	  	<< dataLaserY[beam] << std::endl;
}

void DataReader::drawRectangle(float xl, float yl, float xr, float yr, CvScalar color){ 
    cvRectangle(image, cvPoint(xl,yl), cvPoint(xr,yr), color, 1);
    //cvShowImage("myWindow", image);
}

void DataReader::drawPoint(float x, float y, CvScalar color){
    cvRectangle(image, cvPoint(x,y), cvPoint(x,y), color, 1);
    //cvShowImage("myWindow", image);
}

void DataReader::clearImage(){
    cvSet(image, cvScalar(0,0,0));
    //cvShowImage("myWindow", image);
}

void DataReader::transformCoordinates(float x, float y, float *nx, float *ny){
    /*To (translate/rotate) the points. Parameters and equation used.*/
    int tx = 100, ty = 270, th = 0;
    /*cos(theta)  -sin (theta)  tx             | x
      sin (theta)  cos(theta)   ty             | y
      0                  0      1              | 1
     */
    *nx = (float) (cos(th)*x - sin(th)*y) + tx;
    *ny = (float) (sin(th)*x + cos(th)*y) + ty;

}

void DataReader::saveCurrentImage(){
	std::string fileNameTemp("");
   std::ostringstream convert("");
 	
	convert << fileIndex;
	if (fileIndex < 10)
        fileNameTemp = std::string("result_000") + convert.str() + std::string(".jpg");
	else if (fileIndex < 100)
        fileNameTemp = std::string("result_00") + convert.str() + std::string(".jpg");
	else
        fileNameTemp = std::string("result_0") + convert.str() + std::string(".jpg");
 
   //std::cout << "file name = " << fileNameTemp.c_str() << std::endl;

   cvSaveImage(fileNameTemp.c_str(),image);

}

DataReader::~DataReader(){
    cvReleaseImage(&image);
}

int DataReader::printMotion() {

	std::cout << "printMotion" << std::endl;
	for(int i=0; i < BN; i++)
        if ( detection[i] )
            std::cout << "beam " << i << ", cluster = "
            << cluster[i] << ", r = "
            << dataLaserR[i] << " cms, theta = "
            << dataLaser[i] << " degres, X = "
            << dataLaserX[i] << " cms, Y = "
            << dataLaserY[i] << " cms" << std::endl;
	std::cout << std::endl;

	return NOERROR;

}

int DataReader::displayMotion() {

	for(int i=0; i < BN; i++)
        if ( detection[i] ) {
			float x1,y1,x2,y2;
    
    			transformCoordinates(0,0,&x1,&y1);
		   	transformCoordinates(dataLaserX[i],dataLaserY[i],&x2,&y2);
		   	cvLine(image, cvPoint(x1,y1), cvPoint(x2,y2), CV_RGB(217,217,217),1,4);
		}
 
	cvShowImage("myWindow", image);
	return NOERROR;

}

int DataReader::displayCluster() {

    float x1, y1, x2, y2;

    transformCoordinates(xmin,ymin,&x1,&y1);
    transformCoordinates(xmax,ymax,&x2,&y2);
    cvRectangle(image, cvPoint(x1,y1), cvPoint(x2,y2), CV_RGB(255,0,0), 1);
    cvShowImage("myWindow", image);

    return NOERROR;

}
