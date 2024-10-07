#pragma once


// �����Ҵ�
// 1. ��Ÿ�ӿ� ���� �� ������ ����
// 2. ���α׷� ���� �� �ݵ�� ��ü �޸� ���� �����־�� ��

// ��������
// 1. ��ü �޸� ������ �Ű澲�� �ʾƵ� ��
// 2. ��Ÿ�� ���� ���������ν� ������ų �� ����

template<typename T>
class qSingleton
{
private:
	static T*		g_Inst;

	typedef void (*EXIT)(void);


public:
	static T* GetInst()
	{
		if (nullptr == g_Inst)
		{
			g_Inst = new T;
		}

		return g_Inst;
	}

	static void Destroy()
	{
		if (g_Inst)
		{
			delete g_Inst;
			g_Inst = nullptr;
		}
	}

public:
	qSingleton()
	{
		atexit((EXIT)&qSingleton<T>::Destroy);
	}
	
	qSingleton(const qSingleton& _Other) = delete;
	virtual ~qSingleton() {}
};

template<typename T>
T* qSingleton<T>::g_Inst = nullptr;