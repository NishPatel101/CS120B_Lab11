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
#include <string.h> // For string-to-LCD
#include <math.h> // For math functions checking user answer

#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
    static double current_frequency;
    if (frequency != current_frequency) {
        if(!frequency) 
            TCCR3B &= 0x08; // stops timer/counter
        else
            TCCR3B |= 0x03; // resumes/continues timer/counter
        if(frequency < 0.954)
            OCR3A = 0xFFFF;
        else if (frequency > 31250)
            OCR3A = 0x0000;
        else
            OCR3A = (short) (8000000 / (128 * frequency)) - 1;
        TCNT3 = 0; // resets counter
        current_frequency = frequency; // Updates the current frequency
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0); // COM3A0: Toggle PB3 on compare match between counter and OCR3A
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}


// Signalers/Gatekeepers
unsigned begin = 0;
unsigned char needNewProb = 1;
unsigned char userEntered = 0;
unsigned int time1 = 0, time2 = 0;

unsigned char p_correct = 0, p_incorrect = 0, p_timeOut = 0, p_speedUp = 0, p_lost = 0;
// Game values
unsigned char userSol[] = {'\0', '\0', '\0'};
int score = 0;
unsigned char lives = 5;
unsigned char x; // Keypad Key

enum Rand_States { R_Wait, R_Pressed, R_Finished };
int Rand(int state) {
    x = GetKeypadKey();

    switch(state) {
        case R_Wait:
            if (x == '#') {
                state = R_Pressed;
		srand(time1);
	    }
	    else
		time1++;
            break;

        case R_Pressed:
            if (!x) {
                state = R_Finished;
		srand(time2);
	    }
	    else
		time2++;
            break;

        default:
            state = R_Wait;
    }
    return state;
}

enum Problem_States { P_Start, P_Wait, P_Generate };
int op;
int a, b, answer;
char answer_str[8];
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
            	    b = (rand() % 16) + 1; // Avoid 0 in denominator

	    	    answer = times_table[a][b];
	    	    int temp = a; // Swapping dividend and quotient
	    	    a = answer;   // since "answer" is quotient, not dividend
	    	    answer = temp;
            	    break;
	    }
	    needNewProb = 0;
	    state = P_Wait;
	    break;

	default:
	    state = P_Start;
    }
    return state;
}

