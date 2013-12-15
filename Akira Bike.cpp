/******************************************************************************
 *    Student: Mathew Yamasaki
 *       Date: Sunday, August 19, 2012
 *      Class: Computer Graphics (CMSC 405)
 * Instructor: Dr. James Robertson
 * 
 * Class Description: Renders an image of the motorcycle featured in the movie,
 *					  Akira.
 *
 * Most of the windows management, error-handling, and rotation fucntions were 
 * created by Yan Wang (2008) and modified for use in this application.
 ******************************************************************************/

#include <iostream>
#include <GL/glut.h>
#include <math.h>
using namespace std;

#define GL_PI	3.14159265359f

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds main window handle
HWND		hWnd2=NULL;		// Holds menu window handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

GLUquadricObj *quadratic;	// Storage For Our Quadratic Objects ( NEW )

const float piover180 = 0.0174532925f;
float heading;
float xpos;
float zpos;
float zoom = 0;
float TWO_PI = GL_PI * 2;
float factor = TWO_PI / 100.0;
float x, y, z;
float HALF_PI = GL_PI/2;
GLfloat	yrot;				// Y Rotation
GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;
GLfloat lookupdown = 0.0f;
int		wire = 0;
int     perspective = 0;
bool	wp;					// Wireframe active? 
bool    pp, op, zp, zout = true;

GLsizei w_window, h_window;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	w_window = width;
	h_window = height;

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 30.0, 30.0, 0.5 };
	GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	quadratic=gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory) (NEW)
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals (NEW)
	return TRUE;										// Initialization Went OK
}

GLvoid glDrawTriangle(float x, float y, float z)
{
	glBegin(GL_QUADS);
		// bottom
		glVertex3f(x, -y, z);
		glVertex3f(-x, -y, z);
		glVertex3f(-x, -y, -z);
		glVertex3f(x, -y, -z);
		// right
		glVertex3f(x, y, -z);						
		glVertex3f(x, y, z);							
		glVertex3f(x, -y, z);							
		glVertex3f(x, -y, -z);							
		// left 
		glVertex3f(x, y, z);
		glVertex3f(x, y, -z);
		glVertex3f(-x, -y, -z);
		glVertex3f(-x, -y, z);
	glEnd();

	glBegin(GL_TRIANGLES);
		// rear
		glVertex3f(x, y, -z);
		glVertex3f(x, -y, -z);
		glVertex3f(-x, -y, -z);
		// face
		glVertex3f( x, y, z);
		glVertex3f(-x, -y, z);
		glVertex3f( x, -y, z);
	glEnd();
	
}

GLvoid glDrawCube(float x, float y, float z)
{
	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f( x, -y, -z);							// Top Right Of The Quad (Bottom)						
		glVertex3f(x, -y, z);							// Top Left Of The Quad (Bottom)
		glVertex3f(-x, -y, z);							// Bottom Left Of The Quad (Bottom)							
		glVertex3f(-x, -y, -z);							// Bottom Right Of The Quad (Bottom)

		glVertex3f( x, y, z);							// Top Right Of The Quad (Front)
		glVertex3f(-x, y, z);							// Top Left Of The Quad (Front)
		glVertex3f(-x, -y, z);							// Bottom Left Of The Quad (Front)
		glVertex3f( x, -y, z);							// Bottom Right Of The Quad (Front)
								
		glVertex3f(-x, y, -z);							// Top Right Of The Quad (Back)					
		glVertex3f(x, y, -z);							// Top Left Of The Quad (Back)						
		glVertex3f(x, -y, -z);							// Bottom Left Of The Quad (Back)						
		glVertex3f(-x, -y, -z);							// Bottom Right Of The Quad (Back)

		glVertex3f(-x, y, z);							// Top Right Of The Quad (Left)
		glVertex3f(-x, y,-z);							// Top Left Of The Quad (Left)
		glVertex3f(-x,-y,-z);							// Bottom Left Of The Quad (Left)
		glVertex3f(-x,-y, z);							// Bottom Right Of The Quad (Left)

		glVertex3f( x, y,-z);							// Top Right Of The Quad (Right)
		glVertex3f( x, y, z);							// Top Left Of The Quad (Right)
		glVertex3f( x,-y, z);							// Bottom Left Of The Quad (Right)
		glVertex3f( x,-y,-z);							// Bottom Right Of The Quad (Right)
	
		glVertex3f(x, y, -z);							// Top-right (Top)
		glVertex3f(-x, y, -z);							// Top-left (Top)
		glVertex3f(-x, y, z);							// Bottom-left (Top)
		glVertex3f(x, y, z);							// Bottom-right (Top)
	glEnd();											// Done Drawing The Quad
}

