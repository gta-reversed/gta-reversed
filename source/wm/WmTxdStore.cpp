#include "WmTxdStore.h"
#include "WmCompat.h"

void(__cdecl *WmTxdStore::PopCurrentTxd)();
void(__cdecl *WmTxdStore::RemoveTxdSlot)(int slot);

void WmTxdStore::Init10U()
{
	PopCurrentTxd = injector::raw_ptr(0x7316B0).get();
	RemoveTxdSlot = injector::raw_ptr(0x731CD0).get();
}

void WmTxdStore::InitSteam()
{
	PopCurrentTxd = injector::aslr_ptr(0x765CE0).get();
	RemoveTxdSlot = injector::aslr_ptr(0x766310).get();
}
