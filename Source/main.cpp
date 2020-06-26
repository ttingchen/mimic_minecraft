#include "../Externals/Include/Include.h"
#include "../Externals/program.h"
#include "../Externals/Skybox.h"
#include "../Externals/Trackball.h"
#include "../Externals/Land.h"
#define MENU_TIMER_START 0
#define MENU_TIMER_STOP 1
#define MENU_EXIT 2

using namespace std;
using namespace glm;

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

mat4 proj_matrix, view_matrix;
int last_x, last_y;

Skybox* skybox;
Trackball* trackball;
Land* land;
vec3 lightpos = vec3( 0.0, 100.0, 150.0);
mat4 um4v_light, um4p_light;

bool wPress, aPress, sPress, dPress, zPress, xPress;

int window_width, window_height;
void My_Reshape(int width, int height);

static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};




void My_Init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//new program

	//init trackball
	trackball = new Trackball(0.001, 0.1);
	//init skybox
	skybox = new Skybox("../skybox.vs.glsl", "../skybox.fs.glsl");
	//init land
	land = new Land((unsigned int)glutGet(GLUT_ELAPSED_TIME));
}




// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static const GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat one = 1.0f;

	glClearBufferfv(GL_COLOR, 0, white);
	glClearBufferfv(GL_DEPTH, 0, &one);
	
	//render skybox
	skybox->render(proj_matrix, view_matrix);


	//render depth map
	mat4 um4vp_light = um4p_light * um4v_light;
	land->renderDepthMap(um4vp_light, window_width, window_height);

	land->setWindowSize(1440, 990);
	//render ssao  tex
	land->renderSsaoTex(view_matrix, proj_matrix);

	//testing
	land->renderTest(proj_matrix);

	//render land
	land->render(view_matrix, proj_matrix, trackball->camPos, 
					lightpos, um4vp_light);



	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	window_width = width;
	window_height = height;
	land->setWindowSize(window_width, window_height);
	glViewport(0, 0, width, height);
	float viewportAspect = (float)width / (float)height;
	proj_matrix = perspective(radians(60.0f), viewportAspect, 0.1f, 1000.0f);
	view_matrix = trackball->udpate(Tlook, 0, 0);

	float range = 150.0f;
	um4p_light = ortho(-range, range, -range, range, 0.0f, 500.0f);
	um4v_light = lookAt(lightpos, vec3(0.0f, 0.0f, 0.0f),
							vec3(0.0f, 1.0f, 0.0f));
}

void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if (timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}

	//keyboard trackball
	if (wPress) view_matrix = trackball->udpate(Tfront, 0, 0);
	else if (sPress) view_matrix = trackball->udpate(Tback, 0, 0);
	if (aPress) view_matrix = trackball->udpate(Tleft, 0, 0);
	else if (dPress) view_matrix = trackball->udpate(Tright, 0, 0);
	if (zPress) view_matrix = trackball->udpate(Tup, 0, 0);
	else if (xPress) view_matrix = trackball->udpate(Tdown, 0, 0);

}

void My_MouseMotion(int x, int y)
{
	int diff_x = x - last_x;
	int diff_y = last_y - y;

	//track ball
	view_matrix = trackball->udpate(Tlook, diff_x, diff_y);
}


void My_Mouse(int button, int state, int x, int y)
{
	last_x = x;
	last_y = y;

	if (state == GLUT_DOWN)
	{
	}
	else if (state == GLUT_UP)
	{
		//printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		wPress = true;
		break;
	case 'a':
		aPress = true;
		break;
	case 's':
		sPress = true;
		break;
	case 'd':
		dPress = true;
		break;
	case 'z':
		zPress = true;
		break;
	case 'x':
		xPress = true;
		break;
	case 'n':
		land->setNormalMap();
		break;
	case 'm':
		land->setShadow();
		break;
	case 'j':
		land->setSsao();
		break;
	default:
		break;

	}
}

void My_KeyboardUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		wPress = false;
		break;	 
	case 'a':	 
		aPress = false;
		break;	 
	case 's':	 
		sPress = false;
		break;	 
	case 'd':	 
		dPress = false;
		break;
	case 'z':
		zPress = false;
		break;
	case 'x':
		xPress = false;
		break;
	default:
		break;

	}
}


void My_SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch (id)
	{
	case MENU_TIMER_START:
		if (!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
#ifdef __APPLE__
	// Change working directory to source code path
	chdir(__FILEPATH__("/../Assets/"));
#endif
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
#ifdef _MSC_VER
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowPosition(100, 0);
	glutInitWindowSize(1440, 900);
	glutCreateWindow("Minecraft"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
#ifdef _MSC_VER
	glewInit();
#endif
	My_Init();

	// Create a menu and bind it to mouse right button.
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Register GLUT callback functions.
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutMotionFunc(My_MouseMotion);
	glutKeyboardFunc(My_Keyboard);
	glutKeyboardUpFunc(My_KeyboardUp);

	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0);

	// Enter main event loop.
	glutMainLoop();

	return 0;
}