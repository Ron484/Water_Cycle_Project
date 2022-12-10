#include <glew.h>  // A cross-platform open-source C/C++ extension loading library
#include <stdlib.h>
#include <glut.h>   // handle the window-managing operations
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <corecrt_math_defines.h>
#include <playsoundapi.h> // for sound

using namespace std;

// window size
GLint win_width = 900, win_hight = 900;

// camera movement
GLfloat cameraPosition[] = { -1.5f, 1.5f, 2.0f };

//for text
string a;
float tex = 0;

// for animation
bool isSnow = false; // to change the land to snowy
GLfloat smove = -0.9;// evaporate steam 
GLfloat goLeft = 0, goRight = 0; // for cloud movements
GLfloat snowTransparent = 0.2; // for snow appearing
GLfloat keyFrameTimes[] = { 0, 17, 23, 35, 45, 55 }; // t starting values of keyframes 
GLfloat t = 0; // the timer

// texture buffers
GLuint myTexture1;
GLuint myTexture2;
GLuint myTexture3;
GLuint myTexture4;
GLuint myTexture5;
GLuint myTexture6;
GLuint myTexture7;
GLuint myTexture8;
GLuint myTexture9;
GLuint myTexture10;
GLuint myTexture11;
GLuint treeTexture;
GLuint landSurfaceTexture;
GLuint landSideTexture;


// the coordinates of the drops and snows
const int NUMBER_OF_DROPS = 50;
GLfloat dropsCoord[NUMBER_OF_DROPS][3] = {
    { -0.14, 0.84, -0.33 }, { -0.23, 0.48, -0.25 },
    {  0.06, 0.23, -0.27 }, {  0.23, 0.50, -0.25 },
    {  0.06, 0.23, -0.27 }, { -0.23, 0.68, -0.25 },
    {  0.24, 0.42, -0.35 }, {  0.06, 0.75, -0.32 },
    {  0.31, 0.78, -0.31 }, {  0.30, 0.65, -0.25 },
    {  0.35, 0.60, -0.32 }, {  0.31, 0.78, -0.21 },
    {  0.08, 0.49, -0.25 }, {  0.27, 0.52, -0.37 },
    { -0.09, 0.43, -0.31 }, {  0.31, 0.25, -0.27 },
    { -0.10, 0.77, -0.25 }, { -0.16, 0.86, -0.19 },
    { -0.21, 0.25, -0.34 }, { -0.27, 0.41, -0.22 },
    {  0.09, 0.56, -0.23 }, {  0.03, 0.72, -0.38 },
    { -0.22, 0.49, -0.28 }, { -0.05, 0.44, -0.35 },
    {  0.18, 0.78, -0.36 }, { -0.19, 0.34, -0.30 },
    { -0.30, 0.21, -0.27 }, { -0.14, 0.64, -0.20 },
    { -0.04, 0.90, -0.22 }, {  0.19, 0.60, -0.38 },
    {  0.08, 0.61, -0.30 }, {  0.31, 0.45, -0.29 },
    {  0.23, 0.29, -0.29 }, {  0.27, 0.71, -0.32 },
    { -0.10, 0.63, -0.34 }, { -0.11, 0.22, -0.23 },
    {  0.26, 0.31, -0.35 }, {  0.24, 0.85, -0.32 },
    { -0.01, 0.26, -0.24 }, { -0.29, 0.78, -0.35 },
    {  0.24, 0.42, -0.35 }, {  0.06, 0.75, -0.32 },
    {  0.31, 0.78, -0.31 }, { -0.11, 0.33, -0.39 },
    {  0.06, 0.23, -0.27 }, {  0.35, 0.50, -0.27 },
    { -0.35, 0.70, -0.31 }, { -0.30, 0.40, -0.39 },
    { -0.39, 0.64, -0.27 }, { -0.35, 0.56, -0.27 }
};


