CMAKE_MINIMUM_REQUIRED (VERSION 3.10)

PROJECT (ICP3038-lab3)

SET (CMAKE_CXX_STANDARD 11)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_CXX_EXTENSIONS OFF)

FIND_PACKAGE (OpenCV REQUIRED)

ADD_EXECUTABLE (Motion-Detection src/MotionDetection.cxx)

IF (OpenCV_FOUND)
	TARGET_INCLUDE_DIRECTORIES (Motion-Detection PUBLIC ${OpenCV_INCLUDE_DIRS})

	TARGET_LINK_LIBRARIES (Motion-Detection ${OpenCV_LIBS})
ENDIF(OpenCV_FOUND)

# FILE (COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgs/leaf.png DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

FILE (COPY ${CMAKE_CURRENT_SOURCE_DIR}/imgs/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
