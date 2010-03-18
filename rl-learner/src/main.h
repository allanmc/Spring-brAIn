#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32
#define FOREGROUND_INTENSITY 0
#define FOREGROUND_GREEN 0
#define FOREGROUND_RED 0
#define FOREGROUND_BLUE 0
typedef int WORD;
#endif


unsigned int *actionStateVisits;
unsigned int *stateVisits;
unsigned int numActions;
unsigned int numStates;

void InitStateVisits();
void SaveStateVisits();
void UpdateStateVisits(brainSpace::RL *r);