/*
 * xsct - X11 set color temperature
 *
 * Original code published by Ted Unangst:
 * http://www.tedunangst.com/flak/post/sct-set-color-temperature
 *
 * Modified by Fabian Foerg in order to:
 * - compile on Ubuntu 14.04
 * - iterate over all screens of the default display and change the color
 *   temperature
 * - fix memleaks
 * - clean up code
 * - return EXIT_SUCCESS
 *
 * Public domain, do as you wish.
 *
 * Compile the code using the following command:
 * gcc -Wall -Wextra -Werror -pedantic -std=c99 -O2 -I /usr/X11R6/include xsct.c -o xsct -L /usr/X11R6/lib -lX11 -lXrandr -lm -s
 *
 */

#ifndef __XSCT_H
#define __XSCT_H

#include <stdio.h>

#define TEMPERATURE_NORM    6500
#define TEMPERATURE_ZERO    700

int
x11_get_temperature();

int
x11_set_temperature(int temp);

#endif /* __XSCT_H */