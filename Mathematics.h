#define sqr(a) ((a) * (a))

typedef double GLdouble;
typedef float GLfloat;

struct GLpoint {
	GLfloat x, y, z;
};

void VectorOffset (GLpoint *pIn, GLpoint *pOffset, GLpoint *pOut);
void VectorGetNormal (GLpoint *a, GLpoint *b, GLpoint *pOut);
bool VectorNormalize (GLpoint *pIn, GLpoint *pOut);
bool ComputeFaceNormal (GLpoint *p1, GLpoint *p2, GLpoint *p3, GLpoint *pOut);
