#include "OptionToSds.h"


std::atomic<bool>					ready(false);//获取期权代码是否完成
map<int64_t, OptionCodeInfo>		Subscried;	 //订阅成功的代码
queue<std::string>					Tasks;		 //订阅回报字符串
mutex								task_cs;	 //同步锁
extern int							date;
#ifdef DEBUG
MyLog		mylog_future("/home/ison/fengqianyong/data/future", GetCurTime());
MyLog		mylog("/home/ison/fengqianyong/data/option", GetCurTime());
#endif // DEBUG
int InitShmOption(char* shm_name, const int main_shm, char *app_name, int& handle)
{
	int retcode = shmInit(main_shm, app_name);
	if (retcode != 0)
	{
		LOG(ERROR) << "\n shmInit error=[" << retcode;
		return retcode;
	}
	int block_recnum = 1000000;
	int record_size = sizeof(SDS20FUTURE);	//maintype和shmid用OTN20171213 1171213
	string shm_id = "117" + string(shm_name + 8);
	retcode = shmCreateForWrite(shm_name, atoi(shm_id.c_str()), 2, record_size, block_recnum);
	if (retcode != 0)
	{
		LOG(ERROR) << "shmCreate error["<<retcode;
		return retcode;
	}
	LOG(INFO) << "maintype : " << shm_name;
	LOG(INFO) << "shmid:" << shm_id;
	LOG(INFO) << "record_size:" << record_size;
	LOG(INFO) << "block_recnum:" << block_recnum;
	handle = shmHandle(shm_name);
	if (handle < 0)
	{
		shmClose(shm_name);
		LOG(ERROR) << "shmHandle error=[" << handle;
		return handle;
	}
	return 0;
}

int InitConnAndSubscribe(CMdRequestMode* lp_OptionMdMode)
{
	lp_OptionMdMode->InitConn();
	if (0 != lp_OptionMdMode->ReqFunction331100())
	{
		LOG(ERROR) << "登陆失败!";
		return -1;
	}
	if (0 != lp_OptionMdMode->ReqFunction338000())
	{
		LOG(ERROR) << "获取期权代码信息失败!";
		return -2;
	}
	while (!ready)
	{
		LOG(WARNING) << "等待期权代码信息返回" << endl;
		this_thread::sleep_for(chrono::seconds(2));
	}
	LOG(WARNING) << "**************************************************************";
	LOG(WARNING) << "期权代码个数:" << lp_OptionMdMode->GetOptionCode().size() << endl;
	LOG(WARNING) << "**************************************************************";
	for (auto &it : lp_OptionMdMode->GetOptionCode())
	{
		if (0 != lp_OptionMdMode->SubFunction26(ISSUE_TYPE_HQ_SECU, it.second.option_code))
		{
			LOG(ERROR) << "订阅期权行情失败!";
			return -3;
		}
	}
	this_thread::sleep_for(chrono::seconds(20));
	LOG(WARNING) << "**************************************************************";
	LOG(WARNING) << "订阅成功的代码数量:" << Subscried.size();
	LOG(WARNING) << "**************************************************************";
	return 0;
}