// A#-466.16, B-493.88, C#-554.37, D#-622.25, E-659.25, F#-739.99, G#-830.61, A#-932.33, B-987.77
double title[] = {493.88, 493.88, 0.0, 0.0, 493.88, 0.0, 493.88, 0.0, 493.88, 493.88, 0.0, 0.0, 622.25, 622.25, 0.0, 0.0, 739.99, 739.99, 739.99, 739.99, 739.99, 739.99, 0.0, 0.0, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 0.0, 0.0, 659.25, 659.25, 0.0, 0.0, 659.25, 0.0, 659.25, 0.0, 659.25, 659.25, 0.0, 0.0, 554.37, 554.37, 0.0, 0.0, 493.88, 493.88, 493.88, 493.88, 493.88, 493.88, 0.0, 0.0, 466.16, 466.16, 466.16, 466.16, 466.16, 466.16, 0.0, 0.0, 493.88, 493.88, 0.0, 0.0, 493.88, 0.0, 493.88, 0.0, 493.88, 493.88, 0.0, 0.0, 622.25, 622.25, 0.0, 0.0, 739.99, 739.99, 739.99, 739.99, 739.99, 739.99, 0.0, 0.0, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 0.0, 0.0, 659.25, 659.25, 0.0, 0.0, 659.25, 0.0, 659.25, 0.0, 659.25, 659.25, 0.0, 0.0, 830.61, 830.61, 0.0, 0.0, 739.99, 739.99, 739.99, 739.99, 739.99, 739.99, 0.0, 0.0, 739.99, 739.99, 0.0, 0.0, 932.33, 932.33, 0.0, 0.0, 987.77, 987.77, 0.0, 0.0, 987.77, 0.0, 987.77, 0.0, 987.77, 987.77, 0.0, 0.0, 932.33, 932.33, 0.0, 0.0, 739.99, 739.99, 739.99, 739.99, 739.99, 739.99, 0.0, 0.0, 622.25, 622.25, 0.0, 0.0, 739.99, 739.99, 0.0, 0.0, 830.61, 830.61, 0.0, 0.0, 830.61, 0.0, 830.61, 0.0, 830.61, 830.61, 0.0, 0.0, 739.99, 739.99, 0.0, 0.0, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 0.0, 0.0, 554.37, 554.37, 0.0, 0.0, 622.25, 622.25, 0.0, 0.0, 659.25, 659.25, 0.0, 0.0, 659.25, 0.0, 659.25, 0.0, 659.25, 659.25, 0.0, 0.0, 622.25, 622.25, 0.0, 0.0, 554.37, 554.37, 554.37, 554.37, 554.37, 554.37, 0.0, 0.0, 659.25, 659.25, 659.25, 659.25, 659.25, 659.25, 0.0, 0.0, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 622.25, 0.0, 0.0, 554.37, 554.37, 0.0, 0.0, 493.88, 493.88, 493.88, 493.88, 493.88, 493.88, 0.0, 0.0, 466.16, 466.16, 466.16, 466.16, 466.16, 466.16, 0.0, 0.0};
//double problem[];
double correct[] = {698.46, 0.0, 659.25, 698.46, 698.46, 698.46, 698.46, 698.46};
double incorrect[] = {293.66, 0.0, 277.18, 0.0, 261.63, 0.0, 246.94, 246.94, 246.94, 246.94, 246.94, 246.94};
double timeOut[] = {554.37, 130.81, 130.81, 130.81, 130.81, 130.81, 130.81, 130.81};
double speedUp[] = {250, 0, 250, 0, 250, 0}; 
double lost[256];
unsigned char note = 0;
unsigned long bpm = 65;
enum Speaker_States { S_Title, S_Problem, S_Correct, S_Incorrect, S_TimeOut, S_SpeedUp, S_Lost };
int Speaker(int state) {
    switch (state) { // Transitions
	case S_Title:
	    if(begin) {
		state = S_Problem;
		set_PWM(0.0);
	    }
	    else {
	        if (note == (sizeof(title)/sizeof(*title)))
		    note = 0; // Loop
		    set_PWM(title[note]);
	            note++;
    	    }
	    break;

	case S_Problem:
	    note = 0;
	    if (p_correct) {
		bpm = 107;
		state = S_Correct;
		p_correct = 0;
	    }
	    else if (p_incorrect) {
		bpm = 167;
		state = S_Incorrect;
		p_incorrect = 0;
	    }
	    else if (p_timeOut) {
		bpm = 150;
		state = S_TimeOut;
		p_timeOut = 0;
	    }
	    else if (p_speedUp) {
		bpm = 500;
		state = S_SpeedUp;
		p_speedUp = 0;
	    }
	    else if (p_lost) {
		bpm = 65;
		state = S_Lost;
		p_lost = 0;
	    }
	    break;

	case S_Correct:
	    bpm = 107;
	    if (note == (sizeof(correct)/sizeof(*correct))) {
                note = 0;
                state = S_Problem;
                set_PWM(0.0);
            }
            else {
                set_PWM(correct[note]);
                note++;
            }
	    break;

	case S_Incorrect:
	    bpm = 167;
	    if (note == (sizeof(incorrect)/sizeof(*incorrect))) {
                note = 0;
                state = S_Problem;
                set_PWM(0.0);
            }
            else {
                set_PWM(incorrect[note]);
                note++;
            }
	    break;

	case S_TimeOut:
	    bpm = 150;
	    if (note == (sizeof(timeOut)/sizeof(*timeOut))) {
                note = 0;
                state = S_Problem;
                set_PWM(0.0);
            }
            else {
                set_PWM(timeOut[note]);
                note++;
            }
	    break;

	case S_SpeedUp:
	    bpm = 500;
	    if (note == (sizeof(speedUp)/sizeof(*speedUp))) {
		note = 0;
		state = S_Problem;
		set_PWM(0.0);
	    }
	    else {
		set_PWM(speedUp[note]);
		note++;
	    }
	    break;

	case S_Lost:
	    bpm = 65;
	    if(begin || needNewProb) {
                state = S_Problem;
		set_PWM(0.0);
	    }
            else {
                if (note == (sizeof(lost)/sizeof(*lost)))
                    note = 0; // Loop
                    set_PWM(lost[note]);
                    note++;
            }
	    break;

	default:
	    state = S_Title;
    }
    return state;
}

