// 1512651_SocketCuoiky.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "afxsock.h"
#include "1512651_SocketCuoiky.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

#define HOST "students.iitk.ac.in"
#define PAGE "/programmingclub/course/lectures/"
#define PORT 80
#define USERAGENT "HTMLGET 1.0"

string get_ip(string host)
{
	struct hostent *hent;
	int ip_length = 15;
	char* ip = (char *)malloc(ip_length + 1);
	memset(ip, 0, ip_length + 1);
	if ((hent = gethostbyname(host.c_str())) == NULL)
	{
		printf("\nCan't get IP");
		exit(1);
	}

	if (!inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, ip_length))
	{
		cout << "Cannot resolve host" << endl;
		exit(1);
	}
	return (string)ip;

}
//html include href to test
string const html = R"(
<td><a href="4.%20Functions,%20scope.ppt">4. Functions, scope.ppt</a></td>
<td><a href="4.%20Functions,%20scope.ppt">4. Functions, scope.ppt</a></td>
<td><a href="4.%20Functions,%20scope.ppt">4. Functions, scope.ppt</a></td>
)";

void get_name_file(string html, vector <string> &list_file_name)
{
	const auto fast_n_loose = regex_constants::optimize | regex_constants::icase;
	//icase: without case
	//optimize: priority matching efficient


	// extract href's
	regex const link{ R"~(href=(["'])(.*?)\1)~", fast_n_loose };
	sregex_iterator itr_end;
	sregex_iterator itr{ begin(html), end(html), link };

	// iterate through the matches
	for (; itr != itr_end; itr++)
	{
		string first_ele = itr->str(2).substr(0, 1);//get first ele of string
		if (first_ele == "?")
		{
			continue;
		}
		list_file_name.push_back(itr->str(2));// i = itr->str(2)
		//tenvt.push_back(phantuthui)
		cout << itr->str(2) << '\n';
	}
	//delete wrong href file

}

void usage()
{
	cout << "\nHost: students.iitk.ac.in ";
	cout <<"\nPage: /programmingclub/course/lectures/";
}


string get_message_header(string host, string page)
{
	const char *Host = host.c_str();
	const char *Page = page.c_str();
	const char *mess_header = "GET %s HTTP/1.0 \nHost: %s \nUser-Agent: %s\r\n\r\n";
	const char *getpage = Page;

	//if (getpage[0] == '/') {
	//	getpage = getpage + 1;
	//	fprintf(stderr, "Removing leading \"/\", converting %s to %s\n", page, getpage);
	//}
	char *query = (char *)malloc(strlen(Host) + strlen(getpage) + strlen(USERAGENT) + strlen(mess_header) - 5);
	//3 tham số đầu tiên là thông tin, 3 cái sau là các component %s
	sprintf_s(query, strlen(query) + 1, mess_header, getpage, HOST, USERAGENT);
	return (string)query;
}

wstring s2ws(const string& s);
wstring s2ws(const string& s)
{
 	int len;
 	int slength = (int)s.length() + 1;
 	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
 	wchar_t* buf = new wchar_t[len];
 	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 	wstring r(buf);
 	delete[] buf;
 	return r;
}

string get_src_page(string host, string page,string& srcPage)
{
	 if (!AfxSocketInit(NULL))
    {
        cout << "Cannot create socket!" << endl;
        return FALSE;
    }
    int tmpres = 0;
    char buf[BUFSIZ + 1];
    string query = "";
 
    //tạo socket
    CSocket client;
    client.Create();
 
    //lấy địa chỉ ip của host
    string ip = get_ip(host);
    if (client.Connect(s2ws(ip).c_str(), PORT) < 0)
    {
        cout << "\nCannot connect!";
        exit(1);
    }
    //quẻy get file
    query = get_message_header(HOST, PAGE);
    cout <<"\nHeader message: "<< query;
 
    //sent content through socket
	send(client, query.c_str(), query.length(), 0);

	memset(buf, 0, sizeof(buf));

	srcPage = "";

	//receive from socket
	while ((tmpres = client.Receive(buf, BUFSIZ, 0)) > 0)
	{
		srcPage.append(buf, tmpres);
		memset(buf, 0, sizeof(buf));
		if (buf)
		{
			fprintf(stdout, buf);
		}
		memset(buf, 0, tmpres);
		tmpres = client.Receive(buf, BUFSIZ, 0);
	}

	if (tmpres < 0)
	{
		cout << "Error receiving data";
	}
	client.Close();
	return srcPage;
}


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
			//string ip = get_ip(HOST);//TEST GET_IP
			//vector <string> listfile;
			//get_name_file(html, listfile);//TEST_GET_NAME_FILE

			//tham số dòng lệnh
			/*if (argc == 1)
			{
			usage();
			exit(2);
			}
			host = argv[1];
			if (argc > 2)
			{
			page = argv[2];
			}
			else
			{
			host = HOST;
			usage();
			page = PAGE;
			}
			*/
			string srcPage;
			string s = get_src_page(HOST, PAGE, srcPage);
			cout << s;
			string ip = get_ip(HOST);//TEST GET_IP
			cout << endl;
			vector <string> listfile;
			get_name_file(srcPage, listfile);//TEST_GET_NAME_FILE
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }
	system("pause");
    return nRetCode;
}