void OpthqToOptionMarket(int& handle)
{
	string temp;//取出字符串
	OptionMarket *option_market = new OptionMarket;
	SDS20FUTURE* sds20future = new SDS20FUTURE;
	while (TimerExit())
	{
		{
			lock_guard<mutex> lock{ task_cs };
			if (Tasks.empty())
			{
				this_thread::sleep_for(chrono::milliseconds(100));
				continue;
			}
			temp = Tasks.front();
			Tasks.pop();
		}
		memset(option_market, 0, sizeof(OptionMarket));
		memset(sds20future, 0, sizeof(SDS20FUTURE));
		std::replace(temp.begin(), temp.end(), ' ', '0');
		std::replace(temp.begin(), temp.end(), ',', ' ');
		stringstream stros;
		stros.str(temp);
		stros >> option_market->hqsrc_type;
		stros >> option_market->exchange_type;
		stros >> option_market->option_code;
		stros >> option_market->option_name;
		stros >> option_market->close_price;
		stros >> option_market->open_price;
		stros >> option_market->business_balance;
		stros >> option_market->high_price;
		stros >> option_market->low_price;
		stros >> option_market->last_price;
		stros >> option_market->business_amount;
		stros >> option_market->buy_price1;
		stros >> option_market->buy_price2;
		stros >> option_market->buy_price3;
		stros >> option_market->buy_price4;
		stros >> option_market->buy_price5;
		stros >> option_market->buy_price6;
		stros >> option_market->buy_price7;
		stros >> option_market->buy_price8;
		stros >> option_market->buy_price9;
		stros >> option_market->buy_price10;
		stros >> option_market->sale_price1;
		stros >> option_market->sale_price2;
		stros >> option_market->sale_price3;
		stros >> option_market->sale_price4;
		stros >> option_market->sale_price5;
		stros >> option_market->sale_price6;
		stros >> option_market->sale_price7;
		stros >> option_market->sale_price8;
		stros >> option_market->sale_price9;
		stros >> option_market->sale_price10;
		stros >> option_market->buy_amount1;
		stros >> option_market->buy_amount2;
		stros >> option_market->buy_amount3;
		stros >> option_market->buy_amount4;
		stros >> option_market->buy_amount5;
		stros >> option_market->buy_amount6;
		stros >> option_market->buy_amount7;
		stros >> option_market->buy_amount8;
		stros >> option_market->buy_amount9;
		stros >> option_market->buy_amount10;
		stros >> option_market->sale_amount1;
		stros >> option_market->sale_amount2;
		stros >> option_market->sale_amount3;
		stros >> option_market->sale_amount4;
		stros >> option_market->sale_amount5;
		stros >> option_market->sale_amount6;
		stros >> option_market->sale_amount7;
		stros >> option_market->sale_amount8;
		stros >> option_market->sale_amount9;
		stros >> option_market->sale_amount10;
		stros >> option_market->exchange_index[32];
		stros >> option_market->close_exchange_index;
		stros >> option_market->pre_square_price;
		stros >> option_market->square_price;
		stros >> option_market->auction_price;
		stros >> option_market->auction_amount;
		stros >> option_market->undrop_amount;
		stros >> option_market->optexch_status;
		stros >> option_market->opt_open_restriction;
		stros >> option_market->trading_time;
		option_market->recvus = GetCurTime() * 1000;
		option_market->trading_date = date;
		OptionMarketToSDS20FUTURE(option_market, sds20future);
#ifdef DEBUG
		//写入源字符串
		mylog.append_(temp);
		char c = '\n';
		mylog.append_(c);
		//写入转换为sds20future之后的字符串
		mylog_future.append_more_with_dim(',',
			sds20future->szWindCode,
			sds20future->nActionDay,
			sds20future->nTime,
			sds20future->nStatus,
			sds20future->iPreOpenInterest,
			sds20future->nPreClose,
			sds20future->nPreSettlePrice,
			sds20future->nOpen,
			sds20future->nHigh,
			sds20future->nLow,
			sds20future->nMatch,
			sds20future->iVolume,
			sds20future->iTurnover,
			sds20future->iOpenInterest,
			sds20future->nClose,
			sds20future->nSettlePrice,
			sds20future->nHighLimited,
			sds20future->nLowLimited,
			sds20future->nPreDelta,
			sds20future->nCurrDelta,
			sds20future->nAskPrice[0],
			sds20future->nAskPrice[1],
			sds20future->nAskPrice[2],
			sds20future->nAskPrice[3],
			sds20future->nAskPrice[4],
			sds20future->nAskVol[0],
			sds20future->nAskVol[1],
			sds20future->nAskVol[2],
			sds20future->nAskVol[3],
			sds20future->nAskVol[4],
			sds20future->nBidPrice[0],
			sds20future->nBidPrice[1],
			sds20future->nBidPrice[2],
			sds20future->nBidPrice[3],
			sds20future->nBidPrice[4],
			sds20future->nBidVol[0],
			sds20future->nBidVol[1],
			sds20future->nBidVol[2],
			sds20future->nBidVol[3],
			sds20future->nBidVol[4],
			sds20future->nAuctionPrice,
			sds20future->nAuctionQty,
			sds20future->nAvgPrice,
			sds20future->nRecvTime,
			sds20future->nSn
		);
#endif // DEBUG
		//将option_market写入内存块
		auto ret = shmAppend(handle, (char*)sds20future);
		if (ret < 0)
		{
			delete option_market;
			delete sds20future;
			LOG(ERROR) << "写入内存块出错!" << endl;
			return;
		}
		LOG(INFO) << "写入成功 id = " << ret;
	}
	delete option_market;
	delete sds20future;
	return;
}

