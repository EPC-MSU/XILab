#include <windows.h> 
#include <functions.h>
#include <math.h>
#include <QFile>
#include <myexception.h>

#define BUFSIZE PACKET_SIZE

#define READ_STRUCTURE(Name) memcpy(&Name, ReadPacket.Data, sizeof(Name))

#define S(CODE) \
  case S##CODE##_CMD:\
  READ_STRUCTURE(all.BCDFlashParams.S##CODE);\
  break;

#define G(CODE, STRUCT) \
  case G##CODE##_CMD: \
  zerofill = false; \
  CRC.U16 = CRC16((INT8U *)&all.BCD##STRUCT##Params.S##CODE, sizeof(all.BCD##STRUCT##Params.S##CODE)); \
  memcpy(Array + StartPos, &all.BCD##STRUCT##Params.S##CODE, sizeof(all.BCD##STRUCT##Params.S##CODE)); \
  StartPos += sizeof(all.BCD##STRUCT##Params.S##CODE); \
  break;


DWORD WINAPI InstanceThread(LPVOID); 

#include "fwmain.h"
#include "iobuf.h"
#include "iobuf.c"
#include "com_hl.h"
#include "fwprotocol.c"

typedef struct
{
	LPVOID pipe;
	INT32U serial;
} ThreadParameters;

// a struct that holds all info about our controller (it is saved to file)
typedef struct
{
	BCDRamParamsStr BCDRamParams;
	BCDFlashParamsStr BCDFlashParams;
	BCDStageParamsStr BCDStageParams;
	BCDInfoStr BCDInfo;
	long long last_tick;
} AllParamsStr;

// returns time passed since fixed date in 1600s in 100ns units as 64-bit int
long long getXtime()
{
	_FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;
	return ul.QuadPart;
}

// returns random int between "low" and "high" (inclusive)
int rand(int low, int high)
{
	return ((rand() % (high - low + 1)) + low);
}

template <typename T> int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

unsigned short CRC16(INT8U *pbuf, unsigned short n)
{
  unsigned short crc, i, j, carry_flag, a;
  crc = 0xffff;
  for(i = 0; i < n; i++)
  {
    crc = crc ^ pbuf[i];
    for(j = 0; j < 8; j++)
    {
      a = crc;
      carry_flag = a & 0x0001;
      crc = crc >> 1;
      if ( carry_flag == 1 ) crc = crc ^ 0xa001;
    }
  }
  return crc;
}

/*
 *  This function is a lookup routine to get additional data length in the packet
 *  If command is listed in the switch placeholder - it returns corresponding data length.
 *  If not - it returns zero, which means that no additional data is attached.
 */

static INT16U GetReadDataSize(unsigned long Command)
{
  for (INT32U i=0; CmdLengths[i].Cmd != 0; i++)
  {
    if (Command == CmdLengths[i].Cmd)
    {
      if (CmdLengths[i].SendBytes == 0)
        return 0;
      else
        return CmdLengths[i].SendBytes - _PROTOCOL_CRC_SIZE - _COMMAND_LENGTH; // return bytes that were send to us minus CRC and command code
    }
  }
  return 0;
}

static INT16U GetWriteDataSize(unsigned long Command)
{
  for (INT32U i=0; CmdLengths[i].Cmd != 0; i++)
  {
    if (Command == CmdLengths[i].Cmd)
    {
      if (CmdLengths[i].ReceiveBytes == 0)
        return 0;
      else
        return CmdLengths[i].ReceiveBytes - _PROTOCOL_CRC_SIZE - _COMMAND_LENGTH; // return bytes that were receive to us minus CRC and command code
    }
  }
  return 0;
}


/*
 *  This function gets a command-packet structure and puts response into Array.
 *  The Array size must be enough to hold any single response.
 *  The return value is the size of writtendata into Array
 */
static INT16U GetData(tPacket & ReadPacket, unsigned char * Array, IO_BUF* pTxBuf, INT32U sn, /*BCDRamParamsStr* ramParams*/AllParamsStr& all)
{
  INT16U StartPos=0; // We are going to add data to the Array, so we need a byte counter. We also are going to return its value.
  UU16 CRC; // This value will contain the packet CRC16 code to verify its data
  
  memset(Array, 0, 256); // todo remove
  // Each transmit starts with command code
  memcpy(Array,ReadPacket.Command.U8, _COMMAND_LENGTH);
  StartPos += _COMMAND_LENGTH;
  
  bool zerofill = true; // if true, then return array is filled with [gasp] zeroes

  if (ReadPacket.DataSize!=0) // If there is data
  {

	// ----- Check CRC ----- //
    CRC.U16=*((INT16U*)(ReadPacket.Data+ReadPacket.DataSize));
    if (CRC.U16!=CRC16(&ReadPacket.Data[0],ReadPacket.DataSize))
    {
      // If CRC is wrong send error, set flag and quit
      memcpy(Array,"errd",_COMMAND_LENGTH); // Command length is assumed to be 4 here ("errd" is 4 bytes)
 //     BCDRamParams.GETS.Flags|=STATE_ERRD;
      return _COMMAND_LENGTH;
    }
    
    // ----- Processing commands with data ----- //
    switch(ReadPacket.Command.U32)
    {
	case ASIA_CMD: break;
	case MOVE_CMD: {
		move_cmd_str MoveArr;
		READ_STRUCTURE(MoveArr);
		all.BCDRamParams.MOVE.Position = MoveArr.Position;
		all.BCDRamParams.MOVE.uPosition = MoveArr.uPosition;
		all.BCDRamParams.GETS.MvCmdSts = MVCMD_MOVE | MVCMD_RUNNING;
		all.BCDRamParams.GETS.CurSpeed = all.BCDFlashParams.SMOV.Speed;
		all.BCDRamParams.GETS.uCurSpeed = all.BCDFlashParams.SMOV.uSpeed;
		break;
				   }
	case MOVR_CMD: {
		movr_cmd_str MovrArr;
		READ_STRUCTURE(MovrArr);
		all.BCDRamParams.MOVE.Position += MovrArr.DeltaPosition;
		all.BCDRamParams.MOVE.uPosition += MovrArr.uDeltaPosition;
		all.BCDRamParams.GETS.MvCmdSts = MVCMD_MOVR | MVCMD_RUNNING;
		all.BCDRamParams.GETS.CurSpeed = all.BCDFlashParams.SMOV.Speed;
		all.BCDRamParams.GETS.uCurSpeed = all.BCDFlashParams.SMOV.uSpeed;
		break;
				   }
    case SPOS_CMD:
		spos_cmd_str PosArr;
		READ_STRUCTURE(PosArr);
		if (!(PosArr.PosFlags & SETPOS_IGNORE_POSITION)) {
			all.BCDRamParams.GETS.CurPosition = PosArr.Position;
			all.BCDRamParams.GETS.uCurPosition = PosArr.uPosition;
		}
		if (!(PosArr.PosFlags & SETPOS_IGNORE_ENCODER))
			all.BCDRamParams.GETS.EncPosition = PosArr.EncPosition;
		break;
	case SFBS_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SFBS);
		break;
    case SNMF_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SNMF);
		break;

    case SNME_CMD:
      READ_STRUCTURE(all.BCDStageParams.SNME);
      break;
    case SSTI_CMD:
      READ_STRUCTURE(all.BCDStageParams.SSTI);
      break;
    case SSTS_CMD:
      READ_STRUCTURE(all.BCDStageParams.SSTS);
      break;
    case SMTI_CMD:
      READ_STRUCTURE(all.BCDStageParams.SMTI);
      break;
    case SMTS_CMD:
      READ_STRUCTURE(all.BCDStageParams.SMTS);
      break;
    case SENI_CMD:
      READ_STRUCTURE(all.BCDStageParams.SENI);
      break;
    case SENS_CMD:
      READ_STRUCTURE(all.BCDStageParams.SENS);
      break;
    case SHSI_CMD:
      READ_STRUCTURE(all.BCDStageParams.SHSI);
      break;
    case SHSS_CMD:
      READ_STRUCTURE(all.BCDStageParams.SHSS);
      break;
    case SGRI_CMD:
      READ_STRUCTURE(all.BCDStageParams.SGRI);
      break;
    case SGRS_CMD:
      READ_STRUCTURE(all.BCDStageParams.SGRS);
      break;
    case SACC_CMD:
      READ_STRUCTURE(all.BCDStageParams.SACC);
      break;

    case SENT_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SENT);
		break;
    case SSEC_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SSEC);
		break;
    case SEDS_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SEDS);
		break;
    case SPID_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SPID);
		break;
    case SHOM_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SHOM);
		break;
    case SMOV_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SMOV);
		break;
    case SENG_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SENG);
		break;
    case SURT_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SURT);
		break;
    case SPWR_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SPWR);
		break;
    case SSNI_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SSNI);
		break;
    case SSNO_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SSNO);
		break;
    case SEIO_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SEIO);
		break;
    case SBRK_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SBRK);
		break;
    case SCTL_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SCTL);
		break;
    case SJOY_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SJOY);
		break;
    case SCTP_CMD:
		READ_STRUCTURE(all.BCDFlashParams.SCTP);
		break;
	default:
      memcpy(Array,"errc",_COMMAND_LENGTH);
      return _COMMAND_LENGTH;
    }
    memcpy(Array, &ReadPacket.Command.U32, _COMMAND_LENGTH);
    return _COMMAND_LENGTH;
  }
    
  // ----- Processing commands with no data ----- //
  switch(ReadPacket.Command.U32)
  {
  case REST_CMD: break;
  case UPDF_CMD: break;
  case CLFR_CMD: break;
  case PWOF_CMD: break;
  case STOP_CMD:
	  all.BCDRamParams.GETS.MvCmdSts = MVCMD_STOP;
	  all.BCDRamParams.GETS.CurSpeed = 0;
	  all.BCDRamParams.GETS.uCurSpeed = 0;
	  break;
  case SSTP_CMD:
	  all.BCDRamParams.GETS.MvCmdSts = MVCMD_SSTP;
	  all.BCDRamParams.GETS.CurSpeed = 0;
	  all.BCDRamParams.GETS.uCurSpeed = 0;
	  break;
  case ZERO_CMD:
	  all.BCDRamParams.GETS.CurPosition = 0;
	  all.BCDRamParams.GETS.uCurPosition = 0;
	  all.BCDRamParams.GETS.EncPosition = 0;
	  all.BCDRamParams.MOVE.Position = 0;
	  all.BCDRamParams.MOVE.uPosition = 0;
	  break;
  case HOME_CMD:
	  all.BCDRamParams.GETS.MvCmdSts = MVCMD_HOME;
	  break;
  case LEFT_CMD:
	  all.BCDRamParams.GETS.MvCmdSts = MVCMD_LEFT | MVCMD_RUNNING;
	  all.BCDRamParams.GETS.CurSpeed = all.BCDFlashParams.SMOV.Speed;
	  all.BCDRamParams.GETS.uCurSpeed = all.BCDFlashParams.SMOV.uSpeed;
	  all.BCDRamParams.GETS.CurSpeed = -all.BCDRamParams.GETS.CurSpeed;
	  all.BCDRamParams.GETS.uCurSpeed = -all.BCDRamParams.GETS.uCurSpeed;
	  break;
  case RIGT_CMD:
	  all.BCDRamParams.GETS.MvCmdSts = MVCMD_RIGHT | MVCMD_RUNNING;
	  all.BCDRamParams.GETS.CurSpeed = all.BCDFlashParams.SMOV.Speed;
	  all.BCDRamParams.GETS.uCurSpeed = all.BCDFlashParams.SMOV.uSpeed;
	  break;
  case LOFT_CMD: break;
  case SAVE_CMD: break;
  case READ_CMD: break;
  case EESV_CMD: break;
  case EERD_CMD: break;
  case GPOS_CMD: break;
  case GETI_CMD:
	zerofill = false;
	memcpy(&all.BCDInfo.DEVICE_INFO, "XIMCSMXISM-USB", sizeof(all.BCDInfo.DEVICE_INFO) /* 4+2+8 */ );

    CRC.U16 = CRC16((INT8U *)&all.BCDInfo.DEVICE_INFO, sizeof(all.BCDInfo.DEVICE_INFO));
    memcpy(Array + StartPos,&all.BCDInfo.DEVICE_INFO, sizeof(all.BCDInfo.DEVICE_INFO));
    StartPos += sizeof(all.BCDInfo.DEVICE_INFO);
	break;
  case GETS_CMD: {
	zerofill = false;

	// Let's imagine things
	all.BCDRamParams.GETS.Upwr = rand(1201, 1210);
	all.BCDRamParams.GETS.Ipwr = rand(3, 9);
	all.BCDRamParams.GETS.Uusb = rand(480, 520);
	all.BCDRamParams.GETS.Iusb = rand(180, 210);
	all.BCDRamParams.GETS.CurT = 366;

	smov_cmd_str smov = all.BCDFlashParams.SMOV;
	seng_cmd_str seng = all.BCDFlashParams.SENG;
	gets_cmd_str gets = all.BCDRamParams.GETS;
	move_cmd_str move = all.BCDRamParams.MOVE;
	long long this_tick = getXtime();
	double micromult = (1 << (seng.MicrostepMode-1)); // declared as double to have less problems with implicit type conversion
	double diff = (this_tick - all.last_tick)*(smov.Speed + (smov.uSpeed / micromult))/pow(10.0,7); // 10^7 is here because of 100 ns intervals

	double new_pos = gets.CurPosition + gets.uCurPosition / micromult;
	double tgt_pos = move.Position + move.uPosition / micromult;
	if (false) { // for text alignment, lol
	} else if ((gets.MvCmdSts & MVCMD_NAME_BITS) == MVCMD_RIGHT) {
		new_pos += diff;
	} else if ((gets.MvCmdSts & MVCMD_NAME_BITS) == MVCMD_LEFT) {
		new_pos -= diff;
	} else if ((gets.MvCmdSts & MVCMD_NAME_BITS) == MVCMD_MOVE
			|| (gets.MvCmdSts & MVCMD_NAME_BITS) == MVCMD_MOVR) {
		if (new_pos != tgt_pos)
			new_pos -= std::min(abs(diff), abs(new_pos-tgt_pos)) * sgn(new_pos-tgt_pos);
		else {
			gets.MvCmdSts &= ~MVCMD_RUNNING;
			gets.CurSpeed = 0;
			gets.uCurSpeed = 0;
		}
	}
	gets.CurPosition = (long long)new_pos;		
	gets.uCurPosition = (new_pos - (long long)new_pos)*micromult;

	all.BCDRamParams.GETS = gets;
	all.last_tick = this_tick;

	CRC.U16 = CRC16((INT8U *)&all.BCDRamParams.GETS, sizeof(all.BCDRamParams.GETS));
	memcpy(Array + StartPos, &all.BCDRamParams.GETS, sizeof(all.BCDRamParams.GETS));
	StartPos += sizeof(all.BCDRamParams.GETS);
	break;
				 }
  case GETC_CMD:
	zerofill = false;

	// More delusions
	all.BCDRamParams.GETC.Joy = rand(4500, 5000);
	all.BCDRamParams.GETC.Pot = rand(6000, 8000);
	all.BCDRamParams.GETC.DutyCycle = 200;

	CRC.U16 = CRC16((INT8U *)&all.BCDRamParams.GETC, sizeof(all.BCDRamParams.GETC));
	memcpy(Array + StartPos, &all.BCDRamParams.GETC, sizeof(all.BCDRamParams.GETC));
	StartPos += sizeof(all.BCDRamParams.GETC);
	break;
  case RDAN_CMD:
	zerofill = false;

	// Ultimate crazy
	all.BCDRamParams.RDAN.R = rand(0, 20000);
	all.BCDRamParams.RDAN.L = rand(0, 40000);

	CRC.U16 = CRC16((INT8U *)&all.BCDRamParams.RDAN, sizeof(all.BCDRamParams.RDAN));
	memcpy(Array + StartPos, &all.BCDRamParams.RDAN, sizeof(all.BCDRamParams.RDAN));
	StartPos += sizeof(all.BCDRamParams.RDAN);
	break;
  case GFWV_CMD: break;
  G(HOM, Flash);
  G(SNI, Flash);
  G(SNO, Flash);
  G(EIO, Flash);
  G(NMF, Flash);

  G(NME, Stage);
  G(STI, Stage);
  G(STS, Stage);
  G(MTI, Stage);
  G(MTS, Stage);
  G(ENI, Stage);
  G(ENS, Stage);
  G(HSI, Stage);
  G(HSS, Stage);
  G(GRI, Stage);
  G(GRS, Stage);
  G(ACC, Stage);

  G(PID, Flash);
  G(EDS, Flash);
  G(ENG, Flash);
  G(MOV, Flash);
  G(FBS, Flash);
  case GSER_CMD: {
	zerofill = false;
	INT32U buf_SN = sn;
	CRC.U16 = CRC16((INT8U *)&buf_SN, _SERIAL_LENGTH);
	memcpy(Array + StartPos, &buf_SN, _SERIAL_LENGTH);
	StartPos += _SERIAL_LENGTH;
	break;
				 }
  G(ENT, Flash);
  G(URT, Flash);
  G(PWR, Flash);
  G(SEC, Flash);
  G(BRK, Flash);
  G(CTL, Flash);
  G(JOY, Flash);
  G(CTP, Flash);
  case GBLV_CMD: break;
  case DBGR_CMD: break;
  default:
    memcpy(Array, "errc", _COMMAND_LENGTH); // Command length is assumed to be four as it is "errc" size
//	BCDRamParams.GETS.Flags |= STATE_ERRC; // We set a flag to indicate errc state
    return _COMMAND_LENGTH;
  }
  if (zerofill) {
	int writesize = GetWriteDataSize(ReadPacket.Command.U32);
	memset(Array + StartPos, 0, writesize);
	CRC.U16 = CRC16(Array + StartPos, writesize);
	StartPos += writesize;
  }

  if (StartPos > _COMMAND_LENGTH) // If data was added
  {
    memcpy(Array + StartPos, CRC.U8, 2); // Add previously calculated CRC
    StartPos+=2;
  }
  return StartPos;
}