GLuint LoadTexture(const char* filename, int width, int height) {

    //The following code will read in our RAW file
    FILE* file = fopen(filename, "rb");

    if (file == NULL) {
        cout << "Unable to open the image file" << endl << "Program will exit :(" << endl;
        exit(0);
    }

    unsigned char* data = (unsigned char*)malloc(width * height * 3);
    fread(data, width * height * 3, 1, file);
    fclose(file);

    // reorder the image colors to RGB not BGR
    for (int i = 0; i < width * height; ++i) {
        int index = i * 3;
        unsigned char B = data[index], R = data[index + 2];
        data[index] = R, data[index + 2] = B;
    }

    GLuint texture;
    glGenTextures(1, &texture);  //generate the texture with the loaded data
    glBindTexture(GL_TEXTURE_2D, texture); //bind the texture to it's array

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //set texture environment parameters

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // for sphere
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data); //free the texture array

    if (glGetError() != GL_NO_ERROR)
        printf("GLError in genTexture()\n");

    if (texture == -1)
        cout << "Error in loading the texture" << endl;

    return texture; //return whether it was successfull
}

void init() {

    // set backgroud color to sky blue color 
    glClearColor(0.6f, 0.8f, 0.9f, 1.0f);

    // to make transparent in snowing and sun
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65, ((double)win_width / win_hight), 0.01f, 50);

    //------- Texture ---------   
    myTexture1 = LoadTexture("grass.bmp", 900, 900); //grass  
    myTexture2 = LoadTexture("darkGround.bmp", 1200, 720); //dark ground    
    myTexture3 = LoadTexture("ground.bmp", 1140, 1140); //down of ground 
    myTexture4 = LoadTexture("lake.bmp", 588, 360); //lake      
    myTexture5 = LoadTexture("waterflow.bmp", 800, 533); //waterflow    
    myTexture6 = LoadTexture("water.bmp", 612, 408); //water       
    myTexture7 = LoadTexture("sun5.bmp", 500, 500); //Sun      
    myTexture9 = LoadTexture("mountain.bmp", 736, 736); //mountain
    myTexture10 = LoadTexture("snowSurface.bmp", 1024, 1024); //snow surface   
    myTexture11 = LoadTexture("snowSide.bmp", 1200, 720); // snow side 
   
    // cloud light   
    GLfloat light_ambient[] = { 0.7, 0.7, 0.7, 1 };
    GLfloat light_diffuse[] = { 0.9, 1, 1, 1 };
    GLfloat light_cloudposition[] = { -0.8, 3.2, 1.0, 1 };
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_cloudposition);

    // Rock light
    GLfloat white[] = { 1, 1, 1, 1 };
    GLfloat gray[] = { 0.5, 0.5, 0.5, 1 };
    GLfloat rockLightPosition[] = { -1.0, -4.2, 1.0, 0 };
    glLightfv(GL_LIGHT0, GL_POSITION, rockLightPosition);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, gray);
}

// ------------------------------------------- KEYBOARD -------------------------------------------------

// change camera movements

void key(unsigned char key, int x, int y) {

    
    if (key == 'Z' || key == 'z')
        cameraPosition[2] -= 0.1;

    else if (key == 'X' || key == 'x')
        cameraPosition[2] += 0.1;

    else if (key == 27)
        exit(0);

    else
        fprintf(stderr, "\nKeyboard commands:\nq, <esc> - Quit\n");

    // print the camera posion
    printf("camera position: (%0.1f, %0.1f, %0.1f)\n",
        cameraPosition[0], cameraPosition[1], cameraPosition[2]);
}

void specialKey(int key, int x, int y) {
    
    if (key == GLUT_KEY_RIGHT)
        cameraPosition[0] = (cameraPosition[0] <= -6) ? 3.5 : cameraPosition[0] -= 0.1;

    else if (key == GLUT_KEY_LEFT)
        cameraPosition[0] = (cameraPosition[0] >= 6) ? 3.5 : cameraPosition[0] += 0.1;

    else if (key == GLUT_KEY_UP)
        cameraPosition[1] = (cameraPosition[1] <= -6) ? 2.0 : cameraPosition[1] -= 0.1;

    else if (key == GLUT_KEY_DOWN)
        cameraPosition[1] = (cameraPosition[1] >= 6) ? 2.0 : cameraPosition[1] += 0.1;  

    // print the camera posion
    printf("camera position: (%0.1f, %0.1f, %0.1f)\n",
        cameraPosition[0], cameraPosition[1], cameraPosition[2]);
}
 
// -----------------------------------Helper Function------------------------------------------------------
// put texture on quad
void quadTexture(float v1[], float v2[], float v3[], float v4[], GLuint texture) {

    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(1, 1); glVertex3fv(v1);
    glTexCoord2f(0, 1); glVertex3fv(v2);
    glTexCoord2f(0, 0); glVertex3fv(v3);
    glTexCoord2f(1, 0); glVertex3fv(v4);
    glEnd();
}

