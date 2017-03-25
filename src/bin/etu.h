#ifndef _ETU_H
#define _ETU_H

#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <argp.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <Imlib2.h>

#define PACKAGE "etu"

void get_images_attr (char *);
void update_image    (char *, char *, int, int, int);
void update_rescaled (char *, char *);
void scale_imlib2    (char *, char *, int, int, int);
void usage           (void);
int  check_handle    (char *);
char *fullpath       (char *, char *);
char *gettype        (char *);
void getimgattr      (char *);

#endif
