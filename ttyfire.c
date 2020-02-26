#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* default size */
int FIRE_WIDTH=80;
int FIRE_HEIGHT=24;

/*
typedef struct _rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;
*/


uint8_t *firePixels;

char * ANSI_colors[7]  = {
    "0;30m",
    "0;31m",
    "0;91m",
    "0;33m",
    "0;93m",
    "1;97m",
    "0;37m",
};

int setup();
int hostFrame();

void detect_ttysize(int dummy)
{
    struct winsize w;
		int i = 0;
    ioctl(0, TIOCGWINSZ, &w);

		FIRE_WIDTH=w.ws_col;
		FIRE_HEIGHT=w.ws_row;

		firePixels = realloc(firePixels, FIRE_WIDTH * FIRE_HEIGHT);

		/* re-seed */

    for (i = 0 ; i < FIRE_HEIGHT*FIRE_WIDTH; i++) {
        firePixels[i] = 0;
    }

    for (i = 0; i < FIRE_WIDTH; i++) {
        firePixels[(FIRE_HEIGHT-1)*FIRE_WIDTH + i] = 6;
    }

}

int main(int argc, char *argv[])
{
    setup();
    while (1) {
        hostFrame();
        usleep(25000);
    }
    exit(1);
}

void intHandler(int dummy) {

	/* show cursor */
	printf("\e[?25h");
	/* re-enable line wrap */
    printf("\x1b[7h");

	/* clear attributes */ 
    printf("\x1b[0m");
    /* move to home, clear the terminal */
    printf("\x1b[H\x1b[J");
	  exit(0);
}


int setup()
{

		detect_ttysize(0);
		signal(SIGINT, intHandler);
		signal(SIGWINCH, detect_ttysize);

		/* hide cursor */

		printf("\x1b[?25l");

    /* disable line wrap */
    printf("\x1b[7l");
    /* move to home, clear the terminal */
    printf("\x1b[H\x1b[J");
    return 0;

}

int spreadFire(uint32_t src) {
    uint8_t pixel = firePixels[src];
    if(pixel == 0) {
        firePixels[src - FIRE_WIDTH] = 0;
    } else {
        uint32_t randIdx = (uint32_t) round(random() % 3) & (uint32_t )3;
        uint32_t  dst = src - randIdx + 1;
        if (dst >= FIRE_WIDTH) {
            firePixels[dst - FIRE_WIDTH ] = pixel - (randIdx & 1);
        }
    }
    return 0;
}

int doFire()
{
    int x = 0;
    int y = 0;
    for(x=0 ; x < FIRE_WIDTH; x++) {
        for (y = 1; y < FIRE_HEIGHT; y++) {
            spreadFire(y * FIRE_WIDTH + x);
        }
    }
    return 0;
}



int hostFrame()
{
    int x = 0;
    int y = 0;
    doFire();

    printf("\x1b[H");
    for(y = 0; y < FIRE_HEIGHT;  y++) {
        for(x = 0; x < FIRE_WIDTH; x++) {
            uint8_t index = firePixels[y * FIRE_WIDTH + x];
            if (!index) {
                fputc(' ', stdout);
            } else {
                printf("%c[%s", 27, ANSI_colors[index]);
                printf("\xe2\x96\x88");
            }
        }
    }
    return 0;
}


