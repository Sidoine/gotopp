/*

Module chaussettes pour le GOTO++ (accès au réseau)
Copyright 2002 Sidoine de Wispelaere

Ce programme est un logiciel libre ; vous pouvez le redistribuer
et/ou le modifier sous les termes de la GNU General Public Licence
telle que publiée par la Free Software Fundation ; soit dans la
version 2 de la Licence, soit (à votre choix) dans une version
ultérieure.

Ce programme est distribué dans l'espoir qu'il sera utile mais
SANS AUCUNE GARANTIE ; sans même la garantie implicite qu'il soit
COMMERCIALISABLE ou ADEQUAT POUR UN USAGE PARTICULIER. Voir la
GNU General Public Licence pour plus de détails.

Vous devriez avoir reçu une copie de la GNU General Public Licence
avec ce programme ; dans le cas contraire, écrivez à :
Free Software Fundation, Inc.
59 Temple Place - Suite 330, Boston, MA  02111-1307, US

                          ****
					 
Fonctions pour utiliser GLUT.
*/

#include <GL/glut.h> 
#include "../include/gppmodule.h"
#define VERSIONMIN 0.23f

#include <iostream>
using namespace std;
using namespace GotoPP;

class CGLUT:public CIntModule
{
public:
	void MainLoop();
	void FonctionSpeciale();
};

extern "C" {
EXPORT bool Init(CCompModule * Int)
{
	if (Int->Version<VERSIONMIN)
		return false;

	Int->NouvelleFonction("Onyva",voidf(CGLUT::MainLoop));
	Int->NouvelleFonction("FonctionSpeciale",voidf(CGLUT::FonctionSpeciale));
	return true;
}

}

GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
CIntModule * IntGL=NULL;
char* etFonctionSpeciale=NULL;
GLuint text[1];

struct Pixel
{
	unsigned char r, v, b, a;
};

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	/*glRotatef(view_rotx, 1.0, 0.0, 0.0);
	glRotatef(view_roty, 0.0, 1.0, 0.0);
	glRotatef(view_rotz, 0.0, 0.0, 1.0);*/

			
	static GLfloat red[4] =
	  {0.8, 0.1, 0.0, 1.0};
	//glShadeModel(GL_SMOOTH);
	glShadeModel(GL_FLAT);
	//glutSolidSphere(1.0f,10,10);
	glBindTexture(GL_TEXTURE_2D,text[0]);
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(0,0);
		glTexCoord2f(1,0);
		glVertex2f(100,0);	
		glTexCoord2f(1,1);		
		glVertex2f(100,100);
		glTexCoord2f(0,1);		
		glVertex2f(0,100);
	glEnd();
	glPopMatrix();
	glutSwapBuffers();
}

void idle(void)
{
/*  angle += 2.0;
  glutPostRedisplay();*/
}

void visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

void key(unsigned char k, int x, int y)
{
  switch (k) {
  case 'z':
    view_rotz += 5.0;
    break;
  case 'Z':
    view_rotz -= 5.0;
    break;
  case 27:  /* Escape */
    exit(0);
    break;
  default:
    return;
  }
  glutPostRedisplay();
}

void special(int k, int x, int y)
{
	if (etFonctionSpeciale!=NULL)
	{
		IntGL->DebutParametres();
		IntGL->RetournerEntier(y);
		IntGL->RetournerEntier(x);
		IntGL->RetournerEntier(k);
		IntGL->AppelerFonction(etFonctionSpeciale);
		IntGL->LireEntier();
	}
	
	switch (k) {
	case GLUT_KEY_UP:
	view_rotx += 5.0;
	break;
	case GLUT_KEY_DOWN:
	view_rotx -= 5.0;
	break;
	case GLUT_KEY_LEFT:
	view_roty += 5.0;
	break;
	case GLUT_KEY_RIGHT:
	view_roty -= 5.0;
	break;
	default:
	return;
	}
	glutPostRedisplay();
}

void reshape(int width, int height)
{
	GLfloat h = (GLfloat) height / (GLfloat) width;

	glViewport(0, 0, (GLint) width, (GLint) height);
	glMatrixMode(GL_PROJECTION);
/*	glLoadIdentity();
	glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, 0, 1); 

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glTranslatef(0.0, 0.0, -40.0);
}

void CGLUT::MainLoop()
{
	Fin();
	if (IntGL)
	{
		Exception("on ne peut lancer MainLoop qu'une fois");
		return;
	}
	IntGL=this;
	char *fake[1];
	fake[0]="gpp";
	int argc=1;
	glutInit(&argc, fake);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("GOTO++");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutSpecialFunc(special);
	glutVisibilityFunc(visible);

//	glEnable ( GL_COLOR_MATERIAL );
//	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
//	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glEnable(GL_TEXTURE_2D);
	unsigned char Image[64][64][4];
	for (int y=0; y<64; y++)
		for (int x=0; x<64; x++)
		{
			Image[y][x][0]=x*4;
			Image[y][x][1]=y*4;
			Image[y][x][2]=(x+y)*2;
			Image[y][x][3]=(x+y)*2;
		}
	

	glGenTextures(1,text);
	clog <<"Texture : "<<text[0]<<endl;
	glBindTexture(GL_TEXTURE_2D,text[0]);
	if (glGetError()!=GL_NO_ERROR)
		clog <<"erreur bindtexture\n";	
	gluBuild2DMipmaps(GL_TEXTURE_2D,
			GL_RGBA,64,64,
			GL_RGBA,GL_UNSIGNED_BYTE,
			Image); //gluBuild2DMipmaps
	if (glGetError()!=GL_NO_ERROR)
		clog <<"erreur teximage2D\n";

/*	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);*/
	glutMainLoop();
}

void CGLUT::FonctionSpeciale()
{
	//etFonctionSpeciale=LireFonction();
	Fin();
}