/*
*  This function handles protocol parsing and protocol response between controlling unit and the device
*  A fully received command is searched for in incoming cycle buffer. Whence found, it is processed with GetData()
*  the return value describes whether a command has been processed
*  It also handles synchronization zeroes logic.
*/
INT16U iDataProcess(tPacket* RPacket, IO_BUF* pRxBuf, IO_BUF* pTxBuf, INT32U sn, /*BCDRamParamsStr* ramParams*/AllParamsStr& allParams)
{
  tPacket ReadPacket = *RPacket; // A packet under examination
  tData WriteData; // A structure to store the response.
  INT16U retval=0; // Return value (number of new bytes to send)
  
  // First we look whether we have previous knowledge about command length
  if (ReadPacket.DataSize == 0) // Length is zero - check for command
  {
    while(pRxBuf->len) // While there are symbols in the input buffer
    { // If first symbol is '\0' then we must echo and discard it (the same for each next byte)
        INT8U FirstByte;
        IOBuf_PeekC(pRxBuf,&FirstByte); // Look at first byte
        if(FirstByte) // If byte is a command byte - exit cycle and proceed to command recognition
          break; // This is the only condition to quit the cycle
        IOBuf_GetC(pRxBuf, &FirstByte); // Otherwise load zero byte from buffer
        IOBuf_PutC(pTxBuf, FirstByte); // and prepare to send it
        retval++; // One more byte to send
    }
    // Now first byte is not zero and must be the first symbol of a command
    if (pRxBuf->len < _COMMAND_LENGTH) // Command not received yet
      return retval; // Return and wait for missing bytes
    // We have a command. Proceed.
    IOBuf_GetBuf(pRxBuf,ReadPacket.Command.U8,_COMMAND_LENGTH); // Transfer bytes from cycle buffer to packet structure
    ReadPacket.DataSize=(unsigned char)GetReadDataSize(ReadPacket.Command.U32); // Get additional data size
  }
  // At this stage we know our command and its additional data size
  if(ReadPacket.DataSize != 0)    //If there is any additional data to receive. (Not else)
  {
    if(pRxBuf->len < ReadPacket.DataSize+_PROTOCOL_CRC_SIZE) // If not enough additional data
      return retval;   // Quit and wait for more data
    IOBuf_GetBuf(pRxBuf, ReadPacket.Data, ReadPacket.DataSize+_PROTOCOL_CRC_SIZE); // Transfer data to packet structure.
  }
  // At this stage we have a packet with command with possible additional data. Beginning processing.
  WriteData.Size=(unsigned char)GetData(ReadPacket, WriteData.Array, pTxBuf, sn, /*ramParams*/allParams); // Process command.
  IOBuf_PutBuf(pTxBuf,WriteData.Array, WriteData.Size); // Copy response to output buffer.
  retval+=WriteData.Size;
  ReadPacket.DataSize = 0;  // Reinit command sequence.
  return retval;
}



