//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wtype-limits"
//#pragma GCC diagnostic ignored "-Wunused-variable"


#ifdef __AVR__
  #include <avr/wdt.h>
#endif

//#pragma GCC diagnostic pop

#include "AR488_Config.h"
#include "AR488_Layouts.h"

#ifdef USE_INTERRUPTS
  #ifdef __AVR__
    #include <avr/interrupt.h>
  #endif
#endif
#ifdef E2END
  #include <EEPROM.h>
#endif
/*
#ifdef AR_BT_EN
  #include "AR488_Bluetooth.h"
#endif
*/

/***** FWVER "AR488 GPIB controller, ver. 0.47.56, 03/11/2019" *****/

/*
  Arduino IEEE-488 implementation by John Chajecki

  Inspired by the original work of Emanuele Girlando, licensed under a Creative
  Commons Attribution-NonCommercial-NoDerivatives 4.0 International License.
  Any code in common with the original work is reproduced here with the explicit
  permission of Emanuele Girlando, who has kindly reviewed and tested this code.

  Thanks also to Luke Mester for comparison testing against the Prologix interface.
  AR488 is Licenced under the GNU Public licence.

  Thanks to 'maxwell3e10' on the EEVblog forum for suggesting additional auto mode
  settings and the macro feature.

  Thanks to 'artag' on the EEVblog forum for providing code for the 32u4.
*/

/*
   Implements most of the CONTROLLER functions;
   Substantially compatible with 'standard' Prologix "++" commands
   (see +savecfg command in the manual for differences)

   Principle of operation:
   - Commands received from USB are buffered and whole terminated lines processed
   - Interface commands prefixed with "++" are passed to the command handler
   - Instrument commands and data not prefixed with '++' are sent directly to the GPIB bus.
   - To receive from the instrument, issue a ++read command or put the controller in auto mode (++auto 1)
   - Characters received over the GPIB bus are unbuffered and sent directly to USB
   NOTES:
   - GPIB line in a HIGH state is un-asserted
   - GPIB line in a LOW state is asserted
   - The ATMega processor control pins have a high impedance when set as inputs
   - When set to INPUT_PULLUP, a 10k pull-up (to VCC) resistor is applied to the input
*/

/*
   Standard commands

   ++addr         - display/set device address
   ++auto         - automatically request talk and read response
   ++clr          - send Selected Device Clear to current GPIB address
   ++eoi          - enable/disable assertion of EOI signal
   ++eos          - specify GPIB termination character
   ++eot_enable   - enable/disable appending user specified character to USB output on EOI detection
   ++eot_char     - set character to append to USB output when EOT enabled
   ++ifc          - assert IFC signal for 150 miscoseconds - make AR488 controller in charge
   ++llo          - local lockout - disable front panel operation on instrument
   ++loc          - enable front panel operation on instrument
   ++lon          - put controller in listen-only mode (listen to all traffic)
   ++mode         - set the interface mode (0=controller/1=device)
   ++read         - read data from instrument
   ++read_tmo_ms  - read timeout specified between 1 - 3000 milliseconds
   ++rst          - reset the controller
   ++savecfg      - save configration
   ++spoll        - serial poll the addressed host or all instruments
   ++srq          - return status of srq signal (1-srq asserted/0-srq not asserted)
   ++status       - set the status byte to be returned on being polled (bit 6 = RQS, i.e SRQ asserted)
   ++trg          - send trigger to selected devices (up to 15 addresses)
   ++ver          - display firmware version
*/

/*
    Proprietry commands:

   ++aspoll       - serial poll all instruments (alias: ++spoll all)
   ++default      - set configuration to controller default settings
   ++dcl          - send unaddressed (all) device clear  [power on reset] (is the rst?)
   //++ren          - assert or unassert the REN signal
   ++ppoll        - conduct a parallel poll
   ++setvstr      - set custom version string (to identify controller, e.g. "GPIB-USB"). Max 47 chars, excess truncated.
   ++ton          - put controller in talk-only mode (send data only)
   ++verbose      - verbose (human readable) mode

*/

/*
   NOT YET IMPLEMENTED

   ++help     - show summary of commands
   ++debug    - debug mode (0=off, 1=basic, 2=verbose) [maybe best left in ifdef statements?]
   ++myaddr   - aset the controller address
*/

/*
   For information regarding the GPIB firmware by Emanualle Girlando see:
   http://egirland.blogspot.com/2014/03/arduino-uno-as-usb-to-gpib-controller.html
*/


/*
   Pin mapping between the Arduino pins and the GPIB connector.
   NOTE:
   GPIB pins 10 and 18-24 are connected to GND
   GPIB pin 12 should be connected to the cable shield (might be n/c)
   Pin mapping follows the layout originally used by Emanuelle Girlando, but adds
   the SRQ line (GPIB 10) on pin 2 and the REN line (GPIB 17) on pin 13. The program
   should therefore be compatible with the original interface design but for full
   functionality will need the remaining two pins to be connected.
   For further information about the AR488 see:
*/


/*********************************/
/***** CONFIGURATION SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvv *****/
// SEE >>>>> Config.h <<<<<
/***** ^^^^^^^^^^^^^^^^^^^^^ *****/
/***** CONFIGURATION SECTION *****/
/*********************************/


/***************************************/
/***** MACRO CONFIGURATION SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
// SEE >>>>> Config.h <<<<<
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MACRO CONFIGURATION SECTION *****/
/***************************************/


/*****  USER DEFINED MACROS  *****/

#ifdef USE_MACROS

/***** STARTUP MACRO *****/
const char startup_macro[] PROGMEM = {MACRO_0};

/***** USER MACROS 1 - 9 *****/
const char macro_1 [] PROGMEM = {MACRO_1};
const char macro_2 [] PROGMEM = {MACRO_2};
const char macro_3 [] PROGMEM = {MACRO_3};
const char macro_4 [] PROGMEM = {MACRO_4};
const char macro_5 [] PROGMEM = {MACRO_5};
const char macro_6 [] PROGMEM = {MACRO_6};
const char macro_7 [] PROGMEM = {MACRO_7};
const char macro_8 [] PROGMEM = {MACRO_8};
const char macro_9 [] PROGMEM = {MACRO_9};

/***** END OF USER MACROS *****/
/** DO NOT MODIFY BELOW HERE **/
/******************************/

/*
   Macro pointer array
 *** DO NOT MODIFY ***
*/
const char * const macros[] PROGMEM = {
  startup_macro,
  macro_1,
  macro_2,
  macro_3,
  macro_4,
  macro_5,
  macro_6,
  macro_7,
  macro_8,
  macro_9
};
/*** DO NOT MODIFY ***/
#endif

/***** ^^^^^^^^^^^^^^^^^^^^ *****/
/***** MACRO CONFIG SECTION *****/
/********************************/



/*********************************/
/***** COMMON CONFIG SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvv *****/

/***** Serial Port *****/
#ifdef AR_CDC_SERIAL
  Serial_ *arSerial = &(AR_SERIAL_PORT);
#endif
#ifdef AR_HW_SERIAL
  HardwareSerial *arSerial = &(AR_SERIAL_PORT);
#endif
// Note: SoftwareSerial support conflicts with PCINT support
#ifdef AR_SW_SERIAL
  #include <SoftwareSerial.h>
  SoftwareSerial swSerial(AR_SW_SERIAL_RX, AR_SW_SERIAL_TX);
  SoftwareSerial *arSerial = &swSerial;
#endif


/***** GPIB BUS commands *****/
// Universal Multiline commands (apply to all devices)

#define GC_LLO 0x11
#define GC_DCL 0x14
#define GC_PPU 0x15
#define GC_SPE 0x18
#define GC_SPD 0x19
#define GC_UNL 0x3F
#define GC_TAD 0x40
#define GC_PPE 0x60
#define GC_PPD 0x70
#define GC_UNT 0x5F
// Address commands
#define GC_LAD 0x20
// Addressed commands
#define GC_GTL 0x01
#define GC_SDC 0x04
#define GC_PPC 0x05
#define GC_GET 0x08

/***** GPIB control states *****/
// Controller mode
#define CINI 0x01 // Controller idle state
#define CIDS 0x02 // Controller idle state
#define CCMS 0x03 // Controller command state
#define CTAS 0x04 // Controller talker active state
#define CLAS 0x05 // Controller listner active state
// Listner/device mode
#define DINI 0x06 // Device initialise state
#define DIDS 0x07 // Device idle state
#define DLAS 0x08 // Device listener active (listening/receiving)
#define DTAS 0x09 // Device talker active (sending) state

/****** Process status values *****/
#define OK 0
#define ERR 1

/***** Control characters *****/
#define ESC  0x1B   // the USB escape char
#define CR   0xD    // Carriage return
#define LF   0xA    // Newline/linefeed
#define PLUS 0x2B   // '+' character

/***** PARSE BUFFERS *****/
// Serial input parsing buffer
static const uint8_t PBSIZE = 128;
char pBuf[PBSIZE];
uint8_t pbPtr = 0;


/***** Controller configuration *****/
/*   
 * Default values set for controller mode
 */
struct AR488conf {
  uint8_t ew;       // EEPROM write indicator byte
  bool eot_en;      // Enable/disable append EOT char to string received from GPIB bus before sending to USB
  bool eoi;         // Assert EOI on last data char written to GPIB - 0-disable, 1-enable
  uint8_t cmode;    // Controller mode - 0=unset, 1=device, 2=controller
  uint8_t caddr;    // Controller address
  uint8_t paddr;    // Primary device address
  uint8_t saddr;    // Secondary device address
  uint8_t eos;      // EOS (end of send - to GPIB) character flag [0=CRLF, 1=CR, 2=LF, 3=None]
  uint8_t stat;     // Status byte to return in response to a poll
  uint8_t amode;    // Auto mode setting (0=off; 1=Prologix; 2=onquery; 3=continuous;
  int rtmo;         // Read timout (read_tmo_ms) in milliseconds - 0-3000 - value depends on instrument
  char eot_ch;      // EOT character to append to USB output when EOI signal detected
  char vstr[48];    // Custom version string
  uint16_t tmbus;   // Delay to allow the bus control/data lines to settle (1-30,000 microseconds)
};

