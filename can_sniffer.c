#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main()
{
    int sock;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Create CAN socket
    sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (sock < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Select vcan0
    strcpy(ifr.ifr_name, "vcan0");

    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("Interface error");
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind socket to vcan0
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    // Create CSV file
    FILE *file = fopen("can_data.csv", "w");

    if (file == NULL)
    {
        perror("File creation failed");
        return 1;
    }

    fprintf(file, "timestamp,can_id,dlc,payload\n");

    printf("CAN Sniffer started on vcan0...\n");
    printf("Waiting for CAN frames...\n");

    while (1)
    {
        // Receive CAN frame
        int nbytes = read(sock, &frame, sizeof(frame));

        if (nbytes < 0)
        {
            perror("CAN read error");
            break;
        }

        // Get timestamp
        struct timespec ts;

        clock_gettime(CLOCK_REALTIME, &ts);

        double timestamp =
            ts.tv_sec + ts.tv_nsec / 1000000000.0;

        // Display frame
        printf("Timestamp: %.6f | ID: %03X | DLC: %d | Data:",
               timestamp,
               frame.can_id & CAN_SFF_MASK,
               frame.can_dlc);

        // Write basic information to CSV
        fprintf(file, "%.6f,%03X,%d,",
                timestamp,
                frame.can_id & CAN_SFF_MASK,
                frame.can_dlc);

        // Read payload
        for (int i = 0; i < frame.can_dlc; i++)
        {
            printf(" %02X", frame.data[i]);

            fprintf(file, "%02X", frame.data[i]);

            if (i < frame.can_dlc - 1)
            {
                fprintf(file, " ");
            }
        }

        printf("\n");

        fprintf(file, "\n");

        // Immediately save to file
        fflush(file);
    }

    fclose(file);
    close(sock);

    return 0;
}
