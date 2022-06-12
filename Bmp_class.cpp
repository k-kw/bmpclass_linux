#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
//#include <windows.h>
#include "Bmp_class.h"


using namespace std;

// #pragma warning(disable:4996)

#pragma pack(1)
typedef struct tagBITMAPFILEHEADER {
	unsigned short bfType;
	unsigned int  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int  bf0ffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	unsigned int   biSize;
	int		biWidth;
	int		biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int   biCompression;
	unsigned int   biSizeImage;
	int		biXPelsPerMeter;
	int		biYPelsPerMeter;
	unsigned int   biCirUsed;
	unsigned int   biCirImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	unsigned char  rgbBlue;
	unsigned char  rgbGreen;
	unsigned char  rgbRed;
	unsigned char  rgbReserved;
} RGBQUAD;
#pragma pack()

//このファイル内のみで使うinline関数
template <class Type>
inline Type get_max(Type* data, int size)
{
	Type max = data[0];
	for (int i = 0; i < size; i++) {
		if (max < data[i]) {
			max = data[i];
		}
	}

	return max;
}

//最小値
template <class Type>
inline Type get_min(Type* data, int size)
{
	Type min = data[0];
	for (int i = 0; i < size; i++) {
		if (min > data[i]) {
			min = data[i];
		}
	}

	return min;
}

//最大値で正規化
template <class Type>
inline void norm_max(Type* data_in, int size, double* data_out)
{
	Type max;
	max = get_max(data_in, size);

	for (int i = 0; i < size; i++) {
		data_out[i] = (double)((double)data_in[i] / (double)max);
	}

}

//最大値と最小値で正規化(確実に0~1)
template <class Type>
inline void norm_max_min(Type* data_in, int size, double* data_out)
{
	Type max;
	max = get_max(data_in, size);
	Type min;
	min = get_min(data_in, size);

	double* tmp;
	tmp = new double[size];

	for (int i = 0; i < size; i++) {
		tmp[i] = (double)((data_in[i] - min) / (max - min));
	}

	for (int i = 0; i < size; i++) {
		data_out[i] = tmp[i];
	}


	delete[]tmp;
}

//正規化後,n階調化,int型出力
template <typename Type>
inline void norm_reso_n(Type* data_in, int* data_out, int n, int size)
{
	double* tmp;
	tmp = new double[size];
	norm_max_min(data_in, size, tmp);

	for (int i = 0; i < size; i++) {
		data_out[i] = (int)(round(tmp[i] * n));
	}

	delete[]tmp;
}


//intデータをunsigned charに変換後、格納
//0~255のデータに限る
inline void to_uch(int* data_in, int size, unsigned char* data_img)
{
	for (int i = 0; i < size; i++) {
		data_img[i] = (unsigned char)data_in[i];
	}
}




//クラスメンバ関数定義

//コンストラクタ
My_Bmp::My_Bmp(int sx, int sy)
{
	img = new unsigned char[sx * sy];
	im_x = sx;
	im_y = sy;
	//cout << "BMPコンストラクタ" << endl;
}


//BMP読み込み
void My_Bmp::img_read(string imgpath) {


	//余分なヘッダのバッファ
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	int color[1024];


	//読み込み
	fstream ifs(imgpath, ios::binary | ios::in);
	ifs.read((char*)&BmpFileHeader, sizeof(BITMAPFILEHEADER));
	ifs.read((char*)&BmpInfoHeader, sizeof(BITMAPINFOHEADER));
	ifs.read((char*)color, 1024);

	ifs.read((char*)img, sizeof(unsigned char) * im_x * im_y);


}


//読み込んだunsigned charをdoubleにして格納
void My_Bmp::ucimg_to_double(double* data_out)
{
	for (int i = 0; i < im_y * im_x; i++) {
		long tmp = (long)img[i];
		data_out[i] = (double)tmp;
	}
}





//書き込みたいデータを256階調化、unsigned charに変換後imgに格納
//多重定義
//double
void My_Bmp::data_to_ucimg(double* data_in)
{
	int* dataint;
	dataint = new int[im_x * im_y];
	norm_reso_n<double>(data_in, dataint, 255, im_x * im_y);

	to_uch(dataint, im_x * im_y, img);

	delete[]dataint;

}

//int
void My_Bmp::data_to_ucimg(int* data_in)
{
	int* dataint;
	dataint = new int[im_x * im_y];
	norm_reso_n<int>(data_in, dataint, 255, im_x * im_y);

	to_uch(dataint, im_x * im_y, img);

	delete[]dataint;

}

//float
void My_Bmp::data_to_ucimg(float* data_in)
{
	int* dataint;
	dataint = new int[im_x * im_y];
	norm_reso_n<float>(data_in, dataint, 255, im_x * im_y);

	to_uch(dataint, im_x * im_y, img);

	delete[]dataint;

}


//unsigned charをimgに格納
void My_Bmp::uc_to_img(unsigned char* data_in)
{
	for (int i = 0; i < im_x * im_y; i++) {
		img[i] = data_in[i];
	}
}


//BMP書き込み
void My_Bmp::img_write(string imgpath)
{
	//この時点で画素データは256階調化され、imgに格納されていなければならない
	//ヘッダ部用意
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	RGBQUAD			 RGBQuad[256];
	BmpFileHeader = { 19778, 14 + 40 + 1024 + (unsigned int)(im_x * im_y), 0, 0, 14 + 40 + 1024 };
	BmpInfoHeader = { 40, im_x, im_y, 1, 8, 0U, 0U, 0U, 0U, 0U, 0U };
	for (int i = 0; i < 256; i++) {
		RGBQuad[i].rgbBlue = i;
		RGBQuad[i].rgbGreen = i;
		RGBQuad[i].rgbRed = i;
		RGBQuad[i].rgbReserved = 0;
	}

	//出力ファイルオープン
	fstream ofs(imgpath, ios::binary | ios::out);

	//ヘッダ部書き込み
	ofs.write((char*)&BmpFileHeader, sizeof(BITMAPFILEHEADER));
	ofs.write((char*)&BmpInfoHeader, sizeof(BITMAPINFOHEADER));
	ofs.write((char*)RGBQuad, sizeof(RGBQuad));

	//画素データ書き込み
	ofs.write((char*)img, sizeof(unsigned char) * im_x * im_y);
	
}



My_Bmp::~My_Bmp()
{
	delete[]img;
	//cout << "BMPデコンストラクタ" << endl;
}