// apply uniform scaling then translate the passed function 
void scaleAndTranslate(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat s, void(*shapeFunction)()) {
    glPushMatrix();
    glTranslatef(tx, ty, tz);
    glScalef(s, s, s);
    shapeFunction();
    glPopMatrix();
}

// translate the function
void translate(GLfloat x, GLfloat y, GLfloat z, void(*shapeFunction)()) {
    glPushMatrix();
    glTranslatef(x, y, z);
    shapeFunction();
    glPopMatrix();
}

void camera(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2], 0, 0, 0, 0, 1, 0);
}

void reshapeFunc(GLint new_width, GLint new_hight) {
    glViewport(0, 0, new_width, new_hight);
    win_width = new_width;
    win_hight = new_hight;
}

// ------------------------------------------- Nature Parts -------------------------------------------------

// draw cubic shape and put texture for grass or snow
void ground() {

    glColor3f(1, 1, 1);

    // height = 0.5, width = 1.5, length = 2
    float A[] = { -1, 0.25, -0.5 };
    float B[] = { 1, 0.25, -0.5 };
    float C[] = { -1, 0.25,  0.5 };
    float D[] = { 1, 0.25,  0.5 };

    float E[] = { -1, -0.25, -0.5 };
    float F[] = { 1, -0.25, -0.5 };
    float G[] = { -1, -0.25,  0.5 };
    float H[] = { 1, -0.25,  0.5 };

    // will change the texture of land between grass and snow
    if (isSnow) landSurfaceTexture = myTexture10, landSideTexture = myTexture11;
    else landSurfaceTexture = myTexture1, landSideTexture = myTexture2;

    glEnable(GL_TEXTURE_2D);
    quadTexture(C, A, E, G, landSideTexture);
    quadTexture(A, B, F, E, landSideTexture);
    quadTexture(D, B, F, H, landSideTexture);
    quadTexture(C, D, H, G, landSideTexture);
    quadTexture(A, B, D, C, landSurfaceTexture); // up
    quadTexture(E, F, H, G, myTexture3); // down
    glDisable(GL_TEXTURE_2D);
}

// draw ellips and put texture
GLvoid lake(const GLfloat radius, const GLuint num_vertex) {

    glColor3f(0.20000, 0.80000, 1.00000);
    GLfloat vertex[4];
    GLfloat texcoord[2];

    const GLfloat delta_angle = 2.0 * M_PI / num_vertex;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture4);
    glBegin(GL_TRIANGLE_FAN);

    //draw the vertex at the center of the circle
    texcoord[0] = 0.5;
    texcoord[1] = 0.5;
    glTexCoord2fv(texcoord);

    vertex[0] = vertex[1] = vertex[2] = 0.0;
    vertex[3] = 1.0;
    glVertex4fv(vertex);

    for (int i = 0; i < num_vertex; i++) {

        texcoord[0] = (cos(delta_angle * i) + 1.0) * 0.5;
        texcoord[1] = (sin(delta_angle * i) + 1.0) * 0.5;
        glTexCoord2fv(texcoord);

        vertex[0] = cos(delta_angle * i) * radius;
        vertex[1] = sin(delta_angle * i) * radius;
        vertex[2] = 0.0;
        vertex[3] = 1.0;
        glVertex4fv(vertex);
    }

    texcoord[0] = (1.0 + 1.0) * 0.5;
    texcoord[1] = (0.0 + 1.0) * 0.5;
    glTexCoord2fv(texcoord);

    vertex[0] = 1.0 * radius;
    vertex[1] = 0.0 * radius;
    vertex[2] = 0.0;
    vertex[3] = 1.0;
    glVertex4fv(vertex);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// draw texture recatangles on surface and side of ground (cubic) 
void waterfall() {

    float A[3] = { -0.6, 0.251,  -0.3 };
    float B[3] = { -0.4, 0.251,  -0.3 };
    float C[3] = { -0.6, 0.251, 0.501 };
    float D[3] = { -0.4, 0.251, 0.501 };
    float E[3] = { -0.6, -0.5,  0.501 };
    float F[3] = { -0.4, -0.5,  0.501 };

    glColor3f(1, 1, 1);

    glEnable(GL_TEXTURE_2D);
    quadTexture(A, B, D, C, myTexture5);
    quadTexture(C, D, F, E, myTexture5);
    glDisable(GL_TEXTURE_2D);
}

