#include "image.h"


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}

#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

/*Dibuixem un rectangle diferenciant el seu contorn i el color de fons a partir d'un boleà. Si el volem emplenar
utilitzem un for dins d'un altre (matriu). Si el volem contornejar, utilitzem dos fors consecutius (costats verticals
i horitzonal).*/
void Image:: drawRectangle(unsigned int x, unsigned int y, unsigned int w, unsigned int h, Color color, bool fill){

    if(fill){
        for (unsigned int i = x; i < x + w; i++){
            for(unsigned int j = y; j < y + h; j++){
                setPixel(i, j, color);
            }
        }
    }
    else{
        for(unsigned int i = x; i < x + w; i++){
            setPixel(i, y, color);
            setPixel(i, y + h - 1, color);
        }
        for(unsigned int j = y; j < y + h; j++){
            setPixel(x, j, color);
            setPixel(x + w - 1, j, color);
        }
    }
}

/* Funció per dibuixar un cercle amb les mateixes condicions que el rectangle, diferenciant el contorn del color de fons,
tot teninet en compte l'expressió d'una circumferència.*/
void Image:: drawCircle(unsigned int x, unsigned int y, unsigned int r, Color color, bool fill){

    for(unsigned int i = x - r; i <= x + r; i++){
        for(unsigned int j = y - r; j <= y + r; j++){
            unsigned int temp = sqrt(((i - x) * (i - x)) + ((j - y) * (j - y)));
            if(fill){
                if(temp <= r){
                    setPixelSafe(i, j, color);
                }
            }
            else{
                if(temp == r){
                    setPixelSafe(i, j, color);
                }
            }
        }
    }
}

/* Funció per dibuixar una línea de pixels a partir de dos punts (trobant cada vegada el punt mig).
L'algoritme genera línees horitzontals, verticals i diagonals.*/
void Image:: drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, Color color){

    float middle_x = (x2 + x1) / 2;
    float middle_y = (y2 + y1) / 2;

    if(x1 == middle_x && y1 == middle_y){
        setPixel(x1, y1, color);
    }
    else if(x2 == middle_x && y2 == middle_y){
        setPixel(x2, y2, color);
    }
    else{
        setPixel(middle_x, middle_y, color);
        drawLine(x1, y1, middle_x, middle_y, color);
        drawLine(middle_x, middle_y, x2, y2, color);
    }
}

// Amb les funcions definides anteriorment, creem diferents formes per realitzar la tasca 1.
void Image::workWithBasicFigures(Image& framebuffer){

    //Lletra "H"
    drawRectangle(10, 150, 30, 250, Color::PURPLE, true);
    drawRectangle(30, 260, 90, 30, Color::PURPLE, false);
    drawRectangle(120, 150, 30, 250, Color::PURPLE, true);

    //Lletra "E"
    drawRectangle(160, 150, 30, 250, Color::BLUE, true);
    drawRectangle(160, 370, 130, 30, Color::BLUE, false);
    drawRectangle(160, 150, 130, 30, Color::BLUE, false);
    drawRectangle(160, 260, 80, 30, Color::BLUE, false);

    //Lletra "L"
    drawRectangle(300, 150, 30, 250, Color::CYAN, true);
    drawRectangle(300, 150, 125, 30, Color::CYAN, false);

    //Lletra "L"
    drawRectangle(435, 150, 30, 250, Color::WHITE, true);
    drawRectangle(435, 150, 125, 30, Color::WHITE, false);

    //Lletra "O"
    drawCircle(660, 280, 130, Color::GRAY, true);
    drawCircle(640, 255, 105, Color::BLACK, true);
    drawCircle(640, 255, 80, Color::GRAY, false);

    //Figura estrella
    drawLine(650, 450, 700, 580, Color::YELLOW);
    drawLine(700, 580, 750, 450, Color::YELLOW);
    drawLine(650, 450, 775, 530, Color::YELLOW);
    drawLine(620, 530, 775, 530, Color::YELLOW);
    drawLine(620, 530, 750, 450, Color::YELLOW);
}

