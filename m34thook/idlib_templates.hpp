#pragma once


/*
	recreating the most frequently used idlib templates here
	data only, no code
*/
template<typename T, memTag_t tag, bool probablystatic>
struct __cppobj idList
{
	T* list;
	int num;
	int size;
	__int16 granularity;
	unsigned __int8 memTag;
	unsigned __int8 listStatic;
};
template<size_t N, bool canbdynamic = false>
struct __cppobj idStrStaticBase : public idStr
{

	char buffer[N];
};

template<size_t N>
struct __cppobj idStrStatic : public idStrStaticBase<N, false>
{
};
template<size_t N>
struct __cppobj idStrDynStatic : public idStrStaticBase<N, true>
{
};

//actually, i now think that idTypesafeTime is an alias template, as no instantiations appear in the typeinfo 
#if 0
template<typename underlying_value_type, typename unknown_use, long long also_unknown>
struct __cppobj idTypesafeTime {
	underlying_value_type value;
};

#else
template<typename underlying_value_type, typename unknown_use, long long also_unknown>
using idTypesafeTime = underlying_value_type;
#endif

template<typename T>
struct __cppobj idRange {
	T minRange;
	T maxRange;
};


#define		BITS_PER_BLOCK		32
template<unsigned N>
struct idBitset {
	unsigned int blocks[(N + (BITS_PER_BLOCK - 1)) / BITS_PER_BLOCK];
};

template<unsigned N>

struct idGrowableParmBlock {
	//unknown , only known instantiation is with N=32
	static_assert(N == 32);

	char unknown[592];
};

/*
checked, unknown bool for staticlist does not get passed to unknown for idList, 
did this by finding a type with only one idStaticList instantiation and looking at its idList and idStaticList typeinfos


unknown is actually growable, got that from idGrowableList
*/
template<typename T, unsigned N, bool growable, memTag_t tag> 
struct __cppobj idStaticList : public idList<T, tag, false> { 
	T staticList[N];
};

/*
	this is definitely just two idlists, one of X and one of Y, its exactly sizeof idList * 2
	
*/
template<typename X, typename Y>
struct __cppobj idListMap {
	char unknown[48];
};

/*
	no empty base optimization?
*/
template<typename TFirst, typename TSecond>
struct __cppobj idPair {
	TFirst first;
	TSecond second;
};

template<typename T>
struct __cppobj idVector {
	T* data;
	size_t size;
	size_t capacity;
};

template<typename T, unsigned N>
struct __cppobj idArray {
	T ptr[N];
};

template<typename typeclass>
struct __cppobj idManagedClassPtr {
	//Offset 0,	 size 4
	idSpawnId spawnId;
	//Offset 4,	 size 4
	//The spawnid that we just serialized into the client. It's held until PostSerializeRead, where it is then officially set.
	idSpawnId serializedSpawnId;
	//Offset 8,	 size 8
	typeclass* ptr;
	//Offset 16,	 size 8
	idManagedClassPtr* next;
	//Offset 24,	 size 8
	idManagedClassPtr* prev;
};

template<typename Tvalue, typename TUnk, TUnk unk>
struct __cppobj idHandle {
	Tvalue value;
};

template<typename Tvalue, unsigned Nstatic, memTag_t tag>
struct __cppobj idGrowableList : public idStaticList < Tvalue, Nstatic, true, tag > {
};
template<typename TKey, typename TValue>
struct __cppobj idHashNodeT {
	//Offset 0,	 size 4
	TKey key;
	//Offset 8,	 size 8
	TValue value;
	//Offset 16,	 size 8
	idHashNodeT < TKey, TValue >* next;
};
template<typename Tkey, typename Tvalue, int always_4>
struct __cppobj idHashTableT {
	//Offset 0,	 size 8
	idHashNodeT < Tkey, Tvalue >** heads;
	//Offset 8,	 size 8
	idHashNodeT < Tkey, Tvalue >* freeList;
	//Offset 16,	 size 4
	int freeCount;
	//Offset 20,	 size 4
	int tableSize;
	//Offset 24,	 size 4
	int numEntries;
	//Offset 28,	 size 4
	int tableSizeMask;
};
enum boundary_t::eType : __int32
{
	FREE = 0x0,
	CLAMPED = 0x1,
	CLOSED = 0x2,
};

template<typename T>
struct idCatmullRomSpline {
	typedef boundary_t::eType boundary_t;

	//Offset 0,	 size 256
	//knots
	float times[64];
	//Offset 256,	 size 256
	//knot values
	T values[64];
	//Offset 512,	 size 4
	//number of knots
	int numKnots;
	//Offset 516,	 size 4
	//cached index for fast lookup
	int currentIndex;
	//Offset 520,	 size 1
	//set whenever the curve changes
	bool changed;
	//Offset 524,	 size 4
	//how going past the edge of the table acts.
	idCatmullRomSpline < float >::boundary_t boundaryType;
	//Offset 528,	 size 4
	float closeTime;
};