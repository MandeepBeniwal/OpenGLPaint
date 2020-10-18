#include<windows.h>
#include<GL/glut.h>
#include<stdio.h>
#include<math.h>

int MAX_WIDTH, MAX_HEIGHT, width=1000, height=600;
char *imag[5], *image, s0[100], filename[200];
int saveIndex=0, undoCount=0, redoCount=0, startOfApp=1, initialx;
int j=0, openflag=0, saveflag=0, fn=0, startflag=1;
int rgbflag=0, rflag=0, gflag=0, bflag=0, decFlag=0, coun=0;
int linewidth=1, COL=1, ch=0, ch1, flag=0, cutflag=0;
FILE *fp=NULL;
float f=0, xx0=0, yy0=0, xx1=0, yy1=0, R1,G1,B1,fr=0,fg=0,fb=0;
float cutx0=0, cutx1=0, cuty0=0, cuty1=0, backColor[3], curColor[3], r, g, b;

/* resets all the variables to their initial values */
void reset()
{
    ch=0, openflag=0, saveflag=0, fn=0, rgbflag=0, rflag=0, gflag=0, bflag=0, decFlag=0, coun=0, COL=1, saveIndex=0, undoCount=0, redoCount=0, startOfApp=1;
}

/* sets the color of drawing to be the chosen color */
void setColor(int i)
{
    switch(i)
    {
        case 1: if(i==1)//black
                {
                    r=0,g=0,b=0;
                    glColor3f(0.0,0.0,0.0);
                }
                break;
        case 2: if(i==2)//red
                {
                    r=1,g=0,b=0;
                    glColor3f(1.0,0.0,0.0);
                }
                break;
        case 3: if(i==3)//orange
                {
                    r=1,g=0.5,b=0;
                    glColor3f(1.0,0.5,0.0);
                }
                break;
        case 4: if(i==4)//yellow
                {
                    r=1,g=1,b=0;
                    glColor3f(1.0,1.0,0.0);
                }
                break;
        case 5: if(i==5)//light green
                {
                    r=0.5,g=1,b=0.5;
                    glColor3f(0.5,1.0,0.5);
                }
                break;
        case 6: if(i==6)//dark green
                {
                    r=0,g=1,b=0;
                    glColor3f(0.0,1.0,0.0);
                }
                break;
        case 7: if(i==7)//purple
                {
                    r=1,g=0,b=1;
                    glColor3f(1.0,0.0,1.0);
                }
                break;
        case 8: if(i==8)//blue
                {
                    r=0,g=0,b=1;
                    glColor3f(0.0,0.0,1.0);
                }
                break;
        case 9: if(i==9)//cyan
                {
                    r=0,g=1,b=1;
                    glColor3f(0.0,1.0,1.0);
                }
                break;
        case 10:if(i==10)//white
                {
                    r=1,g=1,b=1;
                    glColor3f(1.0,1.0,1.0);
                }
                break;
        case 11:
                r=R1,g=G1,b=B1;
                glColor3f(R1,G1,B1);
    }
}

/* draws a rectangular polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawRectPolygon(float x0,float y0,float x1,float y1)
{
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0,y0);
        glVertex2f(x0,y1);
        glVertex2f(x1,y1);
        glVertex2f(x1,y0);
    glEnd();
}

/* fills the clicked region entirely with the selected color (SCAN-LINE FILLING ALGORITHM) */
int scanlinefill(int x, int y)
{
    int startx, endx, i;
    glReadPixels(x,y,1,1,GL_RGB,GL_FLOAT,curColor);
    if (!((curColor[0] == backColor[0]) && (curColor[1] == backColor[1]) && (curColor[2] == backColor[2])?1:0))
    {
        return x;
    }
    initialx = x;
    /* scans to the left until reaches another color */
    do
    {
        glReadPixels(x--,y,1,1,GL_RGB,GL_FLOAT,curColor);
    }while(((curColor[0] == backColor[0]) && (curColor[1] == backColor[1]) && (curColor[2] == backColor[2])?1:0) && x>=249);
    startx = x+2;
    /* scans to the right until reaches another color */
    x = initialx;
    do
    {
        glReadPixels(x++,y,1,1,GL_RGB,GL_FLOAT,curColor);
    }while(((curColor[0] == backColor[0]) && (curColor[1] == backColor[1]) && (curColor[2] == backColor[2])?1:0) && x<width);
    endx = x-1;

    /* recurses for a higher y coordinate */
    for(i=startx;i<endx;i++)
    {
        /* draws the horizontal line for the current y coordinate */
        if (i==startx)
        {
            glColor3f(r,g,b);
            glDrawBuffer(GL_FRONT_AND_BACK);
            glBegin(GL_LINES);
                glVertex2f(startx,y);
                glVertex2f(endx,y);
            glEnd();
        }

        if ((y+1) < (height-100))
        {
            i=scanlinefill(i,y+1);
        }
    }

    /* recurses for a lower y coordinate */
    for(i=startx;i<endx;i++)
    {
        /* draws the horizontal line for the current y coordinate */
        if (i==startx)
        {
            glColor3f(r,g,b);
            glDrawBuffer(GL_FRONT_AND_BACK);
            glBegin(GL_LINES);
                glVertex2f(startx,y);
                glVertex2f(endx,y);
            glEnd();
        }
        if ((y-1) >= 0)
        {
            i=scanlinefill(i,y-1);
        }
    }
    return endx;
}

