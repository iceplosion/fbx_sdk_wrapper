#pragma once
#include "Types.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
namespace FBXWrapper
{
#define CONCAT(A, B) A ## B
#define ATTRIB_TYPE_NAME(element_type) CONCAT(element_type, Attrib)
#define ATTRIB_PTR_TYPE_NAME(element_type) ATTRIB_TYPE_NAME(element_type)Ptr
#define ATTRIB_MAP_NAME(element_type) CONCAT(element_type, Map)
#define DEF_ATTRIB_TYPE(element_type) \
	typedef std::vector<element_type> ATTRIB_TYPE_NAME(element_type); \
	typedef std::shared_ptr<ATTRIB_TYPE_NAME(element_type)> ATTRIB_PTR_TYPE_NAME(element_type)
#define DEF_ATTRIB_MAP(element_type) \
	std::unordered_map<std::string, ATTRIB_PTR_TYPE_NAME(element_type)> ATTRIB_MAP_NAME(element_type)
#define DEF_ATTRIB_MAP_OP_PROTO(element_type) \
	ATTRIB_PTR_TYPE_NAME(element_type) CONCAT(get, element_type)Attrib(const std::string& name)
#define DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(element_type) \
	ATTRIB_PTR_TYPE_NAME(element_type) CONCAT(create, element_type)Attrib(const std::string& name);	\
	void CONCAT(remove, element_type)Attrib(const std::string& name)

	DEF_ATTRIB_TYPE(Real);
	DEF_ATTRIB_TYPE(Vector2);
	DEF_ATTRIB_TYPE(Vector3);
	DEF_ATTRIB_TYPE(Vector4);
	typedef int Int;
	DEF_ATTRIB_TYPE(Int);
	typedef std::string String;
	DEF_ATTRIB_TYPE(String);

	enum AttribType
	{
		T_Real, T_V2, T_V3, T_V4, T_Int, T_String
	};

	enum AttribDomain
	{
		D_Point, D_Vertex, D_Element, D_Meta
	};

	class AttribMeta
	{
	public:
		AttribType type;
		AttribDomain domain;
	};

	typedef std::shared_ptr<AttribMeta> MetaPtr;

	class AttribMesh
	{
	private:
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(Real);
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(Vector2);
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(Vector3);
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(Vector4);
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(Int);
		DEF_ATTRIB_MAP_OP_PROTO_PRIVATE(String);

	public:
		std::unordered_map<std::string, MetaPtr> metaOfAttrib;
		DEF_ATTRIB_MAP(Real);
		DEF_ATTRIB_MAP(Vector2);
		DEF_ATTRIB_MAP(Vector3);
		DEF_ATTRIB_MAP(Vector4);
		DEF_ATTRIB_MAP(Int);
		DEF_ATTRIB_MAP(String);
		DEF_ATTRIB_MAP_OP_PROTO(Real);
		DEF_ATTRIB_MAP_OP_PROTO(Vector2);
		DEF_ATTRIB_MAP_OP_PROTO(Vector3);
		DEF_ATTRIB_MAP_OP_PROTO(Vector4);
		DEF_ATTRIB_MAP_OP_PROTO(Int);
		DEF_ATTRIB_MAP_OP_PROTO(String);

		bool tryCreateAttrib(const std::string& name, const AttribType type, const AttribDomain domain);
		bool containsAttrib(const std::string& name);
		bool tryRemoveAttrib(const std::string& name);
	};
	typedef std::shared_ptr<AttribMesh> AttribMeshPtr;
}