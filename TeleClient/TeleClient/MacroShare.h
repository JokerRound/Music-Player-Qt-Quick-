#pragma once
#ifndef MACROSHARE_H_
#define MACROSHARE_H_

// IOCP�ȴ�ʱ��(����)
#define IOCP_WAIT_TIME 100000

// Connect�����ȴ�ʱ��(����)
#define CONNECT_WAIT_TIME 10000

// ���ݰ���ͷ��С
#define PACKET_HEADER_SIZE (sizeof(PACKETFORMAT) - 1)

// The max size of packet's content.
#define PACKET_CONTENT_MAXSIZE (1024 * 1024 * 2)

// The max size fo transport file data.
#define FILE_TRANSPORT_MAXSIZE (1024 * 1024)

// ������ʱ��ID
#define TIMER_HEATBEAT 1

// ���������
#define HEATBEAT_ELAPSE 10000

#ifdef _UNICODE
#define __FILET__ __FILEW__
#else
#define __FILET__ __FILE__ 
#endif // _UNICODE

#endif // !MACROSHARE_H_