/* draws a line from (x0,y0) to (x1,y1) */
void drawLine(float x0,float y0,float x1,float y1)
{
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINES);
        glVertex2f(x0,y0);
        glVertex2f(x1,y1);
    glEnd();
}

/* draws a rectangle within the diagonal region of (x0,y0) and (x1,y1) */
void drawRectLineLoop(float x0,float y0,float x1,float y1)
{
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0,y0);
        glVertex2f(x0,y1);
        glVertex2f(x1,y1);
        glVertex2f(x1,y0);
    glEnd();
}

/* draws a Right angle triangle within the diagonal region of (x0,y0) and (x1,y1) */
void drawRTriLineLoop(float x0,float y0,float x1,float y1)
{
    float temp;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0,y0);
        glVertex2f(x1,y0);
        glVertex2f(x0,y1);
    glEnd();
}

/* draws a Right angle triangular polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawRTriPolygon(float x0,float y0,float x1,float y1)
{
    float temp;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0,y0);
        glVertex2f(x1,y0);
        glVertex2f(x0,y1);
    glEnd();
}

/* draws an equilateral triangle within the diagonal region of (x0,y0) and (x1,y1) */
void drawETriLineLoop(float x0,float y0,float x1,float y1)
{
    float temp;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0,y0);
        glVertex2f(x1,y0);
        glVertex2f((x1+x0)/2,y1);
    glEnd();
}

/* draws an equilateral triangular polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawETriPolygon(float x0,float y0,float x1,float y1)
{
    float temp;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0,y0);
        glVertex2f(x1,y0);
        glVertex2f((x0+x1)/2,y1);
    glEnd();
}

/* draws a star within the diagonal region of (x0,y0) and (x1,y1) */
void drawStarLineLoop(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=x1-x0;
    y=y1-y0;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0+2*x/5,y0+2*y/5);
        glVertex2f((x0+x1)/2,y0);
        glVertex2f(x0+3*x/5,y0+2*y/5);
        glVertex2f(x1,(y0+y1)/2);
        glVertex2f(x0+3*x/5,y0+3*y/5);
        glVertex2f((x0+x1)/2,y1);
        glVertex2f(x0+2*x/5,y0+3*y/5);
        glVertex2f(x0,(y0+y1)/2);
    glEnd();
}

/* draws a star polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawStarPolygon(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=x1-x0;
    y=y1-y0;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0+2*x/5,y0+2*y/5);
        glVertex2f((x0+x1)/2,y0);
        glVertex2f(x0+3*x/5,y0+2*y/5);
        glVertex2f(x1,(y0+y1)/2);
        glVertex2f(x0+3*x/5,y0+3*y/5);
        glVertex2f((x0+x1)/2,y1);
        glVertex2f(x0+2*x/5,y0+3*y/5);
        glVertex2f(x0,(y0+y1)/2);
    glEnd();
}

/* draws a circle within the diagonal region of (x0,y0) and (x1,y1) */
void drawCirLineLoop(float x0,float y0,float x1,float y1)
{
    float r,temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    if((x1-x0)<(y1-y0))
        r=(x1-x0)/2;
    else
        r=(y1-y0)/2;

    x0=(x0+x1)/2;
    y0=(y0+y1)/2;

    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
    for(int i=0;i<100;i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

        x = r * cosf(theta);//calculate the x component
        y = r * sinf(theta);//calculate the y component
        glVertex2f(x + x0, y + y0);//output vertex
    }
    glEnd();
}

/* draws a circular polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawCirPolygon(float x0,float y0,float x1,float y1)
{
    float r,temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    if((x1-x0)<(y1-y0))
        r=(x1-x0)/2;
    else
        r=(y1-y0)/2;
    x0=(x0+x1)/2;
    y0=(y0+y1)/2;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
    for(int i=0;i<100;i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(100);//get the current angle

        x = r * cosf(theta);//calculate the x component
        y = r * sinf(theta);//calculate the y component
        glVertex2f(x + x0, y + y0);//output vertex
    }
    glEnd();
}

/* writes the text str at coordinates (x,y) of color (r,g,b) */
void text(int x, int y, float r, float g, float b, char *str)
{
    glColor3f(r,g,b);
    glRasterPos2d(x,y);
    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
    }
}