struct AR488conf AR488;


/****** Global variables with volatile values related to controller state *****/

// GPIB control state
uint8_t cstate = 0;

// Verbose mode
bool isVerb = false;

// We have a line
//bool crFl = false;          // Carriage return flag
uint8_t lnRdy = 0;      // Line ready to process

// GPIB data receive flags
bool isReading = false; // Is a GPIB read in progress?
bool aRead = false;     // GPIB data read in progress
bool rEoi = false;      // read eoi requested
bool rEbt = false;      // read with specified terminator character
bool isQuery = false;   // Direct instrument command is a query
//uint8_t aMode = 0;      // Auto read mode: 0=off; 1=Prologix; 2=on query (CMD?); 3=continuous;
uint8_t tranBrk = 0;    // transmission break on 1=++, 2=EOI, 3=ATN 4=UNL
uint8_t eByte = 0;      // termination character

// Device mode - send data
bool snd = false;

// Escaped character flag
bool isEsc = false;   // Charcter escaped
bool isPle = false;   // Plus escaped

// Received serial poll request?
//bool isSprq = false;

// Read only mode flag
bool isRO = false;

// Talk only mode flag
bool isTO = false;

// GPIB command parser
bool aTt = false;
bool aTl = false;

// State flags
extern volatile bool isATN;  // has ATN been asserted?
extern volatile bool isSRQ;  // has SRQ been asserted?

// SRQ auto mode
bool isSrqa = false;

volatile bool isBAD = false;

uint8_t runMacro = 0;


/***** ^^^^^^^^^^^^^^^^^^^^^ *****/
/***** COMMON CONFIG SECTION *****/
/*********************************/


/*******************************/
/***** COMMON CODE SECTION *****/
/***** vvvvvvvvvvvvvvvvvvv *****/

/******  Arduino standard SETUP procedure *****/
void setup() {

  // Disable the watchdog (needed to prevent WDT reset loop)
#ifdef __AVR__
  wdt_disable();
#endif

  // Turn off internal LED (set OUPTUT/LOW)
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

#ifdef USE_INTERRUPTS
  // Turn on interrupts on port
  interruptsEn();
#endif

  // Initialise parse buffer
  flushPbuf();

  // Initialise serial comms over USB or Bluetooth
//#ifdef AR_BT_EN
  // Initialise Bluetooth  
//  btInit();
//  arSerial->begin(AR_BT_BAUD);
//#else
  // Start the serial port
  arSerial->begin(AR_SERIAL_BAUD);

/*
  #if defined(__AVR_ATmega32U4__)
    while(!*arSerial)
    ;
//    Serial.print(F("Starting "));
    for(int i = 0; i < 20; ++i) {  // this gives you 10 seconds to start programming before it crashes
      Serial.print(".");
      delay(500);
    }
    Serial.println("@>");
  #endif // __AVR_ATmega32U4__
*/
  
//#endif

  // Initialise
  initAR488();

#ifdef E2END
  // Read data from non-volatile memory
  //(will only read if previous config has already been saved)
  epGetCfg();
#endif

  // Print version string
  //  if (strlen(AR488.vstr)>0) {
  //    arSerial->println(AR488.vstr);
  //  }else{
  //    arSerial->println(FWVER);
  //  }

  // Initialize the interface in device mode
  if (AR488.cmode == 1) initDevice();

  // Initialize the interface in controller mode
  if (AR488.cmode == 2) initController();

  isATN = false;
  isSRQ = false;

#if defined(USE_MACROS) && defined(RUN_STARTUP)
  // Run startup macro
  execMacro(0);
#endif

}


/***** ARDUINO MAIN LOOP *****/
void loop() {

/*** Macros ***/
/*
 * Run the startup macro if enabled
 */
#ifdef USE_MACROS
  // Run user macro if flagged
  if (runMacro > 0) {
    execMacro(runMacro);
    runMacro = 0;
  }
#endif

/*** Pin Hooks ***/
/*
 * Not all boards support interrupts or have PCINTs. In this
 * case, use in-loop checking to detect when SRQ and ATN have 
 * been signalled
 */
#ifdef USE_PINHOOKS
  isATN = (digitalRead(ATN)==LOW ? true : false);
  isSRQ = (digitalRead(SRQ)==LOW ? true : false);
#endif

/*** In loop serial buffer checking ***/
/* SerialEvent() handles the serial interrupt but some boards 
 * do not support SerialEvent. In this case use in-loop checking
 */
//#ifndef USE_SERIALEVENT 
  // Serial input handler
  while (arSerial->available()) {
    lnRdy = parseInput(arSerial->read());
  }
//#endif

/*** Process the buffer ***/
/* Each received char is passed through parser until an un-escaped 
 * CR is encountered. If we have a command then parse and execute.
 * If the line is data (inclding direct instrument commands) then
 * send it to the instrument.
 * NOTE: parseInput() sets lnRdy in serialEvent, readBreak or in the
 * above loop
 * lnRdy=1: process command;
 * lnRdy=2: send data to Gpib
 */

  // lnRdy=1: received a command so execute it...
  if (lnRdy == 1) {
    processLine(pBuf, pbPtr, 1);
  }

  // Controller mode:
  if (AR488.cmode == 2) {
    // lnRdy=2: received data - send it to the instrument...
    if (lnRdy == 2) {
      processLine(pBuf, pbPtr, 2);
      // Auto-read data from GPIB bus following any command
      if (AR488.amode == 1) {
        //        delay(10);
        gpibReceiveData();
      }
      // Auto-receive data from GPIB bus following a query command
      if (AR488.amode == 2 && isQuery) {
        //        delay(10);
        gpibReceiveData();
        isQuery = false;
      }
    }

    // Check status of SRQ and SPOLL if asserted
    if (isSRQ && isSrqa) {
      spoll_h(NULL);
      isSRQ = false;
    }

    // Continuous auto-receive data from GPIB bus
    if (AR488.amode == 3 && aRead) gpibReceiveData();
  }

  // Device mode:
  if (AR488.cmode == 1) {
    if (isTO) {
      if (lnRdy == 2) processLine(pBuf, pbPtr, 2);
    }else if (isRO) {
      lonMode();
    }else{
      if (isATN) attnRequired();
    }
  }

  delayMicroseconds(5);
}
/***** END MAIN LOOP *****/


/***** Initialise the interface *****/
void initAR488() {
  // Set default values ({'\0'} sets version string array to null)
  AR488 = {0xCC, false, false, 2, 0, 1, 0, 0, 0, 0, 1200, 0, {'\0'}, 0};
}


/***** Initialise device mode *****/
void initDevice() {
  // Set GPIB control bus to device idle mode
  setGpibControls(DINI);
/*  
  // Disable SRQ and enable ATN interrupt
  cli();
  //  PCMSK2 &= SRQint; // SRQ interrupt now controlled by ++status command
  PCMSK2 |= ATNint;
  sei();
*/  
  // Initialise GPIB data lines (sets to INPUT_PULLUP)
  readGpibDbus();
}


/***** Initialise controller mode *****/
void initController() {
  // Set GPIB control bus to controller idle mode
  setGpibControls(CINI);  // Controller initialise state
  // Initialise GPIB data lines (sets to INPUT_PULLUP)
  readGpibDbus();
  // Assert IFC to signal controller in charge (CIC)
  ifc_h();
}


/***** Serial event interrupt handler *****/
/*
#ifdef USE_SERIALEVENT
void serialEvent() {
  lnRdy = parseInput(arSerial->read());
}
#endif
*/


/***** Interrupt data transfer when escape pressed *****/
void readBreak() {
  // Check whether EOI is asserted
  if (digitalRead(EOI) == LOW) {
    tranBrk = 5;
    return;
  }

  // Check serial input to see if we need to break on ++ character
  if (arSerial->available()) {   // Only need to parse if a character is available
    lnRdy = parseInput(arSerial->read());
    if (lnRdy == 1) tranBrk = 7;
  }
}


/*************************************/
/***** Device operation routines *****/
/*************************************/


/***** Unrecognized command *****/
void errBadCmd() {
  arSerial->println(F("Unrecognized command"));
}


/***** Read configuration from EEPROM *****/
#ifdef E2END
void epGetCfg() {
  //  int ew = 0x00;
  int epaddr = 0;
  int val;
  val = EEPROM.read(0);
  if (val == 0xCC) {
    EEPROM.get(epaddr, AR488);
  }
}


/*****Save configuraton to EEPROM ****/
/*
 * At present requires a 128 byte 'page'  
 */
//uint8_t epSaveCfg(uint8_t page){
uint8_t epSaveCfg() {

  //  if (page<0 || page>4) return ERR;

#ifdef debug5
  long int sz;
  sz = sizeof(AR488);
  arSerial->print(F("Size of structure: "));
  arSerial->println(sz);
#endif
  int epaddr = 0;
  //  epaddr = 128 * (page-1);
  EEPROM.put(epaddr, AR488);
  if (isVerb) arSerial->print(F("Settings saved."));
  //  if (isVerb) { arSerial->print(F("Settings saved to configuration profile ")); arSerial->println(page); };
  return OK;
}
#endif  // E2END


