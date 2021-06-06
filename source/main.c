/*	Author: npate145
 *  	Partner(s) Name: Nish Patel
 *	Lab Section: 022
 *	Assignment: Lab #11  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // For string-to-LCD
#include <math.h> // For math functions checking user answer

#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "simAVRHeader.h"
#endif

// Signalers/Gatekeepers
unsigned begin = 0;
unsigned char needNewProb = 1;
unsigned char probReady = 0;
unsigned char userEntered = 0;
// Game values
unsigned char userSol[] = {'\0', '\0', '\0'};
int score = 0;
unsigned char lives = 3;
unsigned char x; // Keypad Key

enum Problem_States { P_Start, P_Wait, P_Generate };
int op;
int a, b, answer;
char answer_str[8];
int times_table[17][17];
int Problem(int state) {
    x = GetKeypadKey();

    switch (state) {
	case P_Start:
	    for (int i = 0; i < 17; i++) { // Populating times_table only once
	        for (int j = 0; j < 17; j++) {
	            times_table[i][j] = i * j;
	        }
	    }
	    srand(time(0)); // Altering seed
	    state = P_Wait;
	    break;
	
	case P_Wait:
	    if (needNewProb)
		state = P_Generate;
	    break;

	case P_Generate:
	    op = rand() % 4;
	
	    switch (op) {
		case 0: // Addition
	    	    a = rand() % 129;
	    	    b = rand() % 129;

	    	    answer = a + b;
            	    break;

		case 1: // Subtraction
            	    a = rand() % 129;
            	    b = rand() % 129;
	
	    	    if (a < b) {
			int temp = a;
			a = b;
			b = temp;
	    	    }

            	    answer = a - b;
            	    break;

		case 2: // Multiplication
            	    a = rand() % 17;
            	    b = rand() % 17;

	    	    answer = times_table[a][b];
            	    break;

		case 3: // Division
            	    a = rand() % 17;
            	    b = rand() % 17;

	    	    answer = times_table[a][b];
	    	    int temp = a; // Swapping dividend and quotient
	    	    a = answer;   // since "answer" is quotient, not dividend
	    	    answer = temp;
            	    break;
	    }
	    needNewProb = 0;
	    probReady = 1;
	    state = P_Wait;
	    break;

	default:
	    state = P_Start;
    }
    return state;
}

/*
enum Speaker_States { S_Start };
int Speaker(int state) {
    switch (state) { // Transitions
	case S_Start:
	    state = ;
	    break;

	default:
	    state = S_Start;
    }

    switch (state) { // Actions
	case S_Start:
	    break;
    }
    return state;
}
*/

