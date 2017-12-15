#ifndef DATA_DEF_H_
#define DATA_DEF_H_
#include <map>
#include <string>
#include <algorithm>
using namespace std;

#pragma  pack(push)
#pragma pack(1)

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef THANDLE
typedef void* THANDLE;
#endif

#ifndef __int64
#define __int64 long long
#endif
//future对应结构
struct SDS20FUTURE
{
	char			szWindCode[32];         //期货IF1510.CF 期权10000034.SH 8位 
	int				nActionDay;             //业务发生日(自然日)
	int				nTime;					//时间(HHMMSSmmm)	
	int				nStatus;				//状态
	__int64			iPreOpenInterest;		//昨持仓
	unsigned int	nPreClose;				//昨收盘价
	unsigned int	nPreSettlePrice;		//昨结算
	unsigned int	nOpen;					//开盘价	
	unsigned int	nHigh;					//最高价
	unsigned int	nLow;					//最低价
	unsigned int	nMatch;					//最新价
	__int64			iVolume;				//成交总量
	__int64			iTurnover;				//成交总金额
	__int64			iOpenInterest;			//持仓总量
	unsigned int	nClose;					//今收盘
	unsigned int	nSettlePrice;			//今结算
	unsigned int	nHighLimited;			//涨停价
	unsigned int	nLowLimited;			//跌停价
	int				nPreDelta;			    //昨虚实度
	int				nCurrDelta;				//今虚实度
	unsigned int	nAskPrice[5];			//申卖价
	unsigned int	nAskVol[5];				//申卖量
	unsigned int	nBidPrice[5];			//申买价
	unsigned int	nBidVol[5];				//申买量
											//Add 20140605
	int	nAuctionPrice;						//波动性中断参考价
	int	nAuctionQty;						//波动性中断集合竞价虚拟匹配量	
	int nAvgPrice;							//郑商所期货均价
	int				nRecvTime;				//收到时间(HHMMSSmmm)
	unsigned long long		nSn;					//收到序号
};


struct OptionCodeInfo
{
    char        exchange_type[4];			//交易类别
    char        option_code[12];			//期权合约编码
    char        option_type;				//期权种类
    char        optcontract_id[32];			//合约交易代码
    char        stock_code[8];				//证券代码
    char        stock_type[4];				//证券类别
    char        stock_name[32];				//证券名称
    char        option_name[32];			//期权名称
    char        money_type[4];				//货币种类
    double      opt_price_step;				//最小价差（元）
	int32_t     amount_per_hand;			//合约乘数
	double		close_price;				//合约前收盘价
	double		opt_last_price;				//期权最新价
	double		opt_close_price;			//合约前收盘价
	double		pre_square_price;			//昨日结算价
	double		opt_up_price;				//上限价
	double		opt_down_price;				//下限价
	double		exercise_price;				//行权价格
	double		initper_balance;			//单位保证金
	int32_t		begin_date;					//起始日期
	int32_t		end_date;					//到期日期
	int32_t		exe_begin_date;				//行权开始日期
	int32_t		exe_end_date;				//行权截至日期
	int32_t		modify_date;				//当前日期
	int32_t		option_version;				//期权合约版本号
	double		limit_high_amount;			//单笔限价申报最高数量
	double		limit_low_amount;			//单笔限价申报最低数量
	int32_t		mkt_high_amount;			//单笔市价申报最高数量
	int32_t		mkt_low_amount;				//单笔市价申报最低数量
	char		optcode_status;				//期权代码状态
	char		opt_open_status;			//期权开仓状态
	char		opt_final_status;			//期权合约临近到期标志
	char		opt_updated_status;			//期权合约调整标志
	char		option_flag;				//期权合约挂牌标志
	char		opt_open_restriction[64];	//期权实时开仓限制
};

