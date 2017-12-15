/*******************************************************************************************************

   1、此C++ Demo使用异步发送接收模式；
   2、Demo连接的是恒生仿真测试环境，如要对接券商环境，需要修改t2sdk.ini文件中的IP和许可证文件等；
   3、不同环境的验证信息可能不一样，更换连接环境时，包头字段设置需要确认；
   4、接口字段说明请参考接口文档"恒生统一接入平台_周边接口规范(股票期权).xls"；
   5、T2函数技术说明参考开发文档“T2SDK 外部版开发指南.docx"；
   6、如有UFX接口技术疑问可联系大金融讨论组（261969915）；
   7、optTrade.h&optTrade.cpp用来测试普通交易；
   8、optMD.h&optMD.cpp 用来测试订阅成交回报主推委托主推；
   9、UFX技术支持网站 https://ufx.hscloud.cn/；
   10、demo仅供参考。

*******************************************************************************************************/

#include "optMD.h"
#include"string.h"

extern	map<int64_t, OptionCodeInfo>		Subscried;
extern  queue<std::string>					Tasks;
extern  mutex								task_cs;
unsigned long CMdCallback::QueryInterface(const char *iid, IKnown **ppv)
{
    return 0;
}

unsigned long CMdCallback::AddRef()
{
    return 0;
}

unsigned long CMdCallback::Release()
{
    return 0;
}

// 以下各回调方法的实现仅仅为演示使用
void CMdCallback::OnConnect(CConnectionInterface *lpConnection)
{
    LOG(INFO) << "CMdCallback::OnConnect";
}

void CMdCallback::OnSafeConnect(CConnectionInterface *lpConnection)
{
	LOG(INFO) << "CMdCallback::OnSafeConnect";
}

void CMdCallback::OnRegister(CConnectionInterface *lpConnection)
{
	LOG(INFO) << "CMdCallback::OnRegister";
}

void CMdCallback::OnClose(CConnectionInterface *lpConnection)
{
	LOG(ERROR) << "CMdCallback::OnClose 连接断开!";
	//abort();
	lpReqMode->CloseSelf();
	lpReqMode->InitConn();
	if (0 != lpReqMode->ReqFunction331100())
	{
		LOG(ERROR) << "登陆失败!";
		return ;
	}
	puts("**************************************************************\n");
	LOG(WARNING) << "期权代码个数:" << lpReqMode->GetOptionCode().size();
	puts("**************************************************************\n");
	for (auto &it : lpReqMode->GetOptionCode())
	{
		if (0 != lpReqMode->SubFunction26(ISSUE_TYPE_HQ_SECU, it.second.option_code))
		{
			LOG(ERROR) << "订阅期权行情失败!";
			return ;
		}
	}
	this_thread::sleep_for(chrono::seconds(20));
	puts("**************************************************************\n");
	LOG(WARNING) << "订阅成功的代码数量:" << Subscried.size();
	puts("**************************************************************\n");
}

void CMdCallback::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
}

void CMdCallback::Reserved1(void *a, void *b, void *c, void *d)
{
}

void CMdCallback::Reserved2(void *a, void *b, void *c, void *d)
{
}

