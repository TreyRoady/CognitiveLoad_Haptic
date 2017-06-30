//Header for Validation study

#include <stdio.h>
#include <string>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm> //for random_shuffle
#include <ctime> // For randomizer seed
//#include <cstdlib>
#include <math.h> //for floor()
#include <string> //for std::string
#include <windows.h>
#include "tactor_cHeader.h"

enum notes{ //Set the Frequencies for Tactor Tuning, in deciHz
	//http://www.phy.mtu.edu/~suits/notefreqs.html
	LE = 1648,
	LEb= 1556,
	LD = 1468,
	LC = 1308,
	LB = 1235,
	LA = 1100,
	F  = 1746,
	Fs = 1850,
	Gb = Fs,
	G  = 1960,
	Gs = 2077,
	Ab = Gs,
	A  = 2200,
	As = 2331,
	Bb = As,
	B  = 2469,
	C  = 2616,
	Cs = 2772,
	Db = Cs,
	D  = 2937,
	Ds = 3111,
	Eb = Ds,
	E  = 3296
};

enum rests{
	R4   = 400,
	R45  = 500,
	R8   = 300,
	R85  = 350,
	R16  = 150,
	R32  = 75
};



//Forward declare functions

int method_convert(int treatment);
int level_convert(int treatment);
int present(int mthd, int sig);  //Presents the signal for given method
int study_signals(int mthd, int levels, int num); //Present the correct signal for the treatment
void play(int note, int length); //Plays a note (e.g. Cb), in the correct length 
	//(1 == whole, 2 == half, 4== quarter, 45 == dotted quarter, etc.) 
void treatment(int subject, int trtmnt);
void training(unsigned int trtmnt);
void training_loop(int trtmnt);
int order_call(int level_indx);
void ClearScreen();


//Globally declare the signal timing & Treatment orders.
//May move this to a local scope once I determine where it should live
int study_time = 1000 * 4 * 60; //4 minutes in milliseconds, set to 1 minute for debug

int level_counter[3] = {0,0,0}; // Counter for # of times Level 3, 4, & 5 have been accessed
int signal_cues[18][14] = //Set 18 treatments of arrival times for timing loop; 
	//Subject is 1st value, 2nd value is signal in order (seconds)
	//**Be sure to convert to Ms for use!**//
	{
	{18,31,46,59,73,87,101,118,133,146,163,176,193,207},
	{14,28,47,60,76,88,105,121,134,149,163,177,194,212},
	{15,32,48,61,75,91,108,121,140,156,171,184,200,216},
	{16,30,46,61,76,93,110,128,146,163,175,189,202,218},
	{15,29,44,59,74,90,108,124,141,156,170,186,202,216}, //5th Treatment
	{15,31,45,62,77,90,106,123,140,153,168,183,202,220},
	{16,33,52,64,82,98,115,133,147,160,175,191,210,226},
	{13,31,47,62,79,93,109,124,138,153,169,185,201,215},
	{15,33,48,64,81,97,112,127,140,157,174,188,207,220},
	{16,35,50,67,80,95,114,131,146,163,179,197,212,230}, //10th
	{14,29,44,58,76,91,109,125,143,159,172,191,206,220},
	{15,28,42,58,73,87,101,115,132,146,163,176,189,203},
	{16,31,46,62,77,91,104,119,133,148,164,180,199,214},
	{15,31,46,60,75,89,108,124,142,157,175,189,203,221},
	{14,30,46,62,81,98,116,131,147,164,181,200,213,231}, //15th
	{13,30,42,59,76,92,106,122,136,152,167,182,197,212},
	{17,35,50,63,78,97,113,129,142,156,171,189,208,224},
	{14,28,44,58,73,88,102,117,130,146,160,176,191,205} //18th
};
	
int treatment_order[18][18]= // each row is a subject, (Remember that subject #'s go from 1, but rows start at 0)
	{
	{1,2,9,3,8,4,7,5,6,7,4,9,3,1,5,6,2,8},
	{2,3,1,4,9,5,8,6,7,8,5,1,4,2,6,7,3,9},
	{3,4,2,5,1,6,9,7,8,9,6,2,5,3,7,8,4,1},
	{4,5,3,6,2,7,1,8,9,1,7,3,6,4,8,9,5,2},
	{5,6,4,7,3,8,2,9,1,2,8,4,7,5,9,1,6,3},
	{6,7,5,8,4,9,3,1,2,3,9,5,8,6,1,2,7,4},
	{7,8,6,9,5,1,4,2,3,4,1,6,9,7,2,3,8,5},
	{8,9,7,1,6,2,5,3,4,5,2,7,1,8,3,4,9,6},
	{9,1,8,2,7,3,6,4,5,6,3,8,2,9,4,5,1,7},
	{6,5,7,4,8,2,9,2,1,9,3,7,4,6,2,1,5,8},
	{7,6,8,5,9,4,1,3,2,1,4,8,5,7,3,2,6,9},
	{8,7,9,6,1,5,2,4,3,2,5,9,6,8,4,3,7,1},
	{9,8,1,7,2,6,3,5,4,3,6,1,7,9,5,4,8,2},
	{1,9,2,8,3,7,4,6,5,4,7,2,8,1,6,5,9,3},
	{2,1,3,9,4,8,5,7,6,5,8,3,9,2,7,6,1,4},
	{3,2,4,1,5,9,6,8,7,6,9,4,1,3,8,7,2,5},
	{4,3,5,2,6,1,7,9,8,7,1,5,2,4,9,8,3,6},
	{5,4,6,3,7,2,8,1,9,8,2,6,3,5,1,9,4,7}
};

int signal_order[18][14] =
	{
	//3 Signals 0-5 index
	{3,1,3,2,2,3,1,1,3,1,2,2,2,1}, 
	{3,3,2,3,3,1,2,2,1,1,2,2,1,1},
	{3,2,1,1,2,3,2,1,3,3,1,2,1,2},
	{2,3,1,2,2,1,1,3,1,2,1,3,3,2},
	{2,3,3,1,1,3,2,1,2,2,1,1,2,3},
	{1,1,3,1,2,2,1,2,3,3,3,2,2,1},
	//4 Signals 6 - 11 index
	{1,3,1,2,3,4,1,1,4,2,2,4,2,3},
	{1,3,3,1,4,2,4,4,1,1,3,2,2,2},
	{2,2,1,1,3,4,1,4,1,3,4,2,2,3},
	{4,1,1,1,2,1,4,2,3,2,2,4,3,3},
	{4,4,1,1,2,3,1,4,3,2,1,2,3,2},
	{3,2,4,4,1,1,2,3,1,2,1,4,3,2},
	//5 Signals 12-15 index
	{5,1,4,1,2,5,3,4,3,3,1,2,4,2},
	{1,3,4,2,3,4,2,5,1,5,2,4,1,3},
	{2,5,1,5,4,4,3,2,1,1,2,4,3,3},
	{5,1,2,4,4,1,5,2,4,3,3,3,2,1},
	{2,3,3,2,3,5,5,4,4,4,1,1,2,1},
	{4,3,2,2,3,2,4,1,1,5,3,4,1,5}
};