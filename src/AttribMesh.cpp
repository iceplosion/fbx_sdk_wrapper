#include "AttribMesh.h"

namespace FBXWrapper
{
#define DEF_ATTRIB_MAP_OP(element_type) \
	ATTRIB_PTR_TYPE_NAME(element_type) AttribMesh::CONCAT(get, element_type)Attrib(const std::string& name) { \
		if (ATTRIB_MAP_NAME(element_type).count(name))	\
		{	\
			return ATTRIB_MAP_NAME(element_type)[name];	\
		}	\
		return nullptr;	\
	}	\
	ATTRIB_PTR_TYPE_NAME(element_type) AttribMesh::CONCAT(create, element_type)Attrib(const std::string& name) {	\
		ATTRIB_MAP_NAME(element_type)[name] = std::make_shared<ATTRIB_TYPE_NAME(element_type)>();	\
		return ATTRIB_MAP_NAME(element_type)[name];	\
	}	\
	void AttribMesh::CONCAT(remove, element_type)Attrib(const std::string& name)	\
	{	\
		ATTRIB_MAP_NAME(element_type).erase(name);	\
	}

	DEF_ATTRIB_MAP_OP(Real)
	DEF_ATTRIB_MAP_OP(Vector2)
	DEF_ATTRIB_MAP_OP(Vector3)
	DEF_ATTRIB_MAP_OP(Int)

	bool AttribMesh::tryCreateAttrib(const std::string& name, const AttribType type, const AttribDomain domain)
	{
		if (metaOfAttrib.count(name))
		{
			return false;
		}

		switch (type)
		{
		case T_Real:
			createRealAttrib(name);
			break;
		case T_V2:
			createVector2Attrib(name);
			break;
		case T_V3:
			createVector3Attrib(name);
			break;
		case T_Int:
			createIntAttrib(name);
			break;
		}
		auto meta = std::make_shared<AttribMeta>();
		meta->domain = domain;
		meta->type = type;
		metaOfAttrib[name] = meta;
		return true;
	}

	bool AttribMesh::containsAttrib(const std::string& name)
	{
		return metaOfAttrib.count(name) > 0;
	}

	bool AttribMesh::tryRemoveAttrib(const std::string& name)
	{
		if (!metaOfAttrib.count(name))
		{
			return false;
		}
		auto metaPtr = metaOfAttrib[name];
		auto type = metaPtr->type;
		switch (type)
		{
		case T_Real:
			removeRealAttrib(name);
			break;
		case T_V2:
			removeVector2Attrib(name);
			break;
		case T_V3:
			removeVector3Attrib(name);
			break;
		case T_Int:
			removeIntAttrib(name);
			break;
		}
		metaOfAttrib.erase(name);
		return true;
	}
}