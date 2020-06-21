/*
 * vt.c
 *
 * vt switch handling
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/vt.h>

/*
 * signal handler: leave the virtual terminal
 */
void sigusr1(int s) {
	(void) s;
	ioctl(STDIN_FILENO, VT_RELDISP, 1);
}

/*
 * signal handler: enter the virtual terminal
 */
void sigusr2(int s) {
	(void) s;
	ioctl(STDIN_FILENO, VT_RELDISP, VT_ACKACQ);
}

/*
 * setup the virtual terminal for enter and leave
 */
void vt_setup() {
	struct vt_mode vtmode;

	signal(SIGUSR1, sigusr1);
	signal(SIGUSR2, sigusr2);
	siginterrupt(SIGUSR2, 1);

	ioctl(STDIN_FILENO, VT_GETMODE, &vtmode);
	vtmode.mode = VT_PROCESS;
	vtmode.relsig = SIGUSR1;
	vtmode.acqsig = SIGUSR2;
	ioctl(STDIN_FILENO, VT_SETMODE, &vtmode);
}

