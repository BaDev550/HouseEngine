#pragma once
#include <functional>
#include <vector>
#include <mutex>

namespace House {
	class RenderCommandQueue
	{
	public:
		using CommandFn = std::function<void()>;
		void Submit(CommandFn command)
		{
			std::scoped_lock<std::mutex> lock(_Mutex);
			_Commands.emplace_back(std::move(command));
		}

		void Execute() {
			std::vector<CommandFn> executionQueue;
			{
				std::scoped_lock<std::mutex> lock(_Mutex);
				executionQueue.swap(_Commands);
				_Commands.clear();
			}
			for (auto& command : executionQueue) {
				command();
			}
		}
	private:
		std::vector<CommandFn> _Commands;
		std::mutex _Mutex;
	};
}