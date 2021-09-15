#include "stdafx.h"
#include "main.h"
#include "modelLoader.h"
#define _CRT_SECURE_NO_WARNINGS


#pragma region Zmienne globalne

int framecount = 0, licznik, licznik2, licznik3;
int wynik = 0,highscore = 0;
int menuoption = 0, ingame = 0, failflag = 0, failflag2 = 0;
bool keystate[256], keystate_special[256];
char buffer[128];
float roznicaz, obrot = 0.0, obrotbeny = 0, speed = 1.0 / 60;
int HP, currentsound = 5;
petrol *pet;	signeusz *sig;	FILE * fp;
int mnoznik = 8, starywynik = 0;
int helpflag = 0, authorflag = 0, endcounter;
int cameraflag;

#pragma endregion

int main(int argc, char* argv[])
{
	#pragma region gluty 

	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 720);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Fifo Lafo");
	glutFullScreen();           // making the window full screen
	glutDisplayFunc(OnRender);
	glutReshapeFunc(OnReshape);
	glutKeyboardFunc(OnKeyPress);
	glutKeyboardUpFunc(OnKeyUp);
	glutSpecialFunc(OnSpecialKeyPress); // klawisze specjalne (strzalki, F1-F12, PgUp/PgDn, Home, End, Delete, Insert)
	glutSpecialUpFunc(OnSpecialKeyUp);
	OnTimer(0);
	float gl_amb[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gl_amb);
	glEnable(GL_LIGHTING); // Wlaczenie oswietlenia
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); // Włączenie cullingu - rysowania tylko jednej strony wielokątów
	glCullFace(GL_BACK); // Określenie, którą stronę wielokątów chcemy ukrywać
	glFrontFace(GL_CCW); // Określenie, jaki kierunek definicji wierzchołków oznacza przód wielokątu (GL_CCW - przeciwnie do ruchu wskazówek zegara, GL_CW - zgodnie)
	glutSetCursor(GLUT_CURSOR_NONE); //wylaczenie myszki
	#pragma endregion

	#pragma region inicjalizacja
	
	camera.pos.x = -0.5f;	camera.dir.x = 0.0f;
	camera.pos.y = 3.0f;	camera.dir.y = -0.5f;
	camera.pos.z = 0.0f;	camera.dir.z = 1.0f;
	camera.speed = 0.15f;
	
	player.pos.x = -0.5f;
	player.pos.y = 0.2f;
	player.pos.z = 0.0f;
	player.speed = 0.1f;

	fp = fopen("record.fifolafo", "rb");
	if(fp==NULL) fp= fopen("record.fifolafo", "ab");
	fscanf(fp,"%d", &highscore);
	fclose(fp);

	pet = new petrol;	sig = new signeusz;
	srand(time(NULL));	BuildLists();	LoadTextures();	LoadModels();
	PlaySound("muzyka/intro.wav",NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	currentsound = rand() % 15;

	#pragma endregion

	glutMainLoop();
	return 0;
}

#pragma region rysowanko swiata

void drawgrass(int iy, int ix)
{
	glPushMatrix();
		glScalef(1.0f, .1f, 0.5f);
		glTranslatef(ix, 0.0f, iy * 2);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glCallList(index);
	glPopMatrix();
}

void drawroad(int iz)
{
	glPushMatrix();
		glScalef(3.52f, .5f, .5f);
		glTranslatef(-0.14f, 0.0f, iz*2);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glCallList(index);
	glPopMatrix();
}

void createenemy()
{
	en.push_back(new enemies); // -2 -1 0 1 2 -> -2,5 -1,5 -0,5 0,5 1,5
	en[en.size() - 1]->pos.x = (rand() % 5 - 2) - 0.5f;
	en[en.size() - 1]->pos.z = camera.pos.z + 40.0f;
	en[en.size() - 1]->speed = (rand() % 10 + 10)*0.007f*2.0f;
	en[en.size() - 1]->modnum = rand() % 3;
	en[en.size() - 1]->texnum = rand() % 5;
}

