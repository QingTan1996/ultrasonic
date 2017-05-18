#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include<fstream> 
#include<math.h>

#define WAVE_HEAD_LENGTH 44		//wav头文件长度
#define m_samplefreq 44100
#define m_channels 2
#define m_channelbits 8
#define MATH_PI 3.1415

#pragma comment(lib,"winmm.lib")
using namespace std;
//.wav文件的文件头结构 
typedef struct
{
	char chRIFF[4];
	DWORD dwRIFFLen;
	char chWAVE[4];
	char chFMT[4];
	DWORD dwFMTLen;
	PCMWAVEFORMAT pwf;
	char chDATA[4];
	DWORD dwDATALen;//音频数据长度
	//UINT8* pBufer;
}WaveHeader;
void MakeWaveData(int rate, int freq, int amp, char* p, int len,int flag)//采样率、频率、音量、采样点数
{
	int index = 0;
	if (flag == 0)
	{
		for (int i = 0; i < len; i++)
		{
			*(p + index) = sin(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128;//左声道
			*(p + index + 1) = 128;//右声道音量为0
			index += 2;
		}
	}
	else if (flag == 1)
	{
		for (int i = 0;i < len;i++)
		{
			*(p + index) = 128;//左声道音量为0
			*(p + index + 1) = sin(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128;//右声道
			index += 2;
		}
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			*(p + index) = sin(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128;
			*(p + index + 1)= sin((i+5) * (MATH_PI * 2) / rate * freq) * amp * 128 / 100+128;
			index += 2;
		}
	}
}
bool Create(int freq, int volume, int durations,int flag)//频率、音量、持续时间
{
	WaveHeader *pHeader = new WaveHeader;
	DWORD totalLen = (m_samplefreq * m_channels * m_channelbits / 8) * durations + 44;	//文件总长度=(采样率 * 通道数 * 比特数 / 8) * 持续时间(s)
	pHeader->chRIFF[0] = 'R';
	pHeader->chRIFF[1] = 'I';
	pHeader->chRIFF[2] = 'F';
	pHeader->chRIFF[3] = 'F';
	pHeader->dwRIFFLen = totalLen - 8;	//文件的总长度-8bits

	pHeader->chWAVE[0] = 'W';
	pHeader->chWAVE[1] = 'A';
	pHeader->chWAVE[2] = 'V';
	pHeader->chWAVE[3] = 'E';

	pHeader->chFMT[0] = 'f';
	pHeader->chFMT[1] = 'm';
	pHeader->chFMT[2] = 't';
	pHeader->chFMT[3] = ' ';

	pHeader->dwFMTLen = 0x0010;					//一般情况下Size为16，如果为18则最后多了2个字节的附加信息
	pHeader->pwf.wf.wFormatTag = 0x0001;		//编码方式
	pHeader->pwf.wf.nChannels = m_channels;		//1为单通道，2为双通道
	pHeader->pwf.wf.nSamplesPerSec = m_samplefreq;	//=44.1KHz
	pHeader->pwf.wf.nAvgBytesPerSec = m_samplefreq * m_channels * m_channelbits / 8;	//每秒所需字节数
	pHeader->pwf.wf.nBlockAlign = m_channels * m_channelbits / 8;		//一个采样的字节数
	pHeader->pwf.wBitsPerSample = m_channelbits;		//16位，即设置PCM的方式为16位立体声(双通道)

	pHeader->chDATA[0] = 'd';
	pHeader->chDATA[1] = 'a';
	pHeader->chDATA[2] = 't';
	pHeader->chDATA[3] = 'a';
	pHeader->dwDATALen = totalLen - WAVE_HEAD_LENGTH;	//数据的长度，=文件总长度-头长度(44bit)

	char *pWaveBuffer = new char[totalLen];		//音频数据
	memcpy(pWaveBuffer, pHeader, WAVE_HEAD_LENGTH);

	MakeWaveData(pHeader->pwf.wf.nSamplesPerSec, freq, volume, pWaveBuffer+ WAVE_HEAD_LENGTH, m_samplefreq*durations,flag);//220500 

	ofstream ocout;
	ocout.open("newWave.wav", ios::out | ios::binary);//以二进制形式打开文件
	if (ocout)
		ocout.write(pWaveBuffer, totalLen);
	else
		return false;
	ocout.close();

	delete(pHeader);
	return true;
}
int main()
{
	int freq,flag;
	cout << "输入频率和声道（0：左声道 1：右声道 2：双声道）：" << endl;
	cin >> freq >> flag;
	if (Create(freq, 100, 10,flag))
		cout << "创建成功！" << endl;
	else
		cout << "创建失败！" << endl;
	PlaySound(TEXT("newWave.wav"), NULL, SND_FILENAME | SND_SYNC);
	return 0;
}