GLvoid gldrawTorus(float innerRadius, float outerRadius, int numInner, int numOuter)
{
	int		numMajor = numOuter;
	int		numMinor = numInner;
	float   majorRadius = outerRadius;
	float   minorRadius = innerRadius;
	double  majorStep   = 2.0f * GL_PI/numMajor;
	double  minorStep   = 2.0f * GL_PI/numMinor;
	int     i, j;

	for(i=0; i<numMajor; ++i)
	{
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);

		glBegin(GL_TRIANGLE_STRIP);
		for(j=0; j<= numMinor; ++j)
		{
			double  b = j * minorStep;
			GLfloat c = (GLfloat)cos(b);
			GLfloat r = minorRadius * c + majorRadius;
			GLfloat z = minorRadius * (GLfloat)sin(b);

			glNormal3f(x0*c, y0*c, z/minorRadius);
			glVertex3f(x0*r, y0*r, z);

			glNormal3f(x1*c, y1*c, z/minorRadius);
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}

 void drawHemisphere(int lats, int longs)  
{ 
    int i, j; double k;
    int halfLats = lats / 2;  
    for(i = 0; i <= halfLats; i++)  
    { 
        double lat0 = GL_PI * (-0.5 + (double) (i - 1) / lats); 
        double z0 = sin(lat0); 
        double zr0 = cos(lat0); 
 
        double lat1 = GL_PI * (-0.5 + (double) i / lats); 
        double z1 = sin(lat1); 
        double zr1 = cos(lat1); 
 
        glBegin(GL_QUAD_STRIP); 
        for(j = 0; j <= longs; j++) 
        { 
            double lng = 2 * GL_PI * (double) (j - 1) / longs; 
            double x = cos(lng); 
            double y = sin(lng); 
 
            // glTexCoordf() 
            glNormal3f(x * zr0, y * zr0, z0); 
            glVertex3f(x * zr0, y * zr0, z0);        
 
            // glTexCoordf() 
            glNormal3f(x * zr1, y * zr1, z1); 
            glVertex3f(x * zr1, y * zr1, z1);
        } 
        glEnd(); 
    } 
} 

GLvoid gldrawCurve(float Radius, int num, float length, float width)
{
	double  Step   = 2.0f * GL_PI/num;
	int     i, j;

	for(i=0; i < num; ++i)
	{
		double a0 = i * Step;
		if(a0 >= GL_PI/length)
			continue;
		double a1 = a0 + Step;
		GLfloat x0 = (GLfloat) cos(a0);
		GLfloat y0 = (GLfloat) sin(a0);
		GLfloat x1 = (GLfloat) cos(a1);
		GLfloat y1 = (GLfloat) sin(a1);

		glBegin(GL_TRIANGLE_STRIP);
		for(j=0; j<= num; ++j)
		{
			double  b = j * Step;
			if(b > GL_PI/width && b< (2*width-1)*GL_PI/width)
				continue;
			GLfloat c = (GLfloat)cos(b);
			GLfloat r = (Radius - 0.05f) * c + Radius;
			GLfloat z = (Radius - 0.05f) * (GLfloat)sin(b);

			glNormal3f(x0*c, y0*c, z/(Radius - 0.05f));
			glVertex3f(x0*r, y0*r, z);

			glNormal3f(x1*c, y1*c, z/ (Radius - 0.05f));
			glVertex3f(x1*r, y1*r, z);
		}
		glEnd();
	}
}

GLvoid gldrawCircle(float Radius)
{
	float x, y, angle;
	glBegin(GL_POLYGON);
	for(angle = 0.0; angle<= (2.0*GL_PI); angle +=0.1f)
	{
		x= Radius * (float)sin(angle);
		y= Radius * (float)cos(angle);
		glVertex2f(x, y);
	}
	glEnd();
}

/**********************************************************************************************/
/*                                   Perform all drawing                                       /
/**********************************************************************************************/
int DrawGLScene(GLvoid)									
{
	if(op || pp)
	{
		if(perspective == 0){
			glViewport(0,0,w_window,h_window);						// Reset The Current Viewport
			glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix
			glLoadIdentity();										// Reset The Projection Matrix
			glOrtho(-10.0f, (w_window-10.0f)/80, -5.0f, 
				(h_window-5.0f)/80, -60.0f,60.0f);					// Create Ortho 640x480 View (0,0 At Top Left)
			glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix
			glLoadIdentity();										// Reset The Modelview Matrix
		}
		else{
			ReSizeGLScene(w_window, h_window);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear Screen And Depth Buffer

    if(wire == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glLoadIdentity();												// Reset The Current Modelview Matrix

	GLfloat sceneroty = 360.0f - yrot;
	GLfloat xtrans = -xpos;
	GLfloat ztrans = -zpos;
	GLfloat ytrans = -walkbias-0.25f;

	glTranslatef(0.0f, 0.0f, -10.0f);								// Move Right 1.5 Units And Into The Screen 7.0
 
	glTranslatef(xtrans, ytrans, ztrans);
	glRotatef(lookupdown,1.0f,0,0);
	glRotatef(sceneroty,0,1.0f,0);

	glTranslatef(0.0f, 0.0f, zoom);

	GLfloat body_diffuse[] = { 0.7, 0.0, 0.0, 1.0 };				// red
	GLfloat tire_diffuse[] = {0.2, 0.2, 0.2, 1.0};					// gray
	GLfloat seat_diffuse[] = {0.0, 0.0, 0.0, 0.0};					// black
	GLfloat bolts_diffuse[] = {0.90, 0.91, 0.98, 0.0};				// silver


	/**********************************************************************************************/
	/*                                    front tire and wheel                                     /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(-5.5f, 0.0f, 0.0f);					
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tire_diffuse);
		gldrawTorus(0.25f, 1.0f, 48, 56);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(0.0, 0.0, 0.24);
		drawHemisphere(100, 100);
		glTranslatef(0.0, 0.0, -0.48);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		drawHemisphere(50, 50);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                     rear tire and wheel                                     /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(4.0f, 0.5f, 0.0f);				
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tire_diffuse);
		gldrawTorus(0.5f, 1.5f, 48, 56);
		glTranslatef(0.0, 0.0, -0.08);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		drawHemisphere(100, 100);
		glTranslatef(0.0, 0.0, -1.0);
		gluCylinder(quadratic,0.75f,1.0f,0.3f,32,32);
		gldrawCircle(0.75);
		glTranslatef(0.0, 0.0, -0.3);
		gluCylinder(quadratic,0.25f,0.65f,0.3f,32,32);
		gldrawCircle(0.25);
	glPopMatrix();

	// left-side wheel
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(4.0f, 0.5f, 0.08f);
		glRotatef(180.0, 0.0, 1.0, 0.0);
		drawHemisphere(50, 50);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                 Left-side engine covers                                     /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(1.55, 0.0, 0.75);
		gluCylinder(quadratic,0.45f,0.25f,0.3f,32,32);
		glTranslatef(0.0, 0.0, 0.3);
		gldrawCircle(0.25);
		glTranslatef(-0.75, -0.2, -0.3);
		gluCylinder(quadratic,0.25f,0.15f,0.3f,32,32);
		glTranslatef(0.0, 0.0, 0.3);
		gldrawCircle(0.15);
	glPopMatrix();

	/**********************************************************************************************/
	/*                              Right-side transmission cover                                  /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(1.5, 0.0, -1.05);
		gluCylinder(quadratic,0.25f,0.55f,0.5f,32,32);
		glTranslatef(0.0, 0.0, 0.0);
		gldrawCircle(0.25);								// end cap
		glTranslatef(-0.35, -0.15, 0.0);
		gluCylinder(quadratic,0.15f,0.25f,0.5f,32,32);	
		gldrawCircle(0.15);								// end cap
	glPopMatrix();

	/**********************************************************************************************/
	/*                                   Drive shaft cover                                         /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(2.75, 0.45, -0.935);
		glRotatef(5.0, 0.0, 0.0, 1.0);
		glDrawTriangle(1.0, 0.55, 0.05);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2.45, 0.055, -0.935);
		glRotatef(5.0, 0.0, 0.0, 1.0);
		glDrawCube(1.0, 0.25, 0.05);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                        rear fender                                          /
	/**********************************************************************************************/

	glPushMatrix();
		glTranslatef(3.5f, 0.6f, 0.0f);		
		glRotatef(90.0, 0.0, 0.0, 1.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		gldrawCurve(1.0f, 32, 1.5f, 2.5f); // 0.9, 32, 1.5, 1.0 
	glPopMatrix();

	/**********************************************************************************************/
	/*                                        Handle bars                                          /
	/**********************************************************************************************/
	// left handle bar riser
	glPushMatrix();
	glTranslatef(-3.3f, 0.65f, 0.5f);						
	glRotatef(-75, 0.0f, 0.0f, 1.0f);					
	glRotatef(270, 1.0f, 0.0f, 0.0f);					
	glColor3f(0.2f,0.2f,0.2f);							
	gluCylinder(quadratic,0.1f,0.1f,0.8f,32,32);		
	// y   z   x
	glTranslatef(0.0, 1.0, 0.8);
	gldrawCircle(0.08f);
	glPopMatrix();
	
	// right handle bar riser
	glPushMatrix();
		glTranslatef(-3.3f, 0.65f, -0.5f);						
		glRotatef(-75.0f, 0.0f, 0.0f, 1.0f);					
		glRotatef(270, 1.0f, 0.0f, 0.0f);					
		gluCylinder(quadratic,0.1f,0.1f,0.8f,32,32);
		glTranslatef(0.0, -1.0, 0.8);
		gldrawCircle(0.08f);
	glPopMatrix();
	
	// left handle bar grip
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tire_diffuse);
		glTranslatef(-2.9f, 0.3f, 1.1f);						
		glRotatef(-25, 0.0f, 0.0f, 1.0f);				
		glTranslatef(0.0f, 0.57f, 0.1f);						
		glRotatef(180, 0.0f, 1.0f, 0.0f);											
		gluCylinder(quadratic,0.1f,0.1f,0.6f,32,32);		
		gldrawCircle(0.09f);
	glPopMatrix();
	
	// right handle bar grip
	glPushMatrix();
		glTranslatef(-2.9f, 0.3f, -0.7f);						
		glRotatef(-25, 0.0f, 0.0f, 1.0f);				
		glTranslatef(0.0f, 0.57f, 0.1f);						
		glRotatef(180, 0.0f, 1.0f, 0.0f);											
		gluCylinder(quadratic,0.1f,0.1f,0.6f,32,32);		
		glTranslatef(0.0f, 0.0f, 0.6f);
		gldrawCircle(0.09f);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                         Headlights                                          /
	/**********************************************************************************************/
	
	GLfloat position[] = { 0.0, 0.0, 1.5, 1.0 };

	// right side
	glPushMatrix();
		// top
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(-6.05, 0.25, -0.5);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		gluCylinder(quadratic,0.05f,0.1f,0.3f,32,32);
		// bottom
		glTranslatef(0.0, -0.5, 0.0);
		gluCylinder(quadratic,0.05f,0.1f,0.3f,32,32);
		// bottom lens
		glTranslatef(0.0f, 0.0f, 0.275f);		
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		gldrawCircle(0.08f);
		// top lens
		glTranslatef(0.0, 0.5, 0.0);
		gldrawCircle(0.08f);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	// left side
	glPushMatrix();
		// top
		glTranslatef(-6.05, 0.25, 0.5);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		gluCylinder(quadratic,0.05f,0.1f,0.3f,32,32);
		// bottom
		glTranslatef(0.0, -0.5, 0.0);
		gluCylinder(quadratic,0.05f,0.1f,0.3f,32,32);
		// bottom lens
		glTranslatef(0.0f, 0.0f, 0.275f);		
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 0.0);
		gldrawCircle(0.08f);
		// top lens
		glTranslatef(0.0, 0.5, 0.0);
		gldrawCircle(0.08f);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                        Lower frame                                          /
	/**********************************************************************************************/
	
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(-0.75f, -0.5f, 0.0f);					
		glRotatef(180.0f,1.0f,0.0f,0.0f);						
		glDrawCube(3.0f, 0.05f, 0.5f);
	glPopMatrix();
	
	/**********************************************************************************************/
	/*                                          Headrest                                           /
	/**********************************************************************************************/
	 
	glPushMatrix();
		glTranslatef(2.23, 2.375, 0.0);
		glRotatef(90.0, 0.0, 1.0, -0.06);
		glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
		gldrawCircle(0.69);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, seat_diffuse);
		glTranslatef(0.0, 0.0, -0.25);
		gluCylinder(quadratic,0.35f,0.65f,0.3f,32,32);
		gldrawCircle(0.35);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                            Seat                                             /
	/**********************************************************************************************/
	 
	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(1.15, 0.5, 0.0);
		glDrawTriangle(1.0, 1.0, 0.75);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(1.0, 1.15, 0.0);
		glRotatef(30.0, 0.0, 0.0, 1.0);
		gldrawCurve(0.75f, 32, 3.25, 1.0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, seat_diffuse);
		glTranslatef(-0.25, 0.05, 0.0);
		glRotatef(210.0, 0.0, 0.0, 1.0);
		glDrawCube(0.45, 0.15, 0.75);			
	glPopMatrix();

	// lower backrest
	glPushMatrix();
		glTranslatef(1.1, 0.5, 0.0);
		glRotatef(225.0f, 0.0f, 0.0f, 1.0f);
		glDrawCube(0.75, 0.15, 0.75);
	glPopMatrix();

	// seat bottom
	glPushMatrix();
		glTranslatef(-0.35, -0.05, 0.0);
		glRotatef(180.0, 0.0, 0.0, 1.0);
		glDrawCube(0.75, 0.15, 0.75);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                  Front frame and console                                    /
	/**********************************************************************************************/

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(-0.25, -0.2, 0.0);
		glDrawCube(1.0, 0.25, 0.25);
		glTranslatef(-1.75, 0.5, 0.0);
		glRotatef(180.0, 0.0, 1.0, 0.0);
		glDrawTriangle(2.0, 0.75, 0.25);
	glPopMatrix();

	/**********************************************************************************************/
	/*                                           Front-end                                         /
	/**********************************************************************************************/
	
	glPushMatrix();
		// center
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(-4.0, 0.5, 0.0);
		glRotatef(15.0, 0.0, 0.0, 1.0);
		glDrawCube(1.0, 0.25, 0.25);

		// left
		glTranslatef(0.0, 0.0, 0.5);
		glRotatef(90.0, 1.0, 0.0, 0.0); 
		glDrawTriangle(1.0, 0.25, 0.25);

		// right
		glTranslatef(0.0, -0.99, 0.0);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		glDrawTriangle(1.0, 0.25, 0.25);
	glPopMatrix();

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
		glTranslatef(-3.5, 1.05, 0.0);
		glRotatef(195.0, 0.0, 0.0, 1.0);
		glDrawCube(1.0, 0.15, 0.25);             

		glTranslatef(0.0, -2.0, 0.0);
		glRotatef(180.0, 0.0, 1.0, 1.0);
		glTranslatef(0.0, 0.6, 2.0);
		glDrawTriangle(1.425, 0.35, 0.15);

		glTranslatef(0.0, -1.2, 0.0);
		glRotatef(180.0, 1.0, 0.0, 0.0);
		glDrawTriangle(1.425, 0.35, 0.15);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-2.25, 1.75, 0.0);
		glRotatef(-90.0, 0.0, 1.0, 0.0);
		glRotatef(25.0, 1.0, 0.0, 0.0);
		gluCylinder(quadratic,0.5f,0.25f,2.35f,32,32);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, seat_diffuse);
		gldrawCircle(0.5);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, bolts_diffuse);
		gldrawCircle(0.45);
	glPopMatrix();
	return TRUE;										// Keep Going
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	gluDeleteQuadric(quadratic);						// Delete The Quadratic To Free System Resources

	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/**********************************************************************************************/
