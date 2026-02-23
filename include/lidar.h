//
// Created by Marius on 2/20/2026.
//

#ifndef AUTOMATICGRADER_LIDAR_H
#define AUTOMATICGRADER_LIDAR_H


#define CAMNUM_FROM_FLOATS 9

typedef union CAMRecFloats_s {
    uint16_t rawData[CAMNUM_FROM_FLOATS * 2];
    float floatData[CAMNUM_FROM_FLOATS];
} CAMRecFloats_t;

#define LVNUM_TOFROM_FLOATS 8

typedef union LVRecFloats_s {
    uint16_t rawData[LVNUM_TOFROM_FLOATS * 2];
    float floatData[LVNUM_TOFROM_FLOATS];
} LVRecFloats_t;

typedef union LVSendFloats_s {
    uint16_t rawData[LVNUM_TOFROM_FLOATS * 2];
    float floatData[LVNUM_TOFROM_FLOATS];
} LVSendFloats_t;

#define CMDNUM_FROM_FLOATS 11

typedef union CMDRecFloats_s {
    uint16_t rawData[CMDNUM_FROM_FLOATS * 2];
    float floatData[CMDNUM_FROM_FLOATS];
} CMDRecFloats_t;

#define LADAR_MAX_READING 5700

typedef struct datapts {
    float angle;
    float distance_ping;
    float distance_pong;
} datapts;

typedef struct {
    float x; //in feet
    float y; //in feet
} xy;

typedef struct {
    float x; //in feet
    float y; //in feet
    float theta; // in radians between -PI and PI. O radian is along the +x axis, PI/2 is the +y axis
} pose;

typedef union optiData_s {
    uint16_t rawData[10];
    float floatData[5];
} optiData_t;

#define OPTITRACKDATASIZE 5


#endif //AUTOMATICGRADER_LIDAR_H
