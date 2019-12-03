#include "slot_partition.h"

bool GetSlotPartition(const std::set<int/*busid*/> &busIDSet, SlotKeyMap &res)
{
	if (busIDSet.empty())
	{
		LOG_ERROR("GetSlotPartition error busIDSet empty");
		return false;
	}

	int div = MAX_VOTE_SLOTKEY / busIDSet.size();
	int mod = MAX_VOTE_SLOTKEY % busIDSet.size();
	int cursor = 0;
	auto iter = busIDSet.begin();
	int endBusID = *iter;
	for (; iter != busIDSet.end(); ++iter)
	{
		for (int i = 0; i < div; ++i)
		{
			res[cursor++] = *iter;
		}
		if (busIDSet.end() != iter)
		{
			endBusID = *iter;
		}
	}

	if (mod > 0)
	{
		for (int i = 0; i < mod; ++i)
		{
			res[cursor++] = endBusID;
		}
	}
	LOG_ERROR("SlotPartition start: \n");
	for (auto iter = res.begin(); iter != res.end(); ++iter)
	{
		LOG_ERROR("SlotPartition %hd -> %d", iter->first, iter->second);
	}
	LOG_ERROR("SlotPartition end: \n");
	return true;
}

uint16_t GetSlotKey(const char *id, const int len)
{
	uint32_t hashValue = m3eMurmurHash32(id, len, VOTE_SLOTKEY_SEED);
	return hashValue % MAX_VOTE_SLOTKEY;
}
