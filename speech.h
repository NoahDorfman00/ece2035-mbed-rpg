#ifndef SPEECH_H
#define SPEECH_H

/**
 * Display a speech bubble.
 */
void speech(char* line1, char* line2);

/**
 * Display a long speech bubble (more than 2 lines).
 * 
 * @param lines The actual lines of text to display
 * @param n The number of lines to display.
 */
void long_speech(char* lines[], int n);

#endif // SPEECH_H