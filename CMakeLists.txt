cmake_minimum_required(VERSION 2.8.9)
project (LectureNote)

#Bring the headers, such as Student.h into the project
include_directories(/usr/local/include/ImageMagick-7) 

#the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp")
 
add_executable(LectureNote ${SOURCES})

target_compile_options(LectureNote PRIVATE -std=c++11)

#file(GLOB MYLIBS "${BOOST_PATH}/libboost*.a")

target_link_libraries(LectureNote PRIVATE opencv_core)
target_link_libraries(LectureNote PRIVATE opencv_imgproc)
target_link_libraries(LectureNote PRIVATE opencv_highgui)
target_link_libraries(LectureNote PRIVATE opencv_objdetect)
target_link_libraries(LectureNote PRIVATE opencv_imgcodecs)
target_link_libraries(LectureNote PRIVATE opencv_videoio)

#image magic k libraries ie in /usr/local/lib
target_link_libraries(LectureNote PRIVATE Magick++-7.Q16HDRI)
target_link_libraries(LectureNote PRIVATE MagickCore-7.Q16HDRI)
target_link_libraries(LectureNote PRIVATE MagickWand-7.Q16HDRI)