/***** Add character to the buffer and parse *****/
uint8_t parseInput(char c) {

  uint8_t r = 0;

  // Read until buffer full (buffer-size - 2 characters)
  if (pbPtr < PBSIZE - 2) {
    // Actions on specific characters
    switch (c) {
      // Carriage return or newline? Then process the line
      case CR:
      case LF:
        // Character must not be escaped
        if (isEsc) {
          addPbuf(c);
          isEsc = false;
        } else {
          // Carriage return on blank line?
          if (pbPtr == 0) {
            flushPbuf();
            if (isVerb) {
              arSerial->println();
              arSerial->print("> ");
            }
            return 0;
          } else {
#ifdef DEBUG1
            arSerial->print(F("parseInput: Received ")); arSerial->println(pBuf);
#endif
            // Buffer starts with ++ and contains at least 3 characters - command?
            if (pbPtr > 2 && isCmd(pBuf) && !isPle) {
              //                if (isReading) tranBrk = 7;
              r = 1;
              // Buffer has at least 1 character
            } else if (pbPtr > 0) { // Its other data (or instrument commands from user) so pass characters to GPIB bus
              r = 2;
            }
            isPle = false;
            return r;
          }
        }
        break;
      case ESC:
        // Handle the escape character
        if (isEsc) {
          // Add character to buffer and cancel escape
          addPbuf(c);
          isEsc = false;
        } else {
          // Set escape flag
          isEsc  = true;  // Set escape flag
        }
        break;
      case PLUS:
        if (isEsc) {
          isEsc = false;
          if (pbPtr < 2) isPle = true;
        }
        addPbuf(c);
        if (isVerb) arSerial->print(c);

        // Break on '++'?
        //          if (pbPtr==2 && isCmd(pBuf)) {
        //            if (isReading){
        //              tranBrk = 1;
        //              isAuto = false;
        //            }
        //          }
        break;
      // Something else?
      default: // any char other than defined above
        if (isVerb) arSerial->print(c);  // Humans like to see what they are typing...
        // Buffer contains '++' (start of command). Stop sending data to serial port by halting GPIB receive.
        addPbuf(c);
        isEsc = false;
    }
  } else {
    // Buffer full - cannot be a command so treat as data and pass to GPIB bus
    r = 2;
  }
  return r;
}


/***** Is this a command? *****/
bool isCmd(char *buffr) {
  if (buffr[0] == PLUS && buffr[1] == PLUS) {
#ifdef DEBUG1
    arSerial->println(F("isCmd: Command detected."));
#endif
    return true;
  }
  return false;
}


/***** ++read command detected? *****/
bool isRead(char *buffr) {
  char cmd[4];
  // Copy 2nd to 5th character
  for (int i = 2; i < 6; i++) {
    cmd[i - 2] = buffr[i];
  }
  // Compare with 'read'
  if (strncmp(cmd, "read", 4) == 0) return true;
  return false;
}


/***** Add character to the buffer *****/
void addPbuf(char c) {
  pBuf[pbPtr] = c;
  pbPtr++;
}


/***** Clear the parse buffer *****/
void flushPbuf() {
  memset(pBuf, '\0', PBSIZE);
  pbPtr = 0;
}


/***** Comand function record *****/
struct cmdRec { 
  const char* token; 
  int opmode;
  void (*handler)(char *);
};


/***** Array containing index of accepted ++ commands *****/
/*
 * Commands without parameters require casting to a pointer
 * requiring a char* parameter. The functon is called with
 * NULL by the command processor.
 */
static cmdRec cmdHidx [] = { 
 
  { "addr",        3, addr_h      }, 
  { "allspoll",    2, (void(*)(char*)) aspoll_h  },
  { "auto",        2, amode_h     },
  { "clr",         2, (void(*)(char*)) clr_h     },
  { "dcl",         2, (void(*)(char*)) dcl_h     },
  { "default",     3, (void(*)(char*)) default_h },
  { "eoi",         3, eoi_h       },
  { "eos",         3, eos_h       },
  { "eot_char",    3, eot_char_h  },
  { "eot_enable",  3, eot_en_h    },
  { "ifc",         2, (void(*)(char*)) ifc_h     },
  { "llo",         2, llo_h       },
  { "loc",         2, loc_h       },
  { "lon",         1, lon_h       },
  { "macro",       2, macro_h     },
  { "mode" ,       3, cmode_h     },
  { "ppoll",       2, (void(*)(char*)) ppoll_h   },
  { "read",        2, read_h      },
  { "read_tmo_ms", 2, rtmo_h      },
  { "ren",         2, ren_h       },
  { "repeat",      2, repeat_h    },
  { "rst",         3, (void(*)(char*)) rst_h     },
  { "trg",         2, trg_h       },
  { "savecfg",     3, (void(*)(char*)) save_h    },
  { "setvstr",     3, setvstr_h   },
  { "spoll",       2, spoll_h     },
  { "srq",         2, (void(*)(char*)) srq_h     },
  { "srqauto",     2, srqa_h      },
  { "status",      1, stat_h      },
  { "ton",         1, ton_h       },
  { "ver",         3, ver_h       },
  { "verbose",     3, (void(*)(char*)) verb_h    },
  { "tmbus",       3, tmbus_h     },
  { "xdiag",       3, xdiag_h     }

};


/****** Buffer processor *****/
/* Processes the parse buffer whenever a full CR or LF
 * delimeted line is detected.
 * Executes a ++command or sends characters to instrument
 * mode: 1=command; 2=data;
*/
void processLine(char *buffr, uint8_t dsize, uint8_t mode) {
  char line[PBSIZE];

  // Copy collected chars to line buffer
  memcpy(line, buffr, dsize);

  // Flush the parse buffer
  flushPbuf();
  lnRdy = 0;

#ifdef DEBUG1
  arSerial->print(F("processLine: Received: ")); printHex(line, dsize);
#endif

  // Line contains a valid command after the ++
  if (mode == 1) {
    // Its a ++command so shift everything two bytes left (ignore ++) and parse
    for (int i = 0; i < dsize-2; i++) {
      line[i] = line[i + 2];
    }
    // Replace last two bytes with a null (\0) character
    line[dsize - 2] = '\0';
    line[dsize - 1] = '\0';
#ifdef DEBUG1
    arSerial->print(F("processLine: Sent to the command processor: ")); printHex(line, dsize-2);
#endif
    // Execute the command
    if (isVerb) arSerial->println(); // Shift output to next line
    getCmd(line);
  }

  // This line is not a ++command, so if in controller mode, pass characters to GPIB
  //  if (mode==2 && AR488.cmode==2){
  if (mode == 2) {
#ifdef DEBUG1
    arSerial->print(F("processLine: Sent to the instrument: ")); printHex(line, dsize);
#endif

    // Is this a query command (string ending with ?)
    if (line[dsize-1] == '?') isQuery = true;

    // Send string to instrument
    gpibSendData(line, dsize);
  }

  // Show a prompt on completion?
  if (isVerb) {
    // Print prompt
    arSerial->println();
    arSerial->print("> ");
  }

}


/***** Extract command and pass to handler *****/
void getCmd(char *buffr) {

  char *token;  // pointer to command token
  char params[64]; // array containing command parameters
  int casize = sizeof(cmdHidx) / sizeof(cmdHidx[0]);
  int i = 0, j = 0;

  memset(params, '\0', 64);

#ifdef DEBUG1
  arSerial->print("getCmd: ");
  arSerial->print(buffr); arSerial->print(F(" - length:")); arSerial->println(strlen(buffr));
#endif

  if (*buffr == (NULL || CR || LF) ) return; // empty line: nothing to parse.
  token = strtok(buffr, " \t");

#ifdef DEBUG1
  arSerial->print("getCmd: process token: "); arSerial->println(token);
#endif

  // Look for a valid command token
  i = 0;
  do {
    if (strcasecmp(cmdHidx[i].token, token) == 0) break;
    i++;
  } while (i < casize);

  if (i < casize) {
    // We have found a valid command and handler
#ifdef DEBUG1
    arSerial->print("getCmd: ");
    arSerial->print("found handler for: "); arSerial->println(plusCmdIdx[i].token);
#endif
    // If command is relevant to controller mode then execute it
    if (cmdHidx[i].opmode & AR488.cmode) {
      // If its a command with parameters
      // Copy command parameters to params and call handler with parameters
      do {
        j++;
        token = strtok(NULL, " \t");
        if (strlen(token) > 0) {
          if (j > 1) {
            strcat(params, " ");
          };
          strcat(params, token);
        }
      } while (token != NULL);
      // If command parameters were specified
      if (strlen(params) > 0) {
#ifdef DEBUG1
        arSerial->print(F("Calling handler with parameters: ")); arSerial->println(params);
#endif
        // Call handler with parameters specified
        cmdHidx[i].handler(params);
      }else{
        // Call handler without parameters
        cmdHidx[i].handler(NULL);
      }   
    }else{
      errBadCmd();
      if (isVerb) arSerial->println(F("Command not available in this mode."));
    }

  } else {
    // No valid command found
    errBadCmd();
  }
 
}


/***** Prints charaters as hex bytes *****/
void printHex(char *buffr, int dsize) {
  for (int i = 0; i < dsize; i++) {
    arSerial->print(buffr[i], HEX); arSerial->print(" ");
  }
  arSerial->println();
}


/***** Check whether a parameter is in range *****/
/* Convert string to integer and check whether value is within
 * lowl to higl inclusive. Also returns converted text in param
 * to a uint16_t integer in rval. Returns true if successful, 
 * false if not
*/
bool notInRange(char *param, uint16_t lowl, uint16_t higl, uint16_t &rval) {

  // Null string passed?
  if (strlen(param) == 0) return true;

  // Convert to integer
  rval = 0;
  rval = atoi(param);

  // Check range
  if (rval < lowl || rval > higl) {
    errBadCmd();
    if (isVerb) {
      arSerial->print(F("Invalid: range is between "));
      arSerial->print(lowl);
      arSerial->print(F(" and "));
      arSerial->println(higl);
    }
    return true;
  }
  return false;
}


