// Image data structure. Created via GIMP.

#ifndef SMILE_IMAGE_H_
#define SMILE_IMAGE_H_

extern const struct {
	unsigned int 	 width;
	unsigned int 	 height;
	unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
	char         	*comment;
	unsigned char	 pixel_data[320 * 240 * 2 + 1];
} gimp_image;


#endif
