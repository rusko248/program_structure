#define NOMINMAX
#define _USE_MATH_DEFINES

#include "Room.h"

#define FREE 0
#define TORCH 1
#define BOX 2
#define SPIKES 3
#define PIT 4

Room::Room() {
	dim[0] = dim[1] = dim[2] = 5;
	scale = 0.2f;
	
	initRoom();
}

Room::Room(int w, int h, int l, float s) {
	dim[0] = w;
	dim[1] = h;
	dim[2] = l;
	scale = s;

	initRoom();
}

void Room::initRoom() {
	floor = new Floor;
	floor->width = dim[0];
	floor->length = dim[2];
	floor->objPos.reserve(dim[0]*dim[2]);
	for (int i = 0; i < dim[0]*dim[2]; ++i)
		floor->objPos += '0';

	for (int i = 0; i < 4; ++i) {
		walls[i] = new Wall;
		if (i % 2 == 0) { walls[i]->base = dim[0]; walls[i]->height = dim[1]; }
		else { walls[i]->base = dim[2]; walls[i]->height = dim[1]; }
		walls[i]->objPos.reserve(walls[i]->base*walls[i]->height);
		for (int j = 0; j < walls[i]->base*walls[i]->height; ++j)
			walls[i]->objPos += '0';
	}

	pos = STPoint3(-(float)dim[0]/2.f,-(float)dim[1]/2.f,0.f);

	level = 1;
}

void Room::setLevel(int lv) {
	level = lv;
	generateObstacles();
}

void Room::render() {
	renderLayout();
	renderObstacles();
}

void Room::renderLayout() {
	glBegin(GL_QUADS);
		// floor
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y,pos.z-floor->length);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z-floor->length);

		// near wall
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+walls[0]->base,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+walls[0]->base,pos.y+walls[0]->height,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y+walls[0]->height,pos.z);

		// left wall
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z-walls[1]->base);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y+walls[1]->height,pos.z-walls[1]->base);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y+walls[1]->height,pos.z);

		// far wall
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y,pos.z-floor->length);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+walls[2]->base,pos.y,pos.z-floor->length);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+walls[2]->base,pos.y+walls[2]->height,pos.z-floor->length);
		glNormal3f(0,1,0);
		glVertex3f(pos.x,pos.y+walls[2]->height,pos.z-floor->length);

		// right wall
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y,pos.z);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y,pos.z-walls[3]->base);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y+walls[3]->height,pos.z-walls[3]->base);
		glNormal3f(0,1,0);
		glVertex3f(pos.x+floor->width,pos.y+walls[3]->height,pos.z);
	glEnd();
}

void Room::renderObstacles() {
	// floor
	for (unsigned int i = 0; i < floor->objPos.size(); ++i) {
		glPushMatrix();

		int v = i / floor->width;
		int u = i - v*floor->width;

		glTranslatef(pos.x+scale*((float)u+.5f), pos.y, pos.z-scale*((float)v+.5f));

		switch (floor->objPos[i]) {
		case FREE:
			break;
		case TORCH:
				{
					Torch torch = Torch();
					torch.render();
					break;
				}
		case BOX:
			{
				Box box = Box();
				box.render();
				break;
			}
		case SPIKES:
			{
				Spikes spikes = Spikes();
				spikes.render();
				break;
			}
		case PIT:
			break;
		}

		glPopMatrix();
	}

	// walls
	for (int j = 0; j < 4; ++j) {
		for (unsigned int i = 0; i < walls[j]->objPos.size(); ++i) {
			glPushMatrix();

			int v = i / walls[j]->base;
			int u = i - v*walls[j]->base;

			if (j == 0) glTranslatef(pos.x+scale*(float)floor->width-scale*((float)u+.5f), pos.y+scale*((float)v+.5f), pos.z);
			else if (j == 1) glTranslatef(pos.x, pos.y+scale*((float)v+.5f), pos.z-scale*((float)u+.5f));
			else if (j == 2) glTranslatef(pos.x+scale*((float)u+.5f), pos.y+scale*((float)v+.5f), pos.z-scale*(float)floor->length);
			else glTranslatef(pos.x+scale*(float)floor->width, pos.y+scale*((float)v+.5f), pos.z-scale*(float)floor->length+scale*((float)u+.5f));

			switch (walls[j]->objPos[i]) {
			case FREE:
				break;
			case TORCH:
				{
					Torch torch = Torch();
					torch.render();
					break;
				}
			case BOX:
				{
					Box box = Box();
					box.render();
					break;
				}
			case SPIKES:
				{
					Spikes spikes = Spikes();
					spikes.render();
					break;
				}
			case PIT:
				break;
			}

			glPopMatrix();
		}
	}
}

void Room::generateObstacles() {
	if (level == 1) {
		floor->objPos[floor->getIndex(floor->width/2, floor->length/2)] = BOX;
		walls[2]->objPos[walls[2]->getIndex(walls[2]->base/2, walls[2]->height/2)] = TORCH;
	}
}