/***** If enabled, executes a macro *****/
#ifdef USE_MACROS
void execMacro(uint8_t idx) {
  char c;
  const char * macro = pgm_read_word(macros + idx);
  int ssize = strlen_P(macro);
  //    arSerial->print("Idx:  ");arSerial->println(idx);
  //    arSerial->print("Size: ");arSerial->println(ssize);
  // Read characters from macro character array
  for (int i = 0; i < ssize; i++) {
    c = pgm_read_byte_near(macro + i);
    if (c == CR || c == LF || i == (ssize - 1)) {
      // Reached last character before NL. Add to buffer before processing
      if (i == ssize-1) {
        // Check buffer and add character
        if (pbPtr < (PBSIZE - 2)){
          addPbuf(c);
        }else{
          // Buffer full - clear and exit
          flushPbuf();
          return;
        }
      }
//      arSerial->println(pBuf);
      if (isCmd(pBuf)){
        processLine(pBuf, strlen(pBuf), 1);
      }else{
        processLine(pBuf, strlen(pBuf), 2);
      }
      // Done - clear the buffer
      flushPbuf();
    } else {
      // Check buffer and add character
      if (pbPtr < (PBSIZE - 2)) {
        addPbuf(c);
      } else {
        // Exceeds buffer size - clear buffer and exit
        i = ssize;
        return;
      }
    }
  }

  // Clear the buffer ready for serial input
  flushPbuf();
}
#endif


/*************************************/
/***** STANDARD COMMAND HANDLERS *****/
/*************************************/

/***** Show or change device address *****/
void addr_h(char *params) {
  //  char *param, *stat;
  char *param;
  uint16_t val;
  if (params != NULL) {
    // Primary address
    param = strtok(params, " \t");
    if (notInRange(param, 1, 30, val)) return;
    //    if (strlen(param) > 0) {
    //      val = atoi(param);
    //      if (val < 1 || val > 30) {
    //        errBadCmd();
    //        if (isVerb) arSerial->println(F("Invalid: GPIB primary address is in the range 1 - 30")); return;
    //      }
    if (val == AR488.caddr) {
      errBadCmd();
      if (isVerb) arSerial->println(F("That is my address! Address of a remote device is required."));
      return;
    }
    AR488.paddr = val;
    if (isVerb) {
      arSerial->print(F("Set device primary address to: "));
      arSerial->println(val);
    }
    //  }
    // Secondary address
    AR488.saddr = 0;
    val = 0;
    param = strtok(NULL, " \t");
    if (param != NULL) {
      if (notInRange(param, 96, 126, val)) return;
      //    val = atoi(param);
      //    if (val > 0) {
      //      if (val < 96 || val > 126) {
      //        errBadCmd();
      //        if (isVerb) arSerial->println(F("Invalid: GPIB secondary address is in the range 96 - 126"));
      //        return;
      //      }
      AR488.saddr = val;
      if (isVerb) {
        arSerial->print("Set device secondary address to: ");
        arSerial->println(val);
      }
    }
  } else {
    arSerial->print(AR488.paddr);
    if (AR488.saddr > 0) {
      arSerial->print(F(" "));
      arSerial->print(AR488.saddr);
    }
    arSerial->println();
  }
}


/***** Show or set read timout *****/
void rtmo_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 1 || val > 32000) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: valid timout range is 1 - 32000 ms."));
      return;
    }
    AR488.rtmo = val;
    if (isVerb) {
      arSerial->print(F("Set [read_tmo_ms] to: "));
      arSerial->print(val);
      arSerial->println(F(" milliseconds"));
    }
  } else {
    arSerial->println(AR488.rtmo);
  }
}


/***** Show or set end of send character *****/
void eos_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 3) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected EOS value of 0 - 3"));
      return;
    }
    AR488.eos = val; if (isVerb) {
      arSerial->print(F("Set EOS to: "));
      arSerial->println(val);
    };
  } else {
    arSerial->println(AR488.eos);
  }
}


/***** Show or set EOI assertion on/off *****/
void eoi_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected EOI value of 0 or 1"));
      return;
    }
    AR488.eoi = val ? true : false; if (isVerb) {
      arSerial->print(F("Set EOI assertion: "));
      arSerial->println(val ? "ON" : "OFF");
    };
  } else {
    arSerial->println(AR488.eoi);
  }
}


/***** Show or set interface to controller/device mode *****/
void cmode_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: mode must be 0 or 1"));
      return;
    }
    switch (val) {
      case 0:
        AR488.cmode = 1;
        initDevice();
        break;
      case 1:
        AR488.cmode = 2;
        initController();
        break;
    }
    if (isVerb) {
      arSerial->print(F("Interface mode set to: "));
      arSerial->println(val ? "CONTROLLER" : "DEVICE");
    }
  } else {
    arSerial->println(AR488.cmode - 1);
  }
}


/***** Show or enable/disable sending of end of transmission character *****/
void eot_en_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected EOT value of 0 or 1"));
      return;
    }
    AR488.eot_en = val ? true : false;
    if (isVerb) {
      arSerial->print(F("Set append of EOT character: "));
      arSerial->println(val ? "ON" : "OFF");
    }
  } else {
    arSerial->println(AR488.eot_en);
  }
}


/***** Show or set end of transmission character *****/
void eot_char_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 255) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected EOT character ASCII value in the range 0 - 255"));
      return;
    }
    AR488.eot_ch = val;
    if (isVerb) {
      arSerial->print(F("EOT set to ASCII character: "));
      arSerial->println(val);
    };
  } else {
    arSerial->println(AR488.eot_ch, DEC);
  }
}


/***** Show or enable/disable auto mode *****/
void amode_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 3) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Automode: valid range is [0-disable|1-Prologix|2-on-query|3-continuous]."));
      return;
    }
    if (val > 0 && isVerb) {
      arSerial->println(F("WARNING: automode ON can cause some devices to generate"));
      arSerial->println(F("         'addressed to talk but nothing to say' errors"));
    }
    //    isAuto = val ? true : false; if (isVerb) {arSerial->print(F("Auto mode: ")); arSerial->println(val ? "ON" : "OFF") ;}
    AR488.amode = val;
    if (AR488.amode < 3) aRead = false;
    if (isVerb) {
      arSerial->print(F("Auto mode: "));
      arSerial->println(AR488.amode);
    }
  } else {
    arSerial->println(AR488.amode);
  }
}


/***** Display the controller version string *****/
void ver_h(char *params) {
  // If "real" requested
  if (params != NULL && strncmp(params, "real", 3) == 0) {
    arSerial->println(F(FWVER));
    // Otherwise depends on whether we have a custom string set
  } else {
    if (strlen(AR488.vstr) > 0) {
      arSerial->println(AR488.vstr);
    } else {
      arSerial->println(F(FWVER));
    }
  }
}


/***** Address device to talk and read the sent data *****/
void read_h(char *params) {
  // Clear read flags
  rEoi = false;
  rEbt = false;
  // Read any parameters
  if (params != NULL) {
    if (strlen(params) > 3) {
      if (isVerb) arSerial->println(F("Invalid termination character - ignored.")); void addr_h(char *params);

    } else if (strncmp(params, "eoi", 3) == 0) { // Read with eoi detection
      rEoi = true;
    } else { // Assume ASCII character given and convert to an 8 bit byte
      rEbt = true;
      eByte = atoi(params);
    }
  }
  if (AR488.amode == 3) {
    // in auto continumous mode we set this flag to indicate we are ready for continuous read
    aRead = true;
  } else {
    // If auto mode is disabled we do a single read
    gpibReceiveData();
  }
}


/***** Send device clear (usually resets the device to power on state) *****/
void clr_h() {
  if (addrDev(AR488.paddr, 0)) {
    if (isVerb) arSerial->println(F("Failed to address device"));
    return;
  }
  if (gpibSendCmd(GC_SDC))  {
    if (isVerb) arSerial->println(F("Failed to send SDC"));
    return;
  }
  if (uaddrDev()) {
    if (isVerb) arSerial->println(F("Failed to untalk GPIB bus"));
    return;
  }
  // Set GPIB controls back to idle state
  setGpibControls(CIDS);
}


/***** Send local lockout command *****/
void llo_h(char *params) {
  // NOTE: REN *MUST* be asserted (LOW)
  bool ren = digitalRead(REN);
  if (!ren) {
    // For 'all' send LLO to the bus without addressing any device - device will show REM when addressed
    if (params != NULL) {
      if (0 == strncmp(params, "all", 3)) {
        if (gpibSendCmd(GC_LLO)) {
          if (isVerb) arSerial->println(F("Failed to send universal LLO."));
        }
      }
    } else {
      // Address device
      if (addrDev(AR488.paddr, 0)) {
        if (isVerb) arSerial->println(F("Failed to address the device."));
        return;
      }
      // Send LLO to currently addressed device
      if (gpibSendCmd(GC_LLO)) {
        if (isVerb) arSerial->println(F("Failed to send LLO to device"));
        return;
      }
      // Unlisten bus
      if (uaddrDev()) {
        if (isVerb) arSerial->println(F("Failed to unlisten the GPIB bus"));
        return;
      }
    }
  }
  // Set GPIB controls back to idle state
  setGpibControls(CIDS);
}


/***** Send Go To Local (GTL) command *****/
void loc_h(char *params) {
  // REN *MUST* be asserted (LOW)
  bool ren = digitalRead(REN);
  if (!ren) {
    if (params != NULL) {
      if (strncmp(params, "all", 3) == 0) {
        // Unassert REN
        setGpibState(0b00100000, 0b00100000, 0);
        //        setGpibState(0b00100000, 0b00100000, 0b00100000);
        delay(40);
        // Simultaneously assert ATN and REN
        //        setGpibState(0b10100000, 0b10100000, 1);
        setGpibState(0b00000000, 0b10100000, 0);
        //        setGpibState(0b10100000, 0b00000000, 0b10100000);
        delay(40);
        // Unassert ATN
        setGpibState(0b10000000, 0b10000000, 0);
        //        setGpibState(0b10000000, 0b10000000, 0b10000000, 0);
        //        setGpibState(0b10000000, 0b10000000, 0b10000000);
        // Return REN to previous state
        digitalWrite(REN, ren);
      }
    } else {
      // De-assert REN
      setGpibState(0b00100000, 0b00100000, 0);
      //      setGpibState(0b00100000, 0b00100000, 0b00100000);
      // Address device to listen
      if (addrDev(AR488.paddr, 0)) {
        if (isVerb) arSerial->println(F("Failed to address device."));
        return;
      }
      // Send GTL
      if (gpibSendCmd(GC_GTL)) {
        if (isVerb) arSerial->println(F("Failed sending LOC."));
        return;
      }
      // Unlisten bus
      if (uaddrDev()) {
        if (isVerb) arSerial->println(F("Failed to unlisten GPIB bus."));
        return;
      }
      // Re-assert REN
      setGpibState(0b00000000, 0b00100000, 0);
      //      setGpibState(0b00100000, 0b00000000, 0b00100000);

      // Set GPIB controls back to idle state
      setGpibControls(CIDS);
    }
  }
}