#include <virtualximcserverthread.h>

const int UPDATE_INTERVAL = 100;

VirtualXimcServerThread::VirtualXimcServerThread()
{
;
}

VirtualXimcServerThread::~VirtualXimcServerThread()
{
;
}

void VirtualXimcServerThread::run()
{/*
	timer = new QTimer();
	connect(timer, SIGNAL(timeout()), this, SLOT(update()), Qt::DirectConnection);
	timer->start(UPDATE_INTERVAL);
*/
//	exec();
   BOOL   fConnected = FALSE; 
   DWORD  dwThreadId = 0; 
   HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL; 
   QString pipename = QString("\\\\.\\pipe\\virtualximc_") + QString::number(GetCurrentProcessId());
   LPTSTR lpszPipename = (LPWSTR) pipename.utf16();
   INT32U serial = 1;
 
// The main loop creates an instance of the named pipe and 
// then waits for a client to connect to it. When the client 
// connects, a thread is created to handle communications 
// with that client, and this loop is free to wait for the
// next client connect request. It is an infinite loop.
 
   for (;;) 
   { 
//      _tprintf( TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), lpszPipename);
      hPipe = CreateNamedPipe( 
          lpszPipename,             // pipe name 
          PIPE_ACCESS_DUPLEX,       // read/write access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          BUFSIZE,                  // output buffer size 
          BUFSIZE,                  // input buffer size 
          0,                        // client time-out 
          NULL);                    // default security attribute 

      if (hPipe == INVALID_HANDLE_VALUE) 
      {
//          _tprintf(TEXT("CreateNamedPipe failed, GLE=%d.\n"), GetLastError()); 
          return ;//-1;
      }
 
      // Wait for the client to connect; if it succeeds, 
      // the function returns a nonzero value. If the function
      // returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
 
      fConnected = ConnectNamedPipe(hPipe, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
 
      if (fConnected) 
      { 
         printf("Client connected, creating a processing thread.\n"); 
      
		 ThreadParameters* thParam = new ThreadParameters;
		 thParam->pipe = hPipe;
 		 thParam->serial = serial++;
         // Create a thread for this client. 
         hThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default stack size 
            InstanceThread,    // thread proc
            thParam,           // thread parameter 
            0,                 // not suspended 
            &dwThreadId);      // returns thread ID 

         if (hThread == NULL) 
         {
//            _tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError()); 
            return ;//-1;
         }
         else CloseHandle(hThread); 
       } 
      else 
        // The client could not connect, so close the pipe. 
         CloseHandle(hPipe); 
   } 
}

