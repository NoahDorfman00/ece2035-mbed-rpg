#include "mbed.h"
#include "globals.h"
// new class to play a note on Speaker based on PwmOut class
class SongPlayer
{
public:
    SongPlayer(PinName pin) : _pin(pin) {
// _pin(pin) means pass pin to the constructor
    }
    int on;
// class method to play a note based on PwmOut class
    void PlaySong(float frequency[], float duration[], float volume=1.0) {
        vol = volume;
        notecount = 0;
        _pin.period(1.0/frequency[notecount]);
        _pin = volume/2.0;
        if (on) noteduration.attach(this,&SongPlayer::nextnote, duration[notecount]);
        else _pin = 0.0;
        // setup timer to interrupt for next note to play
        frequencyptr = frequency;
        durationptr = duration;
        //returns after first note starts to play
    }
    void nextnote();
private:
    Timeout noteduration;
    PwmOut _pin;
    int notecount;
    float vol;
    float * frequencyptr;
    float * durationptr;
};
//Interrupt Routine to play next note
void SongPlayer::nextnote()
{
    _pin = 0.0;
    notecount++; //setup next note in song
    if (durationptr[notecount] && on == true) {
        _pin.period(1.0/frequencyptr[notecount]);
        noteduration.attach(this,&SongPlayer::nextnote, durationptr[notecount]);
        _pin = vol/2.0;
    } 
    
    else if(on == false) _pin = 0.0;
    
    else {
        PlaySong(frequencyptr, durationptr, 0.25);
    }
}