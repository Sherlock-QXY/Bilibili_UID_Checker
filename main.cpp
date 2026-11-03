#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "random_set_len.h"
#include "bili_api.h"
#include "lower_alnum_check.h"
#include "black_list_check.h"
#include "result_writer.h"

#ifdef _WIN32
#include <windows.h>
#endif

//设置每次请求的间隔
#define DELAY_MS 500


using std::cin;
using std::cout;
using std::endl;
using std::string;



const string GREEN = "\033[32m";
const string BLODGREEN = "\033[1;32m";
const string RED = "\033[31m";
const string GRAY = "\033[90m";
const string RESET = "\033[0m";



int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);
    
    // 开启 ANSI 转义支持
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    int uid_len;
    char select;
    string start = ""; 
    bool need_limit = false; 
    
    // 界面打印
    cout << string(90, '=') << endl;
    cout << string(33, '=') << " Bilibili UID 检测工具 " << string(34, '=') << endl;
    cout << string(90, '=') << endl << endl;
	
	
	//////////////输入部分////////////////////////////
	
	
	
	
    // 1. 询问是否限定开头
    cout << "请选择是否需要限定检测的UID开头？(需要输入'y'，不需要输入'n')：";
    while (1)
	{
        cin >> select;
        if (select == 'y' || select == 'Y')
		{
            cout << "-> 已选择：需要限定开头" << endl << endl;
            need_limit = true; // 标记要限定
            
            // 清理缓冲区
            cin.ignore(10000, '\n');
            
            // 循环校验:前缀必须是纯数字
            while (1)
			{
                cout << "请输入需要限定的开头（必须全为数字）：";
                cin >> start;
                
                bool isAllDigits = true;
                for (char c : start)
				{
                    if (c < '0' || c > '9')
					{
                        isAllDigits = false;
                        break;
                    }
                }

                if (start.empty())
				{
                    cout << "输入不能为空，请重新输入！" << endl;
                } else if (!isAllDigits)
				{
                    cout << "输入包含非数字字符，请重新输入！" << endl;
                } else
				{
                    break; // 校验通过，跳出小循环
                }
            }
            break; // 跳出外层 y/n 循环
            
        } 
		else if (select == 'n' || select == 'N')
		{
            cout << "-> 已选择：不需要限定开头" << endl;
            need_limit = false; // 标记不限定
            start = ""; 
            break;
        }
		else
		{
            cout << "输入无效，请重新输入 'y' 或 'n'：" << endl;
        }
    }

    // 2. 输入需要检测的 UID 总长度
    cout << "请输入需要检测的 UID 总长度 (例如 6)：";
    while (!(cin >> uid_len) || uid_len <= 0)
	{
        cout << "输入无效，请输入大于 0 的整数长度：";
        cin.clear(); 
        cin.ignore(10000, '\n'); 
    }

    // 3. 逻辑校验：如果限定了开头，总长度不能小于开头长度
    if (need_limit && uid_len < (int)start.length())
	{
        cout << "错误：检测总长度 (" << uid_len << ") 不能小于限定的开头长度 (" << start.length() << ")！" << endl;
        return 1;
    }

    // 4. 最终打印确认信息
    cout << "\n[当前配置确认]" << endl;
    if (need_limit)
	{
        cout << " -> 限定开头: " << start << endl;
    }
	else
	{
        cout << " -> 限定开头: 无" << endl;
    }
    cout << " -> 检测长度: " << uid_len << endl;
	
	
	
	
	
	///////////////检测部分//////////////////
	
	
	//限定开头
	if(need_limit)
	{
		while(1)
		{
			string else_part=getRandomString(uid_len-(int)start.length());
			string full=start+else_part;
			
			//用api
			BiliUserInfo info = getBilibiliUserInfo(full);
			
			//结果
			if(info.is_success)
			{
				if(isAllLowerAlnumWithAlpha(info.name)&&!containsBlackListWord(info.name)&&info.level==0)
				{// 输出绿色 [pass]，然后恢复默认颜色输出后面的文字
					cout << BLODGREEN << "[PASS]" << RESET << "UID: " << full << "  用户名: " << info.name << " 等级：" << info.level << endl;
					appendBiliUserToResult(full, info.name);
				}
				else
				{
					cout << GRAY << "[SKIP]" << RESET << "UID: " << full << "  用户名: " << info.name << " 等级：" << info.level << endl;
				}
			}
			else
			{
				cout << RED << info.error_msg << RESET << endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		}
	}
	
	//不限开头
	if(!need_limit)
	{
		while(1)
		{
			string full=getRandomString(uid_len);
			
			//用api
			BiliUserInfo info = getBilibiliUserInfo(full);
			
			//结果
			if(info.is_success)
			{
				if(isAllLowerAlnumWithAlpha(info.name)&&!containsBlackListWord(info.name)&&info.level==0)
				{// 输出绿色 [pass]，然后恢复默认颜色输出后面的文字
					cout << BLODGREEN << "[PASS]" << RESET << "UID: " << full << "  用户名: " << info.name << " 等级：" << info.level << endl;
					appendBiliUserToResult(full, info.name);
				}
				else
				{
					cout << GRAY << "[SKIP]" << RESET << "UID: " << full << "  用户名: " << info.name << " 等级：" << info.level << endl;
				}
			}
			else
			{
				cout << RED << info.error_msg << RESET << endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_MS));
		}
	}
    return 0;
}