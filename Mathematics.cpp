#include <math.h>
#include "Mathematics.h"

// Offset pIn by pOffset into pOut
void VectorOffset (GLpoint *pIn, GLpoint *pOffset, GLpoint *pOut)
{
	pOut->x = pIn->x - pOffset->x;
	pOut->y = pIn->y - pOffset->y;
	pOut->z = pIn->z - pOffset->z;
}


// Compute the cross product a X b into pOut
void VectorGetNormal (GLpoint *a, GLpoint *b, GLpoint *pOut)
{
	pOut->x = a->y * b->z - a->z * b->y;
	pOut->y = a->z * b->x - a->x * b->z;
	pOut->z = a->x * b->y - a->y * b->x;
}


// Normalize pIn vector into pOut
bool VectorNormalize (GLpoint *pIn, GLpoint *pOut)
{
	GLfloat len = (GLfloat)(sqrt(sqr(pIn->x) + sqr(pIn->y) + sqr(pIn->z)));
	if (len)
	{
		pOut->x = pIn->x / len;
		pOut->y = pIn->y / len;
		pOut->z = pIn->z / len;
		return true;
	}
	return false;
}


// Compute p1,p2,p3 face normal into pOut
bool ComputeFaceNormal (GLpoint *p1, GLpoint *p2, GLpoint *p3, GLpoint *pOut)
{
	// Uses p2 as a new origin for p1,p3
	GLpoint a;
	VectorOffset(p3, p2, &a);
	GLpoint b;
	VectorOffset(p1, p2, &b);
	// Compute the cross product a X b to get the face normal
	GLpoint pn;
	VectorGetNormal(&a, &b, &pn);
	// Return a normalized vector
	return VectorNormalize(&pn, pOut);
}
