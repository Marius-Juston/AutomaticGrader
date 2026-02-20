//
// Created by Marius on 2/20/2026.
//

extern "C" {
#include "lidar.h"


char buf[477]; //store all the data
uint16_t dis_index = 0;
uint16_t G_count = 0; //number of thing received after 'G'
uint16_t LF_count = 0; //number of consecutive line feed
uint16_t buf_index = 0; //for debug
uint16_t get_dis_first; //if it is the first part of the two parts data
uint16_t print_dis_index = 0;
uint32_t numRCXerrors = 0;
int16_t tempscid = 0;
int16_t distance = 0;
float checkdist = 0;

uint16_t COMA_state = 0;
uint16_t COMALSB = 0;
uint16_t received_CAM_countThreshold1 = 0;
uint16_t NewCAMDataThreshold1 = 0; // Flag new data
CAMRecFloats_t DataFromCameraThreshold1;
float fromCAMvaluesThreshold1[CAMNUM_FROM_FLOATS];
uint16_t received_CAM_countThreshold2 = 0;
uint16_t NewCAMDataThreshold2 = 0; // Flag new data
CAMRecFloats_t DataFromCameraThreshold2;
float fromCAMvaluesThreshold2[CAMNUM_FROM_FLOATS];

uint16_t tempLSB = 0;
LVRecFloats_t DataFromLabView;
LVSendFloats_t DataToLabView;
uint16_t received_LV_count = 0;
uint16_t NewLVData = 0;
char LVsenddata[LVNUM_TOFROM_FLOATS * 4 + 2];
float fromLVvalues[LVNUM_TOFROM_FLOATS];

CMDRecFloats_t DataFromLinuxCMD;
uint16_t received_CMD_count = 0;
uint16_t newLinuxCommands = 0;
float LinuxCommands[CMDNUM_FROM_FLOATS];
uint16_t com_state = 0; //the state of receive from pi (SCID)
uint16_t LADARpingpong = 0;
uint16_t read_ladar_state = 0; //states for ladar reading, initialized as wait for S command
datapts ladar_data[228]; //distance data from ladar

uint16_t new_optitrack = 0;
float Optitrackdata[OPTITRACKDATASIZE]; // x, y, heading, trackableID, framecount
uint16_t received_opti_count = 0; //the count for receiving optitrack data
optiData_t optirx;

char path_received[81];
int16_t received_path_count = 0;
int16_t newAstarPath = 0;
}
