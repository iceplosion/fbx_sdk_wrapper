#pragma once

#include "AttribMesh.h"
namespace FBXWrapper
{
	std::vector<std::shared_ptr<AttribMesh> > loadMeshes(const std::string& filepath);
}