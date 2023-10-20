// This header has all the (extern) declarations of the globals.
// "extern" means "this is instantiated somewhere, but here's what the name
// means.
#include "globals.h"

#include "hardware.h"

// We need to actually instantiate all of the globals (i.e. declare them once
// without the extern keyword). That's what this file does!

// Hardware initialization: Instantiate all the things!
uLCD_4DGL uLCD(p9,p10,p11);             // LCD Screen (tx, rx, reset)
SDFileSystem sd(p5, p6, p7, p8, "sd");  // SD Card(mosi, miso, sck, cs)
Serial pc(USBTX,USBRX);                 // USB Console (tx, rx)
MMA8452 acc(p28, p27, 100000);        // Accelerometer (sda, sdc, rate)
DigitalIn button1(p21);                 // Pushbuttons (pin)
DigitalIn button2(p22);
DigitalIn button3(p23);
AnalogOut DACout(p18);                  // Speaker (pin)
PwmOut speaker(p26);
wave_player waver(&DACout);

//Accelerometer Data
int accx=0, *accxp=&accx;
int accy=0, *accyp=&accy;
int accz=0, *acczp=&accz;

// Some hardware also needs to have functions called before it will set up
// properly. Do that here.
int hardware_init()
{
    // Crank up the speed
    uLCD.baudrate(3000000);
    pc.baud(115200);
        
    //Initialize pushbuttons
    button1.mode(PullUp); 
    button2.mode(PullUp);
    button3.mode(PullUp);
    
    
    //Initialize Accelerometer
    if(acc.activate()){
        return ERROR_MEH;
    }
    return ERROR_NONE;
}

GameInputs read_inputs() 
{
    GameInputs in;
    in.b1 = button1;
    in.b2 = button2;
    in.b3 = button3;
    ASSERT_P(acc.isXYZReady(), "Accelerometer not ready!");
    ASSERT_P(!(acc.readXYZCounts(accxp,accyp,acczp)), "Accelerometer read failed!");
    in.ax = accx;
    in.ay = accy;
    in.az = accz;
    return in;
}
