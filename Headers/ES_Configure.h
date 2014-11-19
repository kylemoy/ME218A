/****************************************************************************
 Module
     ES_Configure.h
 Description
     This file contains macro definitions that are edited by the user to
     adapt the Events and Services framework to a particular application.
 Notes
     
 History
 When           Who     What/Why
 -------------- ---     --------
 10/21/13 20:54 jec      lots of added entries to bring the number of timers
                         and services up to 16 each
 08/06/13 14:10 jec      removed PostKeyFunc stuff since we are moving that
                         functionality out of the framework and putting it
                         explicitly into the event checking functions
 01/15/12 10:03 jec      started coding
*****************************************************************************/

#ifndef CONFIGURE_H
#define CONFIGURE_H

/****************************************************************************/
// The maximum number of services sets an upper bound on the number of 
// services that the framework will handle. Reasonable values are 8 and 16
// corresponding to an 8-bit(uint8_t) and 16-bit(uint16_t) Ready variable size
#define MAX_NUM_SERVICES 16

/****************************************************************************/
// This macro determines that nuber of services that are *actually* used in
// a particular application. It will vary in value from 1 to MAX_NUM_SERVICES
#define NUM_SERVICES 2

/****************************************************************************/
// These are the definitions for Service 0, the lowest priority service.
// Every Events and Services application must have a Service 0. Further 
// services are added in numeric sequence (1,2,3,...) with increasing 
// priorities
// the header file with the public function prototypes
#define SERV_0_HEADER "DisarmFSM.h"
// the name of the Init function
#define SERV_0_INIT InitDisarmFSM
// the name of the run function
#define SERV_0_RUN RunDisarmFSM
// How big should this services Queue be?
#define SERV_0_QUEUE_SIZE 5

#if NUM_SERVICES > 1
#define SERV_1_HEADER "KeyPadFSM.h"
// the name of the Init function
#define SERV_1_INIT InitKeyPadFSM
// the name of the run function
#define SERV_1_RUN RunKeyPadFSM
// How big should this services Queue be?
#define SERV_1_QUEUE_SIZE 5
#endif


/****************************************************************************/
// Name/define the events of interest
// Universal events occupy the lowest entries, followed by user-defined events
typedef enum {  ES_NO_EVENT = 0,
                ES_ERROR,  /* used to indicate an error from the service */
                ES_INIT,   /* used to transition from initial pseudo-state */
                ES_TIMEOUT, /* signals that the timer has expired */
                /* User-defined events start here */
                ES_NEW_KEY, /* signals a new key received from terminal */
								THREE_HANDS_ON,
								THREE_HANDS_OFF,
								CORRECT_PASSWORD_ENTERED,
								INCORRECT_PASSWORD_ENTERED,
								KEY_INSERTED,
								CORRECT_VALUE_DIALED,
                } ES_EventTyp_t ;

/****************************************************************************/
// These are the definitions for the Distribution lists. Each definition
// should be a comma separated list of post functions to indicate which
// services are on that distribution list.
#define NUM_DIST_LISTS 2
#if NUM_DIST_LISTS > 0 
#define DIST_LIST0 PostDisarmFSM
#endif
#if NUM_DIST_LISTS > 1 
#define DIST_LIST1 PostKeyPadFSM
#endif		

/****************************************************************************/
// This are the name of the Event checking funcion header file. 
#define EVENT_CHECK_HEADER "EventCheckers.h"

/****************************************************************************/
// This is the list of event checking functions 
#define EVENT_CHECK_LIST CheckForKeyPadButtonPress, Check4Keystroke, CheckPot, CheckSlotDetector, CheckTapeSensor

/****************************************************************************/
// These are the definitions for the post functions to be executed when the
// corresponding timer expires. All 16 must be defined. If you are not using
// a timer, then you should use TIMER_UNUSED
// Unlike services, any combination of timers may be used and there is no
// priority in servicing them
#define TIMER_UNUSED ((pPostFunc)0)
#define TIMER0_RESP_FUNC PostDisarmFSM
#define TIMER1_RESP_FUNC PostDisarmFSM
#define TIMER2_RESP_FUNC PostDisarmFSM
#define TIMER3_RESP_FUNC PostDisarmFSM
#define TIMER4_RESP_FUNC PostDisarmFSM
#define TIMER5_RESP_FUNC TIMER_UNUSED
#define TIMER6_RESP_FUNC TIMER_UNUSED
#define TIMER7_RESP_FUNC TIMER_UNUSED
#define TIMER8_RESP_FUNC TIMER_UNUSED
#define TIMER9_RESP_FUNC TIMER_UNUSED
#define TIMER10_RESP_FUNC TIMER_UNUSED
#define TIMER11_RESP_FUNC TIMER_UNUSED
#define TIMER12_RESP_FUNC TIMER_UNUSED
#define TIMER13_RESP_FUNC TIMER_UNUSED
#define TIMER14_RESP_FUNC TIMER_UNUSED
#define TIMER15_RESP_FUNC PostKeyPadFSM

/****************************************************************************/
// Give the timer numbers symbolc names to make it easier to move them
// to different timers if the need arises. Keep these definitions close to the
// definitions for the response functions to make it easier to check that
// the timer number matches where the timer event will be routed
// These symbolic names should be changed to be relevant to your application 

#define DISARM_TIMER 0
#define POST_DISARM_TIMER 1
#define VIBRATION_TIMER 2
#define FAST_LEDS 3
#define MESSAGE_TIMER 4
#define KEYPAD_TIMER 15

#endif /* CONFIGURE_H */