/***** Assert IFC for 150 microseconds *****/
/* This indicates that the AR488 the Controller-in-Charge on
 * the bus and causes all interfaces to return to their idle 
 * state
 */
void ifc_h() {
  if (AR488.cmode) {
    // Assert IFC
    setGpibState(0b00000000, 0b00000001, 0);
    //    setGpibState(0b00000001, 0b00000000, 0b00000001);
    delayMicroseconds(150);
    // De-assert IFC
    setGpibState(0b00000001, 0b00000001, 0);
    //    setGpibState(0b00000001, 0b00000001, 0b00000001);
    if (isVerb) arSerial->println(F("IFC signal asserted for 150 microseconds"));
  }
}


/***** Send a trigger command *****/
void trg_h(char *params) {
  char *param;
  uint8_t addrs[15];
  uint8_t val = 0;
  uint8_t cnt = 0;

  // Initialise address array
  for (int i = 0; i < 15; i++) {
    addrs[i] = 0;
  }

  // Read parameters
  if (params == NULL) {
    // No parameters - trigger addressed device only
    addrs[0] = AR488.paddr;
    cnt++;
  } else {
    // Read address parameters into array
    while (cnt < 15) {
      if (cnt == 0) {
        param = strtok(params, " \t");
      } else {
        param = strtok(NULL, " \t");
      }
      val = atoi(param);
      if (!val) break;
      if (val < 1 || val > 30) {
        errBadCmd();
        if (isVerb) arSerial->println(F("Invalid: GPIB primary address is in the range 1 - 30"));
        return;
      }
      addrs[cnt] = val;
      cnt++;
    }
  }

  // If we have some addresses to trigger....
  if (cnt > 0) {
    for (int i = 0; i < cnt; i++) {
      // Address the device
      if (addrDev(addrs[i], 0)) {
        if (isVerb) arSerial->println(F("Failed to address device"));
        return;
      }
      // Send GTL
      if (gpibSendCmd(GC_GET))  {
        if (isVerb) arSerial->println(F("Failed to trigger device"));
        return;
      }
      // Unaddress device
      if (uaddrDev()) {
        if (isVerb) arSerial->println(F("Failed to unlisten GPIB bus"));
        return;
      }
    }

    // Set GPIB controls back to idle state
    setGpibControls(CIDS);

    if (isVerb) arSerial->println(F("Group trigger completed."));
  }
}


/***** Reset the controller *****/
/*
 * Arduinos can use the watchdot timer to reset the MCU
 * For other devices, we restart the program instead by
 * jumping to address 0x0000. This is not a hardware reset
 * and will not reset a crashed MCU, but it will re-start
 * the interface program and re-initialise all parameters. 
 */
void(* resetProg) (void) = 0;//declare reset function at address 0

void rst_h() {
#ifdef WDTO_1S
//#ifdef NOTUSED
  // Where defined, reset controller using watchdog timeout
  unsigned long tout;
  tout = millis() + 2000;
  wdt_enable(WDTO_1S);
  while (millis() < tout) {};
  // Should never reach here....
  if (isVerb) {
    arSerial->println(F("Reset FAILED."));
  };
#else
  // Otherwise restart program
  resetProg();
#endif
}


/***** Serial Poll Handler *****/
void spoll_h(char *params) {
  char *param;
  uint8_t addrs[15];
  uint8_t sb = 0;
  uint8_t r;
  //  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t val = 0;
  bool all = false;

  // Initialise address array
  for (int i = 0; i < 15; i++) {
    addrs[i] = 0;
  }

  // Read parameters
  if (params == NULL) {
    // No parameters - trigger addressed device only
    addrs[0] = AR488.paddr;
    j = 1;
  } else {
    // Read address parameters into array
    while (j < 15) {
      if (j == 0) {
        param = strtok(params, " \t");
      } else {
        param = strtok(NULL, " \t");
      }
      // The 'all' parameter given?
      if (strncmp(param, "all", 3) == 0) {
        all = true;
        j = 30;
        if (isVerb) arSerial->println(F("Serial poll of all devices requested..."));
        break;
        // Read all address parameters
      } else if (strlen(params) < 3) { // No more than 2 characters
        val = atoi(param);
        if (!val) break;
        if (val < 1 || val > 30) {
          errBadCmd();
          if (isVerb) arSerial->println(F("Invalid: GPIB addresses are in the range 1-30"));
          return;
        }
        addrs[j] = val;
        j++;
      } else {
        errBadCmd();
        if (isVerb) arSerial->println(F("Invalid parameter"));
        return;
      }
    }
  }

  // Send Unlisten [UNL] to all devices
  if ( gpibSendCmd(GC_UNL) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send UNL"));
#endif
    return;
  }

  // Controller addresses itself as listner
  if ( gpibSendCmd(GC_LAD + AR488.caddr) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send LAD"));
#endif
    return;
  }

  // Send Serial Poll Enable [SPE] to all devices
  if ( gpibSendCmd(GC_SPE) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send SPE"));
#endif
    return;
  }

  // Poll GPIB address or addresses as set by i and j
  for (int i = 0; i < j; i++) {

    // Set GPIB address in val
    if (all) {
      val = i;
    } else {
      val = addrs[i];
    }

    // Don't need to poll own address
    if (val != AR488.caddr) {

      // Address a device to talk
      if ( gpibSendCmd(GC_TAD + val) )  {

#ifdef DEBUG4
        arSerial->println(F("spoll_h: failed to send TAD"));
#endif
        return;
      }

      // Set GPIB control to controller active listner state (ATN unasserted)
      setGpibControls(CLAS);

      // Read the response byte (usually device status) using handshake
      r = gpibReadByte(&sb);

      // If we successfully read a byte
      if (!r) {
        if (j > 1) {
          // If all, return specially formatted response: SRQ:addr,status
          // but only when RQS bit set
          if (sb & 0x40) {
            arSerial->print(F("SRQ:")); arSerial->print(i); arSerial->print(F(",")); arSerial->println(sb, DEC);
            i = j;
          }
        } else {
          // Return decimal number representing status byte
          arSerial->println(sb, DEC);
          if (isVerb) {
            arSerial->print(F("Received status byte ["));
            arSerial->print(sb);
            arSerial->print(F("] from device at address: "));
            arSerial->println(val);
          }
          i = j;
        }
      } else {
        if (isVerb) arSerial->println(F("Failed to retrieve status byte"));
      }
    }
  }
  if (all) arSerial->println();

  // Send Serial Poll Disable [SPD] to all devices
  if ( gpibSendCmd(GC_SPD) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send SPD"));
#endif
    return;
  }

  // Send Untalk [UNT] to all devices
  if ( gpibSendCmd(GC_UNT) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send UNT"));
#endif
    return;
  }

  // Unadress listners [UNL] to all devices
  if ( gpibSendCmd(GC_UNL) )  {
#ifdef DEBUG4
    arSerial->println(F("spoll_h: failed to send UNL"));
#endif
    return;
  }

  // Set GPIB control to controller idle state
  setGpibControls(CIDS);

  // Set SRQ to status of SRQ line. Should now be unasserted but, if it is
  // still asserted, then another device may be requesting service so another
  // serial poll will be called from the main loop
  if (digitalRead(SRQ) == LOW) {
    isSRQ = true;
  } else {
    isSRQ = false;
  }
  if (isVerb) arSerial->println(F("Serial poll completed."));

}


/***** Return status of SRQ line *****/
void srq_h() {
  //NOTE: LOW=asserted, HIGH=unasserted
  arSerial->println(!digitalRead(SRQ));
}


/***** Set the status byte (device mode) *****/
void stat_h(char *params) {
  long int val = 0;
  // A parameter given?
  if (params != NULL) {
    // Byte value given?
    val = atoi(params);
    if (val < 0 || val > 256) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected byte value in the range 0 - 255"));
      return;
    }
    AR488.stat = val;
    if (val & 0x40) {
      setSrqSig();
      if (isVerb) arSerial->println(F("SRQ asserted."));
    } else {
      clrSrqSig();
      if (isVerb) arSerial->println(F("SRQ un-asserted."));
    }
  } else {
    // Return the currently set status byte
    arSerial->println(AR488.stat);
  }
}


/***** Save controller configuration *****/
void save_h() {
#ifdef E2END
  epSaveCfg();
#else
  arSerial->println(F("EEPROM not supported."));
#endif

  //  epSaveCfg(1);


  /*
    sz = sizeof(AR488);
    #ifdef DEBUG6
    arSerial->print(F("Size of structure: "));
    arSerial->println(sz);
    #endif
    EEPROM.put(epaddr, AR488);
    if (isVerb) arSerial->println(F("Settings saved."));
  */

}


/***** Show state or enable/disable listen only mode *****/
void lon_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("LON: valid range is [0-disable|1-enable]."));
      return;
    }
    isRO = val ? true : false;
    isTO = !isRO; // Talk-only mode must be disabled!
    if (isVerb) {
      arSerial->print(F("LON: "));
      arSerial->println(val ? "ON" : "OFF") ;
    }
  } else {
    arSerial->println(isRO);
  }
}


