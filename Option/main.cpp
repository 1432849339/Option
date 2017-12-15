/****************************************************************************************

	创建日期:20171211
	作者:fqy
	功能:利用永安行情接口实现期权行情的订阅,并将数据保存至本地
	流程及思路:

	最后修改日期:2017年12月xx日

***************************************************************************************/


#include "OptionToSds.h"


int			date(0);
int main(int argc,char** argv)
{
	ison::base::iblog_init(argv[0], "log");
	ison::base::iblog_v(1002);
	ison::base::iblog_stderrthreshold(1);
	ison::base::iblog_logbufsecs(1);
	puts("**************************************************************\n");
	puts("======================期权行情程序启动========================\n");
	puts("**************************************************************\n");

	date = GetCurDate();
	int handle = 0;
	char shm_name[12];
	sprintf(shm_name, "OTN-%d", date);
	char app_name[] = "OptionMaket";
	try
	{
		if (InitShmOption(shm_name, 9901, app_name, handle))
		{
			return -1;
		}
		CMdRequestMode* lp_OptionMdMode = new CMdRequestMode();
		if (InitConnAndSubscribe(lp_OptionMdMode))
		{
			return -1;
		}
		thread write2shm(OpthqToOptionMarket, std::ref(handle));
		write2shm.join();
		lp_OptionMdMode->CloseConn();
		while (TimerExit())
		{
			this_thread::sleep_for(chrono::milliseconds(1000));
		}
	}
	catch (...)
	{
		shmClose(shm_name);
		LOG(ERROR) << "出现未知异常!";
	}
	shmClose(shm_name);
	LOG(INFO) << GetCurTime();
	puts("**************************************************************\n");
	puts("======================结束期权行情对的接收====================\n");
	puts("**************************************************************\n");
    return 0;
}