// Sadly, this SM would not work with the others. Might be conflicts with using PORTB, so oh well :(
unsigned char lights[] = {0x41, 0x43, 0x47, 0x4F, 0x5F, 0xEF, 0xFF}; // Set PORTB to values for Timer light
unsigned char timeLight = 7;
unsigned char timeLimitSecs = 7;
enum Timer_States { T_Start, T_Wait, T_Light };
int Timer(int state) {
    switch(state) {
	case T_Start:
	    if (begin)
		state = T_Wait;
	    break;

	case T_Wait:
	    if (!needNewProb)
		state = T_Light;
	    break;

	case T_Light:
	    timeLight--;
	    if (timeLight >= 0)
	        PORTB = lights[timeLight];
	    else {
		state = T_Wait;
		timeLight = timeLimitSecs;
	    }	
	    break;

	default:
	    state = T_Start;
    }
    return state;
}

enum Game_States { G_Title, G_Start, G_WaitBegin, G_NewProblem, G_Problem_Wait, G_Problem_Pressed, G_Correct, G_SpeedUp, G_Incorrect, G_TimedOut, G_PostProbDisplay, G_Lost };
char a_str[12]; b_str[8], score_str[8], lives_str[8];
unsigned short sz_a;
unsigned char userCursor = 0; // Index in user_answer array
unsigned char cursor = 0; // Actual LCD cursor for user display
int user_answer = 0;
int count = 0;
unsigned short timePassed = 0, timeLimit = 7000;
int Game(int state) {
    switch (state) {
	case G_Title:
	    LCD_Message(1, "**Math--Master**");
            LCD_Message(17, "Press # to play!");
	    state = G_Start;
	    break;
	
	case G_Start:
	    score = 0;
	    lives = 5;
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
	    if (timePassed >= timeLimit) {
		state = G_TimedOut;
		p_timeOut = 1;
	    }
	    else {
	    	if (x) { // User input something
		    if (x == '#') { // User submitted answer
		        userEntered = 1;
		    }
		    else if (x == '*') { // Backspace ('*')
		        if (userCursor != 0)
			    userSol[--userCursor] = ' ';
		    }
		    else { // Regular number
		        if (userCursor < 3)
			    userSol[userCursor++] = x;
		    }
		    LCD_Message(sz_a, userSol);
		    LCD_Message(15, "  ");
		    state = G_Problem_Pressed;
	        }
	    }
	    timePassed += 50;
	    count = 0; // Resetting for result screen
	    break;

	case G_Problem_Pressed:
	    if (timePassed >= timeLimit) {
		state = G_TimedOut;
		p_timeOut = 1;
	    }
	    else {
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
		        if (user_answer == answer) {// CHECK ANSWER
			    state = G_Correct;
			    p_correct = 1;
			}
		        else {
			    state = G_Incorrect;
			    p_incorrect = 1;
			}
			
		        itoa(answer, answer_str, 10); // Solution string for displaying on 'Correct' and 'Incorrect'
		    }
		    else
		        state = G_Problem_Wait;
	        }
	    }
	    timePassed += 50;
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

	case G_SpeedUp:
	    if (count == 0 || count == 1000 || count == 2000) { // Blink on
	        LCD_Message(5, "SPEEDING");
		LCD_Message(24, "UP");
	    }
	    else if (count == 500 || count == 1500 || count == 2500) // Blink off
		LCD_ClearScreen();
	    else if (count >= 3000) // Screen finished; go to next problem
		state = G_NewProblem;
	    count += 50; // Incrementing
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

	case G_TimedOut:
	    itoa(answer, answer_str, 10); // Solution string for displaying on 'Timed Out'
	    LCD_ClearScreen();
	    LCD_Message(4, "Time's up!");
	    LCD_Message(19, "Solution: ");
	    LCD_Message(29, answer_str);
	    lives--;
	    needNewProb = 1;
	    state = G_PostProbDisplay;
	    break;

	case G_PostProbDisplay:
	    timePassed = 0; // Reset time passed
	    userEntered = 0; // Resetting user submission checker for next problem
            userSol[0] = '\0'; userSol[1] = '\0'; userSol[2] = '\0'; // Reset array for next problem
            userCursor = 0; // Reset user cursor for next problem
	    count += 50;
	    if (count == 1500) { // Display 'Correct','Incorrect' or 'Timed Out' screen for 1.4 seconds
		if (lives == 0) {
		    state = G_Lost;
		    p_lost = 1;
		}
		else if (score == 7 && timeLimit != 6000) { // Second condition if speed up already happened and score stayed the same
		    state = G_SpeedUp;
		    p_speedUp = 1;
		    timeLimit = 6000;
		    timeLight = 6;
		    timeLimitSecs = 6;
		}
		else if (score == 12 && timeLimit != 5000) {
		    state = G_SpeedUp;
		    p_speedUp = 1;
		    timeLimit = 5000;
		    timeLight = 5;
		    timeLimitSecs = 5;
		}
		else
		    state = G_NewProblem;
		count = 0; // Resetting for next problem's results display
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

    for (unsigned p = 0; p < 256; p++)
	lost[p] = pow(title[p], 1.0595);

    // Declare an array of tasks
    static task task1, task2, task3, task4, task5;
    task* tasks[] = { &task4, &task1, &task3, &task2 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;
    // Task 1 (Problem)
    task1.state = start; // Task initial state
    task1.period = 25; // Task period
    task1.elapsedTime = task1.period; // Task current elapsed time
    task1.TickFct = &Problem; // Function pointer for the tick
    
    // Task 2 (Speaker)
    task2.state = start; // Task initial state
    task2.period = 65; // Task period
    task2.elapsedTime = task2.period; // Task current elapsed time
    task2.TickFct = &Speaker; // Function pointer for the tick
    
    // Task 3 (Game)
    task3.state = start; // Task initial state
    task3.period = 50; // Task period
    task3.elapsedTime = task3.period; // Task current elapsed time
    task3.TickFct = &Game; // Function pointer for the tick
 
    // Task 4 (Rand)
    task4.state = start; // Task initial state
    task4.period = 1; // Task period
    task4.elapsedTime = task4.period; // Task current elapsed time
    task4.TickFct = &Rand; // Function pointer for the tick

    // Task 5 (Timer) NOT USED :(
    task5.state = start; // Task initial state
    task5.period = 1000; // Task period
    task5.elapsedTime = task5.period; // Task current elapsed time
    task5.TickFct = &Timer; // Function pointer for the tick

    unsigned long GCD = tasks[0]->period; // Setting timer to GCD of task periods
    for (unsigned i = 1; i < numTasks; i++)
	GCD = findGCD(GCD, tasks[i]->period);
    TimerSet(GCD);
    TimerOn();

    PWM_on(); set_PWM(0.0);
    LCD_init();
    unsigned short i; // Scheduler for-loop iterator
    while (1) {
	task2.period = bpm; // Dynamically changing bpm

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