/***** Talk only mode *****/
void ton_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("TON: valid range is [0-disable|1-enable]."));
      return;
    }
    isTO = val ? true : false;
    isRO = !isTO; // Read-only mode must be disabled!
    if (isVerb) {
      arSerial->print(F("TON: "));
      arSerial->println(val ? "ON" : "OFF") ;
    }
  } else {
    arSerial->println(isTO);
  }
}


/***** Set the SRQ signal *****/
void setSrqSig() {
  // Set SRQ line to OUTPUT HIGH (asserted)
  setGpibState(0b01000000, 0b01000000, 1);
  setGpibState(0b00000000, 0b01000000, 0);
}


/***** Clear the SRQ signal *****/
void clrSrqSig() {
  // Set SRQ line to INPUT_PULLUP (un-asserted)
  setGpibState(0b00000000, 0b01000000, 1);
  setGpibState(0b01000000, 0b01000000, 0);
}



/***********************************/
/***** CUSTOM COMMAND HANDLERS *****/
/***********************************/

/***** All serial poll *****/
/*
 * Polls all devices, not just the currently addressed instrument
 * This is an alias wrapper for ++spoll all
 */
void aspoll_h() {
  //  char all[4];
  //  strcpy(all, "all\0");
  spoll_h((char*)"all");
}


/***** Send Universal Device Clear *****/
/*
 * The universal Device Clear (DCL) is unaddressed and affects all devices on the Gpib bus.
 */
void dcl_h() {
  if ( gpibSendCmd(GC_DCL) )  {
    if (isVerb) arSerial->println(F("Sending DCL failed"));
    return;
  }
  // Set GPIB controls back to idle state
  setGpibControls(CIDS);
}


/***** Re-load default configuration *****/
void default_h() {
  initAR488();
}


/***** Parallel Poll Handler *****/
/*
 * Device must be set to respond on DIO line 1 - 8
 */
void ppoll_h() {
  uint8_t sb = 0;

  // Poll devices
  // Start in controller idle state
  setGpibControls(CIDS);
  delayMicroseconds(20);
  // Assert ATN and EOI
  setGpibState(0b00000000, 0b10010000, 0);
  //  setGpibState(0b10010000, 0b00000000, 0b10010000);
  delayMicroseconds(20);
  // Read data byte from GPIB bus without handshake
  sb = readGpibDbus();
  // Return to controller idle state (ATN and EOI unasserted)
  setGpibControls(CIDS);

  // Output the response byte
  arSerial->println(sb, DEC);

  if (isVerb) arSerial->println(F("Parallel poll completed."));
}


/***** Assert or de-assert REN 0=de-assert; 1=assert *****/
void ren_h(char *params) {
  // char *stat;
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      if (isVerb) arSerial->println(F("Invalid: expected 0=de-assert, 1=assert"));
      return;
    }
    digitalWrite(REN, !val); if (isVerb) {
      arSerial->print(F("REN: "));
      arSerial->println(val ? "REN asserted" : "REN un-asserted") ;
    };
  } else {
    arSerial->println(digitalRead(REN) ? 0 : 1);
  }
}


/***** Enable verbose mode 0=OFF; 1=ON *****/
void verb_h() {
  isVerb = !isVerb;
  arSerial->print("Verbose: ");
  arSerial->println(isVerb ? "ON" : "OFF");
}


/***** Set version string *****/
/* Replace the standard AR488 version string with something else
 *  NOTE: some instrument software requires a sepcific version string to ID the interface
 */
void setvstr_h(char *params) {
  int len;
  if (params != NULL) {
    len = strlen(params);
    if (len>47) len=47; // Ignore anything over 47 characters
    memset(AR488.vstr, '\0', 48);
    strncpy(AR488.vstr, params, len);
    if (isVerb) {
      arSerial->print(F("Changed version string to: "));
      arSerial->println(params);
    };
  }
}


/***** SRQ auto - show or enable/disable automatic spoll on SRQ *****/
/*
 * In device mode, when the SRQ interrupt is triggered and SRQ
 * auto is set to 1, a serial poll is conducted automatically
 * and the status byte for the instrument requiring service is
 * automatically returned. When srqauto is set to 0 (default)
 * an ++spoll command needs to be given manually to return
 * the status byte.
 */
void srqa_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 1) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected 0=disable or 1=enable"));
      return;
    }
    switch (val) {
      case 0:
        isSrqa = false;
        break;
      case 1:
        isSrqa = true;
        break;
    }
    if (isVerb) arSerial->println(isSrqa ? "SRQ auto ON" : "SRQ auto OFF") ;
  } else {
    arSerial->println(isSrqa);
  }
}


/***** Repeat a given command and return result *****/
void repeat_h(char *params) {

  uint16_t count;
  uint16_t tmdly;
  char *param;

  if (params != NULL) {
    // Count (number of repetitions)
    param = strtok(params, " \t");
    if (strlen(param) > 0) {
      if (notInRange(param, 2, 255, count)) return;
    }
    // Time delay (milliseconds)
    param = strtok(NULL, " \t");
    if (strlen(param) > 0) {
      if (notInRange(param, 0, 30000, tmdly)) return;
    }

    // Pointer to remainder of parameters string
    param = strtok(NULL, "\n\r");
    if (strlen(param) > 0) {
      for (uint16_t i = 0; i < count; i++) {
        // Send string to instrument
        gpibSendData(param, strlen(param));
        delay(tmdly);
        gpibReceiveData();
      }
    } else {
      errBadCmd();
      if (isVerb) arSerial->println(F("Missing parameter"));
      return;
    }
  } else {
    errBadCmd();
    if (isVerb) arSerial->println(F("Missing parameters"));
  }

}


/***** Run a macro *****/
void macro_h(char *params) {
#ifdef USE_MACROS
  uint16_t val;
  const char * macro;

  if (params != NULL) {
    if (notInRange(params, 0, 9, val)) return;
    //    execMacro((uint8_t)val);
    runMacro = (uint8_t)val;
  } else {
    for (int i = 0; i < 10; i++) {
      macro = (pgm_read_word(macros + i));
      //      arSerial->print(i);arSerial->print(F(": "));
      if (strlen_P(macro) > 0) {
        arSerial->print(i);
        arSerial->print(" ");
      }
    }
    arSerial->println();
  }
#else
  memset(params, '\0', 5);
  arSerial->println(F("Disabled"));
#endif
}


/***** Bus diagnostics *****/
/*
 * Usage: xdiag mode byte
 * mode: 0=data bus; 1=control bus
 * byte: byte to write on the bus
 * Note: values to switch individual bits = 1,2,4,8,10,20,40,80
 */
void xdiag_h(char *params){
  char *param;
  uint8_t mode = 0;
  uint8_t val = 0;
  
  // Get first parameter (mode = 0 or 1)
  param = strtok(params, " \t");
  if (param != NULL) {
    if (strlen(param)<4){
      mode = atoi(param);
      if (mode>2) {
        arSerial->println(F("Invalid: 0=data bus; 1=control bus"));
        return;
      }
    }
  }
  // Get second parameter (8 bit byte)
  param = strtok(NULL, " \t");
  if (param != NULL) {
    if (strlen(param)<4){
      val = atoi(param);
    }

    if (mode) {
      setGpibState(0xFF, 0xFF, 1);
      setGpibState(val, 0xFF, 0);
      delay(10000);
      if (AR488.cmode==2) {
        setGpibControls(CINI);
      }else{
        setGpibControls(DINI);
      }
    }else{
      setGpibDbus(val);
      delay(10000);
      setGpibDbus(0);
    }
  }

}


/****** Timing parameters ******/

void tmbus_h(char *params) {
  int val;
  if (params != NULL) {
    val = atoi(params);
    if (val < 0 || val > 30000) {
      errBadCmd();
      if (isVerb) arSerial->println(F("Invalid: expected EOT character ASCII value in the range 0 - 30,000"));
      return;
    }
    AR488.tmbus = val;
    if (isVerb) {
      arSerial->print(F("TMbus set to: "));
      arSerial->println(val);
    };
  } else {
    arSerial->println(AR488.tmbus, DEC);
  }
}


/******************************************************/
/***** Device mode GPIB command handling routines *****/
/******************************************************/

/***** Attention handling routine *****/
/*
 * In device mode is invoked whenever ATN is asserted
 */
void attnRequired() {

  uint8_t db = 0;
  uint8_t stat = 0;
  bool mla = false;
  bool mta = false;
  bool spe = false;
  bool spd = false;
  
  // Set device listner active state (assert NDAC+NRFD (low), DAV=INPUT_PULLUP)
  setGpibControls(DLAS);

#ifdef DEBUG5
  arSerial->println(F("Answering attention!"));
#endif

  // Read bytes
//  while (isATN) {
  while (digitalRead(ATN)==LOW) {
    stat = gpibReadByte(&db);
    if (!stat) {

#ifdef DEBUG5
      arSerial->println(db, HEX);
#endif

      // Device is addressed to listen
      if (AR488.paddr == (db ^ 0x20)) { // MLA = db^0x20
#ifdef DEBUG5
        arSerial->println(F("attnRequired: Controller wants me to data accept data <<<"));
#endif
        mla = true;
      }

      // Device is addressed to talk
      if (AR488.paddr == (db ^ 0x40)) { // MLA = db^0x40

/*
        if (isRO) {
#ifdef DEBUG5
          arSerial->println(F("attnRequired: cannot respond in listen-only [++lon 1] mode"));
          // Clear the buffer to prevent it getting blocked
          if (lnRdy==2) flushPbuf();
#endif
        }else{
*/ 

          // Call talk handler to send data
          mta = true;
#ifdef DEBUG5
          if (!spe) {
            arSerial->println(F("attnRequired: Controller wants me to send data >>>"));
          }
#endif

/*
        }
*/        

      }
      
/*
      // Some other device being addressed to talk (LON mode -> isRO = true)
      if (db > 0x3F && db < 0x5F && isRO) {
        // Can't talk - no point listening to self!
        // Listen only to other devices on the bus
        if (AR488.paddr != (db ^ 0x40)) {
          mla = true; // Not actually MLA, but need to listen as if it were
        }
      }
*/

      // Serial poll enable request
      if (db==GC_SPE) spe = true;

      // Serial poll disable request
      if (db==GC_SPD) spd = true;
 
      // Unlisten
      if (db==GC_UNL) unl_h();

      // Untalk
      if (db==GC_UNT) unt_h();

    }
  
  }

#ifdef DEBUG5
  arSerial->println(F("End ATN loop."));
#endif

//setGpibControls(DIDS);


  if (mla) { 
#ifdef DEBUG5
    arSerial->println(F("Listening..."));
#endif
    // Call listen handler (receive data)
    mla_h();
    mla = false;
  }

  // Addressed to listen?
  if (mta) {
    // Serial poll enabled
    if (spe) {
#ifdef DEBUG5
      arSerial->println(F("attnRequired: Received serial poll enable."));
#endif
      spe_h();
      spe = false;
    // Otherwise just send data
    }else{
//      Wait_on_pin_state(HIGH, ATN, AR488.rtmo);
      mta_h();
      mta = false;
    }
  }

  // Serial poll disable received
  if (spd) {
#ifdef DEBUG5
    arSerial->println(F("attnRequired: Received serial poll disable."));
#endif
    spd_h();
    mta = false;
    spe = false;
    spd = false;
  }

  // Finished attention - set controls to idle
  setGpibControls(DIDS);

#ifdef DEBUG5
  arSerial->println(F("END attnReceived."));
#endif

}


