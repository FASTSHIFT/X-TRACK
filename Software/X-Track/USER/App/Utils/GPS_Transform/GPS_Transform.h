#ifndef __GPS_TRANSFORM_H
#define __GPS_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

void GPS_Transform(double wgLat, double wgLon, double* mgLat, double* mgLon);

#ifdef __cplusplus
}
#endif

#endif
