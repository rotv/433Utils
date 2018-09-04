/*
Usage: ./RFSniffer [pulselength]
pulselength    - pulselength in microseconds

Hacked from http://code.google.com/p/rc-switch/
by @justy to provide a handy RF code sniffer
*/
#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

RCSwitch mySwitch;

int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	int PIN = 2;

	// If wrong number of arguments or argument -h/--help is given, print the help text
	if (argc > 2 || argc == 2 && 
			(!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
			) {
		printf("Usage: %s [pulselength]\n", argv[0]);
		printf("Listens to RF signals and outputs any received commands as decimal codes.\n");
		printf("\n");
		printf("pulselength - optional. pulselength in microseconds\n");
		return -1;
	}

	if(wiringPiSetup() == -1) {
		printf("wiringPiSetup failed, exiting...");
		return 0;
	}

	int pulseLength = 0;
	if (argv[1] != NULL) pulseLength = atoi(argv[1]);

	mySwitch = RCSwitch();
	if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
	mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2


	while(1) {
		if (mySwitch.available()) {
			int value = mySwitch.getReceivedValue();

			if (value == 0) {
				printf("Unknown encoding\n");
			} else {    
				printf("Received %i\n", mySwitch.getReceivedValue() );
			}

			fflush(stdout);
			mySwitch.resetAvailable();
		}
		usleep(100); 
	}

	exit(0);
}

