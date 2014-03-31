

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