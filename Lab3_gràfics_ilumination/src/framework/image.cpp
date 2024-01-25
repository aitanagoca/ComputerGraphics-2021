#include "image.h"
#include "light.h"
#include "material.h"
#include "camera.h"

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
		for(unsigned int y = 0; y < height; ++x)
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


FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width*height];
	memset(pixels, 0, width * height * sizeof(float));
}

//copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
}

//assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width*height * sizeof(float)];
		memcpy(pixels, c.pixels, width*height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}


//change image size (the old one will remain in the top-left corner)
void FloatImage::resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = getPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
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

//Inicialitzem la taula.
void Image::initTable(){

    table.resize(this->height);

    //init table
    for(int i = 0; i < table.size(); i++) {
        table[i].minx = 100000; //very big number
        table[i].maxx = -100000; //very small number
    }
}


//Hem reutilitzat la funció de drawLine amb l'algoritme de Bresenham amb la taula.
// Tot i així, per estalviar-nos errors a l'hora de fer la interacció, afegim una condició que ens permeti donar la sensació de que la figura surt de la pantalla (no està pintant, realment).
void Image::BresenhamWithTable(int x0, int y0, int x1, int y1, std::vector<sCelda> &table, bool with_color){

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

    if (dx > dy) {
        for (int i = 0; i <= dx; i++) {
            if (d1 >= 0) {
                y0 += incy;
                d1 += inc_NE;
            } else {
                d1 += inc_E;
            }
            x0 += incx;

            if(x0 >= 0 && x0 < width && y0 >= 0 && y0 < height){
                if(with_color){
                    setPixel(x0, y0, Color::WHITE);
                }
                if(x0 < table[y0].minx){
                    table[y0].minx = x0;
                }
                if(x0 > table[y0].maxx){
                    table[y0].maxx = x0;
                }
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

            if(x0 >= 0 && x0 < width && y0 >= 0 && y0 < height){
                if(with_color){
                    setPixel(x0, y0, Color::WHITE);
                }
                if(x0 < table[y0].minx){
                    table[y0].minx = x0;
                }
                if(x0 > table[y0].maxx){
                    table[y0].maxx = x0;
                }
            }
        }
    }
}

//Reutilitzem la funció de dibuix del triangle del lab anterior.
void Image::drawTriangle(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , Color color , bool fill){

    initTable();

    BresenhamWithTable(x0, y0, x1, y1, table, true);
    BresenhamWithTable(x1, y1, x2, y2, table, true);
    BresenhamWithTable(x2, y2, x0, y0, table, true);

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

//Reutilitzem la funció de dibuix del triangle del lab anterior.
void Image::drawTriangleInterpolated(int x0 , int y0 , int x1 , int y1 , int x2 , int y2 , Color c0 , Color c1 , Color c2){

    initTable();

    BresenhamWithTable(x0, y0, x1, y1, table, false);
    BresenhamWithTable(x1, y1, x2, y2, table, false);
    BresenhamWithTable(x2, y2, x0, y0, table, false);

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
                if (((0 <= u) && (u <= 1)) && ((0 <= v) && (v <= 1)) && ((0 <= w) && (w <= 1))) {
                    Color cp = c0 * u + c1 * v + c2 * w;
                    setPixel(j, i, cp);
                }
            }
        }
    }
}

//Aquesta funció és una versió de la funció del triangle interpolat, però afegim el zbuffer i un boleà per determinar si hi ha textura o no.
// En cas que texture sigui diferent de NULL, aquella part del buffer no tindrà textura, sinó, si la tindra.
void Image::drawTriangleInterpolatedTexture(Image& colorbuffer, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2, FloatImage& zbuffer, Image* texture = NULL) {

    initTable();
    BresenhamWithTable(p0.x, p0.y, p1.x, p1.y, table, false);
    BresenhamWithTable(p1.x, p1.y, p2.x, p2.y, table, false);
    BresenhamWithTable(p2.x, p2.y, p0.x, p0.y, table, false);

    Vector2 v0, v1, v2;
    v0.x = p1.x-p0.x;
    v0.y = p1.y-p0.y;
    v1.x = p2.x-p0.x;
    v1.y = p2.y-p0.y;

    for(int i = 0; i < table.size(); i++){
        if(table[i].minx < table[i].maxx) {
            for (int j = table[i].minx; j <= table[i].maxx; j++) {

                v2.x = j - p0.x;
                v2.y = i - p0.y;

                float d00 = v0.dot(v0);
                float d01 = v0.dot(v1);
                float d11 = v1.dot(v1);
                float d20 = v2.dot(v0);
                float d21 = v2.dot(v1);
                float denom = d00 * d11 - d01 * d01;
                float v = (d11 * d20 - d01 * d21) / denom;
                v = clamp(v, 0, 1);
                float w = (d00 * d21 - d01 * d20) / denom;
                w = clamp(w, 0, 1);
                float u = 1.0 - v - w;

                float depth_P = p0.z * u + p1.z * v + p2.z * w;
                float uvx = uv0.x * u + uv1.x * v + uv2.x * w;
                float uvy = uv0.y * u + uv1.y * v + uv2.y * w;

                if(texture != NULL){
                    Color ct = texture->getPixelSafe(uvx, uvy);
                    if(depth_P < zbuffer.getPixel(j,i)){
                        colorbuffer.setPixel(j, i, ct);
                        zbuffer.setPixel(j, i, depth_P);
                    }
                }
                else {
                    Color cp = Color::RED * u + Color::BLUE * v + Color::GREEN * w;
                    if (depth_P < zbuffer.getPixel(j,i)) {
                        colorbuffer.setPixel(j, i, cp);
                        zbuffer.setPixel(j, i, depth_P);
                    }
                }
            }
        }
    }
}

