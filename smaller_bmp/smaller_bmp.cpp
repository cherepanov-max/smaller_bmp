#include "pch.h"


using namespace std;

struct HSL {
	float h;
	float s;
	float l;
};
struct RGB {
	int r;
	int g;
	int b;
};
struct RGBf {
	float r;
	float g;
	float b;
};
HSL rgb_to_hsl(int r_, int g_, int b_) {
	float r = r_ / 255.0;
	float g = g_ / 255.0;
	float b = b_ / 255.0;
	float c_max, c_min, delta;
	float h, s, l;
	if (r >= g & r >= b) {
		c_max = r;
	}
	else if (g >= r & g >= b) {
		c_max = g;
	}
	else {
		c_max = b;
	}

	if (r <= g & r <= b) {
		c_min = r;
	}
	else if (g <= r & g <= b) {
		c_min = g;
	}
	else {
		c_min = b;
	}

	delta = c_max - c_min;

	if (delta == 0) {
		h = 0.0;
	}
	else if (c_max == r) {
		h = 60.0 * (((g - b) / delta) + (g < b ? 6 : 0));
	}
	else if (c_max == g) {
		h = 60.0 * (((b - r) / float(delta)) + 2);
	}
	else {
		h = 60.0 * (((r - g) / float(delta)) + 4);
	}

	l = (c_max + c_min) / 2.0;

	if (delta == 0) {
		s = 0.0;
	}
	else {
		s = float(delta) / (1 - abs(2 * l - 1));
	}

	HSL result = { h, s * 100, l * 100 };
	return result;
}
RGB hsl_to_rgb(float h, float s_, float l_) {
	float c, x, m, r, g, b;
	float s = s_ / 100.0;
	float l = l_ / 100.0;
	RGB rgb;
	RGBf rgbf;

	c = (1 - abs(2.0 * l - 1)) * s;
	x = c * (1 - fabs(fmod(((float)(h) / 60), 2) - 1));
	m = l - c / 2.0;

	if (h >= 0 and h < 60) {
		rgbf.r = c;
		rgbf.g = x;
		rgbf.b = 0;
	}
	else if (h >= 60 and h < 120) {
		rgbf.r = x;
		rgbf.g = c;
		rgbf.b = 0;
	}
	else if (h >= 120 and h < 180) {
		rgbf.r = 0;
		rgbf.g = c;
		rgbf.b = x;
	}
	else if (h >= 180 and h < 240) {
		rgbf.r = 0;
		rgbf.g = x;
		rgbf.b = c;
	}
	else if (h >= 240 and h < 300) {
		rgbf.r = x;
		rgbf.g = 0;
		rgbf.b = c;
	}
	else if (h >= 300 and h < 360) {
		rgbf.r = c;
		rgbf.g = 0;
		rgbf.b = x;
	}
	else {
		cout << "H > 359!";
	}

	rgb.r = round((rgbf.r + m) * 255.0);
	rgb.g = round((rgbf.g + m) * 255.0);
	rgb.b = round((rgbf.b + m) * 255.0);

	return rgb;

}
struct img_param {
	int size;
	int pixels_adress;
	int width;
	int height;
	short int bits_per_pixel;
};
struct px_arr {
	unsigned int *r;
	unsigned int *g;
	unsigned int *b;;
};
int remap(int dimensions1, int dimensions2, ifstream &file, ofstream &os, int k) {
	int dop = 4 - dimensions1 % 4;
	if (dop == 4) {
		dop = 0;
	}

	unsigned int r, g, b, null;
	r = 0;
	g = 0;
	b = 0;

	null = 0;

	for (int i = 0; i < dimensions2; ++i)
	{
		int u = 0;
		for (int j = 0; j < dimensions1 + dop; ++j)
		{

			file.read((char*)&b, 1);
			file.read((char*)&g, 1);
			file.read((char*)&r, 1);
			cout << r << ":" << g << ":" << b << " = ";
			HSL hsl = rgb_to_hsl(r, g, b);
			hsl.l = hsl.l + hsl.l * k;
			if (hsl.l > 100) {
				hsl.l = 100;
			}
			RGB rgb = hsl_to_rgb(hsl.h, hsl.s, hsl.l);
			//cout << rgb.r << ":" << rgb.g << ":" << rgb.b << endl;
			os.write(reinterpret_cast<char*>(&rgb.b), sizeof(char));
			os.write(reinterpret_cast<char*>(&rgb.g), sizeof(char));
			os.write(reinterpret_cast<char*>(&rgb.r), sizeof(char));
			if (i >= dimensions1) {
				os.write(reinterpret_cast<char*>(&null), sizeof(null));
				os.write(reinterpret_cast<char*>(&null), sizeof(null));
				os.write(reinterpret_cast<char*>(&null), sizeof(null));
			}
			u++;
		}
	}
	return 0;
}
px_arr small_size(int dimensions1, int dimensions2, ifstream &file, ofstream &os, int k) {
	int dop = 4 - dimensions1 % 4;
	if (dop == 4) {
		dop = 0;
	}

	int null = 0;

	px_arr old;
	old.r = new unsigned int[dimensions1 * dimensions2];
	old.g = new unsigned int[dimensions1 * dimensions2];
	old.b = new unsigned int[dimensions1 * dimensions2];

	px_arr young;
	young.r = new unsigned int[k * dimensions2];
	young.g = new unsigned int[k * dimensions2];
	young.b = new unsigned int[k * dimensions2];

	unsigned int r = 0;
	unsigned int g = 0;
	unsigned int b = 0;



	for (int i = 0; i < dimensions1 * dimensions2; i++) {
		file.read((char*)&b, 1);
		file.read((char*)&g, 1);
		file.read((char*)&r, 1);
		old.r[i] = r;
		old.g[i] = g;
		old.b[i] = b;
		/* cout << old.r[i] << ":" << old.g[i] << ":" << old.b[i] << endl;*/

	}

	for (int i = 0; i < dimensions2; i++) {
		for (int j = 0; j < k; j++) {
			young.r[k * i + j] = old.r[(int)round(((float)dimensions1 / (float)k) * j + dimensions1 * i)];
			young.g[k * i + j] = old.g[(int)round(((float)dimensions1 / (float)k) * j + dimensions1 * i)];
			young.b[k * i + j] = old.b[(int)round(((float)dimensions1 / (float)k) * j + dimensions1 * i)];
			os.write(reinterpret_cast<char*>(&young.b[k * i + j]), sizeof(char));
			os.write(reinterpret_cast<char*>(&young.g[k * i + j]), sizeof(char));
			os.write(reinterpret_cast<char*>(&young.r[k * i + j]), sizeof(char));
			//cout << young.r[k * i + j] << ":" << young.g[k * i + j] << ":" << young.b[k * i + j] << endl;
			//cout << (int)round(((float)k / (float)dimensions1) * j + dimensions1 * i) << endl;
			//cout << ((float)k / (float)dimensions1) * j << endl;

		}
	}


	//for (int i = 0; i < dimensions2; ++i)
	//{
	   // int u = 0;
	   // for (int j = 0; j < dimensions1 + dop; ++j)
	   // {

	   //	 file.read((char*)&b, 1);
	   //	 file.read((char*)&g, 1);
	   //	 file.read((char*)&r, 1);
	   //	 //cout << r << ":" << g << ":" << b << " = ";
	   //	 HSL hsl = rgb_to_hsl(r, g, b);
	   //	 hsl.l = hsl.l + hsl.l * k;
	   //	 if (hsl.l > 100) {
	   //		 hsl.l = 100;
	   //	 }
	   //	 RGB rgb = hsl_to_rgb(hsl.h, hsl.s, hsl.l);
	   //	 //cout << rgb.r << ":" << rgb.g << ":" << rgb.b << endl;
	   //	 os.write(reinterpret_cast<char*>(&rgb.b), sizeof(char));
	   //	 os.write(reinterpret_cast<char*>(&rgb.g), sizeof(char));
	   //	 os.write(reinterpret_cast<char*>(&rgb.r), sizeof(char));
	   //	 if (i >= dimensions1) {
	   //		 os.write(reinterpret_cast<char*>(&null), sizeof(null));
	   //		 os.write(reinterpret_cast<char*>(&null), sizeof(null));
	   //		 os.write(reinterpret_cast<char*>(&null), sizeof(null));
	   //	 }
	   //	 u++;
	   // }
	//}

	//delete [] old.r, old.g, old.b;
	//delete [] old.r, old.g, old.b;
	return old;
}

