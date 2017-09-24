#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <time.h>


using namespace std;

class Log
	{
		private:
			std::streambuf* clog_save;
			std::ofstream ofs;
		public:
			Log(const std::string &LogFilename)
			{

				clog_save = std::clog.rdbuf();
				ofs.open(LogFilename.c_str(), ios_base::app);
				std::clog.rdbuf(ofs.rdbuf());

				//clog<<endl<<endl;
				//time_t now = time((time_t*)NULL);
				//struct tm *l_time = localtime(&now);
				//char * cptime;
				//sprintf(cptime, "[%.2d:%.2d:%.2d]", l_time->tm_hour, l_time->tm_min, l_time->tm_sec);
				//clog<<cptime<<endl;

			}

			~Log()
			{
				//reset the buffer
				std::clog.rdbuf(clog_save);
				ofs.close();
			}

	};
#endif
