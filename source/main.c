/*	Author: npate145
 *  	Partner(s) Name: Nish Patel
 *	Lab Section: 022
 *	Assignment: Lab #11
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "simAVRHeader.h"
#endif

enum Keypad_States { K_Start, K_Released, K_Pressed };
unsigned char x;
int KeypadSM(int state) {
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
/*
enum Sound_States {};
int SoundSM(int state) {
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

enum */

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0x00; PORTC = 0xFF;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    // Declare an array of tasks
    static task task1, task2, task3, task4, task5;
    task* tasks[] = { &task1, &task2, &task3, &task4, &task5 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;
    // Task 1 (KeypadLED)
    task1.state = start; // Task initial state
    task1.period = 50; // Task period
    task1.elapsedTime = task1.period; // Task current elapsed time
    task1.TickFct = &KeypadSM; // Function pointer for the tick

    /*
    // Task 2 (UnlockCombination)
    task2.state = start; // Task initial state
    task2.period = 50; // Task period
    task2.elapsedTime = task2.period; // Task current elapsed time
    task2.TickFct = &UnlockCombination; // Function pointer for the tick
    
    // Task 3 (LockDoor)
    task3.state = start; // Task initial state
    task3.period = 50; // Task period
    task3.elapsedTime = task3.period; // Task current elapsed time
    task3.TickFct = &LockDoor; // Function pointer for the tick

    // Task 4 (Doorbell)
    task4.state = start; // Task initial state
    task4.period = 200; // Task period
    task4.elapsedTime = task4.period; // Task current elapsed time
    task4.TickFct = &Doorbell; // Function pointer for the tick

    // Task 5 (UserMadeCombo)
    task5.state = start; // Task initial state
    task5.period = 50; // Task period
    task5.elapsedTime = task5.period; // Task current elapsed time
    task5.TickFct = &UserMadeCombo; // Function pointer for the tick
    */

    unsigned long GCD = tasks[0]->period; // Setting timer to GCD of task periods
    for (unsigned i = 1; i < numTasks; i++)
	GCD = findGCD(GCD, tasks[i]->period);
    TimerSet(GCD);
    TimerOn();

    //PWM_on(); set_PWM(0);
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
