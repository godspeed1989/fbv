#include "config.h"

#ifdef FBV_SUPPORT_BMP
#include "fbv.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BMP_TORASTER_OFFSET   10
#define BMP_SIZE_OFFSET       18
#define BMP_BPP_OFFSET        28
#define BMP_RLE_OFFSET        30
#define BMP_COLOR_OFFSET      54

#define fill4B(a)	( ( 4 - ( (a) % 4 ) ) & 0x03)

struct color
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

int fh_bmp_id(char *name)
{
	int fd;
	char id[2];
	fd = open(name, O_RDONLY);
	if(fd == -1) return(0);
	read(fd, id, 2);
	close(fd);
	if(id[0]=='B' && id[1]=='M') return(1);
	return(0);
}


void fetch_pallete(int fd, struct color pallete[], int count)
{
	unsigned char buff[4];
	int i;

	lseek(fd, BMP_COLOR_OFFSET, SEEK_SET);
	for (i=0; i<count; i++)
	{
		read(fd, buff, 4);
		pallete[i].red = buff[2];
		pallete[i].green = buff[1];
		pallete[i].blue = buff[0];
	}
	return;
}


int fh_bmp_load(char *name, unsigned char *buffer, unsigned char **alpha, int x,int y)
{
	int fd, bpp, raster, i, j, k, skip;
	unsigned char buff[4];
	unsigned char *wr_buffer = buffer + x*(y-1)*3;
	struct color pallete[256];

	fd = open(name, O_RDONLY);
	if (fd == -1) {
		return(FH_ERROR_FILE);
	}

	if (lseek(fd, BMP_TORASTER_OFFSET, SEEK_SET) == -1) {
		return(FH_ERROR_FORMAT);
	}
	read(fd, buff, 4);
	raster = buff[0] + (buff[1]<<8) + (buff[2]<<16) + (buff[3]<<24);

	if (lseek(fd, BMP_BPP_OFFSET, SEEK_SET) == -1) {
		return(FH_ERROR_FORMAT);
	}
	read(fd, buff, 2);
	bpp = buff[0] + (buff[1]<<8);

	switch (bpp)
	{
		case 1: /* monochrome */
			skip = fill4B(x/8+(x%8?1:0));
			lseek(fd, raster, SEEK_SET);
			for (i=0; i<y; i++) {
				for (j=0; j<x/8; j++) {
					read(fd, buff, 1);
					for (k=0; k<8; k++) {
						if (buff[0] & 0x80) {
							*wr_buffer++ = 0xff;
							*wr_buffer++ = 0xff;
							*wr_buffer++ = 0xff;
						} else {
							*wr_buffer++ = 0x00;
							*wr_buffer++ = 0x00;
							*wr_buffer++ = 0x00;
						}
						buff[0] = buff[0]<<1;
					}

				}
				if (x%8) {
					read(fd, buff, 1);
					for (k=0; k<x%8; k++) {
						if (buff[0] & 0x80) {
							*wr_buffer++ = 0xff;
							*wr_buffer++ = 0xff;
							*wr_buffer++ = 0xff;
						} else {
							*wr_buffer++ = 0x00;
							*wr_buffer++ = 0x00;
							*wr_buffer++ = 0x00;
						}
						buff[0] = buff[0]<<1;
					}

				}
				if (skip) {
					read(fd, buff, skip);
				}
				wr_buffer -= x*6; /* backoff 2 lines - x*2 *3 */
			}
			break;
		case 4: /* 4bit palletized */
			skip = fill4B(x/2+x%2);
			fetch_pallete(fd, pallete, 16);
			lseek(fd, raster, SEEK_SET);
			for (i=0; i<y; i++) {
				for (j=0; j<x/2; j++) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					buff[2] = buff[0] & 0x0f;
					*wr_buffer++ = pallete[buff[1]].red;
					*wr_buffer++ = pallete[buff[1]].green;
					*wr_buffer++ = pallete[buff[1]].blue;
					*wr_buffer++ = pallete[buff[2]].red;
					*wr_buffer++ = pallete[buff[2]].green;
					*wr_buffer++ = pallete[buff[2]].blue;
				}
				if (x%2) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					*wr_buffer++ = pallete[buff[1]].red;
					*wr_buffer++ = pallete[buff[1]].green;
					*wr_buffer++ = pallete[buff[1]].blue;
				}
				if (skip) {
					read(fd, buff, skip);
				}
				wr_buffer -= x*6; /* backoff 2 lines - x*2 *3 */
			}
			break;
		case 8: /* 8bit palletized */
			skip = fill4B(x);
			fetch_pallete(fd, pallete, 256);
			lseek(fd, raster, SEEK_SET);
			for (i=0; i<y; i++) {
				for (j=0; j<x; j++) {
					read(fd, buff, 1);
					*wr_buffer++ = pallete[buff[0]].red;
					*wr_buffer++ = pallete[buff[0]].green;
					*wr_buffer++ = pallete[buff[0]].blue;
				}
				if (skip) {
					read(fd, buff, skip);
				}
				wr_buffer -= x*6; /* backoff 2 lines - x*2 *3 */
			}
			break;
		case 16: /* 16bit RGB */
			return(FH_ERROR_FORMAT);
			break;
		case 24: /* 24bit RGB */
			skip = fill4B(x*3);
			lseek(fd, raster, SEEK_SET);
			for (i=0; i<y; i++) {
				for (j=0; j<x; j++) {
					read(fd, buff, 3);
					*wr_buffer++ = buff[2];
					*wr_buffer++ = buff[1];
					*wr_buffer++ = buff[0];
				}
				if (skip) {
					read(fd, buff, skip);
				}
				wr_buffer -= x*6; /* backoff 2 lines - x*2 *3 */
			}
			break;
		default:
			return(FH_ERROR_FORMAT);
	}

	close(fd);
	return(FH_ERROR_OK);
}

int fh_bmp_getsize(char *name,int *x,int *y)
{
	int fd;
	unsigned char size[4];

	fd = open(name, O_RDONLY);
	if (fd == -1) return(FH_ERROR_FILE);
	if (lseek(fd, BMP_SIZE_OFFSET, SEEK_SET) == -1) return(FH_ERROR_FORMAT);

	read(fd, size, 4);
	*x = size[0] + (size[1]<<8) + (size[2]<<16) + (size[3]<<24);
	read(fd, size, 4);
	*y = size[0] + (size[1]<<8) + (size[2]<<16) + (size[3]<<24);

	close(fd);
	return(FH_ERROR_OK);
}
#endif /*FBV_SUPPORT_BMP*/

