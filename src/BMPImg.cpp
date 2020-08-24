#include"BMPImg.h"
#include<iostream>
#include<fstream>
#include<cmath>
#include"file_reading.cpp"

struct BMPImg::Loader {
	//exceptions:
	class CorruptBMPFile { };
	class BadFile { };

	Loader() = default;

	//reads a T from array of bytes, as if it was an array of Ts,
	//then increments pointer to the array to the next T.
	template<typename T>
	static T readVar(const byte* *content) {
		T& result = *( (T*) *content );
		*content += sizeof(T) / sizeof(const byte);
		return result;
	}

	//load file content to a string
	static std::string loadFile(std::string const& path) {
		return readFileContent(path);
	}

	//////////-- Note About Parsing Functions --//////////
	//all parsing functions here have a feature, when an output variable is equal to nullptr,
	//then it assumes this variable is not interesting, and skips it.
	//It Does Not Crash.

	//parse header of BMP file, setting all atributes in it to the given out variables.
	static void parseHeader(const byte* content, int *fileSize, int *pixelArrOffset) {
		if (readVar<char>(&content) != 'B' || readVar<char>(&content) != 'M') {
			throw CorruptBMPFile{};
		}

		if (fileSize != nullptr) {
			*fileSize = readVar<int>(&content);
		}

		//reserved bytes:
		readVar<short>(&content);
		readVar<short>(&content);

		if (pixelArrOffset != nullptr) {
			*pixelArrOffset = readVar<int>(&content);
		}
	}

	//parse DIB Header
	static void parseDIBHeader(const byte* content, int *bmapWidth, int *bmapHeight,
	                    short *bits4Pixel, int *colPltSize) {
		int DIBHeaderSize = readVar<int>(&content);
		if (DIBHeaderSize != 40) {
			throw CorruptBMPFile{};
		}

		if (bmapWidth != nullptr) {
			*bmapWidth = readVar<int>(&content);
		}
		if (bmapHeight != nullptr) {
			*bmapHeight = readVar<int>(&content);
		}

		if (readVar<short>(&content) != 1) {
			throw CorruptBMPFile{};
		}

		if (bits4Pixel != nullptr) {
			*bits4Pixel = readVar<short>(&content);
		}

		content += 16; //skip compression and real-world scale data

		if (colPltSize != nullptr) {
			*colPltSize = readVar<int>(&content);
			if (*colPltSize == 0) {
				*colPltSize = std::pow(2, *bits4Pixel);
			}
		}
	}

	//parse color palette, and return array of colors in that palette.
	static std::vector<Color> parseColPlt(const byte* content, int pltSize) {
		std::vector<Color> result (pltSize);

		for (int i = 0; i < pltSize; ++i) {
			result[i] = Color{readVar<byte>(&content), readVar<byte>(&content), readVar<byte>(&content)};
			++content;//skip the redundant 0 every 4'th element
		}
		return result;
	}

	//parse pixel array for 8bit images (return type is the array).
	static std::vector<byte> parsePixelArr8(const byte* content, int width, int height) {
		int padding = 4 - ((width) % 4);
		if (padding == 4) { padding = 0; }
		int rowWidth = width + padding;
		std::vector<byte> result (rowWidth * height);

		for (int i = 0, j = 0; i < rowWidth * height; ++i, ++j) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[j] = content[i];
		}
		return result;
	}

	//parse pixel array for 24 bit images (return type is the array).
	static std::vector<Color> parsePixelArr24(const byte* content, int width, int height) {
		int padding = 4 - ((width * 3) % 4);
		if (padding == 4) { padding = 0; }
		int rowWidth = width + padding;
		std::vector<Color> result (width * height);

		for (int i = 0, j = 0; i < rowWidth * height * 3; i += 3, ++j) {
			if (i % rowWidth == width) {
				i += padding;
			}
			result[j] = Color{content[i], content[i + 1], content[i + 2]};
			if (i == rowWidth * height - 1) {
			}
		}
		return result;
	}
};

struct BMPImg::Writer {
	//concatunate string s with t, as bytes.
	template<typename T>
	static void writeVar(std::string* s, T t) {
		const byte* contentOfT = (const byte*) &t;
		for (int i = 0; (long unsigned int) i < sizeof(T) / sizeof(byte); ++i) {
			*s += (const char) contentOfT[i];
		}
	}