void drawenemy()
{
	for (int i = 0; i<en.size(); i++)
	{
		int num = en[i]->texnum;
		glPushMatrix();
			glTranslatef(en[i]->pos.x, 0.1f, en[i]->pos.z);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
			glScalef(0.17f, 0.19f, 0.13f);
			if (en[i]->modnum == 0)
			{
				if (num == 0) glBindTexture(GL_TEXTURE_2D, enemytexture11);
				else if (num == 1) glBindTexture(GL_TEXTURE_2D, enemytexture12);
				else if (num == 2) glBindTexture(GL_TEXTURE_2D, enemytexture13);
				else if (num == 3) glBindTexture(GL_TEXTURE_2D, enemytexture14);
				else if (num == 4) glBindTexture(GL_TEXTURE_2D, enemytexture15);
				glCallList(enemy1);
			}
			else if (en[i]->modnum == 1)
			{
				if(num==0 ) glBindTexture(GL_TEXTURE_2D, enemytexture31);
				else if(num==1 || num==3) glBindTexture(GL_TEXTURE_2D, enemytexture32);
				else if(num==2|| num==4) glBindTexture(GL_TEXTURE_2D, enemytexture33);
				glCallList(enemy3);
			}
			else
			{
				if(num==0) glBindTexture(GL_TEXTURE_2D, enemytexture21);
				else if(num==1) glBindTexture(GL_TEXTURE_2D, enemytexture22);
				else if(num==2) glBindTexture(GL_TEXTURE_2D, enemytexture23);
				else if(num==3) glBindTexture(GL_TEXTURE_2D, enemytexture24);
				else if(num==4) glBindTexture(GL_TEXTURE_2D, enemytexture25);
				glCallList(enemy2);
			}
		glPopMatrix();
	}
}

void drawplayer()
{
	glPushMatrix();
		glTranslatef(player.pos.x, player.pos.y, player.pos.z);
		glRotatef(obrot, 0.0f, 1.0f, 0.0f);
		glScalef(0.14f, 0.19f, 0.13f);
		glCallList(plaayer);
	glPopMatrix();
}

void RenderText(float x, float y, char * string)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); // save
	glLoadIdentity();// and clear
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST); // also disable the depth test so renders on top
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(x, y); // center of screen. (-1,0) is center left.
	const char * p = string;
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p); while (*(++p));
	glEnable(GL_DEPTH_TEST); // Turn depth testing back on
	glMatrixMode(GL_PROJECTION);
	glPopMatrix(); // revert back to the matrix I had before.
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void createpetrol()
{
	pet->pos.x = (rand() % 5 - 2) - 0.5f;
	pet->pos.z = player.pos.z + 40.0f;
}

void drawpetrol()
{
	glColor3f(0.0f , 0.0f, 0.0f);
		glPushMatrix();
		glTranslatef(pet->pos.x, 0.2f, pet->pos.z);
		glRotatef(obrotbeny++/4, 0.0f, 1.0f, 0.0f);
		glScalef(0.015f, 0.015f, 0.0215f);
		glCallList(petroll);
	glPopMatrix();
}

void createcactus()
{
	cac.push_back(new cactuses);
	cac[cac.size() - 1]->posXL = rand() % 7 + 2;
	cac[cac.size() - 1]->posXR = rand() % 7 - 9;
	cac[cac.size() - 1]->posZ = player.pos.z+35.0f;
	cac[cac.size() - 1]->rotX = rand() % 90;
	cac[cac.size() - 1]->size = (rand() % 16 + 15)*0.001f;
}

