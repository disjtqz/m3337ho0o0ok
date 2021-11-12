#pragma once
#pragma pack(push, 1)
#include <string>
//segment for compressed tables embedded in dll
#pragma section("cmptbl",read,discard)
#include <cstdint>
#include "pregenerated/doom_eternal_properties_generated.hpp"

struct enumValueInfo_t {
	//offset 0 , size 8
	char* name;
	//offset 8 , size 4
	long long value;

};
struct enumTypeInfo_t {
	//offset 0 , size 8
	char* name;
	//offset 8 , size 4
	unsigned long long flags;
	int type;
	int valueIndexLength;

	//offset 16 , size 8
	enumValueInfo_t* values;
	int* valueIndex;
};
struct typedefInfo_t {
	char* name;
	char* type;
	char* ops;
	int size;
	int padding;
};
struct classMetaDataInfo_t
{
  char *metaData;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct classVariableInfo_t
{
	char* type;
	char* ops;
	char* name;
	int offset;
	int size;
	int flags;
	//char pad36[4];
	int m_mh_added_delta2type; //delta to FindClassInfo(type)
	char* comment;//i dont think anything in the engine actually uses comment, so i might be able to pack it into a delta and put more data here if i ever need it
	int (*get)(void* ptr);
	//offset 56 , size 8
	void (*set)(void* ptr, int value);
	//offset 64 , size 8
	void* (*reallocate)(void* ptr, const int oldNum, const int newNum, const int tag, const bool zeroBuffer);

};
#pragma pack(pop)
struct idClass;
struct idRenderModel;
struct idDeclModelAsset;

#pragma pack(push, 1)


/*
	we replace all classes metaData pointers with pointers to the one in here.
	then we just recast to be able to search for a fields index. when you have the index you can just look it up in variables
*/
struct mh_classtypeextra_t {
	
	classMetaDataInfo_t m_metadata;

	//
	unsigned m_num_fields;

	unsigned m_offset2fields; //add to this to get base of field props
};

struct classTypeInfo_t
{
	char* name;
	char* superType;
	int size;

	//char pad20[4];
	//this is padding in the engine originally, but we co-opt it to store the byte delta from the start of this class to
	//the start of its super class, allowing us to traverse the inheritance chain without having to go through findclassinfo's hashing and searching
	//this could be made to be fewer bits if i need other stuff in here in the future
	//also, it could be shifted to the right by 3 to make room for more bits since it should always be 8 byte aligned
	int m_mh_added_delta2super;
	classVariableInfo_t* templateParms;
	classVariableInfo_t* variables;
	unsigned long long* variableNameHashes;

	idClass* (*createInstance)();
	idRenderModel* (*createModel)(const idDeclModelAsset * modelAsset , const unsigned int createFlags);
	classMetaDataInfo_t* metaData;
};
#pragma pack(pop)


namespace idType {

	static inline classTypeInfo_t* get_class_superclass(classTypeInfo_t* cl) {
		intptr_t delta = cl->m_mh_added_delta2super;
		if (delta) {
			return mh_lea<classTypeInfo_t>(cl, delta);
		}
		else
		{
			return nullptr;
		}

	}

	static inline classTypeInfo_t* get_field_class(classVariableInfo_t* vr) {

		intptr_t delta = vr->m_mh_added_delta2type;

		if (delta) {
			return mh_lea<classTypeInfo_t>(vr, delta);
		}
		else
			return nullptr;
	}
	MH_NOINLINE
	classTypeInfo_t* FindClassInfo(const char* cname);
	MH_NOINLINE
	enumTypeInfo_t* FindEnumInfo(const char* enumname);

	MH_NOINLINE
	long long* get_enum_member_value(const char* ename, const char* mname);
	MH_NOINLINE
	classVariableInfo_t* FindClassField(const char* cname, const char* fieldname);

	classTypeInfo_t* ClassTypes(unsigned& out_n, unsigned whichsource=0);

	unsigned NumClassesTotal();
	enumTypeInfo_t* EnumTypes(unsigned& out_n, unsigned whichsource=0);
	typedefInfo_t* TypedefTypes(unsigned& out_n, unsigned whichsource=0);
	void do_idlib_dump();
	//generate an ida idc file that tries to  define all structs, enums and their fields
	void generate_idc();

	//returns formatted search results
	//defined in mh_kwsearch
	std::string keyword_search(const char** args, unsigned argc);
	MH_NOINLINE
	void generate_unique_property_key_tree();
	void generate_json();
	MH_NOINLINE
	classVariableInfo_t* try_locate_var_by_name(classVariableInfo_t* from, const char* field);
	MH_NOINLINE
	classVariableInfo_t* try_locate_var_by_name_inher(classTypeInfo_t* clstype, const char* field);

