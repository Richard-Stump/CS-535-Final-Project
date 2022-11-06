/* PS2Pad.cpp - implementation for PS2Pad
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
 * */

#include "../include/PS2Pad.hpp"

/* initialize static member variables */
bool PS2Pad::modulesLoaded = false;
bool PS2Pad::padInitted = false;

/* constructor */
PS2Pad::PS2Pad(void)
{

    //this->port = port;
    //this->slot = slot;
    this->port = this->slot = 0;

    actuators = 0;
    padRet = 0;
    paddata = 0;
    old_pad = 0;
    new_pad = 0;

    padBuf = NULL;

    inPressureMode = false;

    PS2Pad::modulesLoaded = false;
}

bool PS2Pad::init(int port, int slot)
{
    int ret;
    
    printf("New Pad: (%d, %d)\n", this->port, this->slot);

    /* load rpc controller modules if they haven't been loaded */
    if (!PS2Pad::modulesLoaded) {
        loadModules();
    }

    /* initialize pad library */
    if (!PS2Pad::padInitted) {
        /* according to libpad.h:
         * padInit(0) should work... */
        padInit(0);
    }

    /* Info about the port and slot */
    printf("PortMax: %d\n", padGetPortMax());
    printf("SlotMax: %d\n", padGetSlotMax(port));

    /* allocate memory for the pad buffer */
    padBuf = (char *)memalign(64, sizeof(char) * 256);
    if (!padBuf) {
        printf("Error allocating padbuf memory!\n");
        //SleepThread();
        return false;
    }

    /* open the port and slot */
    if ((ret = padPortOpen(port, slot, padBuf)) == 0) {
        printf("padOpenPort failed: %d\n", ret);
        //SleepThread();
        return false;
    }

    if (!initializePad()) {
        printf("pad initalization failed!\n");
        //SleepThread();
        return false;
    }

    /* try entering pressure mode by default */
    if (padInfoPressMode(port, slot)) {
        padEnterPressMode(port, slot);
        inPressureMode = true;
        printf("Entered pressure mode!\n");
    } else {
        printf("Pressure mode not available?\n");
    }

    
    return true;
}

/* deconstructor */
PS2Pad::~PS2Pad()
{
    if (padBuf) {
        free(padBuf);
        padBuf = NULL;
    }
    printf("End of pad deconstructor!\n");
}

void PS2Pad::loadModules(void)
{

    if (!(PS2Pad::modulesLoaded)) {
        int ret;

        ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
        if (ret < 0) {
            printf("sifLoadModule sio failed: %d\n", ret);
            SleepThread();
        }

        ret = SifLoadModule("rom0:PADMAN", 0, NULL);
        if (ret < 0) {
            printf("sifLoadModule pad failed: %d\n", ret);
            SleepThread();
        }

        PS2Pad::modulesLoaded = true;
    }
    return;
}

int PS2Pad::waitPadReady(void)
{
    int state;
    int lastState;
    char stateString[16];

    state = padGetState(port, slot);
    lastState = -1;
    while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s\n",
                   port, slot, stateString);
        }
        lastState = state;
        state = padGetState(port, slot);
    }
    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        printf("Pad OK!\n");
    }
    return 0;
}

int PS2Pad::initializePad(void)
{

    int ret;
    int modes;
    int i;

    waitPadReady();

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
    printf("The device has %d modes\n", modes);

    if (modes > 0) {
        printf("( ");
        for (i = 0; i < modes; i++) {
            printf("%d ",
                   padInfoMode(port, slot, PAD_MODETABLE, i));
        }
        printf(")");
    }

    printf("It is currently using mode %d\n",
           padInfoMode(port, slot, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
        printf("This is a digital controller?\n");
        return 1;
    }
    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(port, slot, PAD_MODETABLE, i) ==
            PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        printf("This is no Dual Shock controller\n");
        return 1;
    }
    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
    if (ret == 0) {
        printf("This is no Dual Shock controller??\n");
        return 1;
    }

    printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady();
    printf("infoPressMode: %d\n", padInfoPressMode(port, slot));

    waitPadReady();
    printf("enterPressMode: %d\n", padEnterPressMode(port, slot));

    waitPadReady();
    actuators = padInfoAct(port, slot, -1, 0);
    printf("# of actuators: %d\n", actuators);

    if (actuators != 0) {
        actAlign[0] = 0;    // Enable small engine
        actAlign[1] = 1;    // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady();
        printf("padSetActAlign: %d\n",
               padSetActAlign(port, slot, actAlign));
    } else {
        printf("Did not find any actuators.\n");
    }

    waitPadReady();

    return 1;
}

