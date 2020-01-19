#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <errno.h>

#define READONE 1
#define BUFFER_LENGTH 256               ///< The buffer length
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the chardev

int main(int argc, char *argv[])
{
	int fd, ret, z;
	char *myfifo = "/tmp/myfifo";
	int y=0;

	fd = open(myfifo, O_RDONLY);

	read(fd, &y, READONE);
	printf("Received mutiplied by 2: %d\n", (y*2));
	close(fd);



	//write to chardev via proc file
	//mkfifo(myfifo, 0666);
	int fd2 = open("/dev/04_chardev", O_RDWR);            // otwarcie urządzenia z prawami zapisu i odczytu
    if (fd2 < 0)
	{
      perror("Failed to open the device...");
      return errno;
    }
    /* otwarcie, odczyt, wyświetlenie wartości */
   ret = write(fd, &y, sizeof(y)); // wysłanie wartości 'y' do modułu jądra (LKM)
   if (ret < 0)
   	{
      perror("write to device failed");
      return errno;
    }
	
	printf("Press ENTER to read back from the device...\n");
	getchar();
	
	printf("Reading from the device...\n");
	ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
	if (ret < 0){
		perror("Failed to read the message from the device.");
		return errno;
	}
	z = *receive;
	printf("The received message is: [%d]\n", z);
	printf("End of the program\n");
	return 0;




	return (0);
}
