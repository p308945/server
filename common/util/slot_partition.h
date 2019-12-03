#ifndef _SLOT_PARTITION_H_
#define _SLOT_PARTITION_H_

#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include "util/Log.h"
#include "util/teaencrypt/M3eHashing.h"

const int MAX_VOTE_SLOTKEY = 2053;
const uint32_t VOTE_SLOTKEY_SEED = 6;

typedef std::map<uint16_t/*slotkey*/, int /*busid*/> SlotKeyMap;

bool GetSlotPartition(const std::set<int/*busid*/> &busIDSet, SlotKeyMap &res);

uint16_t GetSlotKey(const char *id, const int len);

#endif
