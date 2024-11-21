#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3*PI/2
#define DR 0.0174533 // One degree radian

float px,py,pdx,pdy,pa; //player position

void drawPlayer(){
	glColor3f(1,0,0);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2i(px,py);
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
			glVertex2i(0 + xo + 1,yo + 1); // Bottom Left Corner
			glVertex2i(0 + xo + 1,yo+mapS - 1); // Top Left Corner
			glVertex2i(xo+mapS - 1, yo+mapS - 1); // Top Right Corner
			glVertex2i(xo+mapS - 1 ,0 + yo + 1); // Bottom Right Corner
			// The +1 and -1 are basically pushing the box one pixel inward and thats how we have that grid effect
			glEnd(); // End Drawing
		}
	}
}

float dist(float ax, float ay, float bx, float by, float ang){ // Calculate distance between player and ray endpoint which is the hypothenuse which is literally just the distance the ray has been travelling
	return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) ); // bx-ax is just the horizontal cathetus and by-ay is just the vertical cathetus (pythagorean theorem)
}

void drawRays2D(){
	int r,mx,my,mp,dof; float rx,ry,ra,xo,yo,disT;
	ra=pa-DR*30; if(ra<0) { ra+=2*PI; } if(ra>2*PI){ ra-=2*PI; }
	for(r=0;r<60;r++){ // How many rays we want to cast?
		// Check horizontal lines
		dof=0;
		float disH=1000000,hx=px,hy=py;
		float aTan=-1/tan(ra); // This tells you how much distance is traveled horizontally when we shift our vision vertically
		if(ra>PI){ // Looking up cause angle in rad is more than PI first two quadrants
			ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry) * aTan+px; // >>n is equal to /2^n, so you can derive >>6 to be divided 2^6 = 64 and doing <<6 which is multiplying it back with 64
			yo=-64; xo=-yo*aTan; // Basically just offsetting it by 64 since its the second line it finds
		} // ry is reaching the position of the nearest multiple of 64 with that calculation which is the closest horizontal line and rx will be the x position that the end of the line would be proportional to the y position it is when touching the nearest square
		if(ra<PI){ // If looking down
			ry=(((int)py>>6)<<6)+64; rx=(py-ry) * aTan+px; yo=64; xo=-yo*aTan;
		}
		if(ra==0 || ra==PI) { rx=px; ry=py; dof=8; } // Looking straight left or right doesn't touch any horizontal lines.
		
		while(dof<8){
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx; // Map positions
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof = 8; } // If were hitting a wall
			else{ rx+=xo; ry+=yo; dof+=1; } // If were not hitting a wall we literally just add another offset until we reach a wall
		}

		// Check vertical lines
		dof=0;
		float disV=1000000,vx=px,vy=py;
		float nTan=-tan(ra);
		if(ra>PI2 && ra<PI3){ rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx) * nTan+py; xo=-64; yo=-xo*nTan;} // Looking left
		if(ra<PI2 || ra>PI3){ rx=(((int)px>>6)<<6)+64; ry=(px-rx) * nTan+py; xo=64; yo=-xo*nTan; } // Looking right
		if(ra==PI/2 || ra==3*PI/2) { rx=px; ry=py; dof=8; } // Looking straight up or down doesn't touch any vertical lines.

		while(dof<8){
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx; // Map positions
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof = 8; } // If were hitting a wall
			else{ rx+=xo; ry+=yo; dof+=1; } // If were not hitting a wall we literally just add another offset until we reach a wall
		}

		if(disV<disH){ rx=vx; ry=vy; disT=disV; glColor3f(0,0,0.9);}
		if(disH<disV){ rx=hx; ry=hy; disT=disH; glColor3f(0,0,0.7);}

		glLineWidth(3); glBegin(GL_LINES); glVertex2i(px,py); glVertex2i(rx,ry); glEnd();

		// Draw 3D Walls
		float ca=pa-ra; if(ca<0) { ca+=2*PI; } if(ca>2*PI){ ca-=2*PI; } disT=disT*cos(ca); // Fix fisheye
		float lineH=(mapS*380)/disT; if(lineH>380) { lineH=380; }// This visualizes the horizontal length of the 3d object we see, max 320 px tall, and the farther we are so higher distance means smaller object thats why its /disT
		float lineO=160-lineH/2; // Offsetting the camera up
		float offsetX = r * 8 + 540;
		glLineWidth(8); glBegin(GL_LINES); glVertex2i(offsetX,lineO); glVertex2i(offsetX,lineH+lineO); glEnd();

		ra+=DR; if(ra<0) { ra+=2*PI; } if(ra>2*PI){ ra-=2*PI; } // Increase next ray cast by one radian
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawRays2D();
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
	gluOrtho2D(0,1024,510,0);
	px=150; py=400; pdx=cos(pa)*5; pdy=sin(pa)*5; 
}

int main(int argc, char** argv){ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 510);
	glutCreateWindow("CRayCast");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutMainLoop();
}