void PS2Pad::read(void)
{
    int i = 0;
    padRet = padGetState(port, slot);
    while ((padRet != PAD_STATE_STABLE) && (padRet != PAD_STATE_FINDCTP1)) {
        if (padRet == PAD_STATE_DISCONN) {
            printf("Pad(%d, %d) is disconnected\n", port, slot);
        }
        padRet = padGetState(port, slot);
    }
    if (i == 1) {
        printf("Pad: OK!\n");
    }

    padRet = padRead(port, slot, &buttons); // port, slot, buttons

    if (padRet != 0) {
        paddata = 0xffff ^ buttons.btns;

        new_pad = paddata & ~old_pad;
        old_pad = paddata;
    } else {
        printf("Error reading pad!\n");
    }

    return;
}

int PS2Pad::clicked(int button)
{
    return new_pad & button;
}

int PS2Pad::held(int button)
{
    /* flip the bits of the button,
     * since by default power is coming from
     * all buttons; pressin one interrupts it,
     * then return it and'd with the desired
     * button */
    return (0xffff ^ buttons.btns) & button;
}

void PS2Pad::startLittleEngine(void)
{
    if (actAlign[0] != 1) {
        actAlign[0] = 1;
        padSetActDirect(port, slot, actAlign);
    } else {
        printf("Little engine already running!\n");
    }
    return;
}

void PS2Pad::stopLittleEngine(void)
{
    if (actAlign[0] != 0) {
        actAlign[0] = 0;
        padSetActDirect(port, slot, actAlign);
    } else {
        printf("Little engine already stopped!\n");
    }
    return;
}

void PS2Pad::startBigEngine(int speed)
{
    if (speed > 255) {
        printf("Invalid speed: %d, setting to 255 intead\n", speed);
        speed = 255;
    } else if (speed < 0) {
        printf("Invalid speed: %d, setting to 0 instead\n", speed);
        speed = 0;
    }
    actAlign[1] = speed;
    padSetActDirect(port, slot, actAlign);

    return;
}

void PS2Pad::stopBigEngine(void)
{
    if (actAlign[1] != 0) {
        actAlign[1] = 0;
        padSetActDirect(port, slot, actAlign);
    } else {
        printf("Big engine already stopped!\n");
    }

    return;
}

int PS2Pad::getLeftJoyX(void)
{
    return buttons.ljoy_h;
}

int PS2Pad::getLeftJoyY(void)
{
    return buttons.ljoy_v;
}

int PS2Pad::getRightJoyX(void)
{
    return buttons.rjoy_h;
}

int PS2Pad::getRightJoyY(void)
{
    return buttons.rjoy_v;
}

void PS2Pad::enterPressureMode(void)
{
    if (padInfoPressMode(port, slot)) {
        if (!inPressureMode) {
            padEnterPressMode(port, slot);
            inPressureMode = true;
        } else {
            printf("Already in pressure mode!\n");
        }
    } else {
        printf("No pressure mode available!\n");
    }

    return;
}

void PS2Pad::exitPressureMode(void)
{
    if (padInfoPressMode(port, slot)) {
        if (inPressureMode) {
            padExitPressMode(port, slot);
            inPressureMode = false;
        } else {
            printf("Already out of pressure mode!\n");
        }
    } else {
        printf("No pressure mode available!\n");
    }

    return;
}

int PS2Pad::getPressure(int button)
{
    int pressure = 0;   // hold the status of the given button

    if (inPressureMode) {
        switch (button) {
        case PAD_LEFT:
            pressure = buttons.left_p;
            break;
        case PAD_RIGHT:
            pressure = buttons.right_p;
            break;
        case PAD_DOWN:
            pressure = buttons.down_p;
            break;
        case PAD_UP:
            pressure = buttons.up_p;
            break;
        case PAD_SQUARE:
            pressure = buttons.square_p;
            break;
        case PAD_CROSS:
            pressure = buttons.cross_p;
            break;
        case PAD_CIRCLE:
            pressure = buttons.circle_p;
            break;
        case PAD_TRIANGLE:
            pressure = buttons.triangle_p;
            break;
        case PAD_R1:
            pressure = buttons.r1_p;
            break;
        case PAD_R2:
            pressure = buttons.r2_p;
            break;
        case PAD_L1:
            pressure = buttons.l1_p;
            break;
        case PAD_L2:
            pressure = buttons.l2_p;
            break;
        default:
            printf("Invalid button: %d\n", button);
            break;
        }
    } else {
        printf("Pad doesn't support button pressure!\n");
    }

    return pressure;
}
