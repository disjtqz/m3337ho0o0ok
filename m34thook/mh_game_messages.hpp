#pragma once

#define	__cppobj
class idClientGameMsg {
public:
	void* vtbl;
	unsigned short peerMask;
	unsigned short padding;
	int fromPeer;
};
struct __cppobj idClientGameMsg_Use : public idClientGameMsg
{
	idManagedClassPtr instigator;
	int useMode;
	int padding;
	idManagedClassPtr usedEntity;
	int usableState;
};

struct __cppobj idClientGameMsg_PlayerCommand_Base : public idClientGameMsg
{
	int playerNumber;
	unsigned __int8 unknown;
};

struct __cppobj idClientGameMsg_PlayerCommand_SpawnEntity : public idClientGameMsg_PlayerCommand_Base
{
	idVec3 spawnLocation;
	void* entityDef;
	idManagedClassPtr spawnedEntity;
};
