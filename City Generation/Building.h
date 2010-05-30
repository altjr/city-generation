

#ifndef _BUILDING_H_
#define _BUILDING_H_

#define WINDOW_OFFSET 1000000
#define SIDEWALK_OFFSET 2000000

#define STANDARD_MAX_HEIGHT 30
#define STANDARD_MIN_HEIGHT 4
#define STACKED_MAX_HEIGHT 100
#define STACKED_MIN_HEIGHT 60
#define MODERN_MAX_HEIGHT 64
#define MODERN_MIN_HEIGHT 32

#include <math.h>
#include "BuildingType.h"
#include "Texture.h"
#include "MyUtil.h"

class Building
{
private:
	Texture windows;
	Texture sidewalk;
	Mat4x4 trans;
	GLint id;
	int levels;
	int width, depth; // Of the base to work with
	void generate();
	void drawBox(const Vec3& min, const Vec3& max);
	void drawSidewalk();
	void generateStandard();
	void generateStacked();
	void generateState();
	void generateModern();
	void generateWindows(int width, int height);
	void generateSidewalk();

public: 
	BuildingType type;
	void draw() const;
	void increaseHeight();
	void decreaseHeight();

	inline Building(BuildingType t, int width, int depth, Mat4x4 position, GLuint ident);
};

void Building::draw() const
{
	glPushMatrix();
	glMultMatrix(trans);
	glCallList(id);
	glPopMatrix();
}

void Building::drawBox(const Vec3& min, const Vec3& max)
{
	glBegin(GL_QUAD_STRIP);
	glVertex(min);
	glVertex3f(min.x, min.y, max.z);
	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, min.y, max.z);
	glVertex3f(max.x, max.y, min.z);
	glVertex(max);
	glVertex3f(min.x, max.y, min.z);
	glVertex3f(min.x, max.y, max.z);
	glVertex(min);
	glVertex3f(min.x, min.y, max.z);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex(min);
	glVertex3f(max.x, min.y, min.z);
	glVertex3f(max.x, max.y, min.z);
	glVertex3f(min.x, max.y, min.z);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(min.x, min.y, max.z);
	glVertex3f(min.x, max.y, max.z);
	glVertex(max);
	glVertex3f(max.x, min.y, max.z);
	glEnd();
}

void Building::drawSidewalk()
{
	double w,d;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sidewalk.id);
	w = (width / 2.0) + 0.2;
	d = (depth / 2.0) + 0.2;
	glBegin(GL_POLYGON);
	glTexCoord2f(1.0, 1.0); glVertex3f(w, d, 0.01);
	glTexCoord2f(1.0, 0.0); glVertex3f(w, -d, 0.01);
	glTexCoord2f(0.0, 0.0); glVertex3f(-w, -d, 0.01);
	glTexCoord2f(0.0, 1.0);glVertex3f(-w, d, 0.01);
	glEnd();
}

void Building::generateStandard()
{
	double w = (width / 2) - ((rand() % (width / 2)) + 1) / 2;
	double d = (depth / 2) - ((rand() % (depth / 2)) + 1) / 2;
	generateWindows(w >= d ? w*2 : d*2, levels);

	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE );
	glNewList( id, GL_COMPILE );
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, windows.id);

	glColor(Color(0, 0, 0));

	float textureCoord = (w >= d? 1.0 : w/d);

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, -d, 0);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, -d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, -d, levels);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, -d, levels);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, d, levels);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, d, levels);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, d, 0);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, d, 0);
	glEnd();

	textureCoord = (d >= w? 1.0 : d/w);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);				glVertex3f(w, -d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(w, -d, levels);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, d, levels);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, d, 0);
	glEnd();
	
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, d, levels);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(-w, -d, levels);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(-w, -d, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	// Roof
	glBegin(GL_POLYGON);
	glVertex3f(w, -d, levels);
	glVertex3f(-w, -d, levels);
	glVertex3f(-w, d, levels);
	glVertex3f(w, d, levels);
	glEnd();

	drawSidewalk();

	glDisable(GL_TEXTURE_2D);

	glEndList();
	gluDeleteQuadric( qobj );
}