// usign textured built-in sphere function 
void sun(void) {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture7);
    glColor3f(1, 1, 0); // yellow
    GLUquadric* quadratic = gluNewQuadric();
    gluQuadricTexture(quadratic, GL_TRUE);		// Create Texture Coords   
    gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals  
    gluSphere(quadratic, 0.15f, 20, 20);
    glDisable(GL_TEXTURE_2D);

    // light by making transparent sphere
    glEnable(GL_BLEND);
    glColor4f(1, 1, 0, 0.3f);
    glutSolidSphere(0.165, 20, 20);
    glDisable(GL_BLEND);
}

// by making white small pyramid 
void mountainSnow(void) {

    float a[3] = { 0.51, 1.01, -0.51 };
    float b[3] = { 0.01, 0.01, -1.01 };
    float c[3] = { 1.01, 0.01,  0.01 };
    float d[3] = { 0.01, 0.01,  0.01 };
    float e[3] = { 1.01, 0.01, -1.01 };

    glPushMatrix();
    glTranslatef(0.39, 0.8, -0.39);
    glScalef(0.21, 0.21, 0.21);
    glBegin(GL_TRIANGLES);
    glVertex3fv(a);
    glVertex3fv(e);
    glVertex3fv(b);

    glVertex3fv(a);
    glVertex3fv(e);
    glVertex3fv(c);

    glVertex3fv(b);
    glVertex3fv(d);
    glVertex3fv(a);

    glVertex3fv(a);
    glVertex3fv(d);
    glVertex3fv(c);
    glEnd();
    glPopMatrix();
}

void drawPyramid() {

    float a[3] = { 0.5, 1.0, -0.5 };
    float b[3] = { 0.0, 0.0, -1.0 };
    float c[3] = { 1.0, 0.0,  0.0 };
    float d[3] = { 0.0, 0.0,  0.0 };
    float e[3] = { 1.0, 0.0, -1.0 };

    glColor3f(0.7, 0.5, 0.2); // brown

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, myTexture9);

    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3fv(b);
    glTexCoord2f(1.0, 0.0); glVertex3fv(d);
    glTexCoord2f(0.0, 0.0); glVertex3fv(c);
    glTexCoord2f(0.0, 1.0); glVertex3fv(e);
    glEnd();

    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5, 0.5); glVertex3fv(a);
    glTexCoord2f(1.0, 1.0); glVertex3fv(e);
    glTexCoord2f(0.0, 1.0); glVertex3fv(b);

    glTexCoord2f(0.5, 0.5); glVertex3fv(a);
    glTexCoord2f(1.0, 1.0); glVertex3fv(e);
    glTexCoord2f(1.0, 0.0); glVertex3fv(c);

    glTexCoord2f(0.0, 1.0); glVertex3fv(b);
    glTexCoord2f(0.0, 0.0); glVertex3fv(d);
    glTexCoord2f(0.5, 0.5); glVertex3fv(a);

    glTexCoord2f(0.5, 0.5); glVertex3fv(a);
    glTexCoord2f(0.0, 0.0); glVertex3fv(d);
    glTexCoord2f(1.0, 0.0); glVertex3fv(c);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    if (isSnow) {
        glEnable(GL_BLEND);
        glColor4f(1, 1, 1, snowTransparent);
        mountainSnow();
        glDisable(GL_BLEND);
    }
}

// group of texture pyramids
void mountain() {

    glPushMatrix();
    glScalef(1.2, 1.2, 1.2);
    drawPyramid();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.3, 0, 0);
    glRotatef(45, 0, 1, 0);
    drawPyramid();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3, 0, 0);
    glRotatef(45, 0, 1, 0);
    drawPyramid();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3, 0, 0);
    glRotatef(-45, 0, 1, 0);
    drawPyramid();
    glPopMatrix();
}

