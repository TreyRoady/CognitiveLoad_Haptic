#include "tactor_functions.h"

//Compilation note for MinGW: need to compile with -lwinmm tag to avoid linker errors.
// g++ -o CogVib.exe CogVib.cpp -lwinmm

int main()
{
	int subject;
	
	//Console Introduction

	//Use this snip whenever accessing the tactors//
	if(InitTactorDLL() == -1)
	{
		std::cerr << "ERROR DURING INIT\n";
		return -1;
	}
	
	std::cout << "Welcome to the Tactor Signal Validation Study" << std::endl << std::endl;
	std::cout << "What is the subject identifier?: ";
	
	std::cin >> subject;
	//char m;
	//int l=0;
	//while(l == 0){
	//	std::cout << "\nAre you using tactor set A.) 16 or B.) 17?: ";
	//	std::cin >> m;
	//	m = toupper(m);
	//	switch(m){
	//	case 'A':
	//		ConnectDirect("\\.\COM40", Serial); // Weird escape deals with squirrelly Windows COM ports.
	//		l = 1;
	//		break;
	//	case 'B':
	//		ConnectDirect("COM9", Serial);
	//		l = 1;
	//		break;
	//	default:
	//		std::cout << std::endl << std::endl << "Please select either A or B." << std::endl;
	//		l = 0;
	//	}
	//	
	//}
	
	
	ConnectDirect("\\\\.\\COM40", Serial); //Connect to EAI 17 w/ squirrely Windows Port notation
	std::cout << "Tactors connected!" << std::endl;
	Sleep(1000);

	ClearScreen();
	std::cout << "Beginning Initial Training" << std::endl;
	Sleep(1500);
	for(int j = 0; j < 9; j++){
		training(j);
		ClearScreen();
		std::cout << "Hit any key to continue" << std::endl;
		std::fflush(stdin);
		std::getchar();
	}
	
	//Need to build the repeat functionality in here.
	for(int j=0; j<9; j++){	
		int trtmnt = treatment_order[subject][j]-1;
		treatment(subject, trtmnt);
		std::cout << "Trial " << j+1 << "of 18 complete." << std::endl;
		Sleep(1000);
		std::cout << "Hit any key to continue" << std::endl;
		std::fflush(stdin);
		std::getchar();
	}
	//SUBJECT REST BREAK
	
	for(int j=9; j<18; j++){	
		int trtmnt = treatment_order[subject][j]-1;
		treatment(subject, trtmnt);
		std::cout << "Trial " << j +1 << "of 18 complete." << std::endl;
		std::cout << "Hit any key to continue" << std::endl;
		std::fflush(stdin);
		std::getchar();
	}
	
	std::cin; //Pause for response
	return 0;
}

//Current test:
// Run through each time mark, output the time, continue until study time is finished.

void treatment(int subject, int trtmnt){
	std::cout << "Beginning pre-treatment training." <<std::endl;
	Sleep(1500);
	if(InitTactorDLL() == -1)
	{
		std::cerr << "ERROR DURING INIT\n";
	}
	
	ClearScreen();
	char c = 'Y';
	//Do treatment for study condition; Repeat if needed. 
	training_loop(trtmnt);
	
	ClearScreen();
	
	int method_indx = method_convert(trtmnt);
	int level_indx = level_convert(trtmnt);
	if(InitTactorDLL() == -1)
	{
		std::cerr << "ERROR DURING INIT\n";
	}
	ClearScreen();
	std::cout << "Subject: "<< subject << std::endl;
	std::cout << "Method: " << method_indx << std::endl;
	std::cout << "Levels: " << level_indx << std::endl << std::endl <<std::endl;
	Sleep(1000);
	std::cout << "Hit key to start trial" << std::endl;
	std::fflush(stdin);
	std::getchar();
	int start = timeGetTime();
	int time = timeGetTime() - start;
	int j = 0; //Iterator for study
	int order = order_call(level_indx);
	while(time <= study_time){ //check to see if study_time has elapsed
		if(j < 14){
			if(time >= signal_cues[subject][j]*1000){ //have we passed the cue time?
				std::cout << "- Signal " << study_signals(method_indx,level_indx,signal_order[order][j]) << " at time " << time << std::endl;
				j++; //Move to next signal marker
			}
		}
		time = timeGetTime()- start; //get next time point
	}
}