void drawcactus()
{
	for (int i = 0; i < cac.size(); i++)
	{
		glPushMatrix();	//lewo
			glTranslatef(cac[i]->posXL+1.5f, 0.8f, cac[i]->posZ);
			glRotatef(cac[i]->rotX, 0.0f, 1.0f, 0.0f);
			glScalef(0.015f, cac[i]->size, 0.0215f);
			glCallList(cactus);
		glPopMatrix();

		glPushMatrix();	//prawo
			glTranslatef(cac[i]->posXR-1.5f, 0.7f, cac[i]->posZ);
			glRotatef(cac[i]->rotX, 0.0f, 1.0f, 0.0f);
			glScalef(0.015f, cac[i]->size, 0.0215f);
			glCallList(cactus);
		glPopMatrix();
	}
}

void createrock()
{
	roc.push_back(new rocks);
	roc[roc.size() - 1]->posXL = rand() % 7 + 2;
	roc[roc.size() - 1]->posXR = rand() % 7 - 9;
	roc[roc.size() - 1]->posZ = player.pos.z + 35.0f;
	roc[roc.size() - 1]->rotX = rand() % 90;
	roc[roc.size() - 1]->rotY = rand() % 90;
	roc[roc.size() - 1]->size = (rand() % 9+5)*0.001;
}

void drawrock()
{
	for (int i = 0; i < roc.size(); i++)
	{
		glPushMatrix();	//lewo
			glTranslatef(roc[i]->posXL+1.5f, 0.1f, roc[i]->posZ);
			glRotatef(roc[i]->rotX, 0.0f, 1.0f, 0.0f);
			glRotatef(roc[i]->rotY, 0.0f, 0.0f, 1.0f);
			glScalef(roc[i]->size, roc[i]->size, 0.01f);
			glCallList(rock);
		glPopMatrix();

		glPushMatrix();	//prawo
			glTranslatef(roc[i]->posXR- 1.5f, 0.1f, roc[i]->posZ);
			glRotatef(roc[i]->rotX, 0.0f, 1.0f, 0.0f);
			glRotatef(roc[i]->rotY, 0.0f, 0.0f, 1.0f);
			glScalef(roc[i]->size, roc[i]->size, 0.01f);
			glCallList(rock);
		glPopMatrix();
	}
}

void createsign()
{
	sig->pos.x = -3.4f;
	sig->pos.z = camera.pos.z + 28.0f;
}

void drawsign()
{
	glPushMatrix();
		glTranslatef(sig->pos.x-0.5f, 0.2f, sig->pos.z);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(0.017f, 0.019f, 0.013f);
		glCallList(sign);
	glPopMatrix();
}

#pragma endregion

void spowolnienie(int x)
{
	if (speed > 0 && !ingame)
	{
		speed *= 0.9998f;
		glutTimerFunc(17, spowolnienie, 0);
	}
	else
	{
		speed = 0.0f;
		return;
	}
}

void paliwko(int x)
{
	HP--;
	if (ingame)
	{
		if (mnoznik >= 15)glutTimerFunc(195, paliwko, 0);
		else if(mnoznik>=25)glutTimerFunc(150, paliwko, 0);
		else if(keystate[119] || keystate_special[GLUT_KEY_DOWN])glutTimerFunc(240, paliwko, 0);
		else if (!keystate[119] || keystate_special[GLUT_KEY_DOWN])glutTimerFunc(300, paliwko, 0);
	}
}