#define ROUND(VALUE) static_cast<unsigned int>(((VALUE)*10000.0))
atomic<int> nSn(0);
void OptionMarketToSDS20FUTURE(OptionMarket* option_market, SDS20FUTURE* sds20future)
{
	strcpy(sds20future->szWindCode , option_market->option_code);
	strcat(sds20future->szWindCode, ".SH");
	sds20future->nActionDay = option_market->trading_date;
	sds20future->nTime = option_market->trading_time;//需处理,类型不符合
	sds20future->nStatus = 0;
	sds20future->iPreOpenInterest = 0;
	sds20future->nPreClose = ROUND(option_market->close_price);
	sds20future->nPreSettlePrice = ROUND(option_market->pre_square_price);
	sds20future->nOpen = ROUND(option_market->open_price);
	sds20future->nHigh = ROUND(option_market->high_price);
	sds20future->nLow = ROUND(option_market->low_price);
	sds20future->nMatch = ROUND(option_market->last_price);
	sds20future->iVolume = option_market->business_amount;
	sds20future->iTurnover = (long long)round(option_market->business_balance);//注意
	sds20future->iOpenInterest = 0;
	sds20future->nClose = ROUND(option_market->last_price);
	sds20future->nSettlePrice = ROUND(option_market->square_price);
	sds20future->nHighLimited = ROUND(Subscried.find(atoi(option_market->option_code))->second.opt_up_price);//注意
	sds20future->nLowLimited = ROUND(Subscried.find(atoi(option_market->option_code))->second.opt_down_price);//注意
	sds20future->nPreDelta = 0;
	sds20future->nCurrDelta = 0;
	sds20future->nAskPrice[0] = ROUND(option_market->sale_price1);
	sds20future->nAskPrice[1] = ROUND(option_market->sale_price2);
	sds20future->nAskPrice[2] = ROUND(option_market->sale_price3);
	sds20future->nAskPrice[3] = ROUND(option_market->sale_price4);
	sds20future->nAskPrice[4] = ROUND(option_market->sale_price5);
	sds20future->nAskVol[0] = option_market->sale_amount1;
	sds20future->nAskVol[1] = option_market->sale_amount2;
	sds20future->nAskVol[2] = option_market->sale_amount3;
	sds20future->nAskVol[3] = option_market->sale_amount4;
	sds20future->nAskVol[4] = option_market->sale_amount5;
	sds20future->nBidPrice[0] = ROUND(option_market->buy_price1);
	sds20future->nBidPrice[1] = ROUND(option_market->buy_price2);
	sds20future->nBidPrice[2] = ROUND(option_market->buy_price3);
	sds20future->nBidPrice[3] = ROUND(option_market->buy_price4);
	sds20future->nBidPrice[4] = ROUND(option_market->buy_price5);
	sds20future->nBidVol[0] = option_market->buy_amount1;
	sds20future->nBidVol[1] = option_market->buy_amount2;
	sds20future->nBidVol[2] = option_market->buy_amount3;
	sds20future->nBidVol[3] = option_market->buy_amount4;
	sds20future->nBidVol[4] = option_market->buy_amount5;
	sds20future->nAuctionPrice = ROUND(option_market->auction_price);
	sds20future->nAuctionQty = (int)round(option_market->auction_amount);
	sds20future->nAvgPrice = 0;
	sds20future->nRecvTime = option_market->recvus;
	sds20future->nSn = nSn++;
}



