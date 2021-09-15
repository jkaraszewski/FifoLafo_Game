#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS

GLuint petroll;
GLuint plaayer;
GLuint cactus;
GLuint rock;
GLuint sign;
GLuint enemy1;
GLuint enemy2;
GLuint enemy3;

struct SFace 
{
	int v[3], n[3], t[3];
};

GLuint LoadObj(char * file) 
{
	FILE * fp = fopen(file, "r");

	if (fp == NULL) 
	{
		printf("ERROR: Cannot read model file \"%s\".\n", file);
		return -1;
	}

	std::vector<vec3> * v = new std::vector<vec3>();
	std::vector<vec3> * n = new std::vector<vec3>();
	std::vector<vec3> * t = new std::vector<vec3>();
	std::vector<SFace> * f = new std::vector<SFace>();

	char buf[128];

	while (fgets(buf, 128, fp) != NULL) 
	{
		if (buf[0] == 'v' && buf[1] == ' ') 
		{
			vec3 * vertex = new vec3();
			sscanf(buf, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
			v->push_back(*vertex);
		}
		if (buf[0] == 'v' && buf[1] == 't') 
		{
			vec3 * vertex = new vec3();
			sscanf(buf, "vt %f %f", &vertex->x, &vertex->y);
			t->push_back(*vertex);
		}
		if (buf[0] == 'v' && buf[1] == 'n')
		{
			vec3 * vertex = new vec3();
			sscanf(buf, "vn %f %f %f", &vertex->x, &vertex->y, &vertex->z);
			n->push_back(*vertex);
		}
		if (buf[0] == 'f' && buf[1] == ' ') 
		{
			SFace * face = new SFace();
			sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&face->v[0], &face->t[0], &face->n[0],
				&face->v[1], &face->t[1], &face->n[1],
				&face->v[2], &face->t[2], &face->n[2]
			);
			f->push_back(*face);
		}
	}
	fclose(fp);

	GLuint dlId;
	dlId = glGenLists(1);
	glNewList(dlId, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < f->size(); ++i)
	{
		for (int j = 0; j < 3; ++j) 
		{
			vec3 * cv = &(*v)[((*f)[i].v[j] - 1)];
			vec3 * ct = &(*t)[((*f)[i].t[j] - 1)];
			vec3 * cn = &(*n)[((*f)[i].n[j] - 1)];
			glTexCoord2f(ct->x, ct->y);
			glNormal3f(cn->x, cn->y, cn->z);
			glVertex3f(cv->x, cv->y, cv->z);
		}
	}
	glEnd();
	glEndList();

	delete v;
	delete n;
	delete t;
	delete f;

	return dlId;
}

void LoadModels()
{
	petroll = LoadObj("models/petrolmodel.obj");
	plaayer = LoadObj("models/PickUp.obj");
	enemy1 = LoadObj("models/bus2.obj");
	enemy2 = LoadObj("models/mosk.obj");
	enemy3 = LoadObj("models/ex2.obj");
	cactus = LoadObj("models/kaktus.obj");
	rock = LoadObj("models/rock.obj");
	sign = LoadObj("models/fiffo.obj");
}