int main(int argc, char **argv)
{
	setlocale(LC_CTYPE, "rus");
	int size = 0, pixels_adress = 0, width = 0, height = 0;
	short int bits_per_pixel = 0;

	ifstream file("night.bmp", ios::in | ios::binary);

	// Переходим на 2 байт
	file.seekg(2, ios::beg);

	// Считываем размер файла
	file.read((char*)&size, sizeof(int));
	std::cout << "Size: " << size << endl;

	// Переходим на 10 байт
	file.seekg(10, ios::beg);

	// Считываем адрес, где лежит информация о пикселях
	file.read((char*)&pixels_adress, sizeof(int));
	std::cout << "pixels_adress: " << pixels_adress << endl;

	// Переходим на 18 байт
	file.seekg(18, ios::beg);

	//Считываем ширину картинки
	file.read((char*)&width, sizeof(int));
	std::cout << "width: " << width << endl;

	// Переходим на 22 байт
	file.seekg(22, ios::beg);

	//Считываем высоту картинки
	file.read((char*)&height, sizeof(int));
	std::cout << "height: " << height << endl;

	// Переходим на 28 байт
	file.seekg(28, ios::beg);

	//Считываем количество бит на пиксель
	file.read((char*)&bits_per_pixel, sizeof(short int));
	std::cout << "bits_per_pixel: " << bits_per_pixel << endl;

	//двигаемся в зону цветов пикселей
	file.seekg(pixels_adress, ios::beg);

	float k;
	std::cout << "новая ширина изображения в пикселях (меньше текушей и делится на 4)" << endl;
	std::cin >> k;
	std::cout << endl;

	float new_width = k;

	std::ofstream os("temp_0.bmp", std::ios::binary);
	unsigned char signature[2] = { 'B', 'M' };
	unsigned int fileSize = 14 + 40 + new_width * height * 3;
	unsigned int reserved = 0;
	unsigned int offset = 14 + 40;
	unsigned int headerSize = 40;
	unsigned int dimensions1 = new_width;
	unsigned int dimensions2 = height;
	unsigned short colorPlanes = 1;
	unsigned short bpp = 24;
	unsigned int compression = 0;
	unsigned int imgSize = new_width * height * 3;
	unsigned int resolution[2] = { 2795, 2795 };
	unsigned int pltColors = 0;
	unsigned int impColors = 0;
	os.write(reinterpret_cast<char*>(signature), sizeof(signature));
	os.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
	os.write(reinterpret_cast<char*>(&reserved), sizeof(reserved));
	os.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	os.write(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
	os.write(reinterpret_cast<char*>(&dimensions1), sizeof(dimensions1));
	os.write(reinterpret_cast<char*>(&dimensions2), sizeof(dimensions2));
	os.write(reinterpret_cast<char*>(&colorPlanes), sizeof(colorPlanes));
	os.write(reinterpret_cast<char*>(&bpp), sizeof(bpp));
	os.write(reinterpret_cast<char*>(&compression), sizeof(compression));
	os.write(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
	os.write(reinterpret_cast<char*>(resolution), sizeof(resolution));
	os.write(reinterpret_cast<char*>(&pltColors), sizeof(pltColors));
	os.write(reinterpret_cast<char*>(&impColors), sizeof(impColors));

	small_size(width, dimensions2, file, os, k);

	os.close();

	return 0;
}



