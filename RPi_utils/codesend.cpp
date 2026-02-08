/*
Usage: ./codesend decimalcode [protocol] [pulselength]
Sends the given decimalcode as an RF command.

decimalcode - As decoded by RFSniffer
protocol    - optional. According to rc-switch definitions
pulselength - optional. pulselength in microseconds

'codesend' hacked from 'send' by @justy

- The provided rc_switch 'send' command uses the form systemCode, unitCode, command
which is not suitable for our purposes.  Instead, we call 
send(code, length); // where length is always 24 and code is simply the code
we find using the RF_sniffer.ino Arduino sketch.

(Use RF_Sniffer.ino to check that RF signals are being produced by the RPi's transmitter 
or your remote control)
*/
#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	int PIN = 0;

	// Parse the first parameter to this command as an integer
	int protocol = 0; // A value of 0 will use rc-switch's default value
	int pulseLength = 0;

    // Help or no arguments
    if (argc == 1 || (argc >= 2 &&
            (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")))) {
        printf("Usage: %s decimalcode [protocol] [pulselength]\n", argv[0]);
        printf("Sends the given decimalcode as an RF command.\n\n");
        printf("decimalcode - As decoded by RFSniffer\n");
        printf("protocol    - optional. According to rc-switch definitions\n");
        printf("pulselength - optional. pulselength in microseconds\n");
        return -1;
    }

    if (argc > 4) {
        fprintf(stderr, "Too many arguments.\n");
        return -1;
    }

    // Helper to parse int with error checking
    char *endptr;
    errno = 0;
    long code = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || errno != 0 || code <= 0) {
        fprintf(stderr, "Invalid decimal code: '%s'\n", argv[1]);
        return -1;
    }

    if (argc >= 3) {
        errno = 0;
        long proto = strtol(argv[2], &endptr, 10);
        if (*endptr != '\0' || errno != 0 || proto < 1) {
            fprintf(stderr, "Invalid protocol: '%s'\n", argv[2]);
            return -1;
        }
        protocol = (int)proto;
    }

    if (argc >= 4) {
        errno = 0;
        long pulse = strtol(argv[3], &endptr, 10);
        if (*endptr != '\0' || errno != 0 || pulse <= 0) {
            fprintf(stderr, "Invalid pulselength: '%s'\n", argv[3]);
            return -1;
        }
        pulseLength = (int)pulse;
    }

	if (wiringPiSetup () == -1) return 1;
	printf("sending code[%i]\n", code);
	RCSwitch mySwitch = RCSwitch();
	if (protocol != 0) mySwitch.setProtocol(protocol);
	if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
	mySwitch.enableTransmit(PIN);

	mySwitch.send(code, 24);

	return 0;
}