// It`s a group of spheres of different sizes and postions
void oneCloud(void) {

    // left
    glPushMatrix();
    glTranslated(-0.5, -0.1, 0);
    glutSolidSphere(0.35, 20, 20);
    glPopMatrix();

    // above right
    glPushMatrix();
    glTranslated(0.4, 0.2, 0);
    glutSolidSphere(0.25, 20, 20);
    glPopMatrix();

    // middle
    glutSolidSphere(0.5, 20, 20);

    // right 
    glPushMatrix();
    glTranslated(0.5, -0.1, 0);
    glutSolidSphere(0.35, 20, 20);
    glPopMatrix();
}

void clouds(void) {

    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);

    glColor3f(1, 1, 1); // white        
    scaleAndTranslate(0.25 + goLeft, 0.9, -0.2, 0.1, oneCloud);
    scaleAndTranslate(-0.6 + goRight, 1.0, -0.3, 0.2, oneCloud);
    scaleAndTranslate(0.45 + goLeft, 0.95, -0.25, 0.15, oneCloud);
    scaleAndTranslate(-0.4 + goRight, 1.1, -0.4, 0.15, oneCloud);
    scaleAndTranslate(-0.06 + goRight, 1, -0.5, 0.2, oneCloud);
    scaleAndTranslate(0.25 + goLeft, 1, -0.3, 0.12, oneCloud);

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);
}

// drawi cylinder and 3 cones
void tree() {

    // trunk
    GLUquadric* trunk = gluNewQuadric();
    glColor3f(0.5, 0.3, 0.1);
    glPushMatrix();
    glTranslatef(0.1, 0.49, 0.13);
    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(trunk, 0.04, 0.04, 0.4, 20, 20);
    glPopMatrix();

    // upper cone
    glColor4f(0.2, 0.4, 0.1, 1);
    glPushMatrix();
    glTranslatef(0.1, 1.25, 0.14);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glutSolidCone(0.15, 0.3, 10, 10);
    glPopMatrix();

    // middle cone
    glColor4f(0.2, 0.3, 0.1, 1);
    glPushMatrix();
    glTranslatef(0.1, 1.07, 0.14);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glutSolidCone(0.18, 0.3, 10, 10);
    glPopMatrix();

    // lower cone
    glColor4f(0.2, 0.4, 0.1, 1);
    glPushMatrix();
    glTranslatef(0.1, 0.9, 0.14);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glutSolidCone(0.19, 0.3, 10, 10);
    glPopMatrix();

    // make snow by white small cone in above 
    if (isSnow) {
        glEnable(GL_BLEND);
        glColor4f(1, 1, 1, snowTransparent);
        glPushMatrix();
        glTranslatef(0.1, 1.45, 0.15);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glutSolidCone(0.07, 0.14, 10, 10);
        glPopMatrix();
        glDisable(GL_BLEND);
    }
}

// using built-in function to Dodecahedron shape and apply light on it
void rocks(void) {

    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 50);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // behink lake    
    scaleAndTranslate(0.6, 0.25, -0.3, 0.02, glutSolidDodecahedron);
    scaleAndTranslate(0.7, 0.25, -0.3, 0.035, glutSolidDodecahedron);

    // front of lake
    scaleAndTranslate(0.2, 0.25, 0.2, 0.025, glutSolidDodecahedron);

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

// part of circle to draw steam 
void arc() {

    glLineWidth(4);
    double theta;
    glBegin(GL_LINE_STRIP);
    for (double i = 0; i <= 15; i++) {
        theta = i * 2.0 * M_PI / 360;
        glVertex2d(0.6f * cos(theta) + 0.9f, 0.9f * sin(theta) + 0.8f);
    }
    glEnd();
    glLineWidth(1);
}

void steam() {

    glColor4f(0.1, 0.1, 0.1, 0.2); // transparent white
    glEnable(GL_BLEND);
    scaleAndTranslate(-0.9, smove, -0.1, 1, arc);
    scaleAndTranslate(-1, smove, -0.23, 1, arc);
    scaleAndTranslate(-0.8, smove, 0, 1, arc);
    scaleAndTranslate(-1.1, smove, -0.1, 1, arc);
    scaleAndTranslate(-1, smove, 0, 1, arc);
    glDisable(GL_BLEND);
}

void drop(void) {

    // draw cone
    glPushMatrix();
    glRotatef(-5, 0, 0, 1);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.01, 0.05, 20, 20);
    glPopMatrix();

    // draw sphere  
    glutSolidSphere(0.01, 20, 20);
}

