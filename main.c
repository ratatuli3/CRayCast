#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535

float px,py,pdx,pdy,pa; //player position

void drawPlayer(){
	glColor3f(1,0,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
	glEnd();

	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(px,py);
	glVertex2i(px+pdx*7,py+pdy*7); // The 7 is the length added to the line
	glEnd();
}

int mapX=8, mapY=8, mapS=64;
int map[]=
{
1,1,1,1,1,1,1,1,
1,0,1,0,0,0,1,1,
1,0,1,0,0,0,0,1,
1,0,1,0,0,0,0,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,1,0,1,
1,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,
};

void drawMap2D(){
	int x,y,xo,yo;
	for(y=0;y<mapY;y++){
		for(x=0;x<mapX;x++){
			if(map[y*mapX+x]==1){ // Checks position and if 1 makes it white (walls)
				glColor3f(1,1,1);
			} else{
				glColor3f(0,0,0);
			} // Black for empty space
			xo=x*mapS; yo=y*mapS;
			glBegin(GL_QUADS); // Specifies that we will draw a 4-sided shape on the specific coordinate with size 64x64
			glVertex2i(xo + 1,yo + 1); // Bottom Left Corner
			glVertex2i(xo + 1,yo+mapS - 1); // Top Left Corner
			glVertex2i(xo+mapS - 1, yo+mapS - 1); // Top Right Corner
			glVertex2i(xo+mapS - 1 , yo + 1); // Bottom Right Corner
			// The +1 and -1 are basically pushing the box one pixel inward and thats how we have that grid effect
			glEnd(); // End Drawing
		}
	}
}

void drawRays3D(){
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo;
	ra=pa;
	for(r=0;r<1;r++){ // How many rays we want to cast?
		// Check horizontal lines
		dof=0;
		float aTan=-1/tan(ra); // This tells you how much distance is traveled horizontally when we shift our vision vertically
		if(ra>PI){ // Looking up cause angle in rad is more than PI first two quadrants
			ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry) * aTan+px; // >>1 is equal to /2, so you can derive >>6 to be divided 2^6 = 64 and doing <<6 which is multiplying it back with 64
			yo=-64; xo=-yo*aTan; // Basically just offsetting it by 64 since its the second line it finds
		} // ry is reaching the position of the nearest multiple of 64 with that calculation which is the closest horizontal line and rx will be the x position that the end of the line would be proportional to the y position it is when touching the nearest square
		if(ra<PI){ // If looking down
			ry=(((int)py>>6)<<6)+64; rx=(py-ry) * aTan+px; yo=64; xo=-yo*aTan;
		}
		if(ra==0 || ra==PI) { rx=px; ry=py; dof=8; } // Looking straight left or right doesn't touch any horizontal lines.
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y){
	if(key=='a'){ pa-=0.1; if(pa<0) {pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5; } // Turning left
	if(key=='d'){ pa+=0.1; if(pa>2*PI) {pa-=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5; } // Turning right
	if(key=='w'){ px+=pdx; py+=pdy; }
	if(key=='s'){ px-=pdx; py-=pdy; }
	glutPostRedisplay();
}

void init(){
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0,1024,512,0);
	px=300; py=300; pdx=cos(pa)*5; pdy=sin(pa)*5; 
}

int main(int argc, char** argv){ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 512);
	glutCreateWindow("CRayCast");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
 return 0;
}