DWORD WINAPI InstanceThread(LPVOID thParam)
{ 
	IO_BUF RxBuf, TxBuf;
	tPacket RPacket;
	RPacket.DataSize = 0;

	ThreadParameters * tp = (ThreadParameters*)thParam;
	LPVOID lpvParam = tp->pipe;
	INT32U serial = tp->serial;
	delete tp;

	char          szBuff[BUFSIZE];
	int           ret;
	AllParamsStr allParamsStr;
	long long time;
	time = getXtime();

	memset(&allParamsStr, 0, sizeof(AllParamsStr));
	QString filename = getDefaultPath() + QString("/V_") + QString::number(serial);
	LPCWSTR filestr = filename.utf16();
	DWORD dwSize = 0, dwRead = 0;
	HANDLE hFile;
	try {
		hFile = CreateFileW(filestr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw my_exception("Could not open file.");
		dwSize = GetFileSize(hFile, NULL);
		if (dwSize != sizeof allParamsStr)
			throw my_exception("File size does not match.");
		ReadFile(hFile, &allParamsStr, dwSize, &dwRead, NULL);
		if (dwRead != dwSize)
			throw my_exception("Read truncated or failed.");
		CloseHandle(hFile);
	}
	catch (my_exception) {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile); // clean up after exception threw up all over the place
		QFile resource(":/vmem/V_");
		resource.open(QIODevice::ReadOnly);
		resource.read((char*)&allParamsStr, sizeof allParamsStr);
		resource.close();
	}
   HANDLE hHeap      = GetProcessHeap();
   TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
   TCHAR* pchReply   = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));

   DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0; 
   BOOL fSuccess = FALSE;
   HANDLE hPipe  = NULL;

   // Do some extra error checking since the app will keep running even if this
   // thread fails.

   if (lpvParam == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }

   if (pchRequest == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       return (DWORD)-1;
   }

   if (pchReply == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   InstanceThread got an unexpected NULL heap allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }

   // Print verbose messages. In production code, this should be for debugging only.
   printf("InstanceThread created, receiving and processing messages.\n");