// draw one line then rotate it
void iceCrystal(void) {

    glColor3f(0.8, 0.9, 1.0); // light blue
    for (GLfloat theta = 0; theta < 360; theta += 60) {
        glPushMatrix();
        glRotatef(theta, 0, 0, 1);
        glBegin(GL_LINES);
        // vertical        
        glVertex3f(0.0, 0.0, 0);   glVertex3f(0, 0.5, 0);
        // first pair
        glVertex3f(0.1, 0.4, 0);   glVertex3f(0.0, 0.3, 0);
        glVertex3f(-0.1, 0.4, 0);  glVertex3f(0.0, 0.3, 0);
        // second pair
        glVertex3f(0.0, 0.18, 0);  glVertex3f(0.12, 0.3, 0);
        glVertex3f(0.0, 0.18, 0);  glVertex3f(-0.12, 0.3, 0);
        glEnd();
        glPopMatrix();
    }
}

// type of precipitation: rain, sleet and snow

void rain(void) {

    if (t > keyFrameTimes[2] && t < keyFrameTimes[3]) {
        glColor3f(0.3, 0.65, 1); // light blue
        for (int i = 0; i < NUMBER_OF_DROPS; i++)
            translate(dropsCoord[i][0], dropsCoord[i][1], dropsCoord[i][2], drop);
    }
}

void sleet(void) {
    if (t > keyFrameTimes[3] && t < keyFrameTimes[4]) {
        glColor4f(1, 1, 1, 1);
        for (int i = 0; i < NUMBER_OF_DROPS; i++) {
            glPushMatrix();
            glTranslatef(dropsCoord[i][0], dropsCoord[i][1], dropsCoord[i][2]);
            glutSolidSphere(0.01, 20, 20);
            glPopMatrix();
        }
    }
}

void snow(void) {

    if (t > keyFrameTimes[4] && t < keyFrameTimes[5]) {
        for (int i = 0; i < NUMBER_OF_DROPS; i++) {
            glPushMatrix();
            scaleAndTranslate(dropsCoord[i][0], dropsCoord[i][1], dropsCoord[i][2], 0.05, iceCrystal);
            glPopMatrix();
        }
    }
}

// -------------------------------------------------Animation-----------------------------------------------

