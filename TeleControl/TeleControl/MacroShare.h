#pragma once
#ifndef MACROSHARE_H_
#define MACROSHARE_H_

// IOCP�ȴ�ʱ��(����)
#define IOCP_WAIT_TIME 100000

#define PACKET_CONTENT_MAXSIZE (1024 * 1024 * 2)

// The max size fo transport file data.
#define FILE_TRANSPORT_MAXSIZE (1024 * 1024)

#define PACKET_HEADER_SIZE (sizeof(PACKETFORMAT) - 1)

// ��Ŀ������������Ϣ�ĵȴ�ʱ�� 
#define GETTARGETINFO_WAITE_TIME 

#define WAIT_HADFILE_EVENT_TIME 100000


#ifdef _UNICODE
#define __FILET__ __FILEW__
#else
#define __FILET__ __FILE__ 
#endif // _UNICODE


#endif // !MACROSHARE_H_

