#include "stdafx.h"

void OnRender();
void OnReshape(int, int);
void OnKeyPress(unsigned char, int, int);
void OnKeyDown(unsigned char, int, int);
void OnKeyUp(unsigned char, int, int);
void OnSpecialKeyPress(int, int, int);
void OnSpecialKeyDown(int, int, int);
void OnSpecialKeyUp(int, int, int);
void OnTimer(int);
void drawgrass(int iy, int ix);
void spowolnienie(int);

GLuint grass; // textura pustyni
GLuint road; // textura drogi
GLuint petroltexture; // benzyny tex
GLuint playertexture; // moja tex
GLuint index; // display lista klocka
GLuint grasss; // display lista trawy 
GLuint cactustexture;
GLuint rocktexture;
GLuint signtexture;
GLuint enemytexture11; // busa tex
GLuint enemytexture12;
GLuint enemytexture13;
GLuint enemytexture14;
GLuint enemytexture15;
GLuint enemytexture21; // tez zlomka
GLuint enemytexture22;
GLuint enemytexture23;
GLuint enemytexture24;
GLuint enemytexture25;
GLuint enemytexture31; 
GLuint enemytexture32; 
GLuint enemytexture33; 
int tyl = -3;
int przod = 32;

struct vec3 
{
	float x, y, z;
};

struct SCameraState 
{
	vec3 pos; // pozycja kamery
	vec3 dir; // kierunek patrzenia (wektor jednostkowy)
	float speed; // mno¿nik zmian pozycji - "szybkoœæ" ruchów kamery
};

struct enemies
{
	vec3 pos;
	float speed;
	int modnum;
	int texnum;
};

struct petrol
{
	vec3 pos;
};

struct signeusz
{
	vec3 pos;
};

struct cactuses
{
	float posXL,posXR, posZ;
	float rotX;
	float size;
};

struct rocks
{
	float posXL, posXR, posZ;
	float rotX,rotY;
	float size;
};

std::vector<enemies*> en;
std::vector <cactuses*> cac;
std::vector <rocks*> roc;
SCameraState camera;
SCameraState player;

char utwory[][128] =
{
	"muzyka/nuta00",
	"muzyka/nuta0",
	"muzyka/nuta1",
	"muzyka/nuta2",
	"muzyka/nuta3",
	"muzyka/nuta4",
	"muzyka/nuta5",
	"muzyka/nuta6",
	"muzyka/nuta7",
	"muzyka/nuta8",
	"muzyka/nuta9",
	"muzyka/nuta10",
	"muzyka/nuta11",
	"muzyka/nuta12"
};

char tytuly[][128] =
{
	"Depeche Mode - Route 66",
	"Lynyrd Skynyrd - Sweet Home Alabama",
	"Boston- Smokin'", 
	"Creedence Clearwater Revival- Green River",
	"Ed Bruce- Mammas",
	"George Strait- All My Ex's Live In Texas",
	"Electro Swing Collection",
	"Kool The Gang- Hollywood Swinging",
	"Tom Petty- Runnin Down A Dream",
	"Black Sabbath- Paranoid ",
	"Ronnie Hudson- West coast poplock",
	"The Animals- House of the Rising Sun",
	"Shakin Stevens- Cry Just A Little Bit",
	"Michael Sembello- Maniac "
};

GLvoid BuildLists()
{
	index = glGenLists(2);
	glNewList(index, GL_COMPILE);
		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f);
		glNormal3f(0.0f, 0.0f, -1.0f); // -> -1.0f
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	glEndList();

	grasss = index + 1;
		glNewList(grasss, GL_COMPILE);
		for (int iz = 0; iz <=  (przod + tyl); iz++)
		{
			for (int ix = 1; ix < 14; ix++)
			{
				int nz = iz - tyl + ix;
				if (nz >  przod) nz =   przod;
				drawgrass(nz , ix + 3);
			}
			for (int ix = -1; ix > -14; ix--)
			{
				int nz = iz - tyl - ix;
				if (nz >  przod) nz =   przod;
				drawgrass(nz , ix - 4);
			}
		}
	glEndList();
}

// Funkcja odczytuj¹ca bitmapê i tworz¹ca na jej podstawie teksturê z zadanym rodzajem filtracji
GLuint LoadTexture(char * file, int magFilter, int minFilter) 
{
	Bitmap *tex = new Bitmap();
	if (!tex->loadBMP(file)) 
	{
		printf("ERROR: Cannot read texture file \"%s\".\n", file);
		return -1;
	}

	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); 
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR || minFilter == GL_LINEAR_MIPMAP_NEAREST) 
	{
		// Automatyczne zbudowanie mipmap i wys³anie tekstury do pamiêci karty graficznej
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, tex->width, tex->height, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}
	else 
	{
		// Wys³anie tekstury do pamiêci karty graficznej 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->width, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->data);
	}
	delete tex;
	return texId;
}

void LoadTextures() 
{
	grass = LoadTexture("Resources/desert4.bmp", GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR); //GL_NEAREST, GL_NEAREST
	road = LoadTexture("Resources/nowadroga.bmp", GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR);
	petroltexture = LoadTexture("Resources/gasscan222.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	playertexture = LoadTexture("Resources/pickup.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture11 = LoadTexture("Resources/bus1.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture12 = LoadTexture("Resources/bus2.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture13 = LoadTexture("Resources/bus3.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture14 = LoadTexture("Resources/bus4.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture15 = LoadTexture("Resources/bus5.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture21 = LoadTexture("Resources/mosk1.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture22 = LoadTexture("Resources/mosk2.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture23 = LoadTexture("Resources/mosk3.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture24 = LoadTexture("Resources/mosk4.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture25 = LoadTexture("Resources/mosk5.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture31 = LoadTexture("Resources/extreme.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture32 = LoadTexture("Resources/extreme1.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	enemytexture33 = LoadTexture("Resources/extreme2.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	cactustexture = LoadTexture("Resources/kaktus.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	rocktexture = LoadTexture("Resources/rocktex.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	signtexture = LoadTexture("Resources/sign.bmp", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
}