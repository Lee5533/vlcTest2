// vlcTest2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <thread>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>

#include <WINSOCK2.H>   
#include <stdio.h>     

#define PORT           5150    
#define MSGSIZE        1024    

#pragma comment(lib, "ws2_32.lib")      

#include <Windows.h>  
#include "vlc/vlc.h" 
#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")
//using namespace std;

void test();
//const char* url="";
std::string url = "";

libvlc_instance_t * inst;
libvlc_media_player_t *mp;
libvlc_media_t *m;

libvlc_time_t length;
int width;
int height;
int wait_time = 5000;
int yxj = 0;




std::vector<std::string> split(const std::string &s, const std::string &seperator){
	std::vector<std::string> result;
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size()){
		//找到字符串中首个不等于分隔符的字母；
		int flag = 0;
		while (i != s.size() && flag == 0){
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[i] == seperator[x]){
				++i;
				flag = 0;
				break;
			}
		}

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0){
			for (string_size x = 0; x < seperator.size(); ++x)
			if (s[j] == seperator[x]){
				flag = 1;
				break;
			}
			if (flag == 0)
				++j;
		}
		if (i != j){
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

int main(int argc, char* argv[])
{
	
	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN local;
	SOCKADDR_IN client;
	char szMessage[MSGSIZE];
	
	int ret;
	int iaddrSize = sizeof(SOCKADDR_IN);
	WSAStartup(0x0202, &wsaData);
	

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sListen, (struct sockaddr *) &local, sizeof(SOCKADDR_IN));

	listen(sListen, 1);
	
	sClient = accept(sListen, (struct sockaddr *) &client, &iaddrSize);
	
	printf("Accepted client:%s:%d\n", inet_ntoa(client.sin_addr),
		ntohs(client.sin_port));
	
	while (TRUE) {
		std::string a;
		std::stringstream ss;
		//std::string v = "stop";
		ret = recv(sClient, szMessage, MSGSIZE, 0);
		
		szMessage[ret] ='\0';
		printf("Received [%d bytes]: '%s'\n", ret, szMessage);
		ss << szMessage;//可以是其他数据类型
		ss >> a; //string -> int

		
		std::vector<std::string> v = split(a, "@");
		//std::string control;
		//std::string url1;
		
		//play@rtmp://live.hkstv.hk.lxdns.com/live/hks@1
		if (v[0] == "play"){//paly
			int int_temp;
			std::stringstream stream(v[2]);
			stream >> int_temp;
			
			if (url == ""){
				//播放
				url = v[1];
				printf("Accepted1 url:%s", url);
				yxj = int_temp;
				//url = v[1].c_str();
				std::thread th1(test);
				th1.detach();
			}
			else if (( url != "") && (url != v[1]) && (int_temp>yxj)){
				libvlc_media_player_stop(mp);
				libvlc_media_player_release(mp);
				libvlc_release(inst);
				
				//结束th1线程  重新启动线程				
				//url = v[1].c_str();
				
				/*std::thread th1(test);
				th1.detach();*/
				yxj = int_temp;
				url = v[1];
				printf("Accepted2 url:%s", url);
				inst = libvlc_new(0, NULL);
				const char * urlstr = url.c_str();
				
				m = libvlc_media_new_location(inst, urlstr);				
				mp = libvlc_media_player_new_from_media(m);
				libvlc_media_release(m);
				libvlc_media_player_play(mp);


			}
			
			
			

		}
		else {//stop
			libvlc_media_player_stop(mp);
			// Free the media_player  
			libvlc_media_player_release(mp);
			libvlc_release(inst);

		}
		//if (a.find(v) != std::string::npos){
		//	printf("Received [%d bytes]:");
		//	url = szMessage;
		//	std::thread th1(test);
		//	
		//	th1.detach();
		//	
		//}
		//else if (a == "12"){
		//	//test();
		//	
		//		// Stop playing  
		//		libvlc_media_player_stop(mp);

		//		// Free the media_player  
		//		libvlc_media_player_release(mp);

		//		libvlc_release(inst);
		//}





	}
	return 0;
	
	
}

void test(){

	/*libvlc_instance_t * inst;
	libvlc_media_player_t *mp;
	libvlc_media_t *m;

	libvlc_time_t length;
	int width;
	int height;
	int wait_time = 5000;*/
	

	//libvlc_time_t length;  

	/* Load the VLC engine */
	inst = libvlc_new(0, NULL);
	const char * urlstr = url.c_str();
	//Create a new item  
	//Method 1:  
	//m = libvlc_media_new_location(inst, "rtmp://live.hkstv.hk.lxdns.com/live/hks");
	m = libvlc_media_new_location(inst, urlstr);
	/*url = "";*/
	//m = libvlc_media_new_location(inst, "rtmp://localhost/live/123");
	//Screen Capture  
	//m = libvlc_media_new_location (inst, "screen://");  
	//Method 2:  
	//m = libvlc_media_new_path(inst, "cuc_ieschool.flv");

	/* Create a media player playing environement */
	mp = libvlc_media_player_new_from_media(m);


	/* No need to keep the media now */
	libvlc_media_release(m);

	// play the media_player  
	libvlc_media_player_play(mp);

	//wait until the tracks are created  
	_sleep(wait_time);
	length = libvlc_media_player_get_length(mp);
	width = libvlc_video_get_width(mp);
	height = libvlc_video_get_height(mp);
	printf("Stream Duration: %ds\n", length / 1000);
	printf("Resolution: %d x %d\n", width, height);
	//Let it play   
	_sleep(length - wait_time);



	// Stop playing  
	//libvlc_media_player_stop(mp);

	// Free the media_player  
	//libvlc_media_player_release(mp);

	//libvlc_release(inst);

	//system("pause");
	//return 0;
}
