#include "Loader.h"
#include <iostream>
#include "Constants.h"
#include "AttribMesh.h"
#include "Exporter.h"

void showMeta(std::shared_ptr<FBXWrapper::AttribMesh> mesh, const std::string& name)
{
	if (mesh->containsAttrib(name))
	{
		auto metaPtr = mesh->metaOfAttrib[name];
		std::cout << name << " <- type: " << metaPtr->type << ", domain: " << metaPtr->domain << std::endl;
	}
	else
	{
		std::cout << name << " <- null" << std::endl;
	}
}
#include "fbxsdk.h"
int main(void)
{
	FbxManager* manager = FbxManager::Create();
	int count = manager->GetIOPluginRegistry()->GetWriterFormatCount();

	for (int i = 0; i < count; ++i)
	{
		if (manager->GetIOPluginRegistry()->WriterIsFBX(i))
		{
			printf("%s\n", manager->GetIOPluginRegistry()->GetWriterFormatDescription(i));

		}
	}

	auto meshes = FBXWrapper::loadMeshes(R"(D:\assets\Meshes\cube.fbx)");
	std::cout << "Mesh count: " << meshes.size() << std::endl;
	for (int index = 0; index < meshes.size(); ++index)
	{
		auto attribMeshPtr = meshes[index];
		auto pos = attribMeshPtr->getVector3Attrib(A_POS);
		auto N = attribMeshPtr->getVector3Attrib(A_NORMAL);
		auto uv = attribMeshPtr->getVector2Attrib(A_UV);
		auto uv2 = attribMeshPtr->containsAttrib(A_UV + std::to_string(2)) ?
			attribMeshPtr->getVector2Attrib(A_UV + std::to_string(2)) : nullptr;
		showMeta(attribMeshPtr, A_POS);
		for (int id = 0; id < pos->size(); ++id)
		{
			auto [x, y, z] = pos->at(id);
			printf("P[%d]: (%f, %f, %f)\n", id, x, y, z);
		}
		showMeta(attribMeshPtr, A_NORMAL);
		for (int id = 0; id < N->size(); ++id)
		{
			auto [x, y, z] = N->at(id);
			printf("N[%d]: (%f, %f, %f)\n", id, x, y, z);
		}
		showMeta(attribMeshPtr, A_UV);
		for (int id = 0; id < uv->size(); ++id)
		{
			auto [u, v] = uv->at(id);
			printf("UV[%d]: (%f, %f)\n", id, u, v);
		}
		if (uv2 != nullptr)
		{
			showMeta(attribMeshPtr, A_UV + std::to_string(2));
			for (int id = 0; id < uv2->size(); ++id)
			{
				auto [u, v] = uv2->at(id);
				printf("UV2[%d]: (%f, %f)\n", id, u, v);
			}
		}
	}
	
	FBXWrapper::exportMeshes(R"(D:\export)", "cube2", meshes);
	return 0;
}