void training(unsigned int trtmnt){
	int levels = level_convert(trtmnt);
	int method = method_convert(trtmnt);
	std::string method_str; 
	switch(method){ //Label method with correct string
	case 0:
		method_str = "Syncopated";
		break;
	case 1:
		method_str = "Melodic";
		break;
	case 2:
		method_str = "Counted";
		break;
	}
	
	ClearScreen();
	//Method:
	//	0 - Syncopated
	//	1 - Melodic
	//	2 - Counted
	
	//Levels:
	//	3, 4, & 5
	
	for(unsigned int k = 1; k <= levels; k++){
		std::cout << "Training" << std::endl;
		std::cout << "Method: " << method_str <<"; "<< levels << " Levels" << std::endl;
		std::cout << "Level: " << (k) << std::endl << std::endl;
		study_signals(method,levels,k);
		//Sleep(1500);
		std::fflush(stdin);
		std::getchar();
		ClearScreen();
	}
}

void training_loop(int trtmnt){
	int m = 0;
	char c = 'Y';
	// Is the response Yes?
	while(c=='Y'){
		training(trtmnt);
		std::cout << "Running training" << std::endl << std::endl;
		int m = 0;
		//Is the response valid?
		while(m == 0){
			std::cout << "Repeat training? [Y/N]:";
			std::cin >> c;
			c = toupper(c);
			std::cout << std::endl;
			switch(c){
				case 'Y':								
				case 'N':
					m = 1;
					break;
				default:
					m = 0; 
					break;
			}
		}
	}
}

void play(int note, int length){ // notes handled by Enum.
	SetSinFreq_Fine1(0, 0, note, false);
	SetSigSrc(0,0,1,1,false);  //Set source to Freq1
	switch(length){
	case 1:  //Whole Note
		TacOnTime(0, 0, Tac1, 900, false);
		Sleep(1000);
		break;
	case 2:  // Half Note
		TacOnTime(0, 0, Tac1, 500, false);
		Sleep(650);
		break;
	case 25: //Dotted Half
		TacOnTime(0, 0, Tac1, 725, false);
		Sleep(800);
	case 4:  //Quarter
		TacOnTime(0, 0, Tac1, 250, false);
		Sleep(400);
		break;
	case 45: // Dotted quarter
		TacOnTime(0, 0, Tac1, 350, false);
		Sleep(500);
		break;
	case 8:	 //Eighth
		TacOnTime(0, 0, Tac1, 100, false);
		Sleep(300);
		break;
	case 85: //Dotted-Eighth
		TacOnTime(0, 0, Tac1, 220, false);
		Sleep(400); //from 400
	case 16: // 16th
		TacOnTime(0, 0, Tac1, 65, false);
		Sleep(150);
		break;
	case 32:
		TacOnTime(0, 0, Tac1, 25, false);
		Sleep(75);
	}
}