struct OptionMarket
{
	char hqsrc_type;				//C1	行情源类型
	char exchange_type[4];			//C4	交易类别
	char option_code[12];			//C8	期权合约编码
	char option_name[32];			//C32	期权合约简称
	double close_price;				//N11.4	昨收盘
	double open_price;				//N16	开盘价
	double business_balance;		//N16.2	成交金额
	double high_price;				//N11.4	最高价
	double low_price;				//N11.4	最低价
	double last_price;				//N16.3	最新价
	int32_t business_amount;		//N10	成交数量
	double buy_price1;				//N11.4	申买价一
	double buy_price2;				//N11.4	申买价二
	double buy_price3;				//N11.4	申买价三
	double buy_price4;				//N11.4	申买价四
	double buy_price5;				//N11.4	申买价五
	double buy_price6	;				//N18.3	申买价六
	double buy_price7	;				//N18.3	申买价七
	double buy_price8	;				//N18.3	申买价八
	double buy_price9	;				//N18.3	申买价九
	double buy_price10	;				//N18.3	申买价十
	double sale_price1;				//N11.4	申卖价一
	double sale_price2;				//N11.4	申卖价二
	double sale_price3;				//N11.4	申卖价三
	double sale_price4;				//N11.4	申卖价四
	double sale_price5;				//N11.4	申卖价五
	double	sale_price6;			//N18.3	申卖价六
	double	sale_price7;			//N18.3	申卖价七
	double	sale_price8;			//N18.3	申卖价八
	double	sale_price9;			//N18.3	申卖价九
	double	sale_price10;			//N18.3	申卖价十
	int32_t buy_amount1;			//N10	申买量一
	int32_t buy_amount2;			//N10	申买量二
	int32_t buy_amount3;			//N10	申买量三
	int32_t buy_amount4;			//N10	申买量四
	int32_t buy_amount5;			//N10	申买量五
	int32_t	buy_amount6;			//N16.2	申买量六
	int32_t	buy_amount7;			//N16.2	申买量七
	int32_t	buy_amount8;			//N16.2	申买量八
	int32_t	buy_amount9;			//N16.2	申买量九
	int32_t buy_amount10;			//N16.2	申买量十
	int32_t sale_amount1;			//N10	申卖量一
	int32_t sale_amount2;			//N10	申卖量二
	int32_t sale_amount3;			//N10	申卖量三
	int32_t sale_amount4;			//N10	申卖量四
	int32_t sale_amount5;			//N10	申卖量五
	int32_t	sale_amount6;			//N16.2	申卖量六
	int32_t	sale_amount7;			//N16.2	申卖量七
	int32_t	sale_amount8;			//N16.2	申卖量八
	int32_t	sale_amount9;			//N16.2	申卖量九
	int32_t	sale_amount10;			//N16.2	申卖量十
	char exchange_index[32];		//C32	交易指数
	char close_exchange_index;		//C1	闭市索引
	double pre_square_price;		//N12.6	昨日结算价
	double square_price;			//N12.6	结算价
	double auction_price;			//N12.6	动态参考价格
	double auction_amount;			//N16.2	虚拟匹配数量
	double undrop_amount;			//N16.2	未平仓数量
	char optexch_status;			//C1	期权交易状态
	char opt_open_restriction[64];	//C64	期权实时开仓限制
	int32_t trading_time;			//交易时间
	int32_t	recvus;					//接收时间
	int	 trading_date;				//交易日期
};
#pragma pack(pop)

class OptionCodeInfoConfig
{
	typedef std::string  FieldName;
	typedef std::string  FieldType;
public:
	OptionCodeInfoConfig()
	{
		memset((void*)&m_data, 0, sizeof(m_data));
		m_conf["exchange_type"]["S"] = m_data.exchange_type;
		m_conf["option_code"]["S"] = m_data.option_code;
		m_conf["option_type"]["C"] = &m_data.option_type;
		m_conf["optcontract_id"]["S"] = m_data.optcontract_id;
		m_conf["stock_code"]["S"] = m_data.stock_code;
		m_conf["stock_type"]["S"] = m_data.stock_type;
		m_conf["stock_name"]["S"] = m_data.stock_name;
		m_conf["option_name"]["S"] = m_data.option_name;
		m_conf["money_type"]["S"] = m_data.money_type;
		m_conf["opt_price_step"]["D"] = &m_data.amount_per_hand;
		m_conf["amount_per_hand"]["I"] = &m_data.amount_per_hand;
		m_conf["close_price"]["D"] = &m_data.close_price;
		m_conf["opt_last_price"]["D"] = &m_data.opt_last_price;
		m_conf["opt_close_price"]["D"] = &m_data.opt_close_price;
		m_conf["pre_square_price"]["D"] = &m_data.pre_square_price;
		m_conf["opt_up_price"]["D"] = &m_data.opt_up_price;
		m_conf["opt_down_price"]["D"] = &m_data.opt_down_price;
		m_conf["exercise_price"]["D"] = &m_data.exercise_price;
		m_conf["initper_balance"]["D"] = &m_data.initper_balance;
		m_conf["begin_date"]["I"] = &m_data.begin_date;
		m_conf["end_date"]["I"] = &m_data.end_date;
		m_conf["exe_begin_date"]["I"] = &m_data.exe_begin_date;
		m_conf["exe_end_date"]["I"] = &m_data.exe_end_date;
		m_conf["modify_date"]["I"] = &m_data.modify_date;
		m_conf["option_version"]["I"] = &m_data.option_version;
		m_conf["limit_high_amount"]["D"] = &m_data.limit_high_amount;
		m_conf["limit_low_amount"]["D"] = &m_data.limit_low_amount;
		m_conf["mkt_high_amount"]["I"] = &m_data.mkt_high_amount;
		m_conf["mkt_low_amount"]["I"] = &m_data.mkt_low_amount;
		m_conf["optcode_status"]["C"] = &m_data.optcode_status;
		m_conf["opt_open_status"]["C"] = &m_data.opt_open_status;
		m_conf["opt_final_status"]["C"] = &m_data.opt_final_status;
		m_conf["opt_updated_status"]["C"] = &m_data.opt_updated_status;
		m_conf["option_flag"]["C"] = &m_data.option_flag;
		m_conf["opt_open_restriction"]["C"] = &m_data.opt_open_restriction;
	}
private:
	OptionCodeInfo								m_data;
	map<FieldName, map<FieldType, void*>>		m_conf;
};

#endif //DATA_DEF_H_