enum Game_States { G_Title, G_Start, G_WaitBegin, G_NewProblem, G_Problem_Wait, G_Problem_Pressed, G_Correct, G_Incorrect, G_PostProbDisplay, G_Lost };
char a_str[12]; b_str[8], score_str[8], lives_str[8];
unsigned short sz_a;
unsigned char userCursor = 0; // Index in user_answer array
unsigned char cursor = 0; // Actual LCD cursor for user display
int user_answer = 0;
int count = 0;
int Game(int state) {
    switch (state) {
	case G_Title:
	    LCD_Message(1, "**Math--Master**");
            LCD_Message(17, "Press # to play!");
	    state = G_Start;
	    break;
	
	case G_Start:
	    score = 0;
	    lives = 3;
	    if (x == '#')
		state = G_WaitBegin;
	    break;

	case G_WaitBegin:
	    if (!x) {
		begin = 1;
		needNewProb = 1;
		state = G_NewProblem;
	    }
	    break;

	case G_NewProblem:
	    if (!needNewProb) { // Begin Problem
		itoa(a, a_str, 10); itoa(b, b_str, 10);

           	if (op == 0)
                    strcat(a_str, " + ");
            	else if (op == 1)
                    strcat(a_str, " - ");
            	else if (op == 2)
                    strcat(a_str, " * ");
            	else if (op == 3)
                    strcat(a_str, " / ");

            	strcat(a_str, b_str); strcat(a_str, " = ");
            	LCD_ClearScreen();
            	sz_a = sizeof(a_str) / sizeof(*a_str);
            	LCD_Message(1, a_str); // Display problem expression

            	LCD_Message(17, "Score: "); itoa(score, score_str, 10); LCD_Message(24, score_str); // Display score

            	itoa(lives, lives_str, 10); LCD_Message(32, lives_str); // Display lives left
		cursor = sz_a;
	    	state = G_Problem_Wait;
	    }
	    break;

	case G_Problem_Wait:
	    if (x) { // User input something
		if (x == '#') { // User submitted answer
		    //probReady = 0; // To prepare for next problem
		    userEntered = 1;
		}
		else if (x == '*') { // Backspace ('*')
		    if (userCursor != 0)
			userSol[--userCursor] = ' ';
		        cursor--;
			LCD_Cursor(sz_a + cursor);
		}
		else { // Regular number
		    if (userCursor < 3) {
			userSol[userCursor++] = x;
			//LCD_WriteData(x);
			cursor++;
			LCD_Cursor(sz_a + cursor);
		    }
		}
		//char temp[5];
		//for (unsigned z = 0; z < 3; z++)
		  //  temp[z] = userSol[z];
		//temp[3] = ' '; temp[4] = ' ';
		//LCD_Message(sz_a, temp);
		LCD_Message(sz_a, userSol);
		LCD_Message(15, "  ");
		state = G_Problem_Pressed;
	    }
	    break;

	case G_Problem_Pressed:
	    if (!x) {
		// Compare user-input and answer
                // Equal        ->      G_Correct
                // Not equal    ->      G_Incorrect ... lives--
		if (userEntered) { // Pressed # ~ check answer
		    user_answer = 0;
		    unsigned char nuls = 0;
		    unsigned char d = 100;
		    for (char m = 2; m > -1; m--) {
			if (userSol[m] == ' ' || !userSol[m])
			    nuls++;
		    }
		    if (nuls == 3)
			user_answer = -1; // WRONG ANSWER
		    else {
			d /= (int) (pow(10, nuls) + 0.5);
		    	for (int n = 0; n < 3; n++) {
			    if (userSol[n])
			    	user_answer += (userSol[n] - 48) * d;
			    d /= 10;
		    	}
		    }
		    if (user_answer == answer) // CHECK ANSWER
			state = G_Correct;
		    else
			state = G_Incorrect;
			
		    itoa(answer, answer_str, 10); // Solution string for displaying on next screen

		    userEntered = 0; // Resetting user submission checker for next problem
		    userSol[0] = '\0'; userSol[1] = '\0'; userSol[2] = '\0'; // Reset array for next problem
		    userCursor = 0; // Reset user cursor for next problem
		    count = 0; // Resetting for next screen
		}
		else
		    state = G_Problem_Wait;	
	    }
	    break;

	case G_Correct:
	    // Output:
	    // 		Correct!! :)
	    // 	Solution: ---
	    // Then score++
	    LCD_ClearScreen();
	    LCD_Message(3, "Correct!! :)");
	    LCD_Message(19, "Solution: ");
	    LCD_Message(29, answer_str);
	    score++;
	    needNewProb = 1;
	    state = G_PostProbDisplay;
	    break;

	case G_Incorrect:
	    // Output:
	    // 		Incorrect! :(
	    //  Solution: --- 
	    // Then check if lives == 0...
	    // 0	->	G_Lost
	    // Not 0	->	G_Problem
	    LCD_ClearScreen();
            LCD_Message(2, "Incorrect.. :(");
            LCD_Message(19, "Solution: ");
            LCD_Message(29, answer_str);
	    lives--;
	    needNewProb = 1;
	    state = G_PostProbDisplay;
	    break;

	case G_PostProbDisplay:
	    count += 50;
	    if (count == 2000) { // Display 'Correct' or 'Incorrect' screen for 2 seconds
		if (lives == 0)
		    state = G_Lost;
		else
		    state = G_NewProblem;
		LCD_ClearScreen();
	    }
	    break;

	case G_Lost:
	    // Output:
	    // 		FINISH! --- pts!
	    // 	Restart with #
	    LCD_ClearScreen();
	    LCD_Message(1, "FINISH! ");
	    itoa(score, score_str, 10); strcat(score_str, " pts!"); LCD_Message(9, score_str);
	    LCD_Message(18, "Restart with #");

	    begin = 0;
	    state = G_Start; // Let player press '#' to restart
	    break;

	default:
	    state = G_Title;
    }
    return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    // Declare an array of tasks
    static task task1, task2, task3;
    task* tasks[] = { &task1, &task3 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;
    // Task 1 (Problem)
    task1.state = start; // Task initial state
    task1.period = 25; // Task period
    task1.elapsedTime = task1.period; // Task current elapsed time
    task1.TickFct = &Problem; // Function pointer for the tick
    /*
    // Task 2 (Speaker)
    task2.state = start; // Task initial state
    task2.period = 200; // Task period
    task2.elapsedTime = task2.period; // Task current elapsed time
    task2.TickFct = &Speaker; // Function pointer for the tick
    */
    // Task 3 (Game)
    task3.state = start; // Task initial state
    task3.period = 50; // Task period
    task3.elapsedTime = task3.period; // Task current elapsed time
    task3.TickFct = &Game; // Function pointer for the tick
 

    unsigned long GCD = tasks[0]->period; // Setting timer to GCD of task periods
    for (unsigned i = 1; i < numTasks; i++)
	GCD = findGCD(GCD, tasks[i]->period);
    TimerSet(GCD);
    TimerOn();

    //PWM_on(); set_PWM(0);
    LCD_init();
    unsigned short i; // Scheduler for-loop iterator
    while (1) {
	for (i = 0; i < numTasks; i++) { // Scheduler code
	    if (tasks[i]->elapsedTime == tasks[i]->period) { // Task is ready to tick
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state); // Set next state
				tasks[i]->elapsedTime = 0; // Reset the elapsed time for next tick
	    }
	    tasks[i]->elapsedTime += GCD;
	}
	
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1; // Error: Program should not exit!
}
