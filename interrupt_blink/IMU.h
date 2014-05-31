#ifndef IMU_H_
#define IMU_H_

extern void SSIInit(void);
extern void IMUConfig(void);
extern void avgconfig(void);
extern signed int xgyroread(void);
extern signed int ygyroread(void);
extern signed int zgyroread(void);
extern signed int xaccelread(void);
extern signed int xaccelread(void);
extern signed int yaccelread(void);
extern signed int zaccelread(void);
extern void printfloat(signed int number);
extern void printfloata(signed int number);
extern void myNumPut( unsigned int z);
extern void myDegPut( signed int x);

#endif /* IMU_H_ */
