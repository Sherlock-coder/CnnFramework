#pragma once
namespace Ros{
	template<typename... Args>
	void getList(std::vector<uint32_t>& inList, uint32_t head, Args... args)
	{
		inList.push_back(head);
		getList(inList, args...);
	}
	template<>
	void getList(std::vector<uint32_t>& inList, uint32_t head) { inList.push_back(head); }
}