#ifndef __OPTIONTOSDS_H__
#define __OPTIONTOSDS_H__
#include "OptionToSds.h"
#include "optMD.h"

int InitShmOption(char* shm_name, const int main_shm, char *app_name, int& handle);

int InitConnAndSubscribe(CMdRequestMode* lp_OptionMdMode);

void OpthqToOptionMarket(int& handle);

void OptionMarketToSDS20FUTURE(OptionMarket* option_market, SDS20FUTURE* sds20future);

inline bool TimerExit()
{
	return GetCurTime() <= 150500;
}

class MyLog
{
public:
	MyLog(const char* filename,int timeus) :m_filename(filename) {
		if (!m_fout.is_open())
		{
			stringstream str;
			str << m_filename;
			str << timeus;
			str << ".log";
			m_fout.open(str.str());
		}
	}
	~MyLog()
	{
		if (m_fout.is_open())
		{
			m_fout.flush();
			m_fout.close();
		}
	}

	template<typename T>
	MyLog& append_(T& value)
	{
		m_fout << value << ",";
		return *this;
	}
	inline void append_more_with_dim(char dim)
	{
		m_fout << endl;
	}

	template<typename T, typename...ARGS>
	inline void append_more_with_dim(char dim, T& val, ARGS&&...args)
	{
		m_fout << val << dim;
		append_more_with_dim(dim,args...);
	}
	void close()
	{
		m_fout.close();
	}
protected:
private:
	ofstream	m_fout;
	string		m_filename;
};

#endif//!__OPTIONTOSDS_H__


