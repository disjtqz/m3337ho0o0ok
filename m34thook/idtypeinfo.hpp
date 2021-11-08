#pragma once
#pragma pack(push, 1)
#include <string>
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
	char pad36[4];
	char* comment;
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
struct classTypeInfo_t
{
	char* name;
	char* superType;
	int size;
	char pad20[4];
	classVariableInfo_t* templateParms;
	classVariableInfo_t* variables;
	unsigned long long* variableNameHashes;

	idClass* (*createInstance)();
	idRenderModel* (*createModel)(const idDeclModelAsset * modelAsset , const unsigned int createFlags);
	classMetaDataInfo_t* metaData;
};
#pragma pack(pop)


namespace idType {
	MH_NOINLINE
	classTypeInfo_t* FindClassInfo(const char* cname);
	MH_NOINLINE
	enumTypeInfo_t* FindEnumInfo(const char* enumname);

	MH_NOINLINE
	long long* get_enum_member_value(const char* ename, const char* mname);
	MH_NOINLINE
	classVariableInfo_t* FindClassField(const char* cname, const char* fieldname);

	classTypeInfo_t* ClassTypes(unsigned& out_n, unsigned whichsource=0);
	enumTypeInfo_t* EnumTypes(unsigned& out_n, unsigned whichsource=0);
	typedefInfo_t* TypedefTypes(unsigned& out_n, unsigned whichsource=0);
	void do_idlib_dump();
	//generate an ida idc file that tries to  define all structs, enums and their fields
	void generate_idc();

	//returns formatted search results
	//defined in mh_kwsearch
	std::string keyword_search(const char** args, unsigned argc);
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
		for (classTypeInfo_t* clptr = clstype; clptr && !located_var; clptr = FindClassInfo(clptr->superType)) {

			located_var = try_locate_var_by_name(clptr->variables, fld);
		}

		offset += located_var->offset;

		if constexpr (sizeof...(restfields) != 0) {
			return _impl_get_nested_field_offset_by_name(offset, FindClassInfo(located_var->type), restfields...);
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