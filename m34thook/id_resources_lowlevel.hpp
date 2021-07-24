#pragma once
struct idResourceStorageDiskStreamer {};
struct idResourceStorageInterface {};
struct idResourceHeader
{
	char bytes[119];
	char end;
};

struct idResourceSpecialHash
{
	int hashType;
	int reserved;
	__int64 hash;
};
struct idResourceMetaData;
struct idResourceEntryOptions
{
	unsigned __int64 uncompressedSize;
	unsigned __int64 dataCheckSum;
	unsigned __int64 generationTimeStamp;
	unsigned __int64 defaultHash;
	unsigned int version;
	unsigned int flags;
	char compMode;
	char reserved0;
	unsigned __int16 variation;
	unsigned int reserved2;
	unsigned __int64 reservedForVariations;
};

struct idResourceEntry
{
	const char* resourceTypeString;
	const char* nameString;
	const char* descString;
	unsigned int* depIndices;
	char** strings;
	idResourceSpecialHash* specialHashes;
	idResourceMetaData** metaEntries;
	unsigned __int64 dataOffset;
	unsigned __int64 dataSize;
	idResourceEntryOptions options;
	unsigned __int16 numStrings;
	unsigned __int16 numSources;
	unsigned __int16 numDependencies;
	unsigned __int16 numSpecialHashes;
	unsigned __int16 numMetaEntries;
	char field_8A;
	char field_8B;
	int field_8C;
};
enum idResourceDependencyType
{
	RES_DEP_UNKNOWN = 0x0,
	RES_DEP_FILE = 0x1,
	RES_DEP_RESOURCE = 0x2,
	RES_DEP_DECL_SRC = 0x3,
	RES_DEP_CVAR = 0x4,
	RES_DEP_EMBEDDED_RESOURCE = 0x5,
	RES_DEP_CALLBACK = 0x6,
};

struct idResourceDependency
{
	char* type;
	char* name;
	idResourceDependencyType depType;
	unsigned int depSubType;
	unsigned __int64 hashOrTimestamp;
};

struct __declspec(align(8)) resourceStorage_t
{
	idStrDynStatic<256> containerFilename;
	struct idFile* containerFile;
	__int64 timestamp;
	__int64 field_140;
	idResourceHeader field_148;
	idResourceEntry* resourceEntries;
	idHashIndexWithExtra resourceEntryMap;
	idResourceDependency* containerDependencies;
	__int64 field_200;
	__int64 field_208;
	__int64** pqword210;
	__int64 magic;
	char field_220;
};
std::uint64_t checksum_override_data(std::uint8_t* a1, int a2, int a3);