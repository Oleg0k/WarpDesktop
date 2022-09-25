#include <windows.h>	// Header File For Windows
#include <gl\gl.h>		// Header File For The OpenGL32 Library
#include <gl\glu.h>		// Header File For The GLu32 Library
#include <math.h>

#include "glaux.h"	// Header File For The GLaux Library


//#include "resource.h"
#include "Mathematics.h"

HWND hWndMain;

const float PI=3.1415926535897932384626433832795f;
const float RadPerDeg = PI/180.0f;


const int Num = 1440;
const float StepDegrees = 1.0f;

void* pDestBegin;

const float DeltaMin=0.05f;

typedef struct _OGLRGBTRIPLE { 
    BYTE rgbRed;
    BYTE rgbGreen;
  	BYTE rgbBlue;
} OGLRGBTRIPLE;

int MaxQuPoints, texOffset;
// long FPSCounter;

BOOL  flagAmplitudeChanged=true; // Was Amplitude changed?

GLfloat	xrot;			// X Rotation
GLfloat	yrot;			// Y Rotation
GLfloat	zrot;			// Z Rotation
GLfloat xspeed = 0.0f;			// X Rotation Speed
GLfloat yspeed = 0.0f;			// Y Rotation Speed
GLfloat zspeed = 0.0f;			// Y Rotation Speed

GLfloat Amplitude=0.00f; // Amplitude of vibration

GLfloat	z=-0.906f;		// Depth Into The Screen

GLuint	filter=1;	// Which Filter To Use
GLuint	texture[2];		// Storage for 3 textures
GLfloat NormalsArray[Num+1], Deltatex[Num];

BOOL	light=true;			// Lighting ON/OFF
BOOL	lp;				// L Pressed?
BOOL	fp;				// F Pressed?

GLpoint aVerticesArray[Num+1];

void InitArrays()
{	int i,j;
  GLfloat X0, Y0, X1, Y1, deltaX, CurrTextured;
	X0=0.0;
	Y0=0.0;
  aVerticesArray[0].x = 0.0;
	aVerticesArray[0].y =	0.0;

	j=0;
	CurrTextured=0.0; // Счетчик длины текстуры
	deltaX = 1.0f/(Num);
	for(i=0;i<=Num;i++)
	{	
		aVerticesArray[i].x = deltaX*i;
		aVerticesArray[i].y = Amplitude*sinf((i*StepDegrees)*RadPerDeg);

		X1 = aVerticesArray[i].x;
		Y1 = aVerticesArray[i].y;
		Deltatex[i] = sqrtf(  powf((X1-X0),2)+powf((Y1-Y0),2)  );
		X0=X1;
		Y0=Y1;
		if (CurrTextured<=1.0)
		{	j++;
			CurrTextured=CurrTextured+Deltatex[i];
		}

	};
	flagAmplitudeChanged=false;
	Deltatex[0]=0.0;
	MaxQuPoints=j;
};