void CMdCallback::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
	
}
//异步回调接受应答，主推包函数
void CMdCallback::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{  
	int issue_type=0;
	if (lpMsg!=NULL)
	{
		//成功,应用程序不能释放lpBizMessageRecv消息
		if (lpMsg->GetReturnCode() ==0)
		{
			switch(lpMsg->GetFunction())
			{
				case 331100:
				{
					int iLen = 0;
					const void * lpBuffer = lpMsg->GetContent(iLen);
				    IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
					if (lpUnPacker){
						lpUnPacker->AddRef();
						show_packer(lpUnPacker);
					    OnResponse_331100(lpUnPacker);
						lpUnPacker->Release();
					}
					break;
				}	
				case MSGCENTER_FUNC_OPTION:
				{
					extern std::atomic<bool> ready;
					int iLen = 0;
					const void * lpBuffer = lpMsg->GetContent(iLen);
					IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
					if (lpUnPacker){
						lpUnPacker->AddRef();
						//show_packer(lpUnPacker);
						this->lpUnPacker2OpVec(lpUnPacker);
						lpUnPacker->Release();
					}
					ready = true;
					break;
				}
				case MSGCENTER_FUNC_HEART:
					if (lpMsg->GetPacketType() == REQUEST_PACKET)
						LOG(WARNING)<<"heartBeat.."<<endl;
						lpReqMode->OnHeartbeat(lpMsg);
					break;
				case MSGCENTER_FUNC_REG:
				{
					int iLen_ley = 0;
					const void * lpBuffer_key = lpMsg->GetKeyInfo(iLen_ley);
					IF2UnPacker * lpUnPacker_key = NewUnPacker((void *)lpBuffer_key, iLen_ley);
					if (lpUnPacker_key){
						lpUnPacker_key->AddRef();
						if (lpUnPacker_key->GetInt("error_no") != 0){
							LOG(INFO)<< lpUnPacker_key->GetStr("error_info");
						}
						else{
							lpUnPacker_key->SetCurrentDatasetByIndex(0);
							int64_t option_code = atoll(lpUnPacker_key->GetStr("option_code"));
							Subscried.emplace(option_code, lpReqMode->GetOptionCode()[option_code]);
							puts("订阅成功\n");
						}
						lpUnPacker_key->Release();
					}	
					break;
				}
				case MSGCENTER_FUNC_REG_CANCLE:
					break;
				case MSGCENTER_FUNC_SENDED:
				{
					issue_type = lpMsg->GetIssueType();
					if (issue_type == ISSUE_TYPE_HQ_SECU)
					{
						int iLen = 0;
						const void * lpBuffer = lpMsg->GetContent(iLen);
						IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
						if (lpUnPacker){
							lpUnPacker->AddRef();
							string optqh = lpUnPacker->GetStr("OPTHQ");
							string tep = replace_all(optqh, "\001", ",");
							{
								lock_guard<mutex>	lock(task_cs);
								Tasks.emplace(tep);
							}
							LOG(WARNING) << tep;
							lpUnPacker->Release();
						}							  
					}
					break;
				 }
			  }
		   }
           else
		   {
				int iLen = 0;
				const void * lpBuffer = lpMsg->GetContent(iLen);
				IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
				if (lpUnPacker != NULL)
				{
					lpUnPacker->AddRef();//添加释放内存引用
					show_packer(lpUnPacker);
					lpUnPacker->Release();
				}
				else
				{
					printf("业务包是空包，错误代码：%d，错误信息:%s\n", lpMsg->GetErrorNo(), lpMsg->GetErrorInfo());
				}
			}	
      }	
}

void CMdCallback::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{

}

int  CMdCallback::Reserved3()
{
    return 0;
}

void CMdCallback::Reserved4()
{
}

void CMdCallback::Reserved5()
{
}

void CMdCallback::Reserved6()
{
}

void CMdCallback::Reserved7()
{
}

void CMdCallback::SetRequestMode(CMdRequestMode* lpMode)
{
	lpReqMode = lpMode; 
}