	//write string to file
	static void writeToFile(std::string const& path, std::string const& content) {
		writeFileContent(path, content);
	}

	//encode header, return content as string.
	static std::string encodeHeader(int fileSize, int pixelArrOffset) {
		std::string s  = "BM";
		writeVar(&s, fileSize);
		writeVar<short>(&s, 0); //reserved bytes
		writeVar<short>(&s, 0); //reserved bytes
		writeVar(&s, pixelArrOffset);
		return s;
	}

	//encode DIB header, return content as string.
	static std::string encodeDIBHeader(short bits4pixel, int width, int height) {
		std::string s = "";
		writeVar(&s, 40); //size of this header
		writeVar(&s, width); //width
		writeVar(&s, height); //height
		writeVar<short>(&s, 1); //contant - must be 1
		writeVar(&s, bits4pixel); //bit for each pixel
		writeVar(&s, 0); //compression stuff - 0 means no compression
		writeVar(&s, 0); //compression stuff - 0 means no compression
		writeVar(&s, width * 8000); //real world scale
		writeVar(&s, height * 8000); //real world scale
		writeVar(&s, 0); //color palette size
		writeVar(&s, 0); //numer of important colors
		return s;
	}

	//encode pixel array (24 bit), return content as string.
	static std::string encodePixelArr(std::vector<Color> const& pixels, int width) {
		std::string s = "";
		int padding = 4 - ((width * 3) % 4);
		if (padding == 4) { padding = 0; }
		int height = pixels.size() / width;
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				Color const& pixel = pixels.at(width * i + j);
				writeVar<byte>(&s, pixel.r());
				writeVar<byte>(&s, pixel.g());
				writeVar<byte>(&s, pixel.b());
			}
			const byte garbage = 17;
			for (int it = 0; it < padding; ++it) {
				writeVar<byte>(&s, garbage );
			}
		}
		return s;
	}
};

BMPImg::BMPImg(std::string const& path) : BMPImg{} {
	load(path);
}

BMPImg::BMPImg(int width, int height) 
	: m_width{width}, m_height{height}, m_data(width * height) { }

void BMPImg::load(std::string const& path) {
	std::string s = Loader::loadFile(path);
	const byte* contentp = (const byte*) s.c_str();

	//////////-- The Header --//////////
	int fileSize, arrayOffset;
	Loader::parseHeader(contentp, &fileSize, &arrayOffset);
	contentp += 14;

	//////////-- The DIB Header --//////////
	short bits4Pixel;
	int paletteSize;
	Loader::parseDIBHeader(contentp, &m_width, &m_height, &bits4Pixel, &paletteSize);
	bool usingColPlt = bits4Pixel == 8;
	contentp += 40;

	//////////-- The Color Palette & Pixel Array --//////////
	if (usingColPlt) {
		std::vector<Color> colPlt = std::move(Loader::parseColPlt(contentp, paletteSize));

		contentp = (const byte*)s.c_str() + arrayOffset;
		std::vector<byte> pixelVals = std::move(Loader::parsePixelArr8(contentp, m_width, m_height));

		m_data = std::move(std::vector<Color>(pixelVals.size()));

		for (int i = 0; i < (int) m_data.size(); ++i) {
			m_data[i] = colPlt[pixelVals[i]];
		}
	} else {
		contentp = (const byte*)s.c_str() + arrayOffset;
		m_data = std::move(Loader::parsePixelArr24(contentp, m_width, m_height));
	}
}

void BMPImg::save(std::string const& path) {
	std::string content = "";
	content += Writer::encodeHeader(14 + 40 + (m_width + 4 - ((m_width * 3) % 4)), 14 + 40);
	content += Writer::encodeDIBHeader(24, m_width, m_height);
	content += Writer::encodePixelArr(m_data, m_width);
	Writer::writeToFile(path, content);
}


Color& BMPImg::operator() (int i, int j) {
	return m_data[m_width * i + j];
}

int BMPImg::width() {
	return m_width;
}
int BMPImg::height() {
	return m_height;
}