GLvoid DrawGLScene(GLvoid)
{	int i, j;
  GLfloat X0, Y0, X1, Y1, deltaX, X0tex;
	GLpoint v1, v2, v3, vOut;
	GLfloat vNormal[3];
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glLoadIdentity();										// Reset The View
	glTranslatef(0.0f,0.0f, z);

	glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	glRotatef(zrot, 0.0f, 0.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture[filter]);

//	auxSolidCube(0.2);

  if (flagAmplitudeChanged) InitArrays();
	deltaX = 1.0f/(Num);
	X0=0.0f;
//  X0=aVerticesArray[texOffset].x;
	Y0=aVerticesArray[texOffset].y;
	Deltatex[0]=Deltatex[Num-1];;
	X0tex=0.0f;
	for(i=0;i<=MaxQuPoints;i++)
	{	j=(i+texOffset) % (Num);
		X1 = deltaX*i;
//		X1=aVerticesArray[i].x;
	  Y1=aVerticesArray[j].y;

// Определяем три вершины для получения вектора нормали:
		v1.x=-0.5f+X0; v1.y= 0.375f; v1.z=Y0;
		v2.x=-0.5f+X1; v2.y= 0.375f; v2.z=Y1;
		v3.x=-0.5f+X1; v3.y=-0.375f; v3.z=Y1;
		ComputeFaceNormal (&v2, &v1, &v3, &vOut);
		vNormal[0]=vOut.x;
		vNormal[1]=vOut.y;
		vNormal[2]=vOut.z;

// Здесь рисуем полигон:
	  glBegin(GL_QUADS);

			glNormal3fv(vNormal);
			glTexCoord2d(X0tex, 0.0);		           glVertex3d(-0.5+X0,  0.375, Y0);
			glTexCoord2d(X0tex+Deltatex[j], 0.0);  glVertex3d(-0.5+X1,  0.375, Y1);
			glTexCoord2d(X0tex+Deltatex[j], 1.0);  glVertex3d(-0.5+X1, -0.375, Y1);
			glTexCoord2d(X0tex, 1.0);			         glVertex3d(-0.5+X0, -0.375, Y0);
		glEnd();
		X0=X1;
		Y0=Y1;
		X0tex=X0tex+Deltatex[j];
	}; // for
	glFinish();
	texOffset++;

	if (texOffset==Num) texOffset=0;

	xrot += xspeed;
	yrot += yspeed;
	zrot += zspeed;

}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures(HBITMAP hBitmapDesk)
{
   // Get bitmap info.
	int i,j;
  BITMAP BM;
  GetObject (hBitmapDesk, sizeof (BM), &BM);
	if (BM.bmBitsPixel==32)
 	{
		RGBQUAD *pSrc, *pSrcBegin;
	  OGLRGBTRIPLE* pDest = (OGLRGBTRIPLE*)malloc(BM.bmWidth*BM.bmHeight*sizeof(OGLRGBTRIPLE));
	  pSrc=(RGBQUAD*) malloc(BM.bmWidth*BM.bmHeight*sizeof(RGBQUAD));
		pSrcBegin=pSrc;
		pDestBegin= pDest;
		i=GetBitmapBits(hBitmapDesk,BM.bmWidth*BM.bmHeight*sizeof(RGBQUAD),pSrc);
			int widthDiff = BM.bmWidthBytes - BM.bmWidth*sizeof(RGBQUAD) ;
			for(j = 0 ; j < BM.bmHeight ; j++)
			{
				for(i = 0 ; i < BM.bmWidth ; i++)
				{
					pDest->rgbRed = pSrc->rgbRed ;
					pDest->rgbGreen = pSrc->rgbGreen ;
					pDest->rgbBlue = pSrc->rgbBlue ;
					pDest++ ;
					pSrc++ ;			
				}
				pSrc = (RGBQUAD*)( (BYTE*)pSrc + widthDiff) ;
			};
	free(pSrcBegin);
	};

	if (BM.bmBitsPixel==24)
 	{
		RGBTRIPLE *pSrc, *pSrcBegin;
	  OGLRGBTRIPLE* pDest = (OGLRGBTRIPLE*)malloc(BM.bmWidth*BM.bmHeight*sizeof(OGLRGBTRIPLE));
	  pSrc=(RGBTRIPLE*) malloc(BM.bmWidth*BM.bmHeight*sizeof(RGBTRIPLE));
		pSrcBegin=pSrc;
		pDestBegin= pDest;
		i=GetBitmapBits(hBitmapDesk,BM.bmWidth*BM.bmHeight*sizeof(RGBTRIPLE),pSrc);
			int widthDiff = BM.bmWidthBytes - BM.bmWidth*sizeof(RGBTRIPLE) ;
			for(j = 0 ; j < BM.bmHeight ; j++)
			{
				for(i = 0 ; i < BM.bmWidth ; i++)
				{
					pDest->rgbRed = pSrc->rgbtRed ;
					pDest->rgbGreen = pSrc->rgbtGreen ;
					pDest->rgbBlue = pSrc->rgbtBlue ;
					pDest++ ;
					pSrc++ ;			
				}
				pSrc = (RGBTRIPLE*)( (BYTE*)pSrc + widthDiff) ;
			};
	free(pSrcBegin);
	};

  glGenTextures(1, &texture[1]);
// Set up all the the texture mapping params.

//  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BM.bmWidth, BM.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, pDestBegin);

	// Create Nearest Filtered Texture
/*
	glGenTextures(3, &texture[0]);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BM.bmWidth, BM.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, pDestBegin);
*/
	// Create Linear Filtered Texture
	glBindTexture(GL_TEXTURE_2D, texture[1]);
//  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BM.bmWidth, BM.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, pDestBegin);
/*
	// Create MipMapped Texture
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, BM.bmWidth, BM.bmHeight, GL_RGB, GL_UNSIGNED_BYTE, pDestBegin);
*/
};