/* draws a pentagon within the diagonal region of (x0,y0) and (x1,y1) */
void drawPentLineLoop(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=x1-x0;
    y=y1-y0;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0+.2*x,y0);
        glVertex2f(x0+.8*x,y0);
        glVertex2f(x1,y0+5*y/8);
        glVertex2f(x/2+x0,y1);
        glVertex2f(x0,y0+5*y/8);
    glEnd();
}

/* draws a pentagonal polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawPentPolygon(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=x1-x0;
    y=y1-y0;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0+.2*x,y0);
        glVertex2f(x0+.8*x,y0);
        glVertex2f(x1,y0+5*y/8);
        glVertex2f(x/2+x0,y1);
        glVertex2f(x0,y0+5*y/8);
    glEnd();
}

/* draws a rhombus within the diagonal region of (x0,y0) and (x1,y1) */
void drawRhomLineLoop(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=(x1-x0)/2;
    y=(y1-y0)/2;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
        glVertex2f(x0+x,y0);
        glVertex2f(x0,y0+y);
        glVertex2f(x0+x,y1);
        glVertex2f(x1,y0+y);
    glEnd();
}

/* draws a rhombus polygon within the diagonal region of (x0,y0) and (x1,y1) */
void drawRhomPolygon(float x0,float y0,float x1,float y1)
{
    float temp,x,y;
    if(x0>x1)
    {
        temp=x0;
        x0=x1;
        x1=temp;
    }
    if(y0>y1)
    {
        temp=y1;
        y1=y0;
        y0=temp;
    }
    x=(x1-x0)/2;
    y=(y1-y0)/2;
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2f(x0+x,y0);
        glVertex2f(x0,y0+y);
        glVertex2f(x0+x,y1);
        glVertex2f(x1,y0+y);
    glEnd();
}

/* draws the introduction Splash Screen */
void drawSplashScreen()
{
    printf("WELCOME !!");
    int w,k=0;
    glColor3f(0.71,0.82,1);
    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_POLYGON);
        glVertex2i(0,0);
        glVertex2i(0,height);
        glVertex2i(width,height);
        glVertex2i(width,0);
    glEnd();

    text(width/2 - 60,height-75,1.0,0.0,0.0,"WELCOME");
    text(width/2 - 225,height-150,1.0,0.0,0.0,"MINI-PROJECT ON GRAPHICS EDITOR");
    text(width/2 - 200-100,height-300,0.0,0.0,1.0,"MANDEEP SINGH");
    text(width/2 - 200-100,height-350,0.0,0.0,1.0,"1MV12CS050");
    text(width/2 + 100,height-300,0.0,0.0,1.0,"MANISH VERMA");
    text(width/2 + 100,height-350,0.0,0.0,1.0,"1MV12CS051");
    text(450,20,1.0,0.0,0.0,"PLEASE WAIT LOADING...");

    /* for loading bar */
    for(w=380,k=0;(w+k)<=599;k+=1)
    {
        glColor3f(0.8,0.9,1.0);
        drawRectPolygon(380,160,600,180);
        glColor3f(0.0,0.0,0.0);
        drawRectLineLoop(380,160,600,180);
        glColor3f(0.5,0.1,0.0);
        drawRectPolygon(w,160,w+k,180);
        Sleep(10);
        glFlush();glutSwapBuffers();
    }
}

/* dynamically initializes the elements of the imag array */
void CircularQueueSet()
{
    int i;
    for(i=0;i<5;i++)
        imag[i]=(char*)malloc((MAX_WIDTH-250) * (MAX_HEIGHT-100) *3);
    image=(char*)malloc((MAX_WIDTH-250) * (MAX_HEIGHT-100) *3);
    startOfApp = 1;
}

/* saves the current image on the screen into imag[saveIndex] */
void save()
{
    saveIndex = (saveIndex+1)%5;
    redoCount=0;
    if (undoCount < 4)
        undoCount++;
    glReadBuffer(GL_FRONT);
    glReadPixels(250, 0, width-250, height-100, GL_RGB, GL_UNSIGNED_BYTE,imag[saveIndex]);
    glutPostRedisplay();
}

/* performs the undo operation from the current stage */
void undo()
{
    /* checks if undo is possible */
    if (undoCount>0)
    {
        undoCount--;
        redoCount++;
        if (saveIndex == 0)
            saveIndex = 4;
        else
            saveIndex--;
        glutPostRedisplay();
    }
}

