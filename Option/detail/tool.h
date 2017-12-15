#ifndef TOOL_H
#define TOOL_H


#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/file.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <stdarg.h>
#include <signal.h>
#include <atomic>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <assert.h>
#include <poll.h>   // 2011-6-30 add ,for poll
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <chrono>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <pwd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <cstring>
#include <set>
#include <ctime>
#include <time.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <map>
#include "t2sdk_interface.h"
#include "data_def.h"
#include "stage/iblog.h"
#include "thread_pool.hpp"
#include "shm.h"

using namespace std;
using namespace ison;
#define DEBUG
#define TEN_POW2(prefix, exp) (static_cast<int64_t>(prefix##e##exp))
#define TEN_POW(exp) TEN_POW2(1, exp)

//解析应答包
inline void show_packer(IF2UnPacker* lpUnPacker)
{
	for (auto i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
	{
		printf("记录集: %d/%d\r\n", i + 1, lpUnPacker->GetDatasetCount());
		lpUnPacker->SetCurrentDatasetByIndex(i);
		//打印词条记录集中的所有信息
		for (decltype(lpUnPacker->GetRowCount()) j = 0; j < lpUnPacker->GetRowCount(); ++j)
		{
			printf("\t第%d/%d条记录：\r\n", j + 1, lpUnPacker->GetRowCount());
			//打印每条记录
			for (auto k = 0; k < lpUnPacker->GetColCount(); ++k)
			{
				switch (lpUnPacker->GetColType(k))
				{
				case 'I': {
					printf("\t【整数】%20s = %35d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetIntByIndex(k));
					break;
				}
				case 'C': {
					printf("\t【字符】%20s = %35c\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetCharByIndex(k));
					break;
				}
				case 'S': {
					if (NULL != strstr((char *)lpUnPacker->GetColName(k), "password"))
					{
						printf("\t【字串】%20s = %35s\r\n", lpUnPacker->GetColName(k), "******");
					}
					else
						printf("\t【字串】%20s = %35s\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetStrByIndex(k));
					break;
				}
				case 'F': {
					printf("\t【数值】%20s = %35f\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetDoubleByIndex(k));
					break;
				}
				case 'R': {
					int nLength = 0;
					void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);
					switch (nLength)
					{
					case 0:
					{
						printf("\t【数据】%20s = %35s\r\n", lpUnPacker->GetColName(k), "(N/A)");
						break;
					}
					default:
					{
						printf("\t【数据】%20s = 0x", lpUnPacker->GetColName(k));
						for (auto t = nLength; t < 11; t++) {
							printf("   ");
						}
						unsigned char *p = (unsigned char *)lpData;
						for (auto t = 0; t < nLength; t++) {
							printf("%3x", *p++);
						}
						printf("\r\n");
						break;
					}
					}
					break;
				}
				case 0: {
					printf("\t【数据】%20s = %35s\r\n", lpUnPacker->GetColName(k), "(N/A)");
					break;
				}
				default: {
					// 未知数据类型
					printf("未知数据类型。\n");
					break;
				}
				}
			}
			putchar('\n');
			lpUnPacker->Next();
		}
		putchar('\n');
	}
}

//获取当前日期
inline int GetCurDate()
{
	time_t tt;
	time(&tt);
	tm *pt = localtime(&tt);
	int date = (pt->tm_year + 1900) * TEN_POW(4) + (pt->tm_mon + 1) * 100 + pt->tm_mday;
	return date;
}
//获取当前时间
inline int GetCurTime()
{
	time_t tt;
	time(&tt);
	tm *pt = localtime(&tt);
	int time = pt->tm_hour * TEN_POW(4) + pt->tm_min * TEN_POW(2) + pt->tm_sec;
	return time;
}

inline void print_p()
{
	cerr << endl;
}

template<typename T, typename...ARGS>
inline void print_p(T& val, ARGS&&...args)
{
	cerr << val << ",";
	print_p(args...);
}

#endif
