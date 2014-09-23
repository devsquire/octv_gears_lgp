#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#define CAMERA_VENDOR				0x07ca
#define CAMERA_PRODUCT				0x0875	
#define CAMERA_CONFIGURATION		1
#define CAMERA_INTERACE				0

#define CAMERA_ENDPOINT_ADDRESS_VIDEO_CAPTURE		0x81
#define CAMERA_ENDPOINT_ADDRESS_CONTROL				0x04
#define CAMERA_ENDPOINT_ADDRESS_STATUS_RESPONSE		0x83
#define TIMEOUT	5000

int writecommand(libusb_device_handle *camerahandle, char* commandbuffer, size_t size) {
	static int transferred = 0;
	int err = libusb_bulk_transfer(camerahandle, CAMERA_ENDPOINT_ADDRESS_CONTROL, commandbuffer, size, &transferred, TIMEOUT);
	if(err != 0) {
		printf("Error while sending command: '%s' - '%s',  data sent: %i, data transferred: %i, crashing!\n", libusb_error_name(err), libusb_strerror(err), 512, transferred);
		exit(-5);
	}
}

int readstatus(libusb_device_handle *camerahandle) {
	static char buffer[512];
	memset(buffer, 0, 512);
	static int transferred = 0;


	int err = libusb_bulk_transfer(camerahandle, CAMERA_ENDPOINT_ADDRESS_STATUS_RESPONSE, buffer, 512, &transferred, TIMEOUT);

	if(err != 0) {
		printf("Error while reading command: '%s' - '%s' , data received: %i, crashing!\n", libusb_error_name(err), libusb_strerror(err),transferred);
		exit(-5);
	}

	printf("Status received, bytes %i!, value: ", transferred);
	for(size_t i = 0; i < transferred; i++)
		printf("%i ", buffer[i]);

	printf("\n");
}

