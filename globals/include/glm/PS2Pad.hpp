/* PS2Pad.h - header for PS2Pad
 * class, to be used with ps2sdk 
 * and libpad 
 *
 * most code taken from example
 * in ps2sdk samples rpc folder,
 * author 'pukko' 
 *
 * William (BJ) Blair
 * 04/27/2017 
 *
 *
 * valid buttons to send to clicked/held/getPressure
 * (defined in libpad.h):
 *  #define PAD_LEFT      0x0080
 *  #define PAD_DOWN      0x0040
 *  #define PAD_RIGHT     0x0020
 *  #define PAD_UP        0x0010
 *  #define PAD_START     0x0008
 *  #define PAD_R3        0x0004
 *  #define PAD_L3        0x0002
 *  #define PAD_SELECT    0x0001
 *  #define PAD_SQUARE    0x8000
 *  #define PAD_CROSS     0x4000
 *  #define PAD_CIRCLE    0x2000
 *  #define PAD_TRIANGLE  0x1000
 *  #define PAD_R1        0x0800
 *  #define PAD_L1        0x0400
 *  #define PAD_R2        0x0200
 *  #define PAD_L2        0x0100*/

#ifndef _BJ_PAD_H_
#define _BJ_PAD_H_

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <stdio.h>
#include <malloc.h>

#include "libpad.h"

#include <cstdlib>

class PS2Pad {
 public:
    /* constructor */
    PS2Pad(void);

    /* deconstructor */
    ~PS2Pad();
    
    bool init(int port, int slot);

    /* update pad status */
    void read(void);
    bool b_read(void);

    /* enter and exit pressure
     * sensitive usability */
    void enterPressureMode(void);
    void exitPressureMode(void);

    /* returns true if the
     * specified button (defined in
     * libpad.h) is pressed once */
    int clicked(int button);

    /* returns true if the specified
     * button is being held 
     * again, button is specified
     * in libpress.h */
    int held(int button);

    /* pressure from 0-255 on the
     * given button, assuming
     * pressure mode is enabled (which)
     * it should be by default */
    int getPressure(int button);

    void startLittleEngine(void);
    void stopLittleEngine(void);

    /* speed from 0-255 */
    void startBigEngine(int speed);
    void stopBigEngine(void);

    /* get joystick values - 
     * from 0-255, middle (not moved) is 127 */
    int getLeftJoyX(void);
    int getLeftJoyY(void);

    int getRightJoyX(void);
    int getRightJoyY(void);

 private:
    /* called by constructor */
    void loadModules(void);
    int waitPadReady(void);
    int initializePad(void);

    /* make sure each instance of the class
     * doesn't try to load modules every
     * time */
    static bool modulesLoaded;

    /* make sure the pad library isn't
     * initialized every instance */
    static bool padInitted;

    /* true if pressure mode is enabled currently */
    bool inPressureMode;

    int port;       // where the controller is plugged in
    int slot;

    /* pad buffer - holds pad dma workspace
     * should be size 256, aligned 64 */
    char *padBuf;

    /* holds the status of padGetState */
    int padRet;

    /* handles small and big motor */
    char actAlign[6];
    int actuators;

    /* represents the buttons on the
     * controller */
    struct padButtonStatus buttons;

    /* also holds pad data */
    u32 paddata;
    u32 old_pad;
    u32 new_pad;
};

#endif
