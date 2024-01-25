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

// Funció per determinar si un nombres es negatiu o positiu, retornant -1 o 1 en cada cas.
int Image::sgn(int a) {

    if (a < 0) {
        return -1;
    }
    else {
        return 1;
    }
}

// Funció per dibuixar una línea qualsevol amb l'algoritme DDA.
void Image::drawLineDDA(int x0, int y0, int x1, int y1, Color color){

    float d, x, y;
    float dx = (x1 - x0);
    float dy = (y1 - y0);
    if (fabs(dx) >= fabs(dy)) {
        d = fabs(dx);
    }
    else {
        d = fabs(dy);
    }
    float vx = dx / d;
    float vy = dy / d;
    x = x0 + sgn(x0) * 0.5;
    y = y0 + sgn(y0) * 0.5;

    for (int i = 0; i <= d; i++) {
        setPixel(floor(x), floor(y), color);
        x = x + vx;
        y = y + vy;
    }
}

// funció per dibuixar una línea qualsevol (dels 8 octants) amb l'algoritme Bresenham.
// Això ho aconseguim amb les diferents condicions inicials i els dos casos possibles (dx > dy o dy > dx).
// A més, també afegim diferents increments per dibuixar pixels segons la direcció i el sentit (inc_SE, inc_NE, inc_E, inc_W).
void Image::drawLineBresenham( int x0,  int y0,  int x1,  int y1, Color color) {

    int dx, dy, incx = 1, incy = 1, d1, d2, inc_NE, inc_W, inc_E, inc_SE;
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dy;
    }
    if (x1 < x0) {
        incx = -1;
    }
    if (y1 < y0) {
        incy = -1;
    }

    inc_E = 2 * dy;
    inc_SE = 2 * dx;
    inc_NE = 2 * (dy - dx);
    inc_W = 2 * (dx - dy);
    d1 = 2 * dy - dx;
    d2 = 2 * dx - dy;
    setPixel(x0, y0, color);

    if (dx > dy) {
        for (int i = 0; i <= dx; i++) {
            if (d1 >= 0) {
                y0 += incy;
                d1 += inc_NE;
            } else {
                d1 += inc_E;
            }
            x0 += incx;
            setPixel(x0, y0, color);
        }
    }
    else {
        for (int i = 0; i <= dy; i++) {
            if (d2 >= 0) {
                x0 += incx;
                d2 += inc_W;
            }
            else {
                d2 += inc_SE;
            }
            y0 += incy;
            setPixel(x0, y0, color);
        }
    }
}

// Funció per dibuixar un cercle, amb la condició d'emplenament o no (mitjançant un booleà).
// Seguim el codi propocionat al PowerPoint, afegint els diferents octants necessaris per dibuixar el cercle sencer.
void Image::drawCircle(int p, int q, int r, Color color, bool fill){

    int x, y;
    int v;
    x = 0;
    y = r;
    v = 1 - r;
    // punts extrems de la creu del cercle
    setPixelSafe(p + r, q, color);
    setPixelSafe(p - r, q, color);
    setPixelSafe(p, q + r, color);
    setPixelSafe(p, q - r, color);
    while (y > x){
        if (v < 0) {
            v = v + 2 * x +3;
            x++;
        }
        else {
            v = v + 2 * (x - y) + 5;
            x++;
            y--;
        }

        if(fill){
            for(int i = -x + p; i <= x + p; i++){
                setPixelSafe(i, y + q, color);
                setPixelSafe(i, -y + q, color);
            }
            for(int j = -y + p; j <= y + p; j++){
                setPixelSafe(j, x + q, color);
                setPixelSafe(j, -x + q, color);
                setPixelSafe(j, q, color); // pintar linia horitzontal del mig del cercle (creu eix horitzontal)
            }
        }
        else{
            // els altres punts (pels diferents octants)
            setPixelSafe(x + p, y + q, color);
            setPixelSafe(y + p, x + q, color);
            setPixelSafe(-y + p, x + q, color);
            setPixelSafe(-x + p, y + q, color);
            setPixelSafe(-x + p, -y + q, color);
            setPixelSafe(-y + p, -x + q, color);
            setPixelSafe(y + p, -x + q, color);
            setPixelSafe(x + p, -y + q, color);
        }
    }
}

// Funció per inicialitzar la taula que utilitzarem per dibuixar els triangles.
void Image::initTable(){

    table.resize(this->height);

    //init table
    for(int i = 0; i < table.size(); i++) {
        table[i].minx = 100000; //very big number
        table[i].maxx = -100000; //very small number
    }
}

// Funció per dibuixar línees amb l'algoritme Bresenham, però actualitzem el valor màxim i mínim de la taula després de cada setPixel.
void Image::BresenhamWithTable(int x0, int y0, int x1, int y1, Color color, std::vector<sCelda> &table){

    int dx, dy, incx = 1, incy = 1, d1, d2, inc_NE, inc_W, inc_E, inc_SE;
    dx = x1 - x0;
    dy = y1 - y0;
    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dy;
    }
    if (x1 < x0) {
        incx = -1;
    }
    if (y1 < y0) {
        incy = -1;
    }

    inc_E = 2 * dy;
    inc_SE = 2 * dx;
    inc_NE = 2 * (dy - dx);
    inc_W = 2 * (dx - dy);
    d1 = 2 * dy - dx;
    d2 = 2 * dx - dy;
    setPixel(x0, y0, color);

    if (dx > dy) {
        for (int i = 0; i <= dx; i++) {
            if (d1 >= 0) {
                y0 += incy;
                d1 += inc_NE;
            } else {
                d1 += inc_E;
            }
            x0 += incx;
            setPixel(x0, y0, color);
            if(x0 < table[y0].minx){
                table[y0].minx = x0;
            }
            if(x0 > table[y0].maxx){
                table[y0].maxx = x0;
            }
        }
    }
    else {
        for (int i = 0; i <= dy; i++) {
            if (d2 >= 0) {
                x0 += incx;
                d2 += inc_W;
            }
            else {
                d2 += inc_SE;
            }
            y0 += incy;
            setPixel(x0, y0, color);
            if(x0 < table[y0].minx){
                table[y0].minx = x0;
            }
            if(x0 > table[y0].maxx){
                table[y0].maxx = x0;
            }
        }
    }
}

// Funció per dibuixar el triangle a partir de la crida a la funció de dibuix de línees amb taula.
// Considerem un booleà per decidir si s'emplena o no.
// En cas afirmatiu, recorrem el triangle dibuixat amb dos fors, un per la llargada de la taula, i l'altre des del valor mínim al valor màxim de cada posició de la taula.
void Image::drawTriangle(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , Color color , bool fill){

    initTable();

    BresenhamWithTable(x0, y0, x1, y1, color, table);
    BresenhamWithTable(x1, y1, x2, y2, color, table);
    BresenhamWithTable(x2, y2, x0, y0, color, table);

    if(fill){
        for(int i = 0; i < table.size(); i++){
            if(table[i].minx < table[i].maxx){
                for(int j = table[i].minx; j <= table[i].maxx; j++){
                    setPixel(j, i, color);
                }
            }
        }
    }
}

// Funció per dibuixar el triangle interpolat.
// Cridem a la funció de la mateixa forma que ho fem amb el triangle normal, però aquest cop utilitzem la classe vector2 per trobar el punt
// baricèntric del triangle i pintar-ho amb coordenades independents pels colors (en el nostre cas, c0 com a RED, c1 com a GREEN i c2 com a BLUE).
// Aquestes variables es multiplicaran per cada iteració als pesos de cada color (u, v i w) que determinaran el color en cada pixel.
void Image::drawTriangleInterpolated(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , Color c0 , Color c1 , Color c2){

    initTable();

    BresenhamWithTable(x0, y0, x1, y1, c0, table);
    BresenhamWithTable(x1, y1, x2, y2, c1, table);
    BresenhamWithTable(x2, y2, x0, y0, c2, table);

    Vector2 v0, v1, v2;
    v0.x = x1-x0;
    v0.y = y1-y0;
    v1.x = x2-x0;
    v1.y = y2-y0;

    for(int i = 0; i < table.size(); i++){
        if(table[i].minx < table[i].maxx){
            for(int j = table[i].minx; j <= table[i].maxx; j++){
                v2.x = j-x0;
                v2.y = i-y0;
                //computing the dot of a vector with itself
                //is the same as length*length but faster
                float d00 = v0.dot(v0);
                float d01 = v0.dot(v1);
                float d11 = v1.dot(v1);
                float d20 = v2.dot(v0);
                float d21 = v2.dot(v1);
                float denom = d00 * d11 - d01 * d01;
                float v = (d11 * d20 - d01 * d21) / denom;
                float w = (d00 * d21 - d01 * d20) / denom;
                float u = 1.0 - v - w;

                //use weights to compute final color
                Color color = c0 * u + c1 * v + c2 * w;
                setPixel(j, i, color);
            }
        }
    }
}


