#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

/*typedef struct {
	int w, a, s, d; // Button state on off
}ButtonKeys; ButtonKeys Keys;*/

//-----------------------------MAP----------------------------------------------
#define mapX 8 // Map Width
#define mapY 8 // Map Height
#define mapS 64 // Map Size
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
			if(map[y*mapX+x]==1){ glColor3f(1,1,1); // Checks position and if 1 makesit white (walls)
			} else{ glColor3f(0,0,0); } // Black for empty space
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
//------------------------------------------------------------------------------



//------------------------PLAYER------------------------------------------------
float degToRad(int a){ return a*M_PI/180.0; } // Degree to radian
int FixAng(int a){ if(a>359){ a-=360; } if(a<0){ a+=360; } return a; } // Making the angle a viable value since it has to be between 0 and 360, this will be used to replace the old code which basically had the if statements doing this check

float px,py,pdx,pdy,pa; //player position

void drawPlayer2D(){
	glColor3f(1,1,0); glPointSize(8); glLineWidth(4);
	glBegin(GL_POINTS); glVertex2i(px,py); glEnd();
	glBegin(GL_LINES); glVertex2i(px,py); glVertex2i(px+pdx*20,py+pdy*20); glEnd();
}

void Buttons(unsigned char key,int x,int y)
{
 if(key=='a'){ pa+=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));}
 if(key=='d'){ pa-=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa));}
 if(key=='w'){ px+=pdx*5; py+=pdy*5;}
 if(key=='s'){ px-=pdx*5; py-=pdy*5;}
 glutPostRedisplay();
}
//------------------------------------------------------------------------------

float distance(float ax, float ay, float bx, float by, float ang){ // Calculate distance between player and ray endpoint which is the hypothenuse which is literally just the distance the ray has been travelling
	return cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay); // bx-ax is just the horizontal cathetus and by-ay is just the vertical cathetus (pythagorean theorem)
}

void drawRays2D(){
	glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();
	glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();
	
	int r,mx,my,mp,dof,side; float vx,vy,rx,ry,ra,xo,yo,disV,disH;
	
	pa=FixAng(pa+30);
	
	for(r=0;r<60;r++){ // How many rays we want to cast?
		
		// Check vertical lines
		dof=0; side=0; disV=100000;
		float Tan=tan(degToRad(ra));
		if(cos(degToRad(ra))>0.001){ rx=(((int)px>>6)<<6)+64; ry=(px-rx) * Tan+py; xo=64; yo=-xo*Tan; // Looking left
		} else if(cos(degToRad(ra))<-0.001){ rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx) * Tan+py; xo=-64; yo=-xo*Tan; // Looking right
		} else { rx=px; ry=py; dof=8; } // Looking straight up or down doesn't touch any vertical lines.

		while(dof<8){
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx; // Map positions
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof = 8; disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py); // If were hitting a wall
			} else{ rx+=xo; ry+=yo; dof+=1; } // If were not hitting a wall we literally just add another offset until we reach a wall
		}
		vx=rx; vy=ry;

		// Check horizontal lines
		dof=0; disH=100000;
		Tan=1.0/Tan; // This tells you how much distance is traveled horizontally when we shift our vision vertically
		if(sin(degToRad(ra))>0.001){ // Looking up cause angle in rad is more than PI first two quadrants
			ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry) * Tan+px; // >>n is equal to /2^n, so you can derive >>6 to be divided 2^6 = 64 and doing <<6 which is multiplying it back with 64
			yo=-64; xo=-yo*Tan; // Basically just offsetting it by 64 since its the second line it finds
		 // ry is reaching the position of the nearest multiple of 64 with that calculation which is the closest horizontal line and rx will be the x position that the end of the line would be proportional to the y position it is when touching the nearest square
		} else if(sin(degToRad(ra))<-0.001){ // If looking down
			ry=(((int)py>>6)<<6)+64; rx=(py-ry) * Tan+px; yo=64; xo=-yo*Tan;
		} else{ rx=px; ry=py; dof=8; } // Looking straight left or right doesn't touch any horizontal lines.
		
		while(dof<8){
			mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx; // Map positions
			if(mp>0 && mp<mapX*mapY && map[mp]==1){ dof = 8; disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py); // If were hitting a wall
			} else{ rx+=xo; ry+=yo; dof+=1; } // If were not hitting a wall we literally just add another offset until we reach a wall
		}

		glColor3f(0,0.8,0);
		if(disV<disH){ rx=vx; ry=vy; disH=disV; glColor3f(0,0.6,0); } // If horizontal ray hits first
		glLineWidth(2); glBegin(GL_LINES); glVertex2i(px,py); glVertex2i(rx,ry); glEnd();

		int ca=FixAng(pa-ra); disH=disH*cos(degToRad(ca)); // Fix fisheye
		int lineH=(mapS*320)/(disH); if(lineH>320) { lineH=320; }// This visualizes the horizontal length of the 3d object we see, max 320 px tall, and the farther we are so higher distance means smaller object
		int lineOff=160-(lineH>>1); // Offsetting the camera up
		
		glLineWidth(8); glBegin(GL_LINES); glVertex2i(r*8+530,lineOff); glVertex2i(r*8+530,lineOff+lineH); glEnd();

		ra=FixAng(ra-1); // Increase next ray cast by one radian
	}
}
//------------------------------------------------------------------------------
void init(){
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0,1024,510,0);
	px=150; py=400; pa=90;
	pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); 
}

void display(){

	/*if(Keys.a){ pa+=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); }
	if(Keys.d){ pa-=5; pa=FixAng(pa); pdx=cos(degToRad(pa)); pdy=-sin(degToRad(pa)); }
	if(Keys.w){ px+=pdx*5; py+=pdy*5; }
	if(Keys.s){ px-=pdx*5; py-=pdy*5; }
	glutPostRedisplay();*/
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer2D();
	drawRays2D();
	glutSwapBuffers();
}

/*
void resize(int w, int h){
	glutReshapeWindow(1024, 512);
}*/

/*void ButtonDown(unsigned char key, int x, int y){
	if(key=='a'){ Keys.a=1; }
	if(key=='d'){ Keys.d=1; }
	if(key=='w'){ Keys.w=1; }
	if(key=='s'){ Keys.s=1; }
	glutPostRedisplay();
}*/

/*void ButtonUp(unsigned char key, int x, int y){
	if(key=='a'){ Keys.a=0; }
	if(key=='d'){ Keys.d=0; }
	if(key=='w'){ Keys.w=0; }
	if(key=='s'){ Keys.s=0; }
	glutPostRedisplay();
}*/

int main(int argc, char* argv[]){ 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 510);
	//glutInitWindowPosition(200, 200);
	glutCreateWindow("CRayCast");
	init();
	glutDisplayFunc(display);
	//glutReshapeFunc(resize);
	glutKeyboardFunc(Buttons);
	//glutKeyboardFunc(ButtonDown);
	//glutKeyboardUpFunc(ButtonUp);
	glutMainLoop();
}