	//cant use noclone on this, bug in clang. the params get dropped :/
	template<typename... TRest>
	MH_NOINLINE
	static 
	unsigned _impl_get_nested_field_offset_by_name(unsigned offset, classTypeInfo_t* clstype, const char* fld, TRest... restfields) {
		classVariableInfo_t* located_var = nullptr;
		for (classTypeInfo_t* clptr = clstype; clptr && !located_var; clptr = get_class_superclass(clptr)){//FindClassInfo(clptr->superType)) {

			located_var = try_locate_var_by_name(clptr->variables, fld);
		}

		offset += located_var->offset;

		if constexpr (sizeof...(restfields) != 0) {
			return _impl_get_nested_field_offset_by_name(offset, get_field_class(located_var), restfields...);
		}
		else {
			return offset;
		}
	}
	template<typename... TRest>
	static char* _impl_get_nested_field_by_name(char* objptr, classTypeInfo_t* clstype, const char* fld, TRest... restfields) {
		return objptr + _impl_get_nested_field_offset_by_name(0, clstype, fld, restfields...);
	}


	template<typename T, typename TObj, typename... TRest>
	static T* get_nested_field_by_name(TObj obj, const char* cls, TRest... subfields) {
		return reinterpret_cast<T*>(_impl_get_nested_field_by_name((char*)obj, FindClassInfo(cls), subfields...));
	}
	/*
		processes strings with format like "renderModelInfo.blah.x"

	*/
	MH_NOINLINE
	int fieldspec_calculate_offset(classTypeInfo_t* onclass, char* rwbuff);


	MH_NOINLINE
	const char* get_enum_member_name_for_value(enumTypeInfo_t* enumtype, long long value);

	bool enum_member_is(enumTypeInfo_t* enm, long long value, const char* membername);
	//dumps the property rva info to the console/clipboard
	void dump_prop_rvas();
	void init_prop_rva_table();
	//set up our added "m_mh_added_delta2super" field on all classinfo objects
	void compute_classinfo_mh_payloads();

	//get hash that is usable in a class' variableNameHashes
	uint64_t calculate_field_name_hash(const char* name, size_t length);
}

template<typename TRet>
class mh_fieldcached_t {
	unsigned m_offset;
	template<typename... TRest>
	MH_NOINLINE
	MH_CODE_SEG(".field_init")
	MH_REGFREE_CALL
	MH_NOALIAS
	void init_field(const char* clsname, TRest... restfields) {
		m_offset = idType::_impl_get_nested_field_offset_by_name(0, idType::FindClassInfo(clsname), restfields...);
	}
public:
	constexpr mh_fieldcached_t() : m_offset(~0u) {}




	template< typename TObj, typename... TRest>
	inline TRet* operator ()(TObj obj, const char* cls, TRest... subfields) {
		unsigned tmpoffs = m_offset;
		MH_UNLIKELY_IF (!~tmpoffs)  {
			 init_field(cls, subfields...);
			tmpoffs = *(volatile unsigned*)&m_offset;
		}
		return reinterpret_cast<TRet*>(((char*)obj) + tmpoffs);
	}
};

template<typename TRet,typename TClassname, typename... TYuckyStrings>
class mh_new_fieldcached_t {
	unsigned m_offset;

	static constexpr const char* classname = yuckystring_str_from_type_m(TClassname);

	template<typename... TRest>
	MH_NOINLINE
	MH_CODE_SEG(".field_init")
	MH_REGFREE_CALL
	MH_NOALIAS
	void init_field() {
		m_offset = idType::_impl_get_nested_field_offset_by_name(0, idType::FindClassInfo(classname), yuckystr_parampack_unp_m(TYuckyStrings));
	}
public:
	constexpr mh_new_fieldcached_t() : m_offset(~0u) {}




	template< typename TObj>
	inline TRet* operator ()(TObj obj) {
		unsigned tmpoffs = m_offset;
		MH_UNLIKELY_IF(!~tmpoffs) {
			init_field();
			tmpoffs = *(volatile unsigned*)&m_offset;
		}
		return reinterpret_cast<TRet*>(((char*)obj) + tmpoffs);
	}

};

template<typename yuckyname>
struct mh_typesizecached_t {
	unsigned m_size;
	static constexpr const char* parm = yuckystring_str_from_type_m(yuckyname);

	constexpr mh_typesizecached_t() : m_size(0u) {}

	MH_NOINLINE
	MH_CODE_SEG(".field_init")
	MH_REGFREE_CALL
	MH_NOALIAS
	void init_size() {
		m_size = idType::FindClassInfo(parm)->size;
	}

	inline unsigned operator ()() {
		unsigned tmpsz = m_size;
		MH_UNLIKELY_IF(!tmpsz) {

			init_size();
			tmpsz = m_size;
		}
		return tmpsz;

	}
};


/*
	rva to the string of the property in doom eternal

	add g_blamdll.base to this to get the actual start of the string

	link time optimization merges all occurrences of unique string in the game's exe into a single copy that all the pointers point to
	meaning we can refer to our properties via our huge enum by looking them up in this table and adding the rva, and when searching for a field
	in a classes typeinfo we can compare the pointer instead of doing a strcmp.

	this also means we can represent decls in a more efficient way, using the prop indices (which are shorts and so can quickly be compared in batches of 8/16/32 depending on the arch the user is running on) in a sorted table and the corresponding value
	at the same index in the value table
	
*/
extern unsigned g_propname_rvas[DE_NUMPROPS];
/*
	prehashed property names for use in variableNameHashes
*/
extern uint64_t g_propname_hashes[DE_NUMPROPS];

extern mh_classtypeextra_t* g_typeextra;


classVariableInfo_t* mh_fast_field_get(classTypeInfo_t* cls, de_prop_e propert);

