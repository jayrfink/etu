
#include "etu.h"

int main(int argc, char **argv)
{
	short int iflag;  /* information attributes only flag            */
	int  c;           /* opt counter                                 */
	int  interval;    /* interval for daemon mode                    */
	int  dst_height;  /* height of the destination image(s)          */
	int  dst_quality; /* quality of the destination image(s)         */
	int  dst_width;   /* width of the destination image(s)           */
	char *dst_dp;     /* The directory that new images will be in    */
	char *src_dp;     /* Where the source images live                */
	char *src_fp;     /* Individial handle for one input             */
	char *dst_fp;     /* Individual file handle for output           */
	char *type;       /* Image type by string                        */

	/* Defaults */
	iflag       = 0;
	interval    = 0;
	src_fp      = NULL;
	dst_fp      = NULL;
	src_dp      = NULL;
	dst_dp      = NULL;
	dst_height  = 96;
	dst_quality = 75;
	dst_width   = 128;
	type	    = NULL;

	/* 
	 * Options parsing:
	 * d -dir     Destination directory for a set of input images
     * D          Put etu in daemon mode
	 * h -height  Height of the destination image(s)
	 * f -file    Input image file
     * i          Get informational attributes of image(s) only
	 * o -output  Output file
	 * q -quality Set the quality of the destination image(s)
	 * s -src     Source directory of original files
	 * w -width   Width of the destination image(s)
	 */
	while (1) {
		static struct option long_options[] = {
			{"daemon",  required_argument,	0,	'D'},
			{"dir",		required_argument,	0,	'd'},
			{"height",	required_argument,	0,	'h'},
			{"file",	required_argument,	0,	'f'},
			{"info",    no_argument,     	0,	'i'},
			{"output",	required_argument,	0,	'o'},
			{"quality",	required_argument,	0,	'q'},
			{"src",		required_argument,	0,	's'},
			{"width",	required_argument,	0,	'w'},
			{0,0,0,0}
		};

		int option_index = 0;

		c = getopt_long (argc, argv, "D:d:f:h:io:q:s:w:", 
							long_options, &option_index);

		if (c == -1) 
			break;

		switch (c) {
			case 'D':
				interval = atoi(optarg);
				break;
			case 'd':
				dst_dp = optarg;
				break;
			case 'h':
				dst_height = atoi(optarg);
				break;
			case 'f':
				src_fp = optarg;
				break;
			case 'i':
				iflag = 1;
				break;
			case 'o':
				dst_fp = optarg;
				break;
			case 'q':
				dst_quality = atoi(optarg);
				break;
			case 's':
				src_dp = optarg;
				break;
			case 'w':
				 dst_width = atoi(optarg);
				break;
			default:
				usage();
				return 1;
				break;
		  }
	}

	/* If we only want attributes of a single file get them and exit */
	if ((iflag) && (src_dp == NULL)) {
		if (src_fp != NULL) 
			getimgattr(src_fp);
		else  {
			fprintf(stderr, "Could not find image %s\n", src_fp);
			return 1;
		}

		return 0;
	}

	/* Run through a battery of checks before calling the main updater */
	if ((src_fp != NULL) && (dst_fp != NULL))  {

		type = gettype(src_fp);
		scale_imlib2(src_fp, dst_fp, dst_height, dst_quality, dst_width);

		return 0;
	  } 

	if (check_handle(src_dp) != 0)  {
		fprintf(stderr, "No input directory specified\n");
		usage();
		return 1;
	}

	/* If looking for attributes on all files in a directory do so and exit */
	if (iflag) {
		get_images_attr(src_dp);
		return 0;
	}

	if (check_handle(dst_dp) != 0) 
		if (mkdir(dst_dp, 0755)) {
			fprintf(stderr,
				"Could not create directory %s\n", dst_dp);
			return 1;
		}

	/* Fall through. At this point a src and dst dir should be established.
	   Now just run the updater to see what needs to be generated */

	if (interval) {
		pid_t pid, sid;

		pid = fork();

		if (pid < 0) {
			exit (EXIT_FAILURE);
		} else if (pid > 0) {
			exit (EXIT_SUCCESS);
		}

		umask (0);

		sid = setsid();

		if (sid < 0) 
			exit (EXIT_FAILURE);

		if ((chdir("/")) < 0)
			exit (EXIT_FAILURE);

		while (1) {
			update_image(src_dp, dst_dp, dst_height, dst_quality, dst_width);
			update_rescaled(src_dp, dst_dp);
			sleep(interval);
		}
		
		exit(EXIT_SUCCESS);
	} else {
		update_image(src_dp, dst_dp, dst_height, dst_quality, dst_width);
		update_rescaled(src_dp, dst_dp);
	}

	return 0; 
}

/*
 * get_images_attr - Using a source directory call getimgattr for each
 * file in it.
 */
void get_images_attr(char *images)
{
	int i;
	char src_path[PATH_MAX];
	struct dirent *src_dp;
	DIR * src_dp_handle;

	src_dp_handle = opendir(images);

	i = 0;

	while (src_dp = (readdir(src_dp_handle))) {
		if (i > -1) {
			strncpy(src_path, fullpath(src_dp->d_name, images), 
			  sizeof(src_dp->d_name) + 1);
			getimgattr(src_path);
		}

		i++;
	}

	closedir(src_dp_handle);
}
	