void CMdCallback::OnResponse_331100(IF2UnPacker *lpUnPacker)
{
	int iSystemNo=-1;
	LOG(INFO) << "CTradeCallBack::331100";
	if(lpUnPacker->GetStr("client_id") != nullptr)
	{
		const char *pClientId = lpUnPacker->GetStr("client_id");
		strcpy(lpReqMode->m_client_id, pClientId);
		LOG(INFO) << "clientId:" << lpReqMode->m_client_id;
	}
	if(lpUnPacker->GetStr("user_token") != nullptr)
	{
		lpReqMode->m_opUserToken = lpUnPacker->GetStr("user_token");
	}
	LOG(INFO) <<"UserToken:"<<lpUnPacker->GetStr("user_token")<<endl;
	if (lpUnPacker->GetStr("branch_no") != nullptr)
	{
		lpReqMode->m_BranchNo = lpUnPacker->GetInt("branch_no");
	}	
	LOG(INFO) <<"BranchNo:"<<lpUnPacker->GetStr("branch_no")<<endl;
	if (lpUnPacker->GetInt("sysnode_id") != 0)
	{
		iSystemNo = lpUnPacker->GetInt("sysnode_id");
		LOG(INFO) << "sysnode_id :" << iSystemNo;
	}
	if (lpUnPacker->GetInt("op_branch_no") != 0)
	{
		lpReqMode->m_op_branch_no = lpUnPacker->GetInt("op_branch_no");
	}		
	LOG(INFO) <<"op_branch_no:"<<lpReqMode->m_op_branch_no<<endl;
	return;
}