void OnTimer(int id)
{
	glutTimerFunc(17, OnTimer, 0);

	#pragma region tu i tu 

	if (keystate[57]) PlaySound(NULL, NULL, NULL); // mute
	if (!failflag && !failflag2) player.pos.z += speed * mnoznik; // poruszanie sie na start 8
	else if (!failflag && failflag2)
	{
		player.pos.z += speed * mnoznik;
		spowolnienie(0);
	}
	else if(failflag && !failflag2)
	{
		if(player.pos.z+3.0f>camera.pos.z)	player.pos.z -= speed * mnoznik; // jesli przegram to cofa razem z wrogiem
	}
	roznicaz = player.pos.z - camera.pos.z + 0.5f; //stala odleglosc miedzy kamera z playerem
	if (roznicaz > 0) camera.pos.z += roznicaz * camera.speed *0.2f;
	if (obrot > 0) obrot -= 0.45;
	if (obrot < 0) obrot += 0.45;
	if (!cameraflag) //powrot z "trybu przyspieszenia"
	{
		if (camera.pos.y < 3.0f)	camera.pos.y += 0.009f; 
		if (camera.dir.y > -0.5f)	camera.dir.y -= 0.007f;

	}
	if (endcounter) endcounter--; // czas neimoznosci po przegraniu

	#pragma endregion
	
	if (ingame) // jesli w grze
	{
		speed = 1.0 / 60; // nowa rozgrywaka - ustawienie speeda za pomaco dzielnika
		helpflag = 0, authorflag = 0;

		#pragma region klawisze

		if (keystate['s'] || keystate_special[GLUT_KEY_DOWN])	player.pos.z -= 0.0015f / (2 * speed);
		if (keystate['w'] || keystate_special[GLUT_KEY_UP])
		{
			cameraflag = 1;
			player.pos.z += 0.0013f / (2 * speed);
			if (camera.pos.y > 2.2f)	camera.pos.y -= 0.01f;
			if (camera.dir.y <=-0.43f)	camera.dir.y += 0.004f;
			camera.pos.z += 0.04f;
		}
		if (keystate['a'] || keystate_special[GLUT_KEY_LEFT])
		{
			player.pos.x += 0.0016f / speed;
			obrot += 0.7;
		}
		if (keystate['d'] || keystate_special[GLUT_KEY_RIGHT])
		{
			player.pos.x -= 0.0016f / speed;
			obrot -= 0.7;
		}
		if (!licznik3)
		{
			if (keystate[56])
			{
				currentsound = currentsound == 0 ? sizeof(utwory) / sizeof(utwory[1]) - 1 : currentsound - 1;
				PlaySound(utwory[currentsound], NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
				licznik3 = 40;
			}
			if (keystate[48])
			{
				currentsound = currentsound == sizeof(utwory) / sizeof(utwory[1]) - 1 ? 0 : currentsound + 1;
				PlaySound(utwory[currentsound], NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
				licznik3 = 40;
			}
		}
		if (licznik3>0)	licznik3--;

		#pragma endregion

		#pragma region brzegi

		if (player.pos.x > 1.6)
		{
			obrot = 0;
			player.pos.x -= 0.0016f / speed; //w lewo
		}
		if (player.pos.x < -2.55)
		{
			obrot = 0;
			player.pos.x += 0.0016f / speed; // w prawo
		}
		#pragma endregion

		#pragma region skaly kaktusy znak
		
		for (int i = 0; i < cac.size(); i++)	if (cac[i]->posZ < player.pos.z - 6.0f)	if (!cac.empty())	cac.erase(cac.begin() + i);
		for (int i = 0; i < roc.size(); i++)	if (roc[i]->posZ < player.pos.z - 6.0f)	if (!roc.empty())	roc.erase(roc.begin() + i);

		#pragma endregion

		#pragma region przeciwnicy

		if (!licznik2) // tworzenie przeciwnika co "stala wartosc" xd
		{	
			createcactus();	createrock();	
			createenemy();
			licznik2 = 18;
		}
		else	licznik2--;

		for (int i = 0; i < en.size(); i++) // dla wszystkich przeciwnikow
		{
			en[i]->pos.z -= en[i]->speed;
			for (int j = 0; j < en.size(); j++)
			{
				if (i == j) continue;
				if (fabs(en[i]->pos.z- en[j]->pos.z) <1.5f && en[i]->pos.x == en[j]->pos.x) // kolizje przeciwnikow
				{
					float tmp = en[j]->speed;
					en[j]->speed = en[i]->speed;
					en[i]->speed = tmp;
					tmp = NULL;
					break;
				}
			}
			if (en[i]->pos.z < camera.pos.z - 6.0f) //sprzwdzamy czy minalem przeciwnika
			{
				if (keystate['w'] || keystate_special[GLUT_KEY_UP]) wynik+=2;
				else if (keystate['s'] || keystate_special[GLUT_KEY_DOWN]) wynik = wynik;
				else wynik++;
				if (starywynik <= wynik - 25)
				{
					if (mnoznik <30)	mnoznik += 1;
					if (camera.speed < 0.6)	camera.speed += 0.0199;
					starywynik = wynik;
				}
				if (wynik > highscore)	highscore = wynik;
				if (!en.empty())	en.erase(en.begin() + i); // usuwanie z wektora
				continue;
			}
			if (HP == 0) // jesli skonczy mi sie benzyna
			{
				PlaySound("muzyka/whaa.wav", NULL, SND_ASYNC | SND_FILENAME);
				fp = fopen("record.fifolafo", "wb");
				fprintf(fp, "%d", highscore);
				fclose(fp);
				starywynik = 0;		ingame = 0;				failflag2 = 1;
				mnoznik = 8;		camera.speed = 0.15f;		endcounter = 170;
			}
			if (fabs(player.pos.z - en[i]->pos.z) < 1.5f && fabs(player.pos.x - en[i]->pos.x) < 0.5f) // kolizje 
			{
				PlaySound("muzyka/crash2.wav", NULL, SND_ASYNC | SND_FILENAME);
				fp = fopen("record.fifolafo", "wb");
				fprintf(fp, "%d", highscore);
				fclose(fp);
				starywynik = 0;		ingame = 0;				failflag = 1; 		
				mnoznik = 8;		camera.speed = 0.15f;		endcounter = 170;
			}
		}
		#pragma endregion

		#pragma region benzyna

		if (fabs(pet->pos.z - player.pos.z) < (mnoznik/15.7f) && fabs(pet->pos.x - player.pos.x) < 0.4f) // zebranie paliwa
		{
			createpetrol();
			HP = HP < 80 ? HP + 25 : 100;
		}
		if (pet->pos.z - player.pos.z < -4.0f)	createpetrol(); // jesli nie zbiore paliwa zeby zniknelo
		#pragma endregion
	}
	else // w menu
	{
		
		#pragma region klawisze
		if (!endcounter) // niemoznosc po przegranej
		{
			if (keystate[13]) // spacja - 32
			{
				if (!ingame)
				{
					createsign();
					HP = 100;
					wynik = 0; //doo
					endcounter = 0;
					failflag = 0;	failflag2 = 0;
					PlaySound(utwory[currentsound], NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
					ingame = 1;
					glutTimerFunc(300, paliwko, 0);
				}
				if (menuoption == 1)	glutLeaveMainLoop();
			}
			if (keystate_special[GLUT_KEY_UP])		menuoption = 0;
			if (keystate_special[GLUT_KEY_DOWN])	menuoption = 1;
		}
		#pragma endregion

		#pragma region przeciwnicy

		for (int i = 0; i < en.size(); i++) // dla wszystkich przeciwnikow
		{
			en[i]->pos.z -= en[i]->speed;
			for (int j = 0; j < en.size(); j++)
			{
				if (i == j) continue;
				if (fabs(en[i]->pos.z - en[j]->pos.z) <1.5f && en[i]->pos.x == en[j]->pos.x)
				{
					float tmp = en[j]->speed;
					en[j]->speed = en[i]->speed;
					en[i]->speed = tmp;
					tmp = NULL;
					break;
				}
			}
			if (player.pos.z + 3.0f < camera.pos.z) player.pos.z = camera.pos.z - 1.0f; // zeby auto nie cofalo sie w nieskonczonosc
			if (en[i]->pos.z < camera.pos.z - 1.0f) //sprzwdzamy czy minalem przeciwnika
			{
				if (!en.empty())	en.erase(en.begin() + i); // usuwanie z wektora
				continue;
			}
		}

		#pragma endregion
	}
}

void OnRender()
{
	#pragma region gluty

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z, // Pozycja kamery
		camera.pos.x + camera.dir.x, camera.pos.y + camera.dir.y, camera.pos.z + camera.dir.z, // Punkt na ktory patrzy kamera (pozycja + kierunek)
		0.0f, 1.0f, 0.0f);// Wektor wyznaczajacy pion

	#pragma endregion
	
	#pragma region Swiatlo
	
	 //przytwierdzone do gracza - punktowe
	float l0_amb[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float l0_dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float l0_spe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float l0_pos[] = { player.pos.x, player.pos.y+5.0f, player.pos.z+3.0f, 1 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spe);
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);
	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1f);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, .01f);

	//niezalezne kierunkowe
	float l1_amb[] = { 0.8f, 0.3f, 0.0f, 0.5f };
	float l1_dif[] = { 0.8f, 0.3f, 0.0f, 0.5f };
	float l1_spe[] = { 0.8f, 0.3f, 0.0f, 0.5f };
	float l1_pos[] = { 6.0f, 4.0f, camera.pos.z + 5.0f, 0.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, l1_amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l1_dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l1_spe);
	glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);
	//glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, .02f);
	//glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, .02f);
	//glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, .02f);

	#pragma endregion

	#pragma region rysowaniee

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); 
	drawenemy(); 
	glBindTexture(GL_TEXTURE_2D, playertexture);	drawplayer();
	glBindTexture(GL_TEXTURE_2D, petroltexture);	drawpetrol();
	glBindTexture(GL_TEXTURE_2D, cactustexture);	drawcactus();
	glBindTexture(GL_TEXTURE_2D, rocktexture);		drawrock();
	glBindTexture(GL_TEXTURE_2D, signtexture);		drawsign();
	glBindTexture(GL_TEXTURE_2D, road);				for (int iz = camera.pos.z - 3; iz <= camera.pos.z + 32; iz++)	drawroad(iz);
	glBindTexture(GL_TEXTURE_2D, grass);
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, (int)camera.pos.z); //3.0f
		glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
		glCallList(grasss);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	#pragma endregion

	#pragma region tekst

	if (!ingame && !endcounter)
	{
		RenderText(-0.15f, 0.75f, "FIFO LAFO");
		RenderText(-0.15f, 0.6f, "Wybierz opcje ");
		RenderText(-0.15f, 0.5f, menuoption == 0 ? ">Nowa gra" : "Nowa gra");
		RenderText(-0.15f, 0.4f, menuoption == 1 ? ">Wyjscie" : "Wyjscie");

		if (!authorflag && !helpflag)
		{
			RenderText(0.4f, 0.7f, "Pomoc - nacisnij \"h\"");
			RenderText(0.4f, 0.6f, "O autorze - nacisnij \"o\"");
		}
		if (helpflag)
		{
			RenderText(0.4f, 0.85f, "Jak grac?");
			RenderText(0.4f, 0.75f, "A i D / Lewo i Prawo - skrecanie");
			RenderText(0.4f, 0.65f, "W / Gora- dodatkowe przyspieszenie");
			RenderText(0.4f, 0.55f, "S /  Dol - hamowanie");
			RenderText(0.4f, 0.45f,"Radio: 8  9  0");
			RenderText(0.4f, 0.35f,"         <<  ||  >>");
			RenderText(0.4f, 0.25f,"Zbieraj paliwo i zajedz jak najdalej!");
			RenderText(0.4f, 0.05f, "Wyjscie - \"h\"");
		}
		if (authorflag)
		{
			RenderText(0.4f, 0.85f, "O autorze: ");
			RenderText(0.4f, 0.75f, "Jakub Karaszewski");
			RenderText(0.4f, 0.55f, "Wyjscie - \"o\"");
		}
	}
	else
	{
		if (!endcounter)
		{
			menuoption = 0;
			RenderText(-0.99f, 0.8f, "Teraz gramy:");
			sprintf_s(buffer, "%s", tytuly[currentsound]);	RenderText(-0.99f, 0.7f, buffer);
			sprintf_s(buffer, "PALIWO: %d %%", HP);	RenderText(0.4f, 0.8f, buffer);
			sprintf_s(buffer, "Wynik: %d", wynik);	RenderText(0.4f, 0.7f, buffer);
			sprintf_s(buffer, "Najlepszy wynik: %d", highscore);	RenderText(0.4f, 0.6f, buffer);
			if (mnoznik==30)	RenderText(0.4f, 0.25f, "Maksymalana predkosc!");
			if (keystate['w'] || keystate_special[GLUT_KEY_UP])	RenderText(0.4f, 0.15f, "Punkty za predkosc x2!");
			if (keystate['s'] || keystate_special[GLUT_KEY_DOWN])	RenderText(0.4f, 0.15f, "Brak punktow za zwalnianie!");
		}
	}
	if (failflag &&endcounter)
	{	
		RenderText(-0.1f, 0.6f, "PRZEGRALES ");
		sprintf_s(buffer, "Twoj wynik: %d", wynik);	RenderText(-0.1f, 0.5f, buffer);
	}
	if (failflag2 &&endcounter)
	{
		RenderText(-0.1f, 0.7f, "KONIEC PALIWA");
		RenderText(-0.1f, 0.6f, "PRZEGRALES ");
		sprintf_s(buffer, "Twoj wynik: %d", wynik);	RenderText(-0.1f, 0.5f, buffer);
	}

	#pragma endregion

	#pragma region mgla

	GLfloat fogC[4] = { 0.8f, 0.3f, 0.0f, 0.5f };
	glClearColor(fogC[0], fogC[1], fogC[2], fogC[3]);           // We'll Clear To The Color Of The Fog ( Modified )
	glFogi(GL_FOG_MODE, GL_EXP2);								// Fog Mode
	glFogfv(GL_FOG_COLOR, fogC);								// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.07f);								// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);	
	glFogf(GL_FOG_START, 40.0f);
	glFogf(GL_FOG_END, 42.0f);
	glEnable(GL_FOG);

	#pragma endregion
	
	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