/*
 * update_image - Using a source directory of jpeg files, check a destination
 * directory of jpeg formatted images     to see if they need updated. Also
 * set the height, quality, and width of each jpeg formatted destination image
 */
void update_image(char *images, char *dst, int height, int quality, int width)
{
	int i;
	char dst_path[PATH_MAX];
	char src_path[PATH_MAX];
	char * type;
	struct dirent *src_dp;
	DIR * src_dp_handle;

	src_dp_handle = opendir(images);

	i = 0;

	while (src_dp = (readdir(src_dp_handle)))  {
		if (i > -1)  {
			strncpy(dst_path, fullpath(src_dp->d_name, dst),
				sizeof(src_dp->d_name) + 1);

			if (check_handle(dst_path) != 0)  {
				strncpy(src_path,
					fullpath(src_dp->d_name, images),
					sizeof(src_dp->d_name) + 1);

				scale_imlib2(src_path, dst_path, height, quality, width);
			} 
		}

		i++;
	 }

	closedir(src_dp_handle);
}

/*
 * backcheck - Check to see if any files where deleted in the source directory
 * that also need to be deleted in the rescaled directory.
 */
void update_rescaled(char *images, char *dst)
{
	int i;
	char dst_path[PATH_MAX];
	char src_path[PATH_MAX];
	struct dirent *dst_img;
	DIR * dst_img_handle;

	dst_img_handle = opendir(dst);

	i = 0;

	while (dst_img = (readdir(dst_img_handle))) {
		if (i > -1) {
			strncpy(src_path, fullpath(dst_img->d_name, images),
				sizeof(dst_img->d_name) + 1);

			if (check_handle(src_path) != 0) {
				strncpy(dst_path, fullpath(dst_img->d_name, dst),
					sizeof(dst_img->d_name) + 1);

				unlink(dst_path);
			}

		}

		i++;
	}

	closedir(dst_img_handle);
}

/*
 * scale_imlib2 - Rescale an image.
 */
void scale_imlib2(char *src, char *dst, int height, int quality, int width)
{
	Imlib_Image in_img, out_img;

	in_img = imlib_load_image(src);
	if (!in_img) {
		fprintf(stderr, "Unable to load %s\n", src);
		exit(1);
	}

	imlib_context_set_image(in_img);

	out_img = imlib_create_cropped_scaled_image(0,0, imlib_image_get_width(),
												 imlib_image_get_height(), 
												 width, height);

	if (!out_img) {
		fprintf(stderr, "Failed to create scaled image\n");
		exit(1);
	}

	imlib_context_set_image(out_img);
	imlib_save_image(dst);
}

/*
 * usage - A Simple usage print.
 */
void usage(void)
{
	printf( PACKAGE " [options][arguments]\n"
			PACKAGE "[-D|--daemonize interval][-i|--info]\n"
			PACKAGE "[-d|--dir   dir][-s|--src][-h|--height height]\n"
			PACKAGE "[-w|--width width][-q|--quality percent]\n"
			PACKAGE "[-f|--file  filename][-o|--output filename]\n"
					"Single file Options:\n"
					" -f|--file   file  Single input image filename\n"
					" -o|--output file  Output image filename\n"
					"Directory Options:\n"
					" -s|--src dir   Original images directory\n"
					" -d|--dir dir   Output directory\n"
					"Global  Options:\n"
					" -h|--height  size  Height in pixels      default: 96px\n"
					" -q|--quality level Quality level (1-100) default: 75px\n"
					" -w|--width   size  Width in pixels       default: 128px\n"
                    "Misc Options:\n"
                    " -i|--info  Print attributes of image and exit\n"
	);
}

/*
 * check_handle - Directory exists? Return 1 if it does not. 0 if it does.
 */
int check_handle(char *dir)
{
	struct stat statbuf;

	if (stat(dir, &statbuf) != 0)
		return 1;

	return 0;
}

/*
 * fullpath - Build a full path to a single file. Handy for stats.
 */
char *fullpath(char *file, char *dir)
{
	static char path[PATH_MAX];

	strcpy(path, dir);
	strcat(path, "/");
	strcat(path, file);

	return (path);
}

/*
 * gettype - Get the image type and return it.
 */
char *gettype(char * img_src)
{
	char *image;
	char *format;

	image = imlib_load_image(img_src);
	if (image == NULL) return NULL;
	imlib_context_set_image(image);
	if (( format = imlib_image_format()) == NULL) {
			fprintf(stderr, "Internal error\n");
			return (NULL);
	} 

	imlib_free_image();
	return (format);
}

/* 
 * getimgattr - Get image attributes
 */
void getimgattr(char *img_src) 
{
	int width;
	int height;
	char *image;
	char *type;

	type = gettype(img_src);
	image = imlib_load_image(img_src);
	if (image == NULL) exit (0);
	imlib_context_set_image(image);

	height = imlib_image_get_height();
	width = imlib_image_get_width();

	printf("File:   %s\n", img_src);
	printf("Height: %i\n", height);
	printf("Width:  %i\n", width);
    printf("Type:   %s\n", type);

	imlib_free_image();
}

