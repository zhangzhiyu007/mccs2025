/*
 * freemodbus.cpp
 *
 *   创建日期: 2013-7-22
 *   作         者: mzh
 */

#include "mb.h"
#include "mbport.h"
#include <pthread.h>

#define PROG            "freemodbus"
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 2000
#define REG_HOLDING_NREGS 130

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

static enum ThreadState
{
    STOPPED,
    RUNNING,
    SHUTDOWN
} ePollThreadState;

static pthread_mutex_t xLock = PTHREAD_MUTEX_INITIALIZER;
static BOOL     bDoExit;

/* ----------------------- Static functions ---------------------------------*/
static BOOL     bCreatePollingThread( void );
static enum ThreadState eGetPollingThreadState( void );
static void     vSetPollingThreadState( enum ThreadState eNewState );
static void    *pvPollingThread( void *pvParameter );

///* ----------------------- Start implementation -----------------------------*/
//BOOL
//bSetSignal( int iSignalNr, void ( *pSigHandler ) ( int ) )
//{
//    BOOL            bResult;
//    struct sigaction xNewSig, xOldSig;
//
//    xNewSig.sa_handler = pSigHandler;
//    sigemptyset( &xNewSig.sa_mask );
//    xNewSig.sa_flags = 0;
//    if( sigaction( iSignalNr, &xNewSig, &xOldSig ) != 0 )
//    {
//        bResult = FALSE;
//    }
//    else
//    {
//        bResult = TRUE;
//    }
//    return bResult;
//}
//
//void
//vSigShutdown( int xSigNr )
//{
//    switch ( xSigNr )
//    {
//    case SIGQUIT:
//    case SIGINT:
//    case SIGTERM:
//        vSetPollingThreadState( SHUTDOWN );
//        bDoExit = TRUE;
//    }
//}

//	const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
//	if (!bSetSignal(SIGQUIT, vSigShutdown) || !bSetSignal(SIGINT,
//			vSigShutdown) || !bSetSignal(SIGTERM, vSigShutdown))
//	{
//		fprintf(stderr, "%s: can't install signal handlers: %s!\n", PROG,
//				strerror(errno));
//	}
//	else if (eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE) != MB_ENOERR)
//	{
//		fprintf(stderr, "%s: can't initialize modbus stack!\n", PROG);
//	}
//	else if (eMBSetSlaveID(0x34, TRUE, ucSlaveID, 3) != MB_ENOERR)
//	{
//		fprintf(stderr, "%s: can't set slave id!\n", PROG);
//	}
//
//	 vSetPollingThreadState( STOPPED );
//
//	bool success=bCreatePollingThread(  );
//	zlog_error(Util::m_zlog, "启动串口从站%d",success);


BOOL
bCreatePollingThread( void )
{
    BOOL            bResult;
    pthread_t       xThread;

    if( eGetPollingThreadState(  ) == STOPPED )
    {
        if( pthread_create( &xThread, NULL, pvPollingThread, NULL ) != 0 )
        {
            bResult = FALSE;
        }
        else
        {
            bResult = TRUE;
        }
    }
    else
    {
        bResult = FALSE;
    }

    return bResult;
}

void           *
pvPollingThread( void *pvParameter )
{
    vSetPollingThreadState( RUNNING );

    if( eMBEnable(  ) == MB_ENOERR )
    {
        do
        {
            if( eMBPoll(  ) != MB_ENOERR )
                break;
            usRegInputBuf[0] = ( USHORT ) 0;

        }
        while( eGetPollingThreadState(  ) != SHUTDOWN );
    }

    ( void )eMBDisable(  );

    vSetPollingThreadState( STOPPED );

    return 0;
}

enum ThreadState
eGetPollingThreadState(  )
{
    enum ThreadState eCurState;

    ( void )pthread_mutex_lock( &xLock );
    eCurState = ePollThreadState;
    ( void )pthread_mutex_unlock( &xLock );

    return eCurState;
}

void
vSetPollingThreadState( enum ThreadState eNewState )
{
    ( void )pthread_mutex_lock( &xLock );
    ePollThreadState = eNewState;
    ( void )pthread_mutex_unlock( &xLock );
}


eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

