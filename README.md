# Exercise 2
## Structure
The code of part 2 is separated to 3 files - bmp\_tester.cpp/hpp, BMPImg.cpp/h, Color.cpp/h
Color is a class that represents a color in rgb form.
BMPImg is a class that represents an image in bmp format.
bmp\_tester is a helper for testing BMPImg.

## BMPImg
this class is implemented as follows:
1. It saves the data as an array of pixels, each pixel is a Color
2. It is not linked to any specific file, so you can load, and save the BMPImg to whichever files you want.
3. It supports operations per pixel (by operator()) and other basics.

## Color
I should note that this class uses a type called element\_t, to save the rgb data.
This is a simple typedef now, but it could be subsituted to a class, to a different typedef later on if needed.
This brings some flexibility.

## Compile
This compiles normally.
just go

```
	cd <wherever you want to compile to>
	cmake <path to src dir>
	make
```