#define  STRCPY(DES,SOUC) SOUC==nullptr?memset(DES,0,sizeof(DES)):strncpy(DES,SOUC,sizeof(DES))
void CMdCallback::lpUnPacker2OpVec(IF2UnPacker * lpUnPacker)
{
	for (decltype(lpUnPacker->GetDatasetCount()) i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
	{
		lpUnPacker->SetCurrentDatasetByIndex(i);
		for (decltype(lpUnPacker->GetRowCount())j = 0; j < lpUnPacker->GetRowCount(); ++j)
		{
			OptionCodeInfo   temp_option_info;
			memset((void*)&temp_option_info, 0, sizeof(temp_option_info));
			STRCPY(temp_option_info.exchange_type, lpUnPacker->GetStr("exchange_type"));
			STRCPY(temp_option_info.option_code, lpUnPacker->GetStr("option_code"));
			temp_option_info.option_type = lpUnPacker->GetChar("option_type");
			STRCPY(temp_option_info.optcontract_id, lpUnPacker->GetStr("optcontract_id"));
			STRCPY(temp_option_info.stock_code, lpUnPacker->GetStr("stock_code"));
			STRCPY(temp_option_info.stock_type, lpUnPacker->GetStr("stock_type"));
			STRCPY(temp_option_info.stock_name, lpUnPacker->GetStr("stock_name"));
			STRCPY(temp_option_info.option_name, lpUnPacker->GetStr("option_name"));
			STRCPY(temp_option_info.money_type, lpUnPacker->GetStr("money_type"));
			temp_option_info.opt_price_step = lpUnPacker->GetDouble("opt_price_step");
			temp_option_info.amount_per_hand = lpUnPacker->GetInt("amount_per_hand");
			temp_option_info.close_price = lpUnPacker->GetDouble("close_price");
			temp_option_info.opt_last_price = lpUnPacker->GetDouble("opt_last_price");
			temp_option_info.opt_close_price = lpUnPacker->GetDouble("opt_close_price");
			temp_option_info.pre_square_price = lpUnPacker->GetDouble("pre_square_price");
			temp_option_info.opt_up_price = lpUnPacker->GetDouble("opt_up_price");
			temp_option_info.opt_down_price = lpUnPacker->GetDouble("opt_down_price");
			temp_option_info.exercise_price = lpUnPacker->GetDouble("exercise_price");
			temp_option_info.initper_balance = lpUnPacker->GetDouble("initper_balance");
			temp_option_info.begin_date = lpUnPacker->GetInt("begin_date");
			temp_option_info.end_date = lpUnPacker->GetInt("end_date");
			temp_option_info.exe_begin_date = lpUnPacker->GetInt("exe_begin_date");
			temp_option_info.exe_end_date = lpUnPacker->GetInt("exe_end_date");
			temp_option_info.modify_date = lpUnPacker->GetInt("modify_date");
			temp_option_info.option_version = lpUnPacker->GetInt("option_version");
			temp_option_info.limit_high_amount = lpUnPacker->GetDouble("limit_high_amount");
			temp_option_info.limit_low_amount = lpUnPacker->GetDouble("limit_low_amount");
			temp_option_info.mkt_high_amount = lpUnPacker->GetInt("mkt_high_amount");
			temp_option_info.mkt_low_amount = lpUnPacker->GetInt("mkt_low_amount");
			temp_option_info.optcode_status = lpUnPacker->GetChar("optcode_status");
			temp_option_info.opt_open_status = lpUnPacker->GetChar("opt_open_status");
			temp_option_info.opt_final_status = lpUnPacker->GetChar("opt_final_status");
			temp_option_info.opt_updated_status = lpUnPacker->GetChar("opt_updated_status");
			temp_option_info.option_flag = lpUnPacker->GetChar("option_flag");
			STRCPY(temp_option_info.opt_open_restriction, lpUnPacker->GetStr("opt_open_restriction"));
			this->lpReqMode->GetOptionCode().emplace(atoi(temp_option_info.option_code),temp_option_info);
			lpUnPacker->Next();
		}
	}
}

int CMdRequestMode::InitConn()
{   
	//加载配置文件登陆 
	lpConfig->Load("t2sdk.ini");
	const char *p_fund_account = lpConfig->GetString("ufx", "fund_account", "");//从配置中获取资金账号
	const char *p_password = lpConfig->GetString("ufx", "password", "");//从配置中获取登录密码
	strcpy(m_opAccountName,p_fund_account);
	strcpy(m_opPassword,p_password);
	m_opEntrustWay='7';
	m_opStation="IPMAC";

	int iRet = 0;
	if(lpConnection != NULL)
	{
		lpConnection->Release();
		lpConnection = NULL;
	}
	
	lpConnection = NewConnection(lpConfig);
	LOG(WARNING)<<"Connected successfully"<<endl;
	lpConnection->AddRef();
	if (0 != (iRet = lpConnection->Create2BizMsg(&callback)))
    {
		LOG(ERROR)<<"初始化失败.iRet="<<iRet<<" msg:"<<lpConnection->GetErrorMsg(iRet)<<endl;
		return -1;
	}
	if (0 != (iRet = lpConnection->Connect(5000)))
	{
		LOG(ERROR)<<"连接.iRet="<<iRet<<" msg:"<<lpConnection->GetErrorMsg(iRet)<<endl;
		return -1;
	}
	return 0;
}

unsigned long CMdRequestMode::Release()
{
	delete this;
	return 0;
};

void CMdRequestMode::OnHeartbeat(IBizMessage* lpMsg)
{
	lpMsg->ChangeReq2AnsMessage();
	lpConnection->SendBizMsg(lpMsg, 1);
	return;
}

int CMdRequestMode::ReqFunction331100()
{
	LOG(INFO)<<"客户登陆331100入参:"<<endl;
	IBizMessage* lpBizMessage=NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(331100);
	lpBizMessage->SetPacketType(REQUEST_PACKET);
	IF2Packer *pPacker=NewPacker(2);
	if (!pPacker)
	{
		LOG(ERROR) <<"取打包器失败！\r\n";
		return -1;
	}
	pPacker->AddRef();
	pPacker->BeginPack();

	///加入字段名
	pPacker->AddField("op_branch_no", 'I', 5);//操作分支机构
	pPacker->AddField("op_entrust_way", 'C', 1);//委托方式 
	pPacker->AddField("op_station", 'S', 255);//站点地址
	pPacker->AddField("branch_no", 'I', 5);     
	pPacker->AddField("input_content", 'C'); 
	pPacker->AddField("account_content", 'S', 30);
	pPacker->AddField("content_type", 'S', 6);  
	pPacker->AddField("password", 'S', 10);      
	pPacker->AddField("password_type", 'C');   
	///加入对应的字段值
	pPacker->AddInt(m_op_branch_no);						
	pPacker->AddChar(m_opEntrustWay);				
	pPacker->AddStr(m_opStation.c_str());				
	pPacker->AddInt(1);			
	pPacker->AddChar('1');					
	pPacker->AddStr(m_opAccountName);			
	pPacker->AddStr("0");	
	pPacker->AddStr(m_opPassword);			
	pPacker->AddChar(/*'2'*/'2');	
	
	///结束打包
	pPacker->EndPack();

	lpBizMessage->SetContent(pPacker->GetPackBuf(),pPacker->GetPackLen());
	//打印入参信息
	IF2UnPacker * lpUnPacker = NewUnPacker(pPacker->GetPackBuf(),pPacker->GetPackLen());
	lpUnPacker->AddRef();
	show_packer(lpUnPacker);
	lpUnPacker->Release();
	//发送请求包
    lpConnection->SendBizMsg(lpBizMessage, 1);
    pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	lpBizMessage->Release();
	return 0;
}

int CMdRequestMode::SubFunction26(int issue_type, char* op_code)
{
	 //int iRet = 0, hSend = 0;
	 //IF2UnPacker *pMCUnPacker = NULL;
	
	///获取版本为2类型的pack打包器
	IF2Packer *pPacker = NewPacker(2);
	if(!pPacker)
	{
		LOG(ERROR) << "取打包器失败!\r\n";
		return -1;
	}
	pPacker->AddRef();

	///定义解包器
	//IF2UnPacker *pUnPacker = NULL;
	
	IBizMessage* lpBizMessage = NewBizMessage();

	lpBizMessage->AddRef();
	
	///应答业务消息
	//IBizMessage* lpBizMessageRecv = NULL;
	//功能号
	lpBizMessage->SetFunction(MSGCENTER_FUNC_REG);
	//请求类型
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	///开始打包
	pPacker->BeginPack();
	
	pPacker->AddField("exchange_type",'S',4);
	pPacker->AddField("option_code",'S',8);
	pPacker->AddField("issue_type",'I',8);
	//一个字段可以打入多个值进去实现多只期权行情的订阅；

	//一个字段可以打入多个值进去实现多只期权行情的订阅；
	pPacker->AddStr("1");
	pPacker->AddStr(op_code);
	pPacker->AddInt(issue_type);

	///结束打包
	pPacker->EndPack();
	lpBizMessage->SetKeyInfo(pPacker->GetPackBuf(),pPacker->GetPackLen());
	lpConnection->SendBizMsg(lpBizMessage,1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	lpBizMessage->Release();	
	return 0;
}

int CMdRequestMode::ReqFunction338000()
{
	
	IBizMessage *lpBizMessage = NewBizMessage();
	lpBizMessage->AddRef();
	lpBizMessage->SetFunction(MSGCENTER_FUNC_OPTION);
	lpBizMessage->SetPacketType(REQUEST_PACKET);

	IF2Packer *pPacker = NewPacker(2);
	if (!pPacker)
	{
		LOG(ERROR) <<"取打包器失败！\r\n";
		return -1;
	}
	//加入字段名
	pPacker->AddRef();
	pPacker->BeginPack();
	pPacker->EndPack();

	lpBizMessage->SetContent(pPacker->GetPackBuf(), pPacker->GetPackLen());
	IF2UnPacker * lpUnPacker = NewUnPacker(pPacker->GetPackBuf(), pPacker->GetPackLen());
	show_packer(lpUnPacker);
	lpConnection->SendBizMsg(lpBizMessage, 1);
	pPacker->FreeMem(pPacker->GetPackBuf());
	pPacker->Release();
	lpBizMessage->Release();
	return 0;
}

map<int64_t, OptionCodeInfo>& CMdRequestMode::GetOptionCode()
{
	return m_option_code;
}

string& replace_all(string&   str, const   string&   old_value, const   string&   new_value)
{
	while (true) {
		string::size_type   pos(0);
		if ((pos = str.find(old_value)) != string::npos)
		{
			str.replace(pos, old_value.length(), new_value);
		}
		else
			break;
	}
	return   str;
}