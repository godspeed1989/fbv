#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned char * simple_resize(unsigned char * orgin,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*l;
	int i,j,k,ip;
	assert(cr = (unsigned char*) malloc(dx*dy*3));
	l=cr;
	
	for(j=0;j<dy;j++,l+=dx*3)
	{
	p=orgin+(j*oy/dy*ox*3);
	for(i=0,k=0;i<dx;i++,k+=3)
	{
		ip=i*ox/dx*3;
		l[k]=p[ip];
		l[k+1]=p[ip+1];
		l[k+2]=p[ip+2];
	}
	}
	return(cr);
}

unsigned char * alpha_resize(unsigned char * alpha,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*l;
	int i,j,k;
	cr=(unsigned char*) malloc(dx*dy); l=cr;
	
	for(j=0;j<dy;j++,l+=dx)
	{
		p = alpha+(j*oy/dy*ox);
		for(i=0,k=0;i<dx;i++)
			l[k++]=p[i*ox/dx];
	}
	
	return(cr);
}

unsigned char * color_average_resize(unsigned char * orgin,int ox,int oy,int dx,int dy)
{
	unsigned char *cr,*p,*q;
	int i,j,k,l,xa,xb,ya,yb;
	int sq,r,g,b;
	assert(cr=(unsigned char*) malloc(dx*dy*3)); p=cr;
	
	for(j=0;j<dy;j++)
	{
	for(i=0;i<dx;i++,p+=3)
	{
		xa=i*ox/dx;
		ya=j*oy/dy;
		xb=(i+1)*ox/dx; if(xb>=ox) xb=ox-1;
		yb=(j+1)*oy/dy; if(yb>=oy) yb=oy-1;
		for(l=ya,r=0,g=0,b=0,sq=0;l<=yb;l++)
		{
		q=orgin+((l*ox+xa)*3);
		for(k=xa;k<=xb;k++,q+=3,sq++)
		{
			r+=q[0]; g+=q[1]; b+=q[2];
		}
		}
		p[0]=r/sq; p[1]=g/sq; p[2]=b/sq;
	}
	}
	return(cr);
}

unsigned char * rotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	assert(n = (unsigned char*) malloc(ox * oy * 3));
	
	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1) * 3;
			for(y = 0; y<oy; y++, p -= 3)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy * 3)
				{
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy * 3; p = n;
			for(y = ox * oy; y > 0; y--)
			{
				i -= 3;
				p[0] = i[0];
				p[1] = i[1];
				p[2] = i[2];
				p += 3;
			}
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p += 3)
			{
				unsigned char * r = p + ((ox * 3) * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy * 3;
					r[0] = *(i++);
					r[1] = *(i++);
					r[2] = *(i++);
				}
			}
			break;
	}
	return(n);
}

unsigned char * alpha_rotate(unsigned char *i, int ox, int oy, int rot)
{
	unsigned char * n, * p;
	int x, y;
	assert(n = (unsigned char*) malloc(ox * oy));
	
	switch(rot)
	{
		case 1: /* 90 deg right */
			p = n + (oy - 1);
			for(y = 0; y<oy; y++, p --)
			{
				unsigned char * r = p;
				for(x = 0; x<ox; x++, r += oy)
					*r = *(i++);
			}
			break;
		case 2: /* 180 deg */
			i += ox * oy; p = n;
			for(y = ox * oy; y > 0; y--)
				*(p++) = *(i--);
			break;
		case 3: /* 90 deg left */
			p = n;
			for(y = 0; y<oy; y++, p++)
			{
				unsigned char * r = p + (ox * oy);
				for(x = 0; x<ox; x++)
				{
					r -= oy;
					*r = *(i++);
				}
			}
			break;
	}
	return(n);
}