GLvoid InitGL(GLsizei Width, GLsizei Height, HBITMAP hBitmapDesk)	// This Will Be Called Right After The GL Window Is Created
{
	GLfloat LightAmbient[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightDiffuse[]=		{ 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat LightPosition[]=	{ 0.99f, 0.8f, 1.0f, -1.5f };

	LoadGLTextures(hBitmapDesk);							// Load The Texture(s)

	glEnable(GL_TEXTURE_2D);					// Enable Texture Mapping

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);							// Enables Clearing Of The Depth Buffer
//	glDepthFunc(GL_LESS);						// The Type Of Depth Test To Do
	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
	glShadeModel(GL_SMOOTH);					// Enables Smooth Color Shading

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();							// Reset The Projection Matrix

	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height, 0.5f, 10.0f);	// Calculate The Aspect Ratio Of The Window

	glMatrixMode(GL_MODELVIEW);
//glEnable(GL_COLOR_MATERIAL);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
//	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glDisable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  GLfloat mat1[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE , mat1);

	GLfloat mat2[4] = { 0.9f, 0.9f, 0.9f, 1.0f };    
  // Определяем зеркальный цвет материала
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat2);
//	glFrontFace(GL_CW);
    // Определяем степень зеркального отражения
  glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

}
				 
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
	if (Height==0)								// Prevent A Divide By Zero If The Window Is Too Small
		Height=1;

	glViewport(0, 0, Width, Height);			// Reset The Current Viewport And Perspective Transformation

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)Width/(GLfloat)Height*0.833333333333, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void KeysControl(BOOL* keys)
{
			if (keys[VK_ESCAPE]) SendMessage(hWndMain, WM_CLOSE,0,0);
			if (keys['L'] /*&& !lp*/)
			{
				lp=TRUE;
				light=!light;
				if (!light)
				{
					glDisable(GL_LIGHTING);
				}
				else
				{
					glEnable(GL_LIGHTING);
				}
			}
			if (!keys['L'])
			{
				lp=FALSE;
			}
/*			if (keys['F'] && !fp)
			{
				fp=TRUE;
				filter+=1;
				if (filter>2)
				{
					filter=0;
				}
			}
			if (!keys['F'])
			{
				fp=FALSE;
			}*/
			if (keys['W'])
			{
				Amplitude+=DeltaMin/10;
				flagAmplitudeChanged=true;
			}
			if (keys['S'])
			{
				Amplitude-=DeltaMin/10;
				flagAmplitudeChanged=true;
			}

			if (keys[VK_PRIOR])
			{
				z-=DeltaMin/10;
			}
			if (keys[VK_NEXT])
			{
				z+=DeltaMin/10;
			}
			if (keys[VK_UP])
			{
				xspeed-=DeltaMin;
			}
			if (keys[VK_DOWN])
			{
				xspeed+=DeltaMin;
			}
			if (keys[VK_RIGHT])
			{
				yspeed+=DeltaMin;
			}
			if (keys[VK_LEFT])
			{
				yspeed-=DeltaMin;
			}
			if (keys[VK_HOME])
			{
				zspeed += DeltaMin;
			}
			if (keys[VK_END])
			{
				zspeed -= DeltaMin;
			}

}