/* performs the redo operation from the current stage (if possible) */
void redo()
{
    /* checks if redo is possible */
    if (redoCount>0)
    {
        redoCount--;
        if (undoCount<4)
            undoCount++;
        saveIndex = (saveIndex+1)%5;
        glutPostRedisplay();
    }
}

/* this function draws all the menu items on the screen */
void drawMenu()
{
    if(startflag)
    {
        drawSplashScreen();
        startflag=0;
    }
    glColor3f(0.80,0.9,1.0);
    glLineWidth(1);
    //main margin
    drawRectPolygon(width,height,0,0);

    // inner margin
    glColor3f(1.0,1.0,1.0);
    drawRectPolygon(width,height-100,250,0);
    //for drawing
    glColor3f(0.0,0.0,0.0);
    glLineWidth(2);
    drawRectLineLoop(width-1,height-100,250,1);
    //for colors menu
    text(70,height-500,1.0,0.0,0.0,"COLORS");//to write the text "colors"
    glColor3f(0.0,0.0,0.0);//black
    drawRectPolygon(5,height-595,45,height-555);
    glColor3f(1.0,0.0,0.0);//red
    drawRectPolygon(55,height-595,95,height-555);
    glColor3f(1.0,0.5,0.0);//orange
    drawRectPolygon(105,height-595,145,height-555);
    glColor3f(1.0,1.0,0.0);//yellow
    drawRectPolygon(155,height-595,195,height-555);
    glColor3f(0.5,1.0,0.5);//light green
    drawRectPolygon(205,height-595,245,height-555);
    glColor3f(0.0,1.0,0.0);//dark green
    drawRectPolygon(5,height-545,45,height-505);
    glColor3f(1.0,0.0,1.0);//purple
    drawRectPolygon(55,height-545,95,height-505);
    glColor3f(0.0,0.0,1.0);//blue
    drawRectPolygon(105,height-545,145,height-505);
    glColor3f(0.0,1.0,1.0);//cyan
    drawRectPolygon(155,height-545,195,height-505);
    glColor3f(1.0,1.0,1.0);//white
    drawRectPolygon(205,height-545,245,height-505);

    //for margin of color menu
    glLineWidth(2);
    glColor3f(0.0,0.0,0.0);
    drawRectLineLoop(5,height-595,45,height-555);
    drawRectLineLoop(55,height-595,95,height-555);
    drawRectLineLoop(105,height-595,145,height-555);
    drawRectLineLoop(155,height-595,195,height-555);
    drawRectLineLoop(205,height-595,245,height-555);
    drawRectLineLoop(5,height-545,45,height-505);
    drawRectLineLoop(55,height-545,95,height-505);
    drawRectLineLoop(105,height-545,145,height-505);
    drawRectLineLoop(155,height-545,195,height-505);
    drawRectLineLoop(205,height-545,245,height-505);


    //for shapes
    text(70,height-300,1.0,0.0,0.0,"SHAPES");//to write the text "SHAPES"
    glColor3f(0.60,0.7,1.0);
    glLineWidth(2);
    drawLine(25,height-275,225,height-275);
    drawLine(165,height-145,165,height-25);//to draw line partition(this is in between tool &undo)
    drawLine(245,height-15,245,height-90);
    drawLine(350,height-15,350,height-90);
    drawLine(505,height-15,505,height-90);
    drawLine(25,height-150,225,height-150);
    drawLine(25,height-475,225,height-475);

    //drawing the square borders for shape options
    glColor3f(0.0,0.0,1.0);
    glLineWidth(1);
    drawRectLineLoop(5,height-305,45,height-345);/// Drawing rect of shape
    drawRectLineLoop(55,height-345,95,height-305);/// Drawing rect of shape
    drawRectLineLoop(105,height-345,145,height-305);/// Drawing rect of shape
    drawRectLineLoop(155,height-345,195,height-305);/// Drawing rect of shape
    drawRectLineLoop(205,height-345,245,height-305);/// Drawing rect of shape
    drawRectLineLoop(5,height-395,45,height-355);/// Drawing rect of shape
    drawRectLineLoop(55,height-395,95,height-355);/// Drawing rect of shape
    drawRectLineLoop(105,height-395,145,height-355);/// Drawing rect of shape


    // drawing light background for the shapes
    glColor3f(0.9,0.9,1);
    drawCirPolygon(58,height-348,92,height-302); // circle
    drawRectPolygon(110,height-340,140,height-310); // rectangle
    drawRTriPolygon(159,height-341,192,height-308); //right triangle
    drawETriPolygon(209,height-341,241,height-309); //equilateral triangle
    drawStarPolygon(8,height-392,42,height-358); // star polygon
    drawPentPolygon(59,height-391,91,height-359); //pentagon
    drawRhomPolygon(109,height-391,141,height-359); //rhombus


    //for drawing the shapes inside the square boxes
    glColor3f(0.0,0.0,1.0);
    drawLine(10,height-310,40,height-340);//line
    drawCirLineLoop(58,height-348,92,height-302);
    drawRectLineLoop(110,height-340,140,height-310);
    drawRTriLineLoop(159,height-341,192,height-308);
    drawETriLineLoop(209,height-341,241,height-309);
    drawStarLineLoop(8,height-392,42,height-358);
    drawPentLineLoop(59,height-391,91,height-359);
    drawRhomLineLoop(109,height-391,141,height-359);



    //for line width
    text(50,height-175,1.0,0.0,0.0,"LINE WIDTH");//to write the text "LINE WIDTH"
    glColor3f(0.0,0.0,0.0);
    glLineWidth(1);
    drawLine(205,height-265,205,height-185);
    glLineWidth(2);
    drawLine(175,height-265,175,height-185);
    glLineWidth(3);
    drawLine(145,height-265,145,height-185);
    glLineWidth(4);
    drawLine(115,height-265,115,height-185);
    glLineWidth(5);
    drawLine(85,height-265,85,height-185);
    glLineWidth(6);
    drawLine(55,height-265,55,height-185);
    glLineWidth(7);
    drawLine(25,height-265,25,height-185);
    glLineWidth(1);

    text(70,height-25,1.0,0.0,0.0,"TOOLS");//to write the text "TOOLS"
    glColor3f(0.60,0.7,1.0);
    //drawLine(200,height-225,400,height-225);  //to draw line partition
     //pencil and its box
     //pencil's while background
    glColor3f(0.9,0.9,1.0);
    drawRectPolygon(25,height-145,85,height-90);//color fill
    drawRectPolygon(95,height-145,155,height-90);//select option
    drawRectPolygon(25,height-85,85,height-30);//pencil
    drawRectPolygon(95,height-85,155,height-30);//eraser
    drawRectPolygon(175,height-145,235,height-90);//clear
    drawRectPolygon(175,height-85,235,height-30);//cut

    //pencil's box border
    glColor3f(0.0,0.0,1.0);
    drawRectLineLoop(25,height-145,85,height-90);
    drawRectLineLoop(95,height-145,155,height-90);
    drawRectLineLoop(25,height-85,85,height-30);
    drawRectLineLoop(95,height-85,155,height-30);
    drawRectLineLoop(175,height-145,235,height-90);//clear
    drawRectLineLoop(175,height-85,235,height-30);//cut
    // for pencil's inside red color
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(35,height-65);
        glVertex2f(65,height-35);
        glVertex2f(75,height-45);
        glVertex2f(45,height-75);
    glEnd();

    //for pencil's black border
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(35,height-65);
        glVertex2f(65,height-35);
        glVertex2f(75,height-45);
        glVertex2f(45,height-75);
    glEnd();

    drawRTriPolygon(35,height-65,45,height-75);
    //for eraser
    glColor3f(0.0,1.0,1.0);
    drawRectPolygon(105,height-65,135,height-45);
    glColor3f(0.0,0.0,0.0);
    drawRectLineLoop(105,height-65,135,height-45);
    drawRectPolygon(135,height-65,145,height-45);
    //for paint
    glColor3f(1.0,0.7,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(65,height-135);
        glVertex2f(45,height-115);
        glVertex2f(55,height-105);
        glVertex2f(75,height-125);
    glEnd();
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(65,height-135);
        glVertex2f(45,height-115);
        glVertex2f(55,height-105);
        glVertex2f(75,height-125);
    glEnd();
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_POLYGON);
        glVertex2f(45,height-115);
        glVertex2f(51,height-111);
        glVertex2f(39,height-111);
        glVertex2f(39,height-125);
        glVertex2f(45,height-125);
    glEnd();

    //for select tool
    glColor3f(0.1,0.1,0.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(105,height-135);
        glVertex2f(105,height-99);
        glVertex2f(145,height-99);
        glVertex2f(145,height-135);
    glEnd();

    //setting line width again
    glLineWidth(1);
    //for undo
    drawRectLineLoop(255,height-65,340,height-90);
    text(260,height-85,1,0,0,"UNDO");
    drawRectLineLoop(255,height-50,340,height-25);
    text(260,height-45,1,0,0,"REDO");

    glColor3f(0.0,0.0,1.0);
    drawRectLineLoop(360,height-50,495,height-25);
    text(365,height-45,1,0,0,"Read R G B");
    drawRectLineLoop(510,height-50,600,height-25);
    text(520,height-45,1,0,0,"OPEN");
    drawRectLineLoop(510,height-65,600,height-90);
    text(520,height-85,1,0,0,"SAVE");
    text(180,height-65,1,0,0,"CUT");
    text(180,height-125,1,0,0,"CLR");

    if(!rflag&&rgbflag)
    {
        text(360,height-90,1,0,0,"R=");
        text(385,height-90,1,0,0,s0);
    }
    else if(!gflag&&rgbflag)
    {
        text(360,height-90,1,0,0,"G=");
        text(385,height-90,1,0,0,s0);
    }
    else if(!bflag&&rgbflag)
    {
        text(360,height-90,1,0,0,"B=");
        text(385,height-90,1,0,0,s0);
    }
    if(openflag||saveflag)
    {
        text(630,height-90,1,0,0,"Enter filename:");
        text(800,height-90,1,0,0,filename);
        glFlush();glutSwapBuffers();
    }
}

/* this is the display callback function */
void display()
{
    int i,cuti=0;
    if (startOfApp == 1)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glutSwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
        glReadPixels(250,0,width-250,height-100,GL_RGB,GL_UNSIGNED_BYTE,imag[0]);
        startOfApp = 0;
    }

    // drawing the menu with all the options
    drawMenu();

    glRasterPos2d(250,0);
    glDrawBuffer(GL_FRONT_AND_BACK);
    glDrawPixels(width-250, height-100, GL_RGB, GL_UNSIGNED_BYTE, imag[saveIndex]);

    glFlush();glutSwapBuffers();
}

