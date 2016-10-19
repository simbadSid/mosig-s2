CC=g++
# Path of needed libraries
INCLUDE=/usr/include/opencv
LIB=/opt/opencv/lib/

all: DetectionObject TrackingObject

DataReader.o: DataReader.hpp DataReader.cpp
	$(CC) -c DataReader.cpp -I$(INCLUDE)

DetectionObject.o: DetectionObject.cpp DataReader.hpp
	$(CC) -c DetectionObject.cpp -I$(INCLUDE)

DetectionObject: DetectionObject.o DataReader.o
	$(CC) -o DetectionObject DetectionObject.o DataReader.o -lopencv_core -lopencv_highgui -L$(LIB)

Kalman.o: Kalman.hpp Kalman.cpp
	$(CC) -c Kalman.cpp -I$(INCLUDE)

TrackingObject.o: TrackingObject.cpp DataReader.hpp Kalman.hpp
	$(CC) -c TrackingObject.cpp -I$(INCLUDE)

TrackingObject: TrackingObject.o DataReader.o Kalman.o
	$(CC) -o TrackingObject TrackingObject.o DataReader.o Kalman.o -lopencv_core -lopencv_highgui -L$(LIB)

clean:
	rm *.o
	rm DetectionObject
	rm TrackingObject

