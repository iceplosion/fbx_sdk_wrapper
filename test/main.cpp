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
		const std::string meshName = attribMeshPtr->getStringAttrib(A_NAME)->at(0);
		printf("name: %s\n", meshName.c_str());
		printf("V3 props:\n");
		for (auto& attrib : attribMeshPtr->Vector3Map)
		{
			printf("%s <- domain %d\n", attrib.first.c_str(), attribMeshPtr->metaOfAttrib[attrib.first]->domain);
			int index = 0;
			for (const auto& [x, y, z] : *attrib.second)
			{
				printf("[%d]: %f, %f, %f\n", index++, x, y, z);
			}
		}
		printf("V2 props:\n");
		for (auto& attrib : attribMeshPtr->Vector2Map)
		{
			printf("%s <- domain %d\n", attrib.first.c_str(), attribMeshPtr->metaOfAttrib[attrib.first]->domain);
			int index = 0;
			for (const auto& [x, y] : *attrib.second)
			{
				printf("[%d]: %f, %f\n", index++, x, y);
			}
		}
		printf("Int props:\n");
		for (auto& attrib : attribMeshPtr->IntMap)
		{
			printf("%s <- domain %d\n", attrib.first.c_str(), attribMeshPtr->metaOfAttrib[attrib.first]->domain);
			int index = 0;
			for (const auto& x : *attrib.second)
			{
				printf("[%d]: %d\n", index++, x);
			}
		}
		printf("V4 props:\n");
		for (auto& attrib : attribMeshPtr->Vector4Map)
		{
			printf("%s <- domain %d\n", attrib.first.c_str(), attribMeshPtr->metaOfAttrib[attrib.first]->domain);
			int index = 0;
			for (const auto& [x, y, z, w] : *attrib.second)
			{
				printf("[%d]: %f, %f, %f, %f\n", index++, x, y, z, w);
			}
		}
	}
	
	//FBXWrapper::exportMeshes(R"(D:\export)", "cube2", meshes);
	return 0;
}