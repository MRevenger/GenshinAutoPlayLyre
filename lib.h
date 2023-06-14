#pragma once
#include <iostream>
#include <io.h>
#include <dirent.h>
#include <math.h>
#include <fstream>
#include <string>
#include <stack>
#include <time.h>
#include <windows.h>
#include <limits>
#include <vector>
using namespace std;
//����
int selected;
string path;
int waitTick = 100;
bool playing;
vector<string> musics;//�洢�ļ���
DWORD64 beginTick = GetTickCount(), endTick = GetTickCount();
//��������
void init();//��ʼ��
void getFiles();//�����ļ�
void colorPrint(string s, int color);//��ɫ����
void play();//����
string readFile();//��ȡ����
void control();//��ⰴ��
void show();//չʾ
//��������
void init() {
    srand((unsigned)time(NULL));
    selected = 0;
    char temp[PATH_MAX];
    getcwd(temp, sizeof(temp));
    path.assign(temp).append("//music");//��ʼ��·��
    playing = false;
}

void colorPrint(string s, int color = 7) {
    /*
    0 = ��ɫ 8 = ��ɫ
    1 = ��ɫ 9 = ����ɫ
    2 = ��ɫ 10 = ����ɫ
    3 = ǳ��ɫ 11 = ��ǳ��ɫ
    4 = ��ɫ 12 = ����ɫ
    5 = ��ɫ 13 = ����ɫ
    6 = ��ɫ 14 = ����ɫ
    7 = ��ɫ 15 = ����ɫ
    */
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | color );
    cout << s;
    SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | 7);
}

void getFiles() {
    musics.clear();
    musics.shrink_to_fit();
    _finddata_t fileinfo;
    string temp;
    long long hFile = _findfirst(temp.assign(path).append("//*").c_str(), &fileinfo);
    if(hFile == -1) return;
    while(!_findnext(hFile, &fileinfo)) { if(strcmp(fileinfo.name, "..") && strcmp(fileinfo.name, ".")) musics.push_back(fileinfo.name); }
}

void control() {
    if(GetAsyncKeyState(VK_UP) & 0x8000) {
        if(selected >= 3) selected -= 3;
        else {
            int lines = musics.size() / 3;
            if(musics.size() % 3 == 0) selected += musics.size() - 3;
            else if(musics.size() - lines * 3 >= selected + 1) selected += lines * 3;
            else selected += lines * 3 - 3;
        }
        show();
        Sleep(waitTick);
    }
    if(GetAsyncKeyState(VK_LEFT) & 0x8000) {
        selected = (selected - 1 + musics.size()) % musics.size();
        show();
        Sleep(waitTick);
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        selected = (selected + 1) % musics.size();
        show();
        Sleep(waitTick);
    }
    if(GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if(musics.size() - selected - 1 >= 3) selected += 3;
        else selected %= 3;
        show();
        Sleep(waitTick);
    }
    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) exit(0);
    if(GetAsyncKeyState(VK_NUMPAD0) & 0x8000 && !playing) {
        playing = true;
        play();
    }
}
string readFile() {
    ifstream infile;
    string file;
    file = (string)path + "\\" + musics[selected];
    infile.open(file, ios::in);
    if(!infile.is_open()) {
        colorPrint("File not found, please check again!\n", 4);
        return "";
    }
    string buf, res;
    while(getline(infile, buf)) {res += buf + "\n";}
    return res;
}

void play() {
    Sleep(200);
    auto upper = [] (char c)->char {if('a' <= c && c <= 'z') return 'A' + c - 'a'; else return c;};
    auto isalpha = [] (char c)->bool {return 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';};
    auto press = [] (int key)->void {keybd_event(key, 0, 0, 0);};
    auto release = [] (int key)->void {keybd_event(key, 0, KEYEVENTF_KEYUP, 0);};
    auto PressKeys = [upper, press, release] (string s, float interval) {
        for(int i = 0; i < s.size(); ++i) {
            if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                for(int k = 0; k < s.size(); ++k) {
                    release(upper(s[k]));
                }
                playing = false;
                return;
            }
            press(upper(s[i]));
        }
        beginTick = GetTickCount(), endTick = GetTickCount();
        while(endTick - beginTick < interval * 1000) {
            if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                for(int k = 0; k < s.size(); ++k) {
                    release(upper(s[k]));
                }
                playing = false;
                return;
            }
            endTick = GetTickCount();
        }
        for(int k = 0; k < s.size(); ++k) {
            release(upper(s[k]));
        }
    };
    string t = readFile(), temp = "", keys = "", out = "";
    bool flag = false;
    float interval = 0, part;
    int i = 0, count = 0;
    while(t[i] != '\n') {
        temp.push_back(t[i++]);//��ȡ��һ��
    }
    for(int k = 0; k < temp.size(); ++k) {
        if(temp[k] != '.' && !('0' <= temp[k] && temp[k] <= '9')) {
            colorPrint("Error part number, please check again!\n", 4);
            return;
        }
    }
    part = stof(temp);
    system("cls");
    colorPrint("���ڲ���: ", 4);
    colorPrint(musics[selected].substr(0, musics[selected].size() - 4), 6);
    colorPrint("\n����ESC��ֹ����\n", 4);
    for(i; i < t.size(); ++i) {
        if(!playing) {
            system("cls");
            show();
            return;
        }
        if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            for(int k = 0; k < t.size(); ++k) {
                if(isalpha(t[k])) release(t[k]);
            }
            playing = true;
        }
        if(isalpha(t[i])) {
            if(flag) {
                flag = false;
                PressKeys(keys, interval + part * (2 * (float)rand() / RAND_MAX - 1) * 0.025);//��������,ģ�����˵���
                keys = "";
                interval = 0;
                keys.push_back(t[i]);
            }
            else keys.push_back(t[i]);
        }
        else {
            if(t[i] == '+') {
                flag = true;
                interval += part;
            }
            else if(t[i] == '-') {
                flag = true;
                interval += part / 2;
            }
            else if(t[i] == '=') {
                flag = true;
                interval += part / 4;
            }
        }
        if(t[i]!='\n' && t[i] != '\t') {
            out.push_back(t[i]);
            colorPrint(out, 6);
            ++count;
        }
        if(count >= 30) {
            cout << endl;
            count = 0;
        }
        out = "";
    }
    if(!keys.empty()) {
        PressKeys(keys, interval);
    }
    playing = false;
    colorPrint("\n�������", 4);
    show();
}

void show() {
    system("cls");
    getFiles();
    colorPrint("������������ƶ�,�������ּ�0��ʼ����,����ESCֹͣ����\n", 4);
    int n = musics.size();
    for(int i = 0; i < n; ++i) {
        int length = musics[i].length();
        if(i == selected) colorPrint(musics[i].substr(0, length - 4), 6);
        else colorPrint(musics[i].substr(0, length - 4), 7);
        for(int k = 0; k < 34 - musics[i].size(); ++k) {
            cout << " ";
        }
        if((i + 1) % 3 == 0) cout << endl;
    }
}