/* this is the mouse callback function */
void myMouse(int button,int state,int x, int y)
{
    if(state==GLUT_UP&&button==GLUT_LEFT_BUTTON)
    {
        //to open
        if(x>510&&y>25&&x<600&&y<50)
        {
            openflag=1;
            printf("Clicked on open%d\n",openflag);
            saveflag=0;
            rgbflag=0;
            text(630,height-90,1,0,0,"Enter filename:");
            glFlush();glutSwapBuffers();
        }
        //to save
        else if(x>510&&y>65&&x<600&&y<90)
        {
            printf("Clicked on save\n");
            saveflag=1;
            rgbflag=0;
            openflag=0;
            text(630,height-90,1,0,0,"Enter filename:");
            glutPostRedisplay();
        }

        //for line width
        if (x<230 && x>20 && y<275 && y>185)
        {
            if (x<50)
            {
                linewidth=7;
            }
            else if (x<80)
            {
                linewidth=6;
            }
            else if (x<110)
            {
                linewidth=5;
            }
            else if (x<140)
            {
                linewidth=4;
            }
            else if (x<170)
            {
                linewidth=3;
            }
            else if (x<200)
            {
                linewidth=2;
            }
            else if (x<230)
            {
                linewidth=1;
            }
        }

        //for reading R G B Values
        else if(x>360&&y>25&&x<495&&y<50)
        {
            rgbflag=1;
            saveflag=0;
            openflag=0;
            rflag=0;
            gflag=0;
            bflag=0;
            glutPostRedisplay();
        }

        //setting default color to black
        setColor(COL);
        if(y<595 && y>555)
        {
            if(x>5&&x<45)
                COL=1;
            else if(x>55&&x<95)
                COL=2;
            else if(x>105&&x<145)
                COL=3;
            else if(x>155&&x<195)
                COL=4;
            else if(x>205&&x<245)
                COL=5;
        }
        else if(y<545 && y>505)
        {
            if(x>5&&x<45)
                COL=6;
            else if(x>55&&x<95)
                COL=7;
            else if(x>105&&x<145)
                COL=8;
            else if(x>155&&x<195)
                COL=9;
            else if(x>205&&x<245)
                COL=10;
        }

        //for redo
        else if(x>255&&y>25&&x<340&&y<50)
        {
            redo();
            glutPostRedisplay();
        }

        //for undo
        else if(x>255 && (y>65) && x<340 && (y<90))
        {
            undo();
            glutPostRedisplay();
        }

        //for shape selection
        else if(y<345 && y>305)
        {
            if(x>5 && x<45 )
            {
                ch1=1;
            }
            else if(x>55 &&x<95)
            {
                ch1=2;
            }
            else if(x>105 && x<145)
            {
                ch1=3;
            }
            else if(x>155 && x<195)
            {
                ch1=4;
            }
            else if(x>205&&x<245)
            {
                ch1=5;
            }
        }
        else if(y<395 && y>355)
        {
            if(x>5 && x<45)
            {
                ch1=6;
            }
            else if(x>55 && x<95)
            {
                ch1=7;
            }
            else if(x>105 && x<145)
            {
                ch1=8;
            }
        }
        else if(x>25 && y<85 && x<85 && y>30) //for pencil
        {
            ch1=9;
        }
        else if(x>95&&y<85&&x<155&&y>30) //for eraser
        {
            ch1=10;
        }
        if(x>175&&y<145&&x<235&&y>90)//clear
        {
            ch1=11;
        }
        else if(x>95&&y<145&&x<155&&y>90) //select option
        {
            ch1=12;
        }
        else if(x>175&&y<85&&x<235&&y>30)//cut
        {
            ch1=13;
        }
        else if(x>25&&y>90&&x<85&&y<145) // scanline filling
        {
            ch1 = 14;
        }

        if(ch1)
            ch=ch1;
        switch(ch)
        {
            case 1:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if (x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawLine(xx0,height-yy0,x,height-y);
                    save();
                }
            break;
            case 2:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if (x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawCirLineLoop(xx0,height-yy0,x,height-y);
                    save();
                }
                break;
            case 3:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    xx1=x,yy1=y;
                    glLineWidth(linewidth);
                    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
                        glVertex2f(xx0,height-yy0);
                        glVertex2f(xx0,height-yy1);
                        glVertex2f(xx1,height-yy1);
                        glVertex2f(xx1,height-yy0);
                    glEnd();
                    save();
                }
                break;
            case 4:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawRTriLineLoop(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 5:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawETriLineLoop(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 6:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawStarLineLoop(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 7:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawPentLineLoop(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 8:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawRhomLineLoop(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 9:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    glLineWidth(linewidth);
                    drawLine(xx0,height-yy0,xx1,height-yy1);
                    save();
                }
                break;
            case 10:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    save();
                }
                break;
            case 11:
                reset();
                glutPostRedisplay();
                break;
            case 12:
                if(!flag)
                {
                    if (x>250 && x<width && y>100 && y<height)
                    {
                        xx0=x,yy0=y;
                        flag=1;
                    }
                    cutflag=0;
                }
                else if(flag&&x>250 && x<width && y>100 && y<height)
                {
                    flag=0;
                    xx1=x,yy1=y;
                    glLineWidth(1);
                    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
                        glVertex2f(xx0,height-yy0);
                        glVertex2f(xx0,height-yy1);
                        glVertex2f(xx1,height-yy1);
                        glVertex2f(xx1,height-yy0);
                    glEnd();
                    glFlush();glutSwapBuffers();
                    cutx0=xx0;
                    cutx1=xx1;
                    cuty0=height-yy0;
                    cuty1=height-yy1;
                    flag=0;
                    cutflag=1;
                }
                break;
            case 13:
                if(cutflag)//for cut
                {
                    setColor(10);
                    drawRectPolygon(cutx0,cuty0,cutx1,cuty1);
                    drawRectLineLoop(cutx0,cuty0,cutx1,cuty1);
                    glFlush();glutSwapBuffers();
                    xx0=0,yy0=0,xx1=0,yy1=0;
                    cutflag=0;
                    save();
                }
                break;
        case 14:
            if (x>250 && x<width && y>100 && y<height)
            {
                glReadPixels(x,height-y,1,1,GL_RGB,GL_FLOAT,backColor);
                if (!(backColor[0] == r && backColor[1] == g && backColor[2] == b))
                {
                    scanlinefill(x,height-y);
                    glutSwapBuffers();
                    save();
                }
            }
            break;
        }
    }
    else if(state==GLUT_UP&&button==GLUT_RIGHT_BUTTON)
    {
        flag=0;
        ch=0;
    }
}

/* this is the keyboard callback function */
void myKeyboard(unsigned char key,int x,int y)
{
    int d=0,d1=0;
    d=key-48;
    if(openflag)
    {
        printf("reading open file name\n");
        if(key==13)
        {
            filename[fn]='\0';
            if ((fp = fopen(filename,"r")) == NULL)
			{
				fprintf(stderr,"Failed to open file\n");
			}
			else
            {
                fread(image,(width-250) * (height-100) * 3,1,fp);
                glDrawBuffer(GL_FRONT_AND_BACK);
                glRasterPos2d(250,0);
                glDrawPixels(width-250,height-100,GL_RGB,GL_UNSIGNED_BYTE,image);
                save();
                fclose(fp);
                glFlush();glutSwapBuffers();
            }
            fn=0;
            openflag=0;
            filename[0]='\0';
        }
        if(key==8)
        {
            fn--;
            filename[fn]='\0';
        }
        else
        {
            filename[fn]=key;
            fn++;
            filename[fn]='\0';
        }
    }
    if(saveflag)
    {
        printf("reading save file name\n");
        if(key==13)
        {
            filename[fn]='\0';
            if ((fp = fopen(filename,"w")) == NULL)
			{
				fprintf(stderr,"Failed to save file\n");
			}
			else
            {
                glReadBuffer(GL_FRONT);
                glReadPixels(250, 0, width-250, height-100, GL_RGB, GL_UNSIGNED_BYTE, image);
                fwrite(image,(width-250) * (height-100) * 3,1,fp);
                fclose(fp);
			}
			fn=0;
            saveflag=0;
            filename[0]='\0';
        }
        if(key==8)
        {
            fn--;
            filename[fn]='\0';
        }
        else
        {
            filename[fn]=key;
            fn++;
            filename[fn]='\0';
        }
     }
    if(rgbflag)
    {
        s0[j]=key;
        j++;
        if(key==46)
            decFlag=1;
        else if(key>=48 && key<=57)
        {
            if(!decFlag)
            f=f*10+d;
            if(decFlag)
            {
                d1=d1*10+d;
                coun++;
            }
        }
        f=f+d1/pow(10.0,coun);
        if(key==13)
        {
            if(!rflag)
            {
                rflag=1;
                fr=f;
                f=0;
                coun=0;
                decFlag=0;
            }
            else if(!gflag)
            {
                gflag=1;
                fg=f;
                f=0;
                coun=0;
                decFlag=0;
            }
            else if(!bflag)
            {
                bflag=1;
                fb=f;
                f=0;
                decFlag=0;
                coun=0;
            }
            s0[0]='\0';
            j=0;
        }
        s0[j+1]='\0';

        if(rflag&&gflag&&bflag)
        {
            R1=fr;
            G1=fg;
            B1=fb;
            COL=11;
        }
    }
    glutPostRedisplay();
}

/* this is the passive callback function */
void myPassiveMotion(int xp,int yp)
{
    //xx1=height-xp;yy1=height-yp;
    setColor(COL);
    if(xp<width&&xp>250&&yp<height&&yp>100)
    {
        switch(ch)
        {
            case 1:
                if(flag)
                {
                    glLineWidth(linewidth);
                    drawLine(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 2:
                if(flag)
                {
                    glLineWidth(linewidth);
                    drawCirLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 3:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    glDrawBuffer(GL_FRONT_AND_BACK);glBegin(GL_LINE_LOOP);
                        glVertex2f(xx0,height-yy0);
                        glVertex2f(xx0,height-yy1);
                        glVertex2f(xx1,height-yy1);
                        glVertex2f(xx1,height-yy0);
                    glEnd();
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 4:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawRTriLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 5:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawETriLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 6:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawStarLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 7:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawPentLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 8:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawRhomLineLoop(xx0,height-yy0,xp,height-yp);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
            case 9:
                if(flag)
                {
                    glLineWidth(linewidth);
                    xx1=xp,yy1=yp;
                    drawLine(xx0,height-yy0,xp,height-yp);
                    xx0=xx1,yy0=yy1;
                    glFlush();glutSwapBuffers();
                    //glutPostRedisplay();
                }
                break;
            case 10://eraser
                if(flag && xp>252 && xp<width && yp>102 && yp<height)
                {
                    xx1=xp,yy1=yp;
                    setColor(10);
                    drawRectPolygon(xp-3,height-yp+3,xp+3,height-yp-3);
                    glLineWidth(6);
                    drawLine(xx0,height-yy0,xp,height-yp);
                    xx0=xx1,yy0=yy1;
                    glFlush();glutSwapBuffers();
                }
                break;
            case 12:
                if(flag)
                {
                    glLineWidth(1);
                    xx1=xp,yy1=yp;
                    drawRectLineLoop(xx0,height-yy0,xx1,height-yy1);
                    glFlush();glutSwapBuffers();
                    glutPostRedisplay();
                }
                break;
        }
    }
}

/* this is the reshape callback function */
void myreshape(GLsizei w,GLsizei h)
{
    glMatrixMode(GL_PROJECTION);
   	glLoadIdentity();
   	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,w,h);
   	glFlush();glutSwapBuffers();
	width = w;
   	height = h;
   	reset(); //first choose your window size then start working.
   	glutPostRedisplay();
}

void myinit()
{
    glClearColor(1.0,1.0,1.0,1.0);
    glColor3f(1.0,0.0,0.0);
    glPointSize(10.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0,999.0,0.0,599.0);
}

/* this is the main function (execution starts from here) */
int main(int argc,char** argv)
{
    MAX_WIDTH = glutGet(GLUT_SCREEN_WIDTH);
    MAX_HEIGHT = glutGet(GLUT_SCREEN_HEIGHT);
    CircularQueueSet();
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(0.0,0.0);
    glutCreateWindow("*****EDITOR*****");
    glutDisplayFunc(display);
    glutReshapeFunc(myreshape);
    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);
    glutPassiveMotionFunc(myPassiveMotion);
    myinit();
    glutMainLoop();
}
