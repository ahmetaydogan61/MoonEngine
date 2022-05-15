#pragma once
template <typename T>
struct OrderedStack
{
	T* nodes;
	int m_Size;
	int m_Limit;

	OrderedStack()
		:m_Size(0), m_Limit(2)
	{
		nodes = new T[m_Limit * sizeof(T)];
	}

	OrderedStack(int size)
		:m_Size(0), m_Limit(size)
	{
		nodes = new T[m_Limit * sizeof(T)];
	}

	~OrderedStack()
	{
		delete[] nodes;
		m_Size = 0;
		m_Limit = 2;
	}

	void Push(T value)
	{
		if (m_Size >= m_Limit)
		{
			int newSiz = sizeof(T) * 2 * m_Limit;
			T* temp = new T[newSiz];
			memcpy(temp, nodes, m_Size * sizeof(T));
			temp[m_Size] = value;
			delete[] nodes;
			m_Size++;
			m_Limit *= 2;
			nodes = temp;
			temp = nullptr;
		}
		else
		{
			nodes[m_Size] = value;
			m_Size++;
		}
	}

	void Pop(int index)
	{
		for (int i = 0; i < m_Size; i++)
			if (i >= index)
				nodes[i] = nodes[i + 1];
		m_Size--;
	}

	T& Get(int index)
	{
		return nodes[index];
	}

	int Size()
	{
		return m_Size;
	}
};