/*Per crear un gradient de blau a vermell, hem de crear una matriu de la mida de la pantalla.
f és la variable que va generant el color, que implementarem de forma inversa en les variables R i B de color (Color(R,G,B)).*/
void Image::drawGradientBtoR(){

    for (unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            float f = i / ((float) width); //si en comptes de i posem w-i al numerador ens sortirà al revés (de vermell a blau)
            // si en comptes de i posem j al numerador, es farà vertical (per posar-lo al revés -> h - j)
            // per fer gradient diagonal fem (i+j)/(w+h), i si ho volem girar, (w+h-i-j)/(w+h)
            f = f * 255;
            setPixelSafe(i, j, Color(f, 0, 255-f));
        }
    }
}

/* En aquesta funció apliquem el mateix funcionament del gradient, afegint una condició que modela una forma d'ona (sinusoide).
D'aquesta forma podem crear dos gradients (inversos entre ells).*/
void Image::sinusoidalGradient(){

    for (unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            if(j < ((height/2) + ((height/5) * sin(( (2*PI)/width) * i )))){ //y(i) = desplaçament eix y positiu + A * sin((2pi/T)*i)
                float f = j / ((float) width);
                f = f * 255;
                setPixel(i, j, Color(0, f, 0));
            }
            else{
                float f = (height-j) / ((float) width);
                f = f * 255;
                setPixel(i, j, Color(0, f, 0));
            }
        }
    }
}

// En aquesta funció definim un gradient circular, utilitzant la mateixa fórmula en drawCircle (la de la circumferència).
void Image::radialGradient(){

    for(unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            unsigned int  x = i - (width/2);
            unsigned int y = j - (height/2);
            float dist = (sqrt(((x*x)+(y*y))))/2; // entre 2 perquè és punt mig
            setPixel(i, j, Color(dist, dist, dist));
        }
    }
}

// En aquesta funció mostrem un taulell d'escacs en blanc i negre utilitzant la funció matemàtica fmod().
void Image::bandwBoard(){

    for(unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            if((fmod(i, 100) > 50 && fmod(j, 100) >50) || (fmod(i, 100) < 50 && fmod(j, 100) < 50)) {
                setPixel(i, j, Color::WHITE);
            }
        }
    }

}

// En aquesta funció carreguem una foto qualsevol sense cap edició.
void Image:: originalPhoto(Image& picture){

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            Color color_globus = picture.getPixel(i, j);
            setPixel(i, j, color_globus);
        }
    }
}

// En aquesta funció, modifiquem els colors d'una fotografia per mostrar-la en escala de grisos.
void Image::grayscalePhoto(Image& picture){

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            Color color_globus = picture.getPixel(i, j);
            float intensity = (color_globus.r + color_globus.g + color_globus.b)/3; //método del valor medio
            Color new_color = Color(intensity, intensity, intensity);
            setPixel(i, j, new_color);
        }
    }
}

/* En aquesta funció invertim els colors d'una fotografia, tal que els valors de les tres variables de color seran
255 - (la variable que defineix R, G o B).*/
void Image::invertedPhoto(Image& picture){

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            Color color_globus = picture.getPixel(i, j);
            Color new_color = Color(255 - color_globus.r, 255 - color_globus.g, 255 - color_globus.b);
            setPixel(i, j, new_color);
        }
    }
}

/* En aquesta funció mostrem una imatge rotada per pantalla.
Canviem de posició tots els pixels però el centre d'aquesta està "fixats".
Per tant, estem movent la fotografia de forma angular.*/
void Image::rotateImage(Image& picture, float angle){

    for(unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            int  x = i - (width/2);
            int  y = j - (height/2);
            float new_x = (cos(angle * DEG2RAD) * x) - (sin(angle * DEG2RAD) * y) + (width/2);
            float new_y = (sin(angle * DEG2RAD) * x) + (cos(angle * DEG2RAD) * y) + (height/2);
            setPixelSafe(i, j, picture.getPixelSafe(new_x, new_y));
        }
    }
}

/* En aquesta funció redimensionem la mida d'una imatge, de manera que la podem ampliar o reduir,
mantenint les proporcions d'aquesta.*/
void Image::scaleImage(Image& picture, float scale){
    for(unsigned int i = 0; i < width; i++){
        for(unsigned int j = 0; j < height; j++){
            float centered_x = (width/2) - ((width * scale)/2);
            float centered_y = (height/2) - ((height * scale)/2);
            setPixelSafe((i*scale) + centered_x, (j*scale) + centered_y, picture.getPixelSafe(i, j));
        }
    }
}