/***** Device is addressed to listen - so listen *****/
void mla_h(){
  gpibReceiveData();
}


/***** Device is addressed to talk - so send data *****/
void mta_h(){
  if (lnRdy == 2) processLine(pBuf, pbPtr, 2);
}


/***** Selected Device Clear *****/
void sdc_h() {
  // If being addressed then reset
  if (isVerb) arSerial->println(F("Resetting..."));
#ifdef DEBUG5
  arSerial->print(F("Reset adressed to me: ")); arSerial->println(aTl);
#endif
  if (aTl) rst_h();
  if (isVerb) arSerial->println(F("Reset failed."));
}


/***** Serial Poll Disable *****/
void spd_h() {
  if (isVerb) arSerial->println(F("<- serial poll request ended."));
}


/***** Serial Poll Enable *****/
void spe_h() {
  if (isVerb) arSerial->println(F("Serial poll request received from controller ->"));
  gpibSendStatus();
  if (isVerb) arSerial->println(F("Status sent."));
  // Clear the SRQ bit
  AR488.stat = AR488.stat & ~0x40;
  // Clear the SRQ signal
  clrSrqSig();
  if (isVerb) arSerial->println(F("SRQ bit cleared (if set)."));
}


/***** Unlisten *****/
void unl_h() {
  // Stop receiving and go to idle
#ifdef DEBUG5
  arSerial->println(F("Unlisten received."));
#endif
  rEoi = false;
  tranBrk = 3;  // Stop receving transmission
}


/***** Untalk *****/
void unt_h() {
  // Stop sending data and go to idle
#ifdef DEBUG5
  arSerial->println(F("Untalk received."));
#endif
//  tranBrk = 3;  // Stop sending transmission
}


void lonMode(){

  gpibReceiveData();

  // Clear the buffer to prevent it getting blocked
  if (lnRdy==2) flushPbuf();
  
}


/***************************************/
/***** GPIB DATA HANDLING ROUTINES *****/
/***************************************/

/*****  Send a single byte GPIB command *****/
bool gpibSendCmd(uint8_t cmdByte) {

  bool stat = false;

  // Set lines for command and assert ATN
  setGpibControls(CCMS);

  // Send the command
  stat = gpibWriteByte(cmdByte);
  if (stat && isVerb) {
    arSerial->print(F("gpibSendCmd: failed to send command "));
    arSerial->print(cmdByte, HEX);
    arSerial->println(F(" to device"));
  }

  // Return to controller idle state
  //  setGpibControls(CIDS);
  // NOTE: this breaks serial poll

  return stat ? ERR : OK;
}


/***** Send the status byte *****/
void gpibSendStatus() {
  // Have been addressed and polled so send the status byte
  if (isVerb) {
    arSerial->print(F("Sending status byte: "));
    arSerial->println(AR488.stat);
  };
  setGpibControls(DTAS);
  gpibWriteByte(AR488.stat);
  setGpibControls(DIDS);
}


/***** Send a series of characters as data to the GPIB bus *****/
void gpibSendData(char *data, uint8_t dsize) {

  // If lon is turned on we cannot send data so exit
  if (isRO) return;

  // Controler can unlisten bus and address devices
  if (AR488.cmode == 2) {

    // Address device to listen
    if (addrDev(AR488.paddr, 0)) {
      if (isVerb) {
        arSerial->print(F("gpibSendData: failed to address device "));
        arSerial->print(AR488.paddr);
        arSerial->println(F(" to listen"));
      }
      return;
    }

#ifdef DEBUG3
    arSerial->println(F("Device addressed."));
#endif

    // Set control lines to write data (ATN unasserted)
    setGpibControls(CTAS);

  } else {
    setGpibControls(DTAS);
  }
#ifdef DEBUG3
  arSerial->println(F("Set write data mode."));
  arSerial->print(F("Send->"));
#endif

  // Write the data string
  for (int i = 0; i < dsize; i++) {
    // If EOI asserting is on
    if (AR488.eoi) {
      // Send all characters
      gpibWriteByte(data[i]);
    } else {
      // Otherwise ignore non-escaped CR, LF and ESC
      if ((data[i] != CR) || (data[i] != LF) || (data[i] != ESC)) gpibWriteByte(data[i]);
    }
#ifdef DEBUG3
    arSerial->print(data[i]);
#endif
  }

#ifdef DEBUG3
  arSerial->println("<-End.");
#endif

  // Write terminators according to EOS setting
  // Do we need to write a CR?
  if ((AR488.eos & 0x2) == 0) {
    gpibWriteByte(CR);
#ifdef DEBUG3
    arSerial->println(F("Appended CR"));
#endif
  }
  // Do we need to write an LF?
  if ((AR488.eos & 0x1) == 0) {
    gpibWriteByte(LF);
#ifdef DEBUG3
    arSerial->println(F("Appended LF"));
#endif
  }

  // If EOI enabled then assert EOI
  if (AR488.eoi) {
    setGpibState(0b00000000, 0b00010000, 0);
    //    setGpibState(0b00010000, 0b00000000, 0b00010000);
    delayMicroseconds(40);
    setGpibState(0b00010000, 0b00010000, 0);
    //    setGpibState(0b00010000, 0b00010000, 0b00010000);
#ifdef DEBUG3
    arSerial->println(F("Asserted EOI"));
#endif
  }

  if (AR488.cmode == 2) {

    // Untalk controller and unlisten bus
    if (uaddrDev()) {
      if (isVerb) arSerial->println(F("gpibSendData: Failed to unlisten bus"));
    }

#ifdef DEBUG3
    arSerial->println(F("Unlisten done"));
#endif

    // Controller - set lines to idle?
    setGpibControls(CIDS);

  } else {
    // Device mode - set control lines to idle
    if (AR488.cmode == 1) setGpibControls(DIDS);
  }
#ifdef DEBUG3
    arSerial->println(F("<- End of send."));
#endif
 
}


/***** Receive data from the GPIB bus ****/
/*
 * Readbreak:
 * 5 - EOI detected
 * 7 - command received via serial
 */

bool gpibReceiveData() {

  //  char ch;
  uint8_t r = 0, db;

  int x = 0;
  //  int s=0;


  // Flag read in progress...
  isReading = true;

  // Reset transmission break flag
  tranBrk = 0;

  // If we are a controller
  if (AR488.cmode == 2) {

    // Address device to talk
    if (addrDev(AR488.paddr, 1)) {
      if (isVerb) {
        arSerial->print(F("Failed to address the device"));
        arSerial->print(AR488.paddr);
        arSerial->println(F(" to talk"));
      }
    }

    // Wait for instrument ready
    Wait_on_pin_state(HIGH, NRFD, AR488.rtmo);

    // Set GPIB control lines to controller read mode
    setGpibControls(CLAS);

  } else {
    // Set GPIB controls to device read mode
    setGpibControls(DLAS);

#ifdef DEBUG7
    arSerial->println(F("Start listen ->"));
    arSerial->println(tranBrk);
//    arSerial->println(isATN);
    arSerial->println(digitalRead(ATN) ? "HIGH" : "LOW");
#endif

  }

  // Perform read of data (r: 0=data; 1=cmd; >1=error;
  while ( tranBrk == 0 && digitalRead(ATN)==HIGH && !(r = gpibReadByte(&db)) ) {

    // When reading with EOI=1 or aMode=3 Check for break condition
    if (rEoi || (AR488.amode == 3)) readBreak();

    // If break condition ocurred or ATN asserted then break here
    if (tranBrk == 7 || digitalRead(ATN)==LOW) break;

#ifdef DEBUG1
    arSerial->print(db, HEX), arSerial->print(' ');
#else
    // Output the character to the serial port
    arSerial->print((char)db);
#endif

    // Reading with EOI and EOI detected - print last character and then break on EOI
    if (rEoi && tranBrk == 5) break;

    // Stop if byte = specified EOT character
    if (db == eByte && rEbt) break;
    // Stop on LF unless expecting EOI
    if (db == LF && !rEoi) break;
    // Stop on timeout
    if (r > 0) break;

    // Byte counter
    x++;

  }

#ifdef DEBUG7
  arSerial->println(F("After loop:"));
  arSerial->println(tranBrk);
//  arSerial->println(isATN);
  arSerial->println(r);
#endif

  // End of data - if verbose, report how many bytes read
  if (isVerb) {
    arSerial->print(F("Bytes read: "));
    arSerial->println(x);
  }

  // Detected that EOI has been asserted
  if (tranBrk == 5) {
    if (isVerb) arSerial->println(F("EOI detected!"));
    // If eot_enabled then add EOT character
    if (AR488.eot_en) arSerial->print(AR488.eot_ch);
  }

  // Timeout error?
  if (r > 0) {
    if (isVerb && r == 1) arSerial->println(F("gpibReceiveData: timeout waiting for talker"));
    if (isVerb && r == 2) arSerial->println(F("gpibReceiveData: timeout waiting for transfer to complete"));
  }

  if (AR488.cmode == 2) {

    // Untalk bus and unlisten controller
    if (uaddrDev()) {
      if (isVerb) arSerial->print(F("gpibSendData: Failed to untalk bus"));
    }

    // Set controller back to idle state
    if (AR488.cmode == 2) setGpibControls(CIDS);

  } else {
    // Set device back to idle state
    setGpibControls(DIDS);

#ifdef DEBUG7
    arSerial->println(F("<- End listen."));
#endif

  }

  // Reset flags
  isReading = false;
  if (tranBrk > 0) tranBrk = 0;

  if (r > 0) return ERR;

  return OK;
}