// The thread's parameter is a handle to a pipe object instance. 
   hPipe = (HANDLE) lpvParam; 

	// Init Rx/Tx buffers
	IOBuf_Init(&RxBuf);
	IOBuf_Init(&TxBuf);

    while(1)
    {
   // Read client requests from the pipe. This simplistic code only allows messages
   // up to BUFSIZE characters in length.
      fSuccess = ReadFile( 
         hPipe,        // handle to pipe 
         szBuff,//pchRequest,    // buffer to receive data 
         BUFSIZE,      // size of buffer 
         &cbBytesRead, // number of bytes read 
         NULL);        // not overlapped I/O 

      if (!fSuccess || cbBytesRead == 0)
      {   
          if (GetLastError() == ERROR_BROKEN_PIPE)
          {
//              _tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError()); 
          }
          else
          {
//              _tprintf(TEXT("InstanceThread ReadFile failed, GLE=%d.\n"), GetLastError()); 
          }
          break;
      }
		ret = cbBytesRead;

		IOBuf_PutBuf(&RxBuf, (INT8U*)&szBuff[0], ret);

		// And print it to the screen
		szBuff[ret] = '\0';
//        printf("RECV: '%s'\n", szBuff);

		while (iDataProcess(&RPacket, &RxBuf,&TxBuf, serial, /*&ramParamsStr*/allParamsStr)) { // Process received data and check whether there was a response continuosly. There may be two or more commands in one transmission. We must handle it anyways.
			// While there is a response, put data from Txbuffer to Tx FIFO until Tx empty or FIFO full

			INT16U size = IOBuf_Size(&TxBuf);
			INT8U *cbuf = new INT8U[size+1];

			IOBuf_GetBuf(&TxBuf, cbuf, size);
			
   // Write the reply to the pipe. 
      cbReplyBytes = size;
      fSuccess = WriteFile( 
         hPipe,        // handle to pipe 
         cbuf,//pchReply,     // buffer to write from 
         cbReplyBytes, // number of bytes to write 
         &cbWritten,   // number of bytes written 
         NULL);        // not overlapped I/O 

      if (!fSuccess || cbReplyBytes != cbWritten)
      {   
//          _tprintf(TEXT("InstanceThread WriteFile failed, GLE=%d.\n"), GetLastError()); 
          break;
      }
//			cbuf[size] = '\0';
//			printf("SENT: '%s'\n", cbuf);
			delete[] cbuf;
		}		
    }

   FlushFileBuffers(hPipe); 
   DisconnectNamedPipe(hPipe); 
   CloseHandle(hPipe);

   DWORD dwWrit;
   hFile = CreateFileW(filestr, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile != INVALID_HANDLE_VALUE) {
     WriteFile(hFile, &allParamsStr, sizeof allParamsStr, &dwWrit, NULL);
     CloseHandle(hFile);
   }

   HeapFree(hHeap, 0, pchRequest);
   HeapFree(hHeap, 0, pchReply);

   printf("InstanceThread exiting.\n");
   return 1;
}