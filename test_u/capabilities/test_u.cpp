/*
 * Test program for the ambient capabilities
 *
 *
 * Compile using:
 *	gcc -o ambient_test ambient_test.o
 *
 * This program must have the following capabilities to run properly:
 * CAP_SETPCAP, CAP_NET_RAW, CAP_NET_ADMIN, CAP_SYS_NICE
 *
 * A command to equip this with the right caps is:
 *
 *	setcap cap_setpcap,cap_net_raw,cap_net_admin,cap_sys_nice+eip ambient_test
 *
 * To get a shell with additional caps that can be inherited do:
 *
 * ./ambient_test /bin/bash
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/prctl.h>
#include <linux/capability.h>
#include <unistd.h>

/* Defintion to be updated in the user space include files */
//#define PR_CAP_AMBIENT 45

int main(int argc, char **argv)
{
//  int rc;

//  if (prctl(PR_CAP_AMBIENT, CAP_NET_RAW))
//    perror("Cannot set CAP_NET_RAW");

  if (prctl(PR_CAP_AMBIENT,
            PR_CAP_AMBIENT_RAISE,
            CAP_NET_ADMIN))
    perror("Cannot set CAP_NET_ADMIN");

//  if (prctl(PR_CAP_AMBIENT, CAP_SYS_NICE))
//    perror("Cannot set CAP_SYS_NICE");

  // -------------------------------------

  if (prctl(PR_SET_KEEPCAPS, 1))
    perror("Cannot set PR_SET_KEEPCAPS");

  printf("Ambient_test forking shell\n");
  if (execv(argv[1], argv + 1))
    perror("Cannot exec");

  return 0;
}