/***** Read a SINGLE BYTE of data from the GPIB bus using 3-way handshake *****/
/*
 * (- this function is called in a loop to read data    )
 * (- the GPIB bus must already be configured to listen )
 */
uint8_t gpibReadByte(uint8_t *db) {
  bool atnStat = (digitalRead(ATN) ? false : true); // Set to reverse, i.e. asserted=true; unasserted=false;

  // Unassert NRFD (we are ready for more data)
  setGpibState(0b00000100, 0b00000100, 0);

  // ATN asserted and just got unasserted - abort - we are not ready yet
  if (atnStat && (digitalRead(ATN)==HIGH)) {
    setGpibState(0b00000000, 0b00000100, 0);
    return 3;
  }

  // Wait for DAV to go LOW indicating talker has finished setting data lines..
  if (Wait_on_pin_state(LOW, DAV, AR488.rtmo))  {
    if (isVerb) arSerial->println(F("gpibReadByte: timeout waiting for DAV to go LOW"));
    setGpibState(0b00000000, 0b00000100, 0);
    // No more data for you?
    return 1;
  }

  // Assert NRFD (NOT ready - busy reading data)
  setGpibState(0b00000000, 0b00000100, 0);

  // read from DIO
  *db = readGpibDbus();

  // Unassert NDAC signalling data accepted
  setGpibState(0b00000010, 0b00000010, 0);

  // Wait for DAV to go HIGH indicating data no longer valid (i.e. transfer complete)
  if (Wait_on_pin_state(HIGH, DAV, AR488.rtmo))  {
    if (isVerb) arSerial->println(F("gpibReadByte: timeout waiting DAV to go HIGH"));
    return 2;
  }

  // Re-assert NDAC - handshake complete, ready to accept data again
  setGpibState(0b00000000, 0b00000010, 0);

  // GPIB bus DELAY
  delayMicroseconds(AR488.tmbus);

  return 0;

}


/***** Write a SINGLE BYTE onto the GPIB bus using 3-way handshake *****/
/*
 * (- this function is called in a loop to send data )
 */
bool gpibWriteByte(uint8_t db) {

  // Wait for NDAC to go LOW (indicating that devices are at attention)
  if (Wait_on_pin_state(LOW, NDAC, AR488.rtmo)) {
    if (isVerb) arSerial->println(F("gpibWriteByte: timeout waiting for receiver attention [NDAC asserted]"));
    return true;
  }
  // Wait for NRFD to go HIGH (indicating that receiver is ready)
  if (Wait_on_pin_state(HIGH, NRFD, AR488.rtmo))  {
    if (isVerb) arSerial->println(F("gpibWriteByte: timeout waiting for receiver ready - [NRFD unasserted]"));
    return true;
  }

  // Place data on the bus
  setGpibDbus(db);

  // Assert DAV (data is valid - ready to collect)
  setGpibState(0b00000000, 0b00001000, 0);

  // Wait for NRFD to go LOW (receiving)
  if (Wait_on_pin_state(LOW, NRFD, AR488.rtmo))  {
    if (isVerb) arSerial->println(F("gpibWriteByte: timeout receiving - [NRFD asserted]"));
    return true;
  }

  // Wait for NDAC to go HIGH (data accepted)
  if (Wait_on_pin_state(HIGH, NDAC, AR488.rtmo))  {
    if (isVerb) arSerial->println(F("gpibWriteByte: timeout waiting for data accepted [NDAC unasserted]"));
    return true;
  }

  // Unassert DAV
  setGpibState(0b00001000, 0b00001000, 0);

  // Reset the data bus
  setGpibDbus(0);

  // GPIB bus DELAY
  delayMicroseconds(AR488.tmbus);

  // Exit successfully
  return false;
}


/***** Untalk bus then address a device *****/
/*
 * dir: 0=listen; 1=talk;
 */
bool addrDev(uint8_t addr, bool dir) {
  if (gpibSendCmd(GC_UNL)) return ERR;
  if (dir) {
    // Device to talk, controller to listen
    if (gpibSendCmd(GC_TAD + addr)) return ERR;
    if (gpibSendCmd(GC_LAD + AR488.caddr)) return ERR;
  } else {
    // Device to listen, controller to talk
    if (gpibSendCmd(GC_LAD + addr)) return ERR;
    if (gpibSendCmd(GC_TAD + AR488.caddr)) return ERR;
  }
  return OK;
}


/***** Unaddress a device (untalk bus) *****/
bool uaddrDev() {
  // De-bounce
  delayMicroseconds(30);
  // Utalk/unlisten
  if (gpibSendCmd(GC_UNL)) return ERR;
  if (gpibSendCmd(GC_UNT)) return ERR;
  return OK;
}


/**********************************/
/*****  GPIB CONTROL ROUTINES *****/
/**********************************/


/***** Wait for "pin" to reach a specific state *****/
/*
 * Returns false on success, true on timeout.
 * Pin MUST be set as INPUT_PULLUP otherwise it will not change and simply time out!
 */
boolean Wait_on_pin_state(uint8_t state, uint8_t pin, int interval) {

  unsigned long timeout = millis() + interval;
  bool atnStat = (digitalRead(ATN) ? false : true); // Set to reverse - asserted=true; unasserted=false;

  while (digitalRead(pin) != state) {
    // Check timer
    if (millis() >= timeout) return true;
    // ATN status was asserted but now unasserted so abort
    if (atnStat && (digitalRead(ATN)==HIGH)) return true;
    //    if (digitalRead(EOI)==LOW) tranBrk = 2;
  }
  return false;        // = no timeout therefore succeeded!
}

/***** Control the GPIB bus - set various GPIB states *****/
/*
 * state is a predefined state (CINI, CIDS, CCMS, CLAS, CTAS, DINI, DIDS, DLAS, DTAS);
 * setGpibState (uint8_t direction, uint8_t state[low/high])
 * Bits control lines as follows: 8-ATN, 7-SRQ, 6-REN, 5-EOI, 4-DAV, 3-NRFD, 2-NDAC, 1-IFC
 * setGpibState byte2 (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
 * setGpibState byte3 (mask)     : 0=unaffected, 1=enabled
 * setGpibState byte3 (mode)     : 0=set pin state, 1=set pin direction
 */
void setGpibControls(uint8_t state) {

  // Switch state
  switch (state) {
    // Controller states
    case CINI:  // Initialisation
      // Set pin direction
      setGpibState(0b10111000, 0b11111111, 1);
      // Set pin state
      setGpibState(0b11011111, 0b11111111, 0);
#ifdef DEBUG2
      arSerial->println(F("Initialised GPIB control mode"));
#endif
      break;
    case CIDS:  // Controller idle state
      setGpibState(0b10111000, 0b10011110, 1);
      setGpibState(0b11011111, 0b10011110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines to idle state"));
#endif
      break;
    case CCMS:  // Controller active - send commands
      setGpibState(0b10111001, 0b10011111, 1);
      setGpibState(0b01011111, 0b10011111, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines for sending a command"));
#endif
      break;
    case CLAS:  // Controller - read data bus
      // Set state for receiving data
      setGpibState(0b10100110, 0b10011110, 1);
      setGpibState(0b11011000, 0b10011110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines for reading data"));
#endif
      break;
    case CTAS:  // Controller - write data bus
      setGpibState(0b10111001, 0b10011110, 1);
      setGpibState(0b11011111, 0b10011110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines for writing data"));
#endif
      break;

    /* Bits control lines as follows: 8-ATN, 7-SRQ, 6-REN, 5-EOI, 4-DAV, 3-NRFD, 2-NDAC, 1-IFC */

    // Listener states
    case DINI:  // Listner initialisation
      setGpibState(0b00000000, 0b11111111, 1);
      setGpibState(0b11111111, 0b11111111, 0);
#ifdef DEBUG2
      arSerial->println(F("Initialised GPIB listener mode"));
#endif
      break;
    case DIDS:  // Device idle state
      setGpibState(0b00000000, 0b00001110, 1);
      setGpibState(0b11111111, 0b00001110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines to idle state"));
#endif
      break;
    case DLAS:  // Device listner active (actively listening - can handshake)
      setGpibState(0b00000110, 0b00001110, 1);
      setGpibState(0b11111001, 0b00001110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines to idle state"));
#endif
      break;
    case DTAS:  // Device talker active (sending data)
      setGpibState(0b00001000, 0b00001110, 1);
      setGpibState(0b11111001, 0b00001110, 0);
#ifdef DEBUG2
      arSerial->println(F("Set GPIB lines for listening as addresed device"));
#endif
      break;
#ifdef DEBUG2
    default:
      // Should never get here!
      //      setGpibState(0b00000110, 0b10111001, 0b11111111);
      arSerial->println(F("Unknown GPIB state requested!"));
#endif
  }

  // Save state
  cstate = state;

  // GPIB bus delay (to allow state to settle)
  delayMicroseconds(AR488.tmbus);

}
