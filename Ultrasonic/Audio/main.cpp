#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include<fstream> 
#include<math.h>

#define WAVE_HEAD_LENGTH 44		//wavͷ�ļ�����
#define m_samplefreq 44100
#define m_channels 2
#define m_channelbits 8
#define MATH_PI 3.1415

#pragma comment(lib,"winmm.lib")
using namespace std;
//.wav�ļ����ļ�ͷ�ṹ 
typedef struct
{
	char chRIFF[4];
	DWORD dwRIFFLen;
	char chWAVE[4];
	char chFMT[4];
	DWORD dwFMTLen;
	PCMWAVEFORMAT pwf;
	char chDATA[4];
	DWORD dwDATALen;//��Ƶ���ݳ���
	//UINT8* pBufer;
}WaveHeader;
void MakeWaveData(int rate, int freq, int amp, char* p, int len,int flag)//�����ʡ�Ƶ�ʡ���������������
{
	int index = 0;
	if (flag == 0)
	{
		for (int i = 0; i < len; i++)
		{
			*(p + index) = sin(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128;//������
			*(p + index + 1) = 128;//����������Ϊ0
			index += 2;
		}
	}
	else if (flag == 1)
	{
		for (int i = 0;i < len;i++)
		{
			*(p + index) = 128;//����������Ϊ0
			*(p + index + 1) = sin(i * (MATH_PI * 2) / rate * freq) * amp * 128 / 100 + 128;//������
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
bool Create(int freq, int volume, int durations,int flag)//Ƶ�ʡ�����������ʱ��
{
	WaveHeader *pHeader = new WaveHeader;
	DWORD totalLen = (m_samplefreq * m_channels * m_channelbits / 8) * durations + 44;	//�ļ��ܳ���=(������ * ͨ���� * ������ / 8) * ����ʱ��(s)
	pHeader->chRIFF[0] = 'R';
	pHeader->chRIFF[1] = 'I';
	pHeader->chRIFF[2] = 'F';
	pHeader->chRIFF[3] = 'F';
	pHeader->dwRIFFLen = totalLen - 8;	//�ļ����ܳ���-8bits

	pHeader->chWAVE[0] = 'W';
	pHeader->chWAVE[1] = 'A';
	pHeader->chWAVE[2] = 'V';
	pHeader->chWAVE[3] = 'E';

	pHeader->chFMT[0] = 'f';
	pHeader->chFMT[1] = 'm';
	pHeader->chFMT[2] = 't';
	pHeader->chFMT[3] = ' ';

	pHeader->dwFMTLen = 0x0010;					//һ�������SizeΪ16�����Ϊ18��������2���ֽڵĸ�����Ϣ
	pHeader->pwf.wf.wFormatTag = 0x0001;		//���뷽ʽ
	pHeader->pwf.wf.nChannels = m_channels;		//1Ϊ��ͨ����2Ϊ˫ͨ��
	pHeader->pwf.wf.nSamplesPerSec = m_samplefreq;	//=44.1KHz
	pHeader->pwf.wf.nAvgBytesPerSec = m_samplefreq * m_channels * m_channelbits / 8;	//ÿ�������ֽ���
	pHeader->pwf.wf.nBlockAlign = m_channels * m_channelbits / 8;		//һ���������ֽ���
	pHeader->pwf.wBitsPerSample = m_channelbits;		//16λ��������PCM�ķ�ʽΪ16λ������(˫ͨ��)

	pHeader->chDATA[0] = 'd';
	pHeader->chDATA[1] = 'a';
	pHeader->chDATA[2] = 't';
	pHeader->chDATA[3] = 'a';
	pHeader->dwDATALen = totalLen - WAVE_HEAD_LENGTH;	//���ݵĳ��ȣ�=�ļ��ܳ���-ͷ����(44bit)

	char *pWaveBuffer = new char[totalLen];		//��Ƶ����
	memcpy(pWaveBuffer, pHeader, WAVE_HEAD_LENGTH);

	MakeWaveData(pHeader->pwf.wf.nSamplesPerSec, freq, volume, pWaveBuffer+ WAVE_HEAD_LENGTH, m_samplefreq*durations,flag);//220500 

	ofstream ocout;
	ocout.open("newWave.wav", ios::out | ios::binary);//�Զ�������ʽ���ļ�
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
	cout << "����Ƶ�ʺ�������0�������� 1�������� 2��˫��������" << endl;
	cin >> freq >> flag;
	if (Create(freq, 100, 10,flag))
		cout << "�����ɹ���" << endl;
	else
		cout << "����ʧ�ܣ�" << endl;
	PlaySound(TEXT("newWave.wav"), NULL, SND_FILENAME | SND_SYNC);
	return 0;
}
