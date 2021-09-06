What is VideoLectureToLectureNotesInPDF?    
It is a tool which takes video lecture file (.mp4, .avi etc.) as input and produces notes in pdf as output. Now a days there are lots of video courses available online, in which the instructor write on a black-board/notebook . This software uses advanced video/image processing to produce the relevant lecture notes. 

Building/Installing    
Windows:  msbuild LectureNotes.sln /p:Configuration=Release
Linux: cmake

Compile Prerequisites   
GNU Toolchain, cmake, opencv, Magick++, ghostscript