/*                           Windows procedure event handler                                   *
/   title			- Title To Appear At The Top Of The Window				                   *
/	width			- Width Of The GL Window Or Fullscreen Mode				                   *
/	height			- Height Of The GL Window Or Fullscreen Mode			                   *
/	bits			- Number Of Bits To Use For Color (8/16/24/32)			                   *
/	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	                   *
/**********************************************************************************************/
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","OpenGL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								200, 200,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL))								// Dont Pass Anything To WM_CREATE
								
								||

								!(hWnd2=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								200, 200,							// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)) || !(hDC=GetDC(hWnd2)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window 
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	ShowWindow(hWnd2,SW_SHOW);
	//SetForegroundWindow(hWnd2);
	//SetFocus(hWnd2);

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

/**********************************************************************************************/
/*                           Windows procedure event handler                                   /
/**********************************************************************************************/
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{

	PAINTSTRUCT paintStruct;
	/*	Device Context*/
	HDC hDC; 
	/*	Text for display*/
	char string1[] = "Usage Instructions";
	char string2[] = "'f' : Move bike toward";
	char string3[] = "'b' : Move bike away";
	char string4[] = "'w' : Toggle wire-frame";
	char string5[] = "'o' : Snap to far-perspective";
	char string6[] = "'p' : Snap to close-perspective";
	char string7[] = "'z' : Rotate around a center-point";
	char string8[] = "'c' : Return to default position";
	char string9[] = "Up Arrow: Rotate clockwise about x-axis";
	char string10[] = "Down Arrow: Rotate counter-clockwise about x-axis";
	char string11[] = "Left Arrow: Rotate clockwise about y-axis";
	char string12[] = "Right Arrow: Rotate counter-clockwise about y-axis";
	
	/*	Switch message, condition that is met will execute*/
	switch(uMsg)
	{
		/*	Window is being created*/
		case WM_CREATE: 
			return 0;
			break;
		/*	Window is closing*/
		case WM_CLOSE: 
			PostQuitMessage(0);
			return 0;
			break;
		/*	Window needs update*/
		case WM_PAINT: 
			hDC = BeginPaint(hWnd,&paintStruct);
			/*	Set txt color to blue*/
			SetTextColor(hDC, COLORREF(0x00FF0000));
			/*	Display text in middle of window*/
			TextOut(hDC,120,10,string1,sizeof(string1)-1);
			TextOut(hDC,120,40,string2,sizeof(string2)-1);
			TextOut(hDC,120,70,string3,sizeof(string3)-1);
			TextOut(hDC,120,100,string4,sizeof(string4)-1);
			TextOut(hDC,120,130,string5,sizeof(string5)-1);
			TextOut(hDC,120,160,string6,sizeof(string6)-1);
			TextOut(hDC,120,190,string7,sizeof(string7)-1);
			TextOut(hDC,120,220,string8,sizeof(string8)-1);
			TextOut(hDC,65,250,string9,sizeof(string9)-1);
			TextOut(hDC,40,280,string10,sizeof(string10)-1);
			TextOut(hDC,55,310,string11,sizeof(string11)-1);
			TextOut(hDC,45,340,string12,sizeof(string12)-1);
			EndPaint(hWnd, &paintStruct);
			return 0;
			break;
		default:
			break;
	}

	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if ((LOWORD(wParam) != WA_INACTIVE) && !((BOOL)HIWORD(wParam)))
				active=TRUE;						// Program Is Active
			else
				active=FALSE;						// Program Is No Longer Active

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

/**********************************************************************************************/
/*                                       Main function                                         /
/**********************************************************************************************/
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	WNDCLASSEX  windowClass;		//window class
	HWND		hwnd;				//window handle
	MSG			msg;				//message
	bool		done=FALSE;			//flag saying when app is complete

	/*	Fill out the window class structure*/
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "MyClass";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	
	/*	Register window class*/
	if (!RegisterClassEx(&windowClass))
	{
		return 0;
	}

	/*	Class registered, so now create window*/
	hwnd = CreateWindowEx(NULL,			//extended style
		"MyClass",						//class name
		"Key Commands",					//app name
		WS_OVERLAPPEDWINDOW |			//window style
		WS_VISIBLE |
		WS_SYSMENU,
		1420,200,						//x/y coords
		500,500,						//width,height
		NULL,							//handle to parent
		NULL,							//handle to menu
		hInstance,						//application instance
		NULL);							//no extra parameter's

	/*	Check if window creation failed */
	if (!hwnd)
		return 0;
	done = false; //initialize loop condition variable

	/* Ask user for preferred screen mode */
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "FullScreen Option",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}
	
	/* Create OpenGL window */
	if (!CreateGLWindow("Akira Bike",1200,600,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			/* Draw scene */
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}

			else									// Not Time To Quit, Update Screen
			{
		
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Akira Bike",1200,600,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
			if (keys[VK_UP])
			{
				lookupdown+= 1.0f;
			}

			if (keys[VK_DOWN])
			{
				lookupdown-= 1.0f;
			}
			if (keys[VK_RIGHT])
			{
				heading += 1.0f;
				yrot = heading;
			}

			if (keys[VK_LEFT])
			{
				heading -= 1.0f;	
				yrot = heading;
			}
			if (keys['F'])
			{

				xpos -= (float)sin(heading*piover180) * 0.05f;
				zpos -= (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle >= 359.0f)
				{
					walkbiasangle = 0.0f;
				}
				else
				{
					walkbiasangle+= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if (keys['B'])
			{
				xpos += (float)sin(heading*piover180) * 0.05f;
				zpos += (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle <= 1.0f)
				{
					walkbiasangle = 359.0f;
				}
				else
				{
					walkbiasangle-= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if (keys['Z'] && !zp)
			{
				zp = true;
				if(zout)
				{
					zoom += 5.0f;
					zout = false;
				}
				else
				{
					zoom -= 5.0f;
					zout = true;
				}
			}
			if(!keys['Z'])
				zp = false;

			if (keys[VK_NEXT])
			{

				xpos -= (float)sin(heading*piover180) * 0.05f;
				zpos -= (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle >= 359.0f)
				{
					walkbiasangle = 0.0f;
				}
				else
				{
					walkbiasangle+= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if (keys[VK_PRIOR])
			{
				xpos += (float)sin(heading*piover180) * 0.05f;
				zpos += (float)cos(heading*piover180) * 0.05f;
				if (walkbiasangle <= 1.0f)
				{
					walkbiasangle = 359.0f;
				}
				else
				{
					walkbiasangle-= 10;
				}
				walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
			}

			if(keys['C'])
			{
				heading = 0;
				xpos = 0;
				zpos = 0;
				zoom = 0;
				yrot = 0;				// Y Rotation
				walkbias = 0;
				walkbiasangle = 0;
				lookupdown = 0.0f;
				wire = 0;
				perspective = 1;
				wp = false;
				pp = false;
				op = false;
				zp = false;
				zout = true;
				DrawGLScene();
				
			}
			if(keys['W'] && !wp)
			{
				wp = true;
				if(wire == 0)
					wire = 1;
				else
					wire = 0;
			}
			if(!keys['W'])
			{
				wp = false;
			}
			if(keys['P'] && !pp)
			{
				pp = true;
				perspective = 1;
			}
			if(!keys['P'])
				pp = false;
			if(keys['O'] && !op)
			{
				op = true;
				perspective = 0;
			}
			if(!keys['O'])
				op = false;
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
} 