// first keyfram t0
void evaporate(void) {

    // it`s still in its keyframe time, so make the animation
    if (t > keyFrameTimes[0] && t < keyFrameTimes[1])
        smove += 0.0004;
    else // it reached the cloud so disappear inside the ground
        smove = -0.9;   
    
    if (tex == 0) {
        // add nature sound
        PlaySound(TEXT("NatureSound.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
        
        //for change text
        a = ("Evaporation");
        tex++;
    }
}

// second keyfram t1
void condense(void) {

    if (t > keyFrameTimes[1] && t < keyFrameTimes[2]) {

        // change background color
        glClearColor(0.45882, 0.61961, 0.74118, 1);

        // move the clouds 
        goLeft -= 0.0005;
        goRight += 0.0005;

        //for change text
        if (tex == 1) {
            a = ("Condensation");
            tex++;
        }
    }
}

// each type has it own speed
void precipiting(double speed) {

    // by update drop y position and if hit the ground start from begining    
    // 0.25 the ground, 0.85 height of clouds
    for (int i = 0; i < NUMBER_OF_DROPS; i++)
        dropsCoord[i][1] = dropsCoord[i][1] <= 0.25 ? 0.85 : dropsCoord[i][1] -= speed;
}

// third keyfram t2
void raining(void) {

    if (t > keyFrameTimes[2] && t < keyFrameTimes[3]) {
        glClearColor(0.39216, 0.57255, 0.70588, 1); //change color background        
        precipiting(0.005f); // start raining 

        if (tex == 2) {             
            // add sound
            PlaySound(TEXT("RainingSound.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP); 
            
            //for change text
            a = ("Precipitation-Rain");
            tex++;
        }
    }
}

// fourth keyfram t3
void sleeting(void) {

    if (t > keyFrameTimes[3] && t < keyFrameTimes[4]) {
        glClearColor(0.32549, 0.52157, 0.67451, 1); //change color background 
        precipiting(0.002f); // start sleet   


        if (tex == 3) {
            // add sound
            PlaySound(TEXT("SleetingSound.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            
            // add text
            a = ("Or Precipitation-Sleet");
            tex++;
        }
    }
}

// sixth and final keyfram t5
void snowing(void) {

    if (t > keyFrameTimes[4] && t < keyFrameTimes[5]) {
        isSnow = true; // to change the nature to snowy
        glClearColor(0.29412, 0.47059, 0.60784, 1);//change color background         
        precipiting(0.001f); // start snowing
        snowTransparent += 0.001f;

        if (tex == 4) {
            // add sound
            PlaySound(TEXT("SnowingSound.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            // add text 
            a = ("Or Precipitation-Snowflakes");
            tex++;
        }


    }
}

static void play(void) {

    // The scence has finished so, 
    // reset the time and return the origin nature to start from begining   
    if (t >= keyFrameTimes[5]) {
        tex = 0;  //for change text
        t = 0;
        isSnow = false; // to change the nature to snowy
        glClearColor(0.6f, 0.8f, 0.9f, 1.0f); //light blue
        goLeft = goRight = 0;
        snowTransparent = 0;
    }

    // increase the timer 
    t += 0.01;

    // the water cycle prosseses
    evaporate();
    condense();
    raining();
    sleeting();
    snowing();

    glutPostRedisplay();
}

// add text to screen
void addText(float x, float y, string str) {

    glColor3f(1, 1, 1); // the font color is white
    glRasterPos2f(x, y);
    for (string::iterator c = (&str)->begin(); c != (&str)->end(); ++c)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
}


void drawAxes(GLfloat xNeg, GLfloat xPos, GLfloat yNeg, GLfloat yPos, GLfloat zNeg, GLfloat zPos) {

    glLineWidth(1.5);

    glBegin(GL_LINES);

    // x axis
    glColor3f(1, 0, 0);
    glVertex3f(xNeg, 0.0f, 0.0f);
    glVertex3f(xPos, 0.0f, 0.0f);

    // y axis	
    glColor3f(0, 1, 0);
    glVertex3f(0.0f, yNeg, 0.0f);
    glVertex3f(0.0f, yPos, 0.0f);

    // z axis
    glColor3f(0, 0, 1);
    glVertex3f(0.0f, 0.0f, zNeg);
    glVertex3f(0.0f, 0.0f, zPos);

    glEnd();
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera();

    glEnable(GL_DEPTH_TEST);

    // sun  
    translate(0.6, 1.05, -0.4, sun);

    // ground
    ground();

    //mountain          
    glPushMatrix();
    glTranslatef(-0.57, 0.2, 0.1);
    glRotatef(90, 0, 1, 0);
    glScalef(0.35, 0.6, 0.3);
    mountain();
    glPopMatrix();

    // trees
    scaleAndTranslate(0.0, 0, -0.4, 0.4, tree);
    scaleAndTranslate(-0.1, 0.1, 0.3, 0.3, tree);
    scaleAndTranslate(-0.25, 0, -0.1, 0.4, tree);
    scaleAndTranslate(0.8, 0, -0.4, 0.4, tree);

    // lake
    glPushMatrix();
    glTranslatef(0.5, 0.255, 0);
    glScalef(2.5, 1, 2);
    glRotatef(90, 1, 0, 0);
    lake(0.15, 100);
    glPopMatrix();

    waterfall();

    rocks();

    steam();

    clouds();

    // precipiting types
    rain();
    sleet();
    snow();

    glDisable(GL_DEPTH_TEST);

    // add text to screen that defines the process name
    addText(-0.2, -1.9, const_cast<char*>(a.c_str()));
    //addText(-1, 1.3, "Water Cycle");   
    
    drawAxes(-1, 1, -1, 1, -1, 1);
    glFlush();
    glutSwapBuffers();
}

int main(int argc, char** argv) {

    // Initialize GLUT
    glutInit(&argc, argv);

    // number of buffers and depth for 3D shapes 
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // Set the window's initial width & height
    glutInitWindowSize(win_width, win_hight);

    // Position the window's initial top-left corner
    glutInitWindowPosition(0, 0);

    // title
    glutCreateWindow("Water Cycle");

    // some prepreation to draw as texture, lighting, etc.
    init();

    // display the draw and animation
    glutIdleFunc(play);
    glutDisplayFunc(display);

    // resize the window
    glutReshapeFunc(reshapeFunc);

    // keyboard input from user    
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);

    // Enter the infinitely event-processing loop 
    glutMainLoop();
}


