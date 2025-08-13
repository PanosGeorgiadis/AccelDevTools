// reference: https://blind.guru/simple_cxx11_workqueue.html

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

template <typename Type, typename Queue = std::queue<Type>>
class distributor: Queue, std::mutex, std::condition_variable
{
private:
   typename Queue::size_type capacity;
   bool done = false;
   std::vector<std::thread> threads;

public:
   template<typename Function>
   distributor(Function function,
               unsigned int concurrency = std::thread::hardware_concurrency(),
               typename Queue::size_type max_items_per_thread = 1) :
   capacity { concurrency * max_items_per_thread }
   {
      if (not concurrency)
         throw std::invalid_argument("Concurrency must be non-zero");
      if (not max_items_per_thread)
         throw std::invalid_argument("Max items per thread must be non-zero");

      for (unsigned int count = 0; count < concurrency; count ++)
         threads.emplace_back(static_cast<void (distributor::*)(Function)>
            (&distributor::consume), this, function);
   }

   distributor(distributor &&) = default;
   distributor &operator=(distributor &&) = delete;

   ~distributor()
   {
      {
         std::lock_guard<std::mutex> guard(*this);
         done = true;
         notify_all();
      }
      for (auto&& thread : threads)
         thread.join();
   }

   void operator()(Type &&value)
   {
      std::unique_lock<std::mutex> lock(*this);
      while (Queue::size() == capacity) wait(lock);
      Queue::push(std::forward<Type>(value));
      notify_one();
   }

private:
   template <typename Function>
   void consume(Function process)
   {
      std::unique_lock<std::mutex> lock(*this);
      while (true)
      {
         if (!Queue::empty())
         {
            Type item { std::move(Queue::front()) };
            Queue::pop();
            notify_one();
            lock.unlock();
            process(item);
            lock.lock();
         }
         else if (done)
         {
            break;
         }
         else
         {
            wait(lock);
         }
      }
   }
};

// Client example

#include <iostream>

int main()
{
   typedef std::string item_type;
   distributor<item_type> data_scheduler([](item_type &item)
   {
      std::cout << "MSG : [" << item << "] received" << std::endl;
   });

   std::vector<item_type> input_data = { "Hello", "World" };
   while (data_scheduler.size() < input_data.size())
   {
      data_scheduler(std::move(input_data.back()));
      input_data.pop_back();
   }


   return 0;
}