int main(int argc, char **argv) {
	// Process:
	// 1. Grab USB context
	// 2. Query system devices
	// 3. Figure out which one is the camera
	// 4. Configure the camera
	// 5. Claim interfaces
	// 6. Comm
	// 7. Cleanup


	// 1. Grab USB context
	libusb_context *usbcontext = NULL;
	if(libusb_init(&usbcontext) == 0)
		printf("We got the context\n");
	else {
		printf("We DONT have the context\n");
		exit(1);
	}

	libusb_set_debug(usbcontext, LIBUSB_LOG_LEVEL_WARNING);

	
	// 2. Query system devices
	libusb_device **devicelist;
	size_t devicecount = libusb_get_device_list(usbcontext, &devicelist);
	if(devicecount < 0) {
		printf("Error when counting devices!\n");
		exit(-1);
	}

	// 3. Figure out which one is the camera
	libusb_device_handle *camerahandle = NULL;
	for(size_t i = 0; i < devicecount; i++) {
		libusb_device *dev = devicelist[i];	
		struct libusb_device_descriptor desc;
		libusb_get_device_descriptor(dev, &desc);

		if(desc.idVendor == CAMERA_VENDOR && desc.idProduct == CAMERA_PRODUCT) {
			printf("Found camera!\n");
			if(libusb_open(dev, &camerahandle) != 0) {
				printf("Error getting the handle!\n");
				camerahandle = NULL;
			}
		}
	}


	if(camerahandle == NULL) {
		printf("Couldn't obtain camera handle.\n");
		exit(-2);
	}


	// 4. Configure the camera
	if(libusb_set_configuration(camerahandle, CAMERA_CONFIGURATION) != 0) {
		printf("Failed to set configuration!\n");
		exit(-3);
	}


	// 5. Claim interfaces
	if(libusb_claim_interface(camerahandle, CAMERA_INTERACE) != 0) {
		printf("Failed to claim interface!\n");
		exit(-4);
	}

	// 6. Comm

	char id_00100[] = { 0x0b, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2c, 0x0b  };
	size_t id_00100_s = 10;	
	char id_00101[] = { 0x0b, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2c, 0x03  };
	size_t id_00101_s = 10;	
	char id_00102[] = { 0x0b, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2c, 0x05  };
	size_t id_00102_s = 10;
	char id_00103[] = { 0x01, 0x01, 0x01, 0x00, 0xF8, 0x06, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00  };
	size_t id_00103_s = 12;
	char id_00104[] = { 0x01, 0x01, 0x01, 0x00, 0xCC, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00  };
	size_t id_00104_s = 12;
	char id_00105[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00  };
	size_t id_00105_s = 12;
	char id_00106[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3A, 0x03  };
	size_t id_00106_s = 10;
 	char id_00107[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2B  };
	size_t id_00107_s = 9;
	char id_00108[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3E  };
	size_t id_00108_s = 9;
 	char id_00109[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x1B  };
	size_t id_00109_s = 9;
 	char id_00110[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x1C  };
	size_t id_00110_s = 9;
 	char id_00111[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x25  };
	size_t id_00111_s = 9;
 	char id_00112[] = { 0x0C, 0x01, 0x07, 0x00, 0x15, 0x00, 0x00, 0x00, 0x1D  };
	size_t id_00112_s = 9;
 	char id_00113[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x37  };
	size_t id_00113_s = 9;
 	char id_00114[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2D  };
	size_t id_00114_s = 9;
 	char id_00115[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x26  };
	size_t id_00115_s = 9;
 	char id_00116[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x24  };
	size_t id_00116_s = 9;
 	char id_00117[] = { 0x0C, 0x01, 0x01, 0x00, 0x15, 0x00, 0x00, 0x00, 0x13  };
	size_t id_00117_s = 9;
	char id_00118[] = { 0x0B, 0x01, 0x06, 0x00, 0x15, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0xB0, 0x01, 0x01  };
	size_t id_00118_s = 14;
	char id_00119[] = { 0x0B, 0x01, 0x06, 0x00, 0x15, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0xAF, 0x01, 0x01  };
	size_t id_00119_s = 14;
	char id_00120[] = { 0x0B, 0x01, 0x06, 0x00, 0x15, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0xAC, 0x01, 0x01  };
	size_t id_00120_s = 14;
	char id_00121[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x47, 0x01  };
	size_t id_00121_s = 10;
	char id_00122[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x3A, 0x02  };
	size_t id_00122_s = 10;
	char id_00123[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2E, 0x06  };
	size_t id_00123_s = 10;
	char id_00124[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2E, 0x05  };
	size_t id_00124_s = 10;
	char id_00125[] = { 0x0B, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2E, 0x04  };
	size_t id_00125_s = 10;
	char id_00126[] = { 0x05, 0x01, 0x02, 0x00, 0x35, 0x00, 0x00, 0x00, 0x5A, 0x01  };
	size_t id_00126_s = 10;
	char id_00127[] = { 0x05, 0x01, 0x02, 0x00, 0x35, 0x00, 0x00, 0x00, 0x1D, 0x00  };
	size_t id_00127_s = 10;
	char id_00128[] = { 0x05, 0x01, 0x02, 0x00, 0x35, 0x00, 0x00, 0x00, 0x1C, 0x49  };
	size_t id_00128_s = 10;
	char id_00129[] = { 0x05, 0x01, 0x02, 0x00, 0x35, 0x00, 0x00, 0x00, 0x15, 0xEC  };
	size_t id_00129_s = 10;
	char id_00130[] = { 0x05, 0x01, 0x02, 0x00, 0x35, 0x00, 0x00, 0x00, 0x14, 0x1F  };
	size_t id_00130_s = 10;
	char id_00131[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0xF2, 0x00, 0x00, 0x00  };
	size_t id_00131_s = 12;
	char id_00132[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0xF1, 0x00, 0x00, 0x00  };
	size_t id_00132_s = 12;
	char id_00133[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00  };
	size_t id_00133_s = 12;
	char id_00134[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00  };
	size_t id_00134_s = 12;
	char id_00136[] = { 0x01, 0x01, 0x01, 0x00, 0xFC, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00  };
	size_t id_00136_s = 12;
	char id_00137[] = { 0x01, 0x01, 0x01, 0x00, 0xF8, 0x06, 0x00, 0x00, 0x83, 0x00, 0x00, 0x00  };
	size_t id_00137_s = 12;
	char id_00138[] = { 0x01, 0x01, 0x01, 0x00, 0xF8, 0x06, 0x00, 0x00, 0x19, 0xC2, 0x01, 0x21  };
	size_t id_00138_s = 12;
	char id_00139[] = { 0x01, 0x01, 0x01, 0x00, 0xF4, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
	size_t id_00139_s = 12;
	char id_00140[] = { 0x01, 0x01, 0x01, 0x00, 0xF0, 0x06, 0x00, 0x00, 0xAA, 0xAA, 0xAA, 0x2A  };
	size_t id_00140_s = 12;
	char id_00141[] = { 0x01, 0x01, 0x01, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
	size_t id_00141_s = 12;
	char id_00142[] = { 0x01, 0x01, 0x01, 0x00, 0xEC, 0x06, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00  };
	size_t id_00142_s = 12;
	char id_00143[] = { 0x01, 0x01, 0x01, 0x00, 0xEC, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
	size_t id_00143_s = 12;
	char id_00144[] = { 0x01, 0x01, 0x01, 0x00, 0xE4, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
	size_t id_00144_s = 12;
	char id_00145[] = { 0x01, 0x01, 0x01, 0x00, 0xC8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };
	size_t id_00145_s = 12;
	char id_00146[] = { 0x01, 0x01, 0x01, 0x00, 0x00, 0x08, 0x00, 0x00, 0x07, 0x00, 0x01, 0x00  };
	size_t id_00146_s = 12;
	char id_00147[] = { 0x01, 0x00, 0x07, 0x00, 0xB0, 0x06, 0x00, 0x00  };
	size_t id_00147_s = 8;
	char id_00148[] = { 0x01, 0x00, 0x01, 0x00, 0xCC, 0x06, 0x00, 0x00  };
	size_t id_00148_s = 8;
	char id_00149[] = { 0x01, 0x00, 0x01, 0x00, 0x18, 0x06, 0x00, 0x00  };
	size_t id_00149_s = 8;
	


	// Boot
	printf("Init procedure...\n");
	writecommand(camerahandle, id_00100, id_00100_s);
	readstatus(camerahandle);
	sleep(5);

	writecommand(camerahandle, id_00101, id_00101_s);
	readstatus(camerahandle);
	sleep(5);

	writecommand(camerahandle, id_00102, id_00102_s);
	readstatus(camerahandle);
	sleep(5);
	printf("Done\n");



	// Something	
	printf("Something...\n");
	char i_00200[] = { 0x0b, 0x01, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x2c, 0x0b  };
	size_t id_00200_s = 10;	



	printf("Done\n");



	// 7. Cleanup
	printf("Closing handles...\n");
	libusb_release_interface(camerahandle, CAMERA_INTERACE);
	libusb_close(camerahandle);
	libusb_free_device_list(devicelist, 1); // 1 = unref devices
	libusb_exit(usbcontext);
	printf("Bye\n");
	return 0;
}