int present(int mthd, int sig){
	//Presents Signal # from the correct method (Syncopated or Melodic)
	//  -> This should likely be adapted as a header
	
	//Returns 1 if signal exists, returns 0 if not.
	if(InitTactorDLL() == -1)
	{
		std::cout << "ERROR DURING INIT\n";
	}
	SetGain(0,0,4,11,false); //Set gain to max!
	// Signal Pre-Amble	
	/*
	SetSinFreq_Fine1(0, 0, B, false);
	SetSigSrc(0,0 , 1, 1, false);
	TacOnTime(0, 0, Tac1, 250,false);
	Sleep(300);
	TacOnTime(0, 0, Tac1, 100,false);
	// End Pre-Amble
	*/
	
	switch (mthd){
		// mthd == 0 
		////////////////////////
		// Syncopated Signals //
		////////////////////////
	case 0:
		switch (sig){
		case 0:
			play(B,1);
			return 1;
			break;
		case 1: // 4 Quarter Notes
			play(B,4);
			play(B,4);
			play(B,4);
			play(B,4);
			return 1;
			break;
		case 2: // Sync 2
			play(B,16);
			play(B,16);
			play(B,8);
			//Rest 8 + 16
			Sleep(R8);
			Sleep(R16);
			play(B,4);
			play(B,8);
			play(B,85);
			return 1;
			break;
		case 3: // Sync 3
			//Eigth
			play(B,8);
			// Sixteenth
			play(B,16);
			// Eighth
			play(B,8);
			// Sixteenth
			play(B,16);
			// Eighth
			play(B,8);
			// Eighth
			play(B,8);
			//Sixteenth
			play(B,16);
			// Rest
			Sleep(R16);
			// Sixteenth
			play(B,16);
			// Eighth
			play(B,8);
			return 1;
			break;
		case 4:
			play(B,4);
			play(B,8);
			play(B,8);
			//rest 8;
			Sleep(300);
			play(B,8);
			play(B,4);
			return 1;
			break;
		case 5:
			play(B,4);
			//Triplet. May need editing
			play(B,8);
			play(B,8);
			play(B,8);
			play(B,4);
			play(B,4);

			return 1;
			break;
		case 6:
			play(B,2);
			play(B,8);
			play(B,4);
			play(B,8);
			return 1;
			break;
		case 7:
			play(B,16);
			//rest 16;
			Sleep(R16);
			play(B,4);
			//rest 8;
			Sleep(R8);
			play(B,16);
			//rest 16;
			Sleep(R16);
			play(B,45);
			return 1;
			break;
		case 8:
			play(B,4);
			play(B,8);
			play(B,4);
			play(B,8);
			play(B,4);
			return 1;
			break;
		case 9:
			play(B,32);
			play(B,16);
			play(B,8);
			play(B,4);
			play(B,16);
			play(B,8);
			play(B,45);
			return 1;
			break;
		case 10: //Shouldn't be used, but included here for usage if problems occur with other signals.
			play(B,16);
			play(B,16);
			play(B,8);
			//Rest 8 + 16
			Sleep(R8);
			Sleep(R16);
			play(B,4);
			play(B,8);
			play(B,85);
			return 1;
		default:
			return 0;
			std::cout << "\nNo such signal" << std::endl << std::endl;
		}
		break;
		// mthd == 1
		/////////////////////
		// Melodic Signals //
		/////////////////////
	case 1:
		switch (sig){
		case 0: // Whole Note
			play(B,1);
			return 1;
			break;
		case 1: //4 Alternating Quarter Notes
			play(F,4);
			play(C,4);
			play(F,4);
			play(C,4);
			return 1;
			break;
		case 2: 
			play(A,4);
			play(C,4);
			play(F,4);
			play(C,16);
			play(C,45);
			return 1;
			break;
		case 3: // Shave & a Hair Cut
			play(A,4);
			play(G,8);
			play(G,8);
			play(Ab,4);
			play(G,4);
			//Quarter Rest
			Sleep(R4);
			play(B,4);
			play(C,4);
			return 1;
			break;
		case 4: // Mary had a Little Lamb, 1st measure
			play(B,4);
			play(A,4);
			play(G,4);
			play(A,4);
			play(B,4);
			play(B,4);
			play(B,2);
			
			return 1;
			break;
		case 5: // C Arpeggio
			play(LC,8);
			play(LE,8);
			play(G, 8);
			play(C, 8);
			play(G, 8);
			play(LE,8);
			play(LC,4);
			return 1;
			break;
		case 6:
			play(B,8);
			play(C,4);
			//Rest 8;
			Sleep(R8);
			play(G,16);
			play(A,16);
			play(F,4);
			play(F,8);
			return 1;
			break;
		case 7:
			play(A,8);
			play(C,8);
			play(F,8);
			play(A,8);
			play(F,8);
			play(F,16);
			play(F,4);
			return 1;
			break;
		case 8:
			play(A,4);
			play(B,8);
			//rest 8
			Sleep(R8);
			play(G,8);
			play(D,16);
			play(B,16);
			play(A,8);
			return 1;
			break;
		case 9:
			play(F,45);
			play(C,8);
			Sleep(R8);
			play(C,8);
			play(F,4);
			return 1;
			break;
		default:
			return 0;
		}
		break;
		// mthd == 2
	case 2:
		///////////////
		//  Counted  //
		///////////////
		
		// NOTE: I started the signals from 1, not 0 index here.
		SetSinFreq_Fine1(0, 0, B, false); // Set Frequency to B
		SetSigSrc(0,0,1,1,false);
		switch(sig){
		case 1: //Level 1 signal, 1 signal per second
			play(B,1);
			return 1;
			break;
		case 2: //Level 2 Signal, 2 signals per second
			TacOnTime(0, 0, Tac1, 400, false);
			Sleep(500);
			TacOnTime(0, 0, Tac1, 400, false);
			return 1;
			break;
		case 3: //Level 3 Signal, 3 signals per second
			TacOnTime(0, 0, Tac1, 265, false);
			Sleep(365);
			TacOnTime(0, 0, Tac1, 265, false);
			Sleep(365);
			TacOnTime(0, 0, Tac1, 265, false);
			return 1;
			break;
		case 4: //Level 4 Signal, 4 signals per second
			TacOnTime(0, 0, Tac1, 175, false);
			Sleep(275);
			TacOnTime(0, 0, Tac1, 175, false);
			Sleep(275);
			TacOnTime(0, 0, Tac1, 175, false);
			Sleep(275);
			TacOnTime(0, 0, Tac1, 175, false);
			return 1;
			break;
		case 5: //Level 5 Signal, 5 signals per second
			TacOnTime(0, 0, Tac1, 100, false);
			Sleep(200);
			TacOnTime(0, 0, Tac1, 100, false);
			Sleep(200);
			TacOnTime(0, 0, Tac1, 100, false);
			Sleep(200);
			TacOnTime(0, 0, Tac1, 100, false);
			Sleep(200);
			TacOnTime(0, 0, Tac1, 100, false);
			return 1;
			break;
		default:
			std::cout << "No such signal in Present(), Counted condition" << std::endl;
			return 0;
			break;
		}
		break;
		///////////////////
		// Song Snippets //
		///////////////////
	case 3:
		switch (sig){
		case 0: //1. Mary had a Little Lamb
			play(B,4);
			play(A,4);
			play(G,4);
			play(A,4);
			play(B,4);
			play(B,4);
			play(B,2);
			play(A,4);
			play(A,4);
			play(A,2);
			play(B,4);
			play(D,4);
			play(D,2);
			return 1;
			break;
		case 1: //2. Here We Go Round the Mulberry Bush  
			play(F,8);
			play(F,16);
			play(F,16);
			play(F,8);
			play(A,8);
			play(C,16);
			play(C,16);
			play(A,8);
			play(F,8);
			play(F,8);
			
			play(G,16);
			play(G,16);
			play(LE,8);
			play(LC,8);
			play(LC,8);
			play(C,16);
			play(C,16);
			play(A,8);
			play(F,4);
			return 1;
			break;
		case 2: //3. Ring Around the Rosy
					
			//1st Measure
			play(G,4);
			play(G,8);
			play(LE,4);
			play(A,8);

			play(G,45);
			play(LE,4);
			play(F,8);

			play(G,4);
			play(G,8);
			play(LE,4);
			play(A,8);

			play(G,45);
			play(LE,45);
			
			play(G,45);
			play(LE,45);

			play(G,45);
			play(LE,4);
			play(LE,8);
			
			play(G,45);
			play(G,45);
			
			play(LC,45);
			return 1;
			break;
		case 3: //4. Twinkle Twinkle Little Star
				
			play(LC,4);
			play(LC,4);
			play(G,4);
			play(G,4);
			play(A,4);
			play(A,4);
			play(G,2);
			play(F,4);
			play(F,4);
			play(LE,4);
			play(LE,4);
			play(LD,4);
			play(LD,4);
			play(LC,2);
			return 1;
			break;
		case 4: //5. Frere Jacques
			play(F,4);
			play(G,4);
			play(A,4);
			play(F,4);
			play(F,4);
			play(G,4);
			play(A,4);
			play(F,4);
			play(A,4);
			play(B,4);
			play(C,2);
			play(A,4);
			play(B,4);
			play(C,2);
			return 1;
			break;
		case 5: //6. Bingo
			play(G,8);
			play(C,8);
			play(C,8);
			play(G,8);
			play(G,8);
			play(A,8);
			play(A,8);
			play(G,8);
			play(G,8);
			play(C,8);
			play(C,8);
			play(D,8);
			play(D,8);
			play(E,4);
			play(C,4);
			return 1;
			break;
		case 6: //7. Old MacDonald
			play(C,4);
			play(C,4);
			play(C,4);
			play(G,4);
			play(A,4);
			play(A,4);
			play(G,2);
			play(E,4);
			play(E,4);
			play(D,4);
			play(D,4);
			play(C,2);
			return 1;
			break;
		case 7: //8. The Wheels on the Bus
			play(LC,4);
			play(F,4);
			play(F,8);
			play(F,8);
			play(F,4);
			play(A,4);
			play(C,4);
			play(A,4);
			play(F,2);
			play(G,4);
			play(G,4);
			play(G,2);
			play(LE,4);
			play(LD,4);
			play(LC,4);
			
			return 1;
			break;
		case 8: //9. Incy Wincy Spider
			play(LD,8);
			play(G,4);
			play(G,8);
			play(G,4);
			play(A,8);
			play(B,45);
			play(B,4);
			play(B,8);
			play(A,8);
			play(G,8);
			play(A,4);
			play(B,8);
			play(G,25);
			
			return 1;
			break;
		case 9: //10. Imperial March
			play(G,4);
			play(G,4);
			play(LEb,85);
			play(Bb,16);
			
			play(G,4);
			play(LEb,85);
			play(Bb,16);
			play(G,2);
			
			play(D,4);
			play(D,4);
			play(D,4);
			play(Eb,85);
			play(Bb,16);
			
			play(Gb,4);
			play(LEb,85);
			play(Bb,16);
			play(G,4);
			
			return 1;
			break;
		default:
			return 0;
		}
		break;
	}
}