void Building::generateStacked()
{
	double w = (width / 2) - ((rand() % 3));
	double d = (depth / 2) - ((rand() % 3));

	//These tiers are to calculate the height of the various sections
	int base = ((rand() % 25) + 50);
	float tier1 = levels * (base / 100.0);

	base = 100 - base;
	float tier2Off = levels * ((base / 2)/100.0);
	float tier2 = tier1 + tier2Off;
	float tier3 = tier2 + tier2Off;

	generateWindows(w >= d ? w*2 : d*2, tier1);

	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE );
	glNewList( id, GL_COMPILE );
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, windows.id);

	glColor(Color(0, 0, 0));

	float textureCoord = (w >= d? 1.0 : w/d);

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, -d, 0);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, -d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, -d, tier1);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, -d, tier1);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, d, tier1);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, d, tier1);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, d, 0);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, d, 0);
	glEnd();

	textureCoord = (d >= w? 1.0 : d/w);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);				glVertex3f(w, -d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(w, -d, tier1);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(w, d, tier1);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(w, d, 0);
	glEnd();
	
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);				glVertex3f(-w, d, 0);
	glTexCoord2f(0.0, 1.0);				glVertex3f(-w, d, tier1);
	glTexCoord2f(textureCoord, 1.0);	glVertex3f(-w, -d, tier1);
	glTexCoord2f(textureCoord, 0.0);	glVertex3f(-w, -d, 0);
	glEnd();

	// Roof
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glVertex3f(w, -d, tier1);
	glVertex3f(-w, -d, tier1);
	glVertex3f(-w, d, tier1);
	glVertex3f(w, d, tier1);
	glEnd();

	drawBox(Vec3(-w-0.3, -d-0.3, tier1-0.5), Vec3(w+0.3, d+0.3, tier1+0.5));

	glEnable(GL_TEXTURE_2D);

	double oldW = w;
	double oldD = d;
	w -= 1;
	d -= 1;

	textureCoord = (w >= d? w/oldW : d/oldD);
	float textureHeight = tier2Off / tier1;

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(w, -d, tier1);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-w, -d, tier1);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(-w, -d, tier2);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(w, -d, tier2);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(w, d, tier2);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(-w, d, tier2);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-w, d, tier1);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(w, d, tier1);
	glEnd();

	textureCoord = (d >= w? d/oldD : w/oldW);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);						glVertex3f(w, -d, tier1);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(w, -d, tier2);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(w, d, tier2);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(w, d, tier1);
	glEnd();
	
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-w, d, tier1);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(-w, d, tier2);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(-w, -d, tier2);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(-w, -d, tier1);
	glEnd();
	
	// Roof
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glVertex3f(w, -d, tier2);
	glVertex3f(-w, -d, tier2);
	glVertex3f(-w, d, tier2);
	glVertex3f(w, d, tier2);
	glEnd();

	drawBox(Vec3(-w-0.3, -d-0.3, tier2-0.5), Vec3(w+0.3, d+0.3, tier2+0.5));

	glEnable(GL_TEXTURE_2D);

	w -= 1;
	d -= 1;

	textureCoord = (w >= d? w/oldW : d/oldD);

	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(w, -d, tier2);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(-w, -d, tier2);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-w, -d, tier3);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(w, -d, tier3);
	glEnd();

	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(w, d, tier3);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(-w, d, tier3);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(-w, d, tier2);
	glTexCoord2f(0.0, 0.0);						glVertex3f(w, d, tier2);
	glEnd();

	textureCoord = (d >= w? d/oldD : w/oldW);
	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(w, -d, tier2);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(w, -d, tier3);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(w, d, tier3);
	glTexCoord2f(0.0, 0.0);						glVertex3f(w, d, tier2);
	glEnd();
	
	glBegin(GL_POLYGON);
	glTexCoord2f(textureCoord, 0.0);			glVertex3f(-w, d, tier2);
	glTexCoord2f(textureCoord, textureHeight);	glVertex3f(-w, d, tier3);
	glTexCoord2f(0.0, textureHeight);			glVertex3f(-w, -d, tier3);
	glTexCoord2f(0.0, 0.0);						glVertex3f(-w, -d, tier2);
	glEnd();
	
	// Roof
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POLYGON);
	glVertex3f(w, -d, tier3);
	glVertex3f(-w, -d, tier3);
	glVertex3f(-w, d, tier3);
	glVertex3f(w, d, tier3);
	glEnd();

	drawBox(Vec3(-w-0.3, -d-0.3, tier3-0.5), Vec3(w+0.3, d+0.3, tier3+0.5));

	drawSidewalk();

	glDisable(GL_TEXTURE_2D);
	glEndList();
	gluDeleteQuadric( qobj );
}

void Building::generateState()
{
	//double w = (width / 2) - ((rand() % (width / 2)) + 1) / 2;
	//double d = (depth / 2) - ((rand() % (depth / 2)) + 1) / 2;
	//generateWindows(w >= d ? w*2 : d*2, levels);

	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE );
	glNewList( id, GL_COMPILE );
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, windows.id);

	glDisable(GL_TEXTURE_2D);
	glEndList();
	gluDeleteQuadric( qobj );
}

void Building::generateModern()
{
	double w = (width / 2) - rand()%4;
	double d = (depth / 2) - rand()%4;
	generateWindows(2, levels);

	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE );
	glNewList( id, GL_COMPILE );
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, windows.id);

	Vec3 lowOld = Vec3(0, 0, 0.0);
	Vec3 highOld = Vec3(0, 0, levels);
	float x, y;

	bool flip = true;
	for(int angle = 360; angle >= 0; angle -= 10)
	{
		x = -sinf((float)angle * DEGREES_TO_RADIANS) * w;
		y = cosf((float)angle * DEGREES_TO_RADIANS) * d;
		if(!(lowOld.x == 0 && lowOld.y == 0))
		{
			glBegin(GL_POLYGON);
			if(flip)
			{
				glTexCoord2f(1.0, 1.0);		glVertex3f(x, y, levels);
				glTexCoord2f(0.0, 1.0);		glVertex(highOld);
				glTexCoord2f(0.0, 0.0);		glVertex(lowOld);
				glTexCoord2f(1.0, 0.0);		glVertex3f(x, y, 0.0);
			}
			else
			{
				glTexCoord2f(1.0, 0.0);		glVertex3f(x, y, levels);
				glTexCoord2f(0.0, 0.0);		glVertex(highOld);
				glTexCoord2f(0.0, 1.0);		glVertex(lowOld);
				glTexCoord2f(1.0, 1.0);		glVertex3f(x, y, 0.0);
			}
			glEnd();

			flip = !flip;
		}
		lowOld.x = x;
		lowOld.y = y;
		highOld.x = x;
		highOld.y = y;

		glVertex3f(x, y, levels);
	}

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, levels);
	for(int angle = 360; angle >= 0; angle -= 10)
	{
		x = -sinf((float)angle * DEGREES_TO_RADIANS) * w;
		y = cosf((float)angle * DEGREES_TO_RADIANS) * d;
		glVertex3f(x, y, levels);
	}
	glEnd();

	drawSidewalk();
	glDisable(GL_TEXTURE_2D);
	glEndList();
	gluDeleteQuadric( qobj );
}

void Building::generateWindows(int width, int height)
{
	windows = Texture(id + WINDOW_OFFSET, WINDOWS, (width / 2) * WINDOW_WIDTH, (height / 2) * WINDOW_HEIGHT);
}

void Building::generateSidewalk()
{
	sidewalk = Texture(id + SIDEWALK_OFFSET, SIDEWALK, 64, 64);
}

void Building::generate()
{
	generateSidewalk();
	switch(type)
	{
	case STANDARD:
		levels = rand() % (STANDARD_MAX_HEIGHT - STANDARD_MIN_HEIGHT) + STANDARD_MIN_HEIGHT;
		generateStandard();
		break;
	case STACKED:
		levels = rand() % (STACKED_MAX_HEIGHT - STACKED_MIN_HEIGHT) + STACKED_MIN_HEIGHT;
		generateStacked();
		break;
	case STATE:
		levels = rand() % (STATE_MAX_HEIGHT - STATE_MIN_HEIGHT) + STATE_MIN_HEIGHT;
		generateState();
		break;
	case MODERN:
		levels = rand() % (MODERN_MAX_HEIGHT - MODERN_MIN_HEIGHT) + MODERN_MIN_HEIGHT;
		generateModern();
		break;
	}
}

inline Building::Building(BuildingType t, int w, int d, Mat4x4 position, GLuint ident)
{
	type = t;
	width = w;
	depth = d;
	trans = position;
	id = ident;

	generate();
}

#endif