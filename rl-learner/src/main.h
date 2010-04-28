#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include "game.h"
#include "RL.h"
#include "RL_Q.h"
#include "RL_Action.h"
#include <time.h>

#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/program_options.hpp>

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

using boost::property_tree::ptree;
namespace po = boost::program_options;


unsigned int *actionStateVisits;
unsigned int *stateVisits;
unsigned int numActions;
unsigned int numStates;

void ChangeColour(WORD theColour);
void PrintGameStatus(brainSpace::Game *g);
void PrintAction(bool debug,brainSpace::RL_Action a, unsigned short builder);
void PrintOutputHeader();

void PrintStateVisits();
void InitStateVisits();
void SaveStateVisits();
void UpdateStateVisits(brainSpace::RL *r);
void LoadConfig(int argc, char *argv[]);

#endif