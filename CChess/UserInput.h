/*
File:           UserInput.h
Author:         Toni Lindeman
Description:    Functionality for getting and validating user input.
*/


#ifndef USERINPUT_H
#define USERINPUT_H

int getMenuChoice(int min, int max);
void clearConsole();
void promptReturnToContinue();
void getSelectSquare(int * outX, int * outY, int promptSelectOrMove);
void getScenarioPlacement(int * placement);
int promptYesNo(char * prompt);
int promptPromotePawn();

#endif /* USERINPUT_H */
