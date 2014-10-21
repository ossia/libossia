#include <functional>
#include <chrono>
#include <thread>

template<typename Duration = std::chrono::milliseconds>
class Later
{
public:
	template <class callable, class... arguments>
	Later(int after, bool async, callable&& f, arguments&&... args)
	{
		using namespace std;
		auto task(bind(forward<callable>(f), forward<arguments>(args)...));

		if (async)
		{
			thread([after, task]() {
				this_thread::sleep_for(Duration(after));
				task();
			}).detach();
		}
		else
		{
			this_thread::sleep_for(Duration(after));
			task();
		}
	}
};