#pragma region klawisze

void OnKeyPress(unsigned char key, int x, int y) // Obsługa zdarzenia, gdy zostanie wciśnięty klawisz - zdarzenie nieoodporne na repetycję klawiszy.
{
	if (!keystate[key]) 	OnKeyDown(key, x, y);
	keystate[key] = true;
}

void OnSpecialKeyPress(int key, int x, int y) 
{
	if (!keystate_special[key]) OnSpecialKeyDown(key, x, y);
	keystate_special[key] = true;
}

void OnKeyDown(unsigned char key, int x, int y) // Obsługa naszego własnego zdarzenia, gdy zostanie po raz pierwszy wciśnięty klawisz - zdarzenie odporne na repetycję.
{
	if (key == 27) glutLeaveMainLoop();
	if (key == 'h')	if(!authorflag)	helpflag = !helpflag;
	if (key == 'o')	if (!helpflag)	authorflag = !authorflag;
}

void OnSpecialKeyDown(int key, int x, int y) {}

void OnKeyUp(unsigned char key, int x, int y) 
{ 
	keystate[key] = false; 
	if (!keystate[119]) cameraflag = 0;
}

void OnSpecialKeyUp(int key, int x, int y) 
{ 
	keystate_special[key] = false; 
	if (!keystate_special[GLUT_KEY_UP])cameraflag = 0;
}

#pragma endregion

void OnReshape(int width, int height) 
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(43.0f, (float)width / height, 0.01f, 100.0f);
}