//En aquesta funció ens basem en la funció anterior de dibuix del triangle interpolat, però aplicant la il·luminació Phong.
//D'aquesta forma, podem implementar il·luminació a la figura on abans només podíem crear la figura a base de triangles,
//triangles interpolats i textures.
void Image::drawTriangleInterpolatedIllumination(Image& colorbuffer, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& n0, const Vector3& n1, const Vector3& n2, const Vector3& uv0, const Vector3& uv1, const Vector3& uv2, Vector3& ambient_light, FloatImage& zbuffer, Light* light = NULL, Material* material = NULL, Camera* camera = NULL) {

    initTable();
    BresenhamWithTable(p0.x, p0.y, p1.x, p1.y, table, false);
    BresenhamWithTable(p1.x, p1.y, p2.x, p2.y, table, false);
    BresenhamWithTable(p2.x, p2.y, p0.x, p0.y, table, false);

    Vector2 v0, v1, v2;
    v0.x = p1.x-p0.x;
    v0.y = p1.y-p0.y;
    v1.x = p2.x-p0.x;
    v1.y = p2.y-p0.y;

    for(int i = 0; i < table.size(); i++){
        if(table[i].minx < table[i].maxx) {
            for (int j = table[i].minx; j <= table[i].maxx; j++) {

                v2.x = j - p0.x;
                v2.y = i - p0.y;

                float d00 = v0.dot(v0);
                float d01 = v0.dot(v1);
                float d11 = v1.dot(v1);
                float d20 = v2.dot(v0);
                float d21 = v2.dot(v1);
                float denom = d00 * d11 - d01 * d01;
                float v = (d11 * d20 - d01 * d21) / denom;
                v = clamp(v, 0, 1);
                float w = (d00 * d21 - d01 * d20) / denom;
                w = clamp(w, 0, 1);
                float u = 1.0 - v - w;

                float depth_P = p0.z * u + p1.z * v + p2.z * w;
                Vector3 uv = uv0 * u + uv1 * v + uv2 * w;
                Vector3 nPoint = n0 * u + n1* v + n2 * w;

                Vector3 L, V, R, A, D, S, ci;

                // Calcular els vectors L, V i R i normalitzar-los
                L.set(light->position.x - uv.x, light->position.y - uv.y, light->position.z - uv.z);
                L.normalize();

                V.set(camera->eye.x - uv.x, camera->eye.y - uv.y, camera->eye.z - uv.z);
                V.normalize();

                // El vector R es calcula utilitzant la fórmula: R = -L + 2.0 * dot(N, L) * N
                float dotProd = nPoint.dot(Vector3(L.x, L.y, L.z));
                float x0 = -L.x + 2 * dotProd * nPoint.x;
                float y0 = -L.y + 2 * dotProd * nPoint.y;
                float z0 = -L.z + 2 * dotProd * nPoint.z;
                R.set(x0, y0, z0);
                R.normalize();

                // Fem la l'equació de la llum en 3 passos.
                // 1: Calcular Ka * Ia
                float A_x = material->ambient.x * ambient_light.x;
                float A_y = material->ambient.y * ambient_light.y;
                float A_z = material->ambient.z * ambient_light.z;
                A.set(A_x, A_y, A_z);

                // 2: Calcular Kd * (L · N) * Id
                float dotProdLN = clamp(L.dot(nPoint), 0.0, 1.0);
                float D_x = material->diffuse.x * light->diffuse_color.x * dotProdLN;
                float D_y = material->diffuse.y * light->diffuse_color.y * dotProdLN;
                float D_z = material->diffuse.z * light->diffuse_color.z * dotProdLN;
                D.set(D_x, D_y, D_z);

                // 3: Calcular Ks * ((R · V) ^ alpha) * Is
                float dotProdRV = pow(clamp(R.dot(V), 0.0, 1.0), material->shininess);
                float S_x = material->specular.x * light->specular_color.x * dotProdRV;
                float S_y = material->specular.y * light->specular_color.y * dotProdRV;
                float S_z = material->specular.z * light->specular_color.z * dotProdRV;
                S.set(S_x, S_y, S_z);

                // Mirem que cada coordenada estigui dins del framebuffer
                if(j >= 0 && j < colorbuffer.width){
                    if(depth_P < zbuffer.getPixel(j,i)){
                        ci = A + D + S;
                        ci.set(ci.x * 255, ci.y * 255, ci.z * 255);
                        ci.x = clamp(ci.x, 0.0, 255.0);
                        ci.y = clamp(ci.y, 0.0, 255.0);
                        ci.z = clamp(ci.z, 0.0, 255.0);

                        // Color final de l'equació de la llum
                        Color new_c = Color(ci.x, ci.y, ci.z);
                        colorbuffer.setPixel(j, i, new_c);
                        zbuffer.setPixel(j, i, depth_P);
                    }
                }
            }
        }
    }
}

