#ifndef __NTSERVICEEVENTLOGMSG_H__
#define __NTSERVICEEVENTLOGMSG_H__

//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//      Sev - is the severity code
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//      R - is a reserved bit
//      Facility - is the facility code
//      Code - is the facility's status code
// Define the facility codes


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS			0x00
#define STATUS_SEVERITY_INFORMATIONAL	0x01
#define STATUS_SEVERITY_WARNING			0x02
#define STATUS_SEVERITY_ERROR			0x03

#define MSG_INFO_1						((DWORD)0x40000000L)
#define MSG_WARNING_1					((DWORD)0x80000001L)
#define MSG_ERROR_1						((DWORD)0xC0000002L)
#define MSG_SUCCESS_1					((DWORD)0x00000003L)

#endif // __NTSERVICEEVENTLOGMSG_H__
