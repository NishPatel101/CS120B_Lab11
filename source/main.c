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
#include <windows.h> // For srand()
#include <string.h> // For string-to-LCD

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

enum Keypad_States { K_Start, K_Released, K_Pressed };
unsigned char x;
int Keypad(int state) {
    x = GetKeypadKey();
    switch (state) { // Transitions
        case K_Start:
            state = K_Released;
            break;

        case K_Released:
            if (x)
                state = K_Pressed;
            else
                state = K_Released;
            break;

        case K_Pressed:
            if (!x)
                state = K_Released;
            else
                state = K_Pressed;

        default:
            state = K_Start;
    }
    return state;
}

enum Problem_States { P_Start, P_Wait, P_OpSelect, P_Add, P_Sub, P_Mult, P_Div };
int op;
int a, b, answer;
int times_table[17][17];
int Problem(int state) {
    switch (state) {
	case P_Start:
	    for (int i = 0; i < 17; i++) { // Populating times_table only once
	        for (int j = 0; j < 17; j++) {
	            times_table[i][j] = i * j;
	        }
	    }
	    state = P_Wait;
	    break;
	
	case P_Wait:
	    if (needNewProb)
		state = P_OpSelect;
	    break;

	case P_OpSelect:
	    LARGE_INTEGER cicles;
 	    QueryPerformanceCounter(&cicles);
  	    srand (cicles.QuadPart);

	    op = rand() % 4;
	    if (op == 0)
		state = P_Add;
    	    else if (op == 1)
                state = P_Sub;
	    else if (op == 2)
                state = P_Mult;
	    else if (op == 3)
                state = P_Div;
	    break;

	case P_Add:
	    a = rand() % 129;
	    b = rand() % 129;

	    answer = a + b;
	    needNewProb = 0;
	    state = P_Wait;
            break;

	case P_Sub:
            a = rand() % 129;
            b = rand() % 129;
	
	    if (a < b) {
		int temp = a;
		a = b;
		b = temp;
	    }

            answer = a - b;
	    needNewProb = 0;
	    state = P_Wait;
            break;

	case P_Mult:
            a = rand() % 17;
            b = rand() % 17;

	    answer = times_table[a][b];
	    needNewProb = 0;
	    state = P_Wait;
            break;

	case P_Div:
            a = rand() % 17;
            b = rand() % 17;

	    answer = times_table[a][b];
	    int temp = a; // Swapping dividend and quotient
	    a = answer;   // since "answer" is quotient, not dividend
	    answer = a;
	    needNewProb = 0;
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

enum LCD_States { LCD_Start, LCD_WaitProblem, LCD_Problem, LCD_WaitEnter };
char a_str[8]; b_str[8], score_str[8], lives_str[8];
int LCD(int state) {
    switch (state) {
	case LCD_Start:
	    LCD_Message(3, "Math--Master");
	    LCD_Message(17, "Press # to play!");
	    if (begin)
	        state = LCD_WaitProblem;
	    break;

	case LCD_WaitProblem:
	    if (probReady)
		state = LCD_Problem;
	    break;

	case LCD_Problem:
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
	    LCD_Message(1, a_str); // Display problem expression

	    LCD_Message(17, "Score: "); itoa(score, score_str, 10); LCD_Message(24, score_str); // Display score
	    
	    itoa(lives, lives_str, 10); LCD_Message(32, lives_str); // Display lives left
	    state = LCD_WaitEnter;
	    break;

	case LCD_WaitEnter:
	    break;

	default:
	    state = LCD_Start;
    }

    switch (state) {
	// Stoof
    }
    return state;
}

enum Game_States { G_Start, G_WaitPressed, G_NewProblem, G_Problem, G_Correct, G_Incorrect, G_PostProbDisplay, G_Lost };
unsigned char userCursor = 0;
int user_answer = 0;
int count = 0;
int Game(int state) {
    switch (state) {
	case G_Start:
	    if (x == '#')
		state = G_WaitPressed;
	    break;

	case G_WaitPressed:
	    if (!x) {
		begin = 1;
		needNewProb = 1;
		state = G_NewProblem;
	    }
	    break;

	case G_NewProblem:
	    if (!needNewProb) { // Begin Problem
	    	state = G_Problem;
	    }
	    break;

	case G_Problem_Wait:
	    if (x) { // User input something
		if (x == '#') { // User submitted answer
		    probReady = 0; // To prepare for next problem
		    userEntered = 1;
		}
		else if (x == '*' && userCursor != 0) {
		    userSol[userCursor--] = '\0';
		}
		else { // Regular number
		    if (userCursor != 2)
			userSol[userCursor++] = x;
		}
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
		    unsigned char d = 100;
		    for (int m = 0; m < 3; m++) {
			if (userSol[m])
			    user_answer += (userSol[m] - 48) * d;
			d /= 10;
		    }
		    if (user_answer == answer)
			state = G_Correct;
		    else
			state = G_Incorrect;
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
	    LCD_Message(29, userSol);
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
            LCD_Message(29, userSol);
	    lives--;
	    if (lives == 0)
		state = G_Lost;
	    else {
		needNewProb = 1;
		state = G_PostProbDisplay;
	    }
	    break;

	case G_PostProbDisplay:
	    count += 50;
	    if (count == 2000) // Display 'Correct' or 'Incorrect' screen for 2 seconds
		state = G_NewProblem;
	    break;

	case G_Lost:
	    // Output:
	    // 		FINISH! --- pts!
	    // 	Restart with #
	    LCD_ClearScreen();
	    LCD_Message(1, "FINISH! ");
	    itoa(score, score_str, 10); strcat(score_str, " pts!"); LCD_Message(9, score_str);
	    LCD_Message(18, "Restart with #");
	    break;

	default:
	    state = G_Start;
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
    static task task1, task2, task3, task4, task5;
    task* tasks[] = { &task1, &task2, &task4, &task5 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;
    // Task 1 (Keypad)
    task1.state = start; // Task initial state
    task1.period = 50; // Task period
    task1.elapsedTime = task1.period; // Task current elapsed time
    task1.TickFct = &Keypad; // Function pointer for the tick

    // Task 2 (Problem)
    task2.state = start; // Task initial state
    task2.period = 25; // Task period
    task2.elapsedTime = task2.period; // Task current elapsed time
    task2.TickFct = &Problem; // Function pointer for the tick
    /*
    // Task 3 (Speaker)
    task3.state = start; // Task initial state
    task3.period = 200; // Task period
    task3.elapsedTime = task3.period; // Task current elapsed time
    task3.TickFct = &Speaker; // Function pointer for the tick
    */
    // Task 4 (LCD)
    task4.state = start; // Task initial state
    task4.period = 50; // Task period
    task4.elapsedTime = task4.period; // Task current elapsed time
    task4.TickFct = &LCD; // Function pointer for the tick
    
    // Task 5 (Game)
    task5.state = start; // Task initial state
    task5.period = 50; // Task period
    task5.elapsedTime = task5.period; // Task current elapsed time
    task5.TickFct = &Game; // Function pointer for the tick
 

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
