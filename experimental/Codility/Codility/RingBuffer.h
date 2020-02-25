/*
 *	Ring Buffer or Circular Buffer
 *
 *  A data structure for storing information with FIFO (oldest-first) data access
 *  and a constant maximum capacity
 */
#pragma once

#include <iostream>

#include <memory>
#include <mutex>


template <class T>
class RingBuffer
{
public:
	explicit RingBuffer(size_t size) :
		m_buffer(std::unique_ptr<T[]>(new T[size])),
		m_max_size(size)
	{

	}

	void put(T item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_buffer[m_head] = item;

		if (m_isFull)
		{
			m_tail = (m_tail + 1) % m_max_size;
		}

		m_head = (m_head + 1) % m_max_size;

		m_isFull = m_head == m_tail;
	}

	T get()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (empty())
		{
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		auto val = m_buffer[m_tail];
		m_isFull = false;
		m_tail = (m_tail + 1) % m_max_size;

		return val;
	}

	void reset()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_head = m_tail;
		m_isFull = false;
	}

	bool empty() const
	{
		//if head and tail are equal, we are empty
		return (!m_isFull && (m_head == m_tail));
	}

	bool full() const
	{
		//If tail is ahead the head by 1, we are full
		return m_isFull;
	}

	size_t capacity() const
	{
		return m_max_size;
	}

	size_t size() const
	{
		size_t size = m_max_size;

		if (!m_isFull)
		{
			if (m_head >= m_tail)
			{
				size = m_head - m_tail;
			}
			else
			{
				size = m_max_size + m_head - m_tail;
			}
		}

		return size;
	}

private:
	std::mutex m_mutex;
	std::unique_ptr<T[]> m_buffer;
	size_t m_head = 0;
	size_t m_tail = 0;
	const size_t m_max_size;
	bool m_isFull = 0;
};

int main(void)
{
	RingBuffer<uint32_t> circle(10);
	std::cout << "\n === CPP Circular buffer check ===\n";
	std::cout << "Size: %zu, Capacity: %zu\n", circle.size(), circle.capacity();

	uint32_t x = 1;
	std::cout << "Put 1, val: " << x << std::endl;
	circle.put(x);

	x = circle.get();
	std::cout << "Popped: " << x << std::endl;

	std::cout << "Empty: " << circle.empty() << std::endl;

	std::cout << "Adding " << circle.capacity() - 1 << "values\n";
	for (uint32_t i = 0; i < circle.capacity() - 1; i++)
	{
		circle.put(i);
	}

	circle.reset();

	std::cout << "Full: " << circle.full()<< std::endl;

	std::cout << "Adding " << circle.capacity() << "values\n";
	for (uint32_t i = 0; i < circle.capacity(); i++)
	{
		circle.put(i);
	}

	std::cout << "Full: " << circle.full() << std::endl;

	std::cout << "Reading back values: ";
	while (!circle.empty())
	{
		std::cout << circle.get() << " ";
	}
	std::cout << "\n";

	std::cout << "Adding 15 values\n";
	for (uint32_t i = 0; i < circle.size() + 5; i++)
	{
		circle.put(i);
	}

	std::cout << "Full: " << circle.full() << std::endl;

	std::cout << "Reading back values: ";
	while (!circle.empty())
	{
		std::cout << circle.get() << " ");
	}
	std::cout << std::endl;

	std::cout << "Empty: %d\n", circle.empty());
	std::cout << "Full: %d\n", circle.full());

	return 0;
}