int study_signals(int mthd, int levels, int num){
	switch(mthd){
	case 0: // Syncopated 
		switch(levels){
		case 3: //present() goes from 0 - 9! Decrement signal labels.
			switch(num){
				case 1: // 3 -1 -> 2 
					present(0,2);
					break;
				case 2: // 5 -1 -> 4
					present(0,4);
					break;
				case 3: // 1 -1 -> 0
					present(0,0);
					break;
			}
			break;
		case 4:
			switch(num){
				case 1: // 3 -1
					present(0,2);
					break;
				case 2: // 5 -1
					present(0,4);
					break;
				case 3: // 9 -1
					present(0,8);
					break;
				case 4: // 1 -1
					present(0,0);
					break;
			}
			break;
		case 5:
			switch(num){
				case 1: // 3 -1
					present(0,2);
					break;
				case 2: // 4 -1
					present(0,3);
					break;
				case 3: // 5 -1
					present(0,4);
					break;
				case 4: // 7 -1
					present(0, 6);
					break;
				case 5: // 1 -1
					present(0, 0);
					break;
			}
			break;
		}
		break;
	case 1: // Melodic
		switch(levels){
		case 3:
			switch(num){
				case 1: // 6 -1
					present(1,5);
					break;
				case 2: // 7 -1
					present(1,6);
					break;
				case 3: // 5 -1
					present(1,4);
					break;
			}
			break;
		case 4:
			switch(num){
				case 1: // 6 -1
					present(1,5);
					break;
				case 2: // 8 -1
					present(1,7);
					break;
				case 3: // 4 -1
					present(1,3);
					break;
				case 4: // 5 -1
					present(1,4);
					break;
			}
			break;
		case 5:
			switch(num){
				case 1: // 6 -1
					present(1, 5);
					break;
				case 2: // 8 -1
					present(1, 7);
					break;
				case 3: // 9 -1
					present(1, 8);
					break;
				case 4: // 1 -1
					present(1, 0);
					break;
				case 5: // 5 -1
					present(1, 4);
					break;
			}
			break;
		}
		break;
	case 2: // Counted
		switch(levels){
		case 3:
			switch(num){
				case 1:
					present(2, 1);
					break;
				case 2:
					present(2, 2);
					break;
				case 3:
					present(2, 3);
					break;
			}
			break;
		case 4:
			switch(num){
				case 1:
					present(2, 1);
					break;
				case 2:
					present(2, 2);
					break;
				case 3:
					present(2, 3);
					break;
				case 4:
					present(2, 4);
					break;
			}
			break;
		case 5:
			switch(num){
				case 1:
					present(2, 1);
					break;
				case 2:
					present(2, 2);
					break;
				case 3:
					present(2, 3);
					break;
				case 4:
					present(2, 4);
					break;
				case 5:
					present(2, 5);
					break;
			}
			break;
		}
		break;
	default:
		std::cerr << "Call to study_signals out of range in Mthd";
		break;
	}
	return num;
	//End of Switch Case for method
	
}

int method_convert(int treatment){ //Treatment from 0 to 8.
	return (treatment / 3);
	// 0 == Syncopated
	// 1 == Melodic
	// 2 == Counted
}

int level_convert(int treatment){
	return ((treatment%3)+3);
	// 0 % 3 == 0 -> 3 Levels
	// 1 % 3 == 1 -> 4 Levels
	// 2 % 3 == 2 -> 5 Levels
	} 

int order_call(int level_indx){
	switch(level_indx){
		case 3:	
			return level_counter[0]++;
			break;
		case 4:
			return level_counter[1]++;
			break;
		case 5:
			return level_counter[2]++;
			break;
		default:
			std::cerr << "Error in order_call(). Input parameters out of scope";
	}
	
}
	
void ClearScreen(){
  HANDLE                     hStdOut;
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  DWORD                      count;
  DWORD                      cellCount;
  COORD                      homeCoords = { 0, 0 };

  hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
  if (hStdOut == INVALID_HANDLE_VALUE) return;

  /* Get the number of cells in the current buffer */
  if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
  cellCount = csbi.dwSize.X *csbi.dwSize.Y;

  /* Fill the entire buffer with spaces */
  if (!FillConsoleOutputCharacter(
    hStdOut,
    (TCHAR) ' ',
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Fill the entire buffer with the current colors and attributes */
  if (!FillConsoleOutputAttribute(
    hStdOut,
    csbi.wAttributes,
    cellCount,
    homeCoords,
    &count
    )) return;

  /* Move the cursor home */
  SetConsoleCursorPosition( hStdOut, homeCoords );
  }