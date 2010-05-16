

#ifndef _BUILDING_H_
#define _BUILDING_H_

#include "BuildingType.h"
#include "MyUtil.h"

class Building
{
private:
	Mat4x4 trans;
	GLint id;
	int levels;
	int width, depth; // Of the base to work with
	void generate();
	void generateStandard();

public: 
	BuildingType type;
	void draw() const;
	void increaseHeight();
	void decreaseHeight();

	inline Building(BuildingType t, int width, int depth, Mat4x4 position, GLint ident);
	inline Building();
};

void Building::draw() const
{
	glPushMatrix();
	glMultMatrix(trans);
	glCallList(id);
	glPopMatrix();
}

void Building::generateStandard()
{
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE );
	glNewList( id, GL_COMPILE );
	glEnable( GL_NORMALIZE );
	
	int w = (width / 2) + ((rand() % (width / 2)) + 1) / 2;
	int d = (depth / 2) + ((rand() % (depth / 2)) + 1) / 2;

	glBegin(GL_QUAD_STRIP);
	glVertex3f(w, -d, 0);
	glVertex3f(-w, -d, 0);
	glVertex3f(w, -d, levels);
	glVertex3f(-w, -d, levels);
	glVertex3f(w, d, levels);
	glVertex3f(-w, d, levels);
	glVertex3f(w, d, 0);
	glVertex3f(-w, d, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(w, -d, 0);
	glVertex3f(w, -d, levels);
	glVertex3f(w, d, levels);
	glVertex3f(w, d, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(-w, d, 0);
	glVertex3f(-w, d, levels);
	glVertex3f(-w, -d, levels);
	glVertex3f(-w, -d, 0);
	glEnd();

	glEndList();
	gluDeleteQuadric( qobj );
}

void Building::generate()
{
	switch(type)
	{
	case STANDARD:
		levels = rand() % 40 + 6;
		generateStandard();
		break;
	}
}

inline Building::Building(BuildingType t, int w, int d, Mat4x4 position, GLint ident)
{
	type = t;
	width = w;
	depth = d;
	trans = position;
	id = ident;

	generate();
}

inline Building::Building()
{
	type = NONE;
}

#endif