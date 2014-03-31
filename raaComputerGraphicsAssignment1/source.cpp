#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <iostream>
#include <mmsystem.h>
#include <stdio.h>

#include "raaCamera/raaCamera.h"
#include "raaUtilities/raaUtilities.h"
#include "raaMaths/raaMaths.h"
#include "raaMaths/raaVector.h"
#include "raaMaths/raaMatrix.h"
#include "planet.h"
#include "solarSystem.h"

// Camera Variables
raaCameraInput g_Input;
raaCamera g_Camera;
unsigned long g_ulGrid=0;
int g_aiLastMouse[2];
int g_aiStartMouse[2];
bool g_bExplore=false;
bool g_bFly=false;

unsigned int g_uiLastTime=0;

// Simulation Variables
int g_iTimeStep = 0;
int g_iMaxTimeStep = 30;
int g_piInitialPlanets = 15;
bool g_bRunSimulation = true;
bool g_bMaintainSimulation = false;
extern const int cs_iMAX_TRAIL_POS;

// Selection Variables - code adapted from Lighthouse3D Picking Tutorial
#define RENDER 1
#define SELECT 2
#define BUFSIZE 1024
GLuint g_guiSelectionBuffer[BUFSIZE] = {0};
GLint g_guiHits;
int g_mode = RENDER;
int g_iCursorX;
int g_iCursorY;

// Solar System Variables
extern planet *g_pHead;
extern planet *g_pTail;
solarSystem *g_pSolarSystem;
planet *g_pSelectedPlanet = 0;

void gridInit();
void display();
void idle();
void reshape(int iWidth, int iHeight);

void keyboard(unsigned char c, int iXPos, int iYPos);
void keyboardUp(unsigned char c, int iXPos, int iYPos);
void sKeyboard(int iC, int iXPos, int iYPos);
void sKeyboardUp(int iC, int iXPos, int iYPos);
void mouse(int iKey, int iEvent, int iXPos, int iYPos);
void mouseMotion();

void startPickingSelection();
void stopPickingSelection();
void processHitRecord(GLuint uiHits, GLuint buffer[]);

void speedMenu(int speed);
void createMenus();
void mainMenu(int option);

void myInit();
void drawPlanets();
void drawTrailPositions();
void drawPlanet(planet *pPlanet);



void display()
{
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	camApply(g_Camera);
	gridDraw(g_ulGrid);
	glPushMatrix();

	if (g_mode == SELECT)
		startPickingSelection();

	drawPlanets();

	if (g_mode == SELECT)
		stopPickingSelection();

	drawTrailPositions();

	glPopMatrix();
	glFlush(); 
	glutSwapBuffers();
}
void idle()
{
	if (g_bRunSimulation) {
		// if the simulation is being maintained at the max no of planets
		if (g_bMaintainSimulation)
			maintainSolarSystem(g_pSolarSystem);

		 //Calculate the body forces for each planet
		for (planet *pBody = g_pHead; pBody;) {
			calculateBodyForcesForEach(pBody, g_pHead, g_pSolarSystem);
			pBody = pBody->m_pNext;
		}
		for (planet *pElement = g_pHead; pElement;) {
			if (g_iTimeStep > g_iMaxTimeStep) {
				if (!(pElement->m_fPosition[0] == pElement->m_fPosition[1] && pElement->m_fPosition[1] == pElement->m_fPosition[2] && pElement->m_fPosition[2] == 0)) {
					trailPosition *pPos = createTrailPosition(pElement->m_fPosition);				
					if (pElement->m_iNoTrailPos >= cs_iMAX_TRAIL_POS) {
						destroyTrailPos(pElement, popTrailTail(pElement));
					}
					pushTrailHead(pElement, pPos);
					g_iTimeStep = 0;
				}
			}
			g_iTimeStep++;		
			//Calculate the new velocity and position
			calculateAcceleration(pElement);
			calculateVelocityAndPosition(pElement, g_pSolarSystem);
			generateColourVertex(pElement);
			generatePositionVertex(pElement);

			planet *pNext = pElement->m_pNext;
			calculateCollisionsAndWithinBounds(pElement, g_pHead, g_pSolarSystem);
			pElement = pNext;
		}
	}
	mouseMotion();
	glutPostRedisplay();
}

void reshape(int iWidth, int iHeight)
{
	glViewport(0, 0, iWidth, iHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, ((float)iWidth)/((float)iHeight), 1000.1f, 1000000.0f);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void keyboard(unsigned char c, int iXPos, int iYPos)
{
	switch(c)
	{
	case 'w':
		camInputTravel(g_Input, tri_pos);
		break;
	case 's':
		camInputTravel(g_Input, tri_neg);
		break;
	case 'a':
		if (g_pSolarSystem->m_iCurrentPlanets < g_pSolarSystem->m_iMaxPlanets) {
			generateRandomPlanet();
			g_pSolarSystem->m_iCurrentPlanets++;
		}
		break;
	case 'p':
		g_bRunSimulation = !g_bRunSimulation;
		break;
	case '.':
		g_pSolarSystem->m_fTimeStep += 0.016666f;
		break;
	case ',':
		g_pSolarSystem->m_fTimeStep = g_pSolarSystem->m_fTimeStep / 2;
		break;
	case '/':
		g_pSolarSystem->m_fTimeStep = 0.016666f;
		break;
	case 'm':
		g_bMaintainSimulation = !g_bMaintainSimulation;
		break;
	case 127:
		if (g_pSelectedPlanet) {
			destroy(popMiddle(g_pSelectedPlanet));
			g_pSelectedPlanet = 0;
		}
	}
}

void keyboardUp(unsigned char c, int iXPos, int iYPos)
{
	switch(c)
	{

	case 'w':
	case 's':
		camInputTravel(g_Input, tri_null);
		break;
	case 'f':
		camInputFly(g_Input, !g_Input.m_bFly);
		break;
	}
}

void sKeyboard(int iC, int iXPos, int iYPos)
{
	switch(iC)
	{
	case GLUT_KEY_UP:
		camInputTravel(g_Input, tri_pos);
		break;
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_neg);
		break;
	}
}

void sKeyboardUp(int iC, int iXPos, int iYPos)
{
	switch(iC)
	{
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN:
		camInputTravel(g_Input, tri_null);
		break;
	}
}

void mouse(int iKey, int iEvent, int iXPos, int iYPos)
{
	if(iKey==GLUT_LEFT_BUTTON && iEvent==GLUT_DOWN)
	{
		camInputMouse(g_Input, true);
		camInputSetMouseStart(g_Input, iXPos, iYPos);
		g_iCursorX = iXPos;
		g_iCursorY = iYPos;
		g_mode = SELECT;
	}
	else if(iKey==GLUT_LEFT_BUTTON && iEvent==GLUT_UP)
	{
		camInputMouse(g_Input, false);
	}
}

void motion(int iXPos, int iYPos)
{
	if(g_Input.m_bMouse)
	{
		camInputSetMouseLast(g_Input, iXPos, iYPos);
	}
}

void mouseMotion()
{
	camProcessInput(g_Input, g_Camera);
	glutPostRedisplay();
}

void startPickingSelection() {
	GLint viewport[4];	
	glSelectBuffer(BUFSIZE, g_guiSelectionBuffer);

	glGetIntegerv(GL_VIEWPORT, viewport); 

	glRenderMode(GL_SELECT);
	glInitNames();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(g_iCursorX,viewport[3]-g_iCursorY, 3,3,viewport);
	gluPerspective(30.0f, ((float)viewport[2])/((float)viewport[3]), 1000.1f, 1000000.0f);
		
	glMatrixMode(GL_MODELVIEW);
}

void stopPickingSelection() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	g_guiHits = glRenderMode(GL_RENDER);
	if (g_guiHits != 0)
		processHitRecord(g_guiHits, g_guiSelectionBuffer);	
	g_mode = RENDER;
}

void processHitRecord(GLuint uiHits, GLuint buffer[]) {
	unsigned int i, j;
	GLuint names, *ptr, minZ,*ptrNames, numberOfNames;

	ptr = (GLuint *) buffer;
	minZ = 0xffffffff;
	for (i = 0; i < uiHits; i++) {	
		names = *ptr;
		ptr++;
		// if the pointer's Z is less than the minimum Z 
		if (*ptr < minZ) {
			// add to the names
			numberOfNames = names;
			minZ = *ptr;
			// increase the pointer to the hit name
			ptrNames = ptr+2;
		}	  
		ptr += names+2;
	}
	g_pSelectedPlanet = (planet*) *ptrNames;
	if (g_pSelectedPlanet == g_pSolarSystem->m_pSun)
		g_pSelectedPlanet = 0;
}

void drawTrailPositions() {
	for (planet *pElement = g_pHead; pElement; pElement = pElement->m_pNext) {
		glPushMatrix();
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		
		glDisable(GL_LIGHTING);
		glLineWidth(2.0f);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				
		glColorPointer(3, GL_FLOAT, 0, pElement->m_vfColourVertex);
		glVertexPointer(3, GL_FLOAT, 0, pElement->m_vfTrailVertex);
		
		glDrawArrays(GL_LINE_STRIP, 0, pElement->m_iNoTrailPos);

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_BLEND);
		glPopMatrix();		
	}
}


void drawPlanets() {		

	drawPlanet(g_pSolarSystem->m_pSun);
	glPopName();

	for (planet *pElement = g_pHead; pElement; pElement = pElement->m_pNext) {
		glPushMatrix();
		glTranslatef(pElement->m_fPosition[0], pElement->m_fPosition[1], pElement->m_fPosition[2]);
		drawPlanet(pElement);
		glPopMatrix();
	}
}

void drawPlanet(planet *pPlanet)
{
	int iSlices = 15;
	int iSegments = 20;
	float fRadius = pPlanet->m_fSize;
	float afSelectedPlanetColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	
	float *afPlanetColour = afSelectedPlanetColor;

	if (pPlanet != g_pSelectedPlanet)
		afPlanetColour = pPlanet->m_fColour;

	float afSphereSpecular[]={1.0f, 1.0f, 1.0f, 1.0f};
	float afSphereShininess=5.0f;

	glPushMatrix();	
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, afPlanetColour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afPlanetColour);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, afSphereSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, afSphereShininess);
	
	glPushName((intptr_t) pPlanet);
	glutSolidSphere(fRadius, iSlices, iSegments);	
	glPopName();

	glPopAttrib();
	glPopMatrix();
}

void createMenus() {
	int iSubmenuID = glutCreateMenu(speedMenu);
	glutAddMenuEntry("25%", 1);
	glutAddMenuEntry("50%", 2);
	glutAddMenuEntry("100%", 3);
	glutAddMenuEntry("200%", 4);
	glutAddMenuEntry("400%", 5);
	int iMenuID = glutCreateMenu(mainMenu);
	glutAddSubMenu("Change Speed", iSubmenuID);
	glutAddMenuEntry("Pause", 1);
	glutAddMenuEntry("Maintain System", 2);
	glutAddMenuEntry("Add Planet", 3);
	glutAddMenuEntry("Exit", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mainMenu(int option) {
	switch(option) {
	case 0: exit(0);
	case 1:			
		g_bRunSimulation = !g_bRunSimulation;
		break;
	case 2:
		g_bMaintainSimulation = !g_bMaintainSimulation;
		break;
	case 3:			
		if (g_pSolarSystem->m_iCurrentPlanets < g_pSolarSystem->m_iMaxPlanets) {
			generateRandomPlanet();
			g_pSolarSystem->m_iCurrentPlanets++;
		}
		break;
	}
}

void speedMenu(int speed) {
	switch(speed) {
	case 1:
		g_pSolarSystem->m_fTimeStep = 0.016666f / 4;
		break;
	case 2:			
		g_pSolarSystem->m_fTimeStep = 0.016666f / 2;
		break;
	case 3:
		g_pSolarSystem->m_fTimeStep = 0.016666f;
		break;
	case 4:
		g_pSolarSystem->m_fTimeStep = 0.016666f * 2;
		break;		
	case 5:
		g_pSolarSystem->m_fTimeStep = 0.016666f * 4;
		break;
	}
}


void myInit()
{
	initMaths();
	camInit(g_Camera);
	camInputInit(g_Input);
	camInputExplore(g_Input, true);

	float afGridColour[]={1.0f, 0.1f, 0.3f, 1.0f};
	//gridInit(g_ulGrid, afGridColour, -500, 500, 50.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	
	g_pSolarSystem = createDefaultSolarSystem();
	generateSun();

	generateRandomPlanets(g_piInitialPlanets);
	g_pSolarSystem->m_iCurrentPlanets += g_piInitialPlanets;

	createMenus();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv); 

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(512,512);
	glutCreateWindow("Rachael's Incredible Solar System");

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(sKeyboard);
	glutSpecialUpFunc(sKeyboardUp);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	return 0;
}
