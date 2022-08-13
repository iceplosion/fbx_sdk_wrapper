#include "Exporter.h"
#include "Constants.h"
#include "fbxsdk.h"
#include <vector>
#include <algorithm>
#include <filesystem>


void FBXWrapper::setControlPoints(FbxMesh* mesh, Vector3AttribPtr points)
{
	int pointCount = points->size();
	mesh->InitControlPoints(pointCount);
	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int pid = 0; pid < pointCount; ++pid)
	{
		const auto& [x, y, z] = points->at(pid);
		controlPoints[pid] = FbxVector4(x, y, z, 1);
	}
}

// TODO: supoort material export if we need
void FBXWrapper::setPolygons(FbxMesh* mesh, IntAttribPtr polyOffset, IntAttribPtr vertexIndices)
{
	int polygonCount = polyOffset->size();
	for (int polyId = 0; polyId < polygonCount; ++polyId)
	{
		mesh->BeginPolygon(-1, -1, -1, false);
		int vertEnd = polyId + 1 < polygonCount ? polyOffset->at(polyId + 1) : vertexIndices->size();
		for (int localVertId = polyOffset->at(polyId); localVertId < vertEnd; ++localVertId)
		{
			mesh->AddPolygon(vertexIndices->at(localVertId));
		}
		mesh->EndPolygon();
	}
}

void FBXWrapper::setSurface(FbxMesh* mesh, Vector3AttribPtr points, IntAttribPtr polyOffset, IntAttribPtr vertexIndices)
{
	setControlPoints(mesh, points);
	setPolygons(mesh, polyOffset, vertexIndices);
}

void FBXWrapper::addNormalSet(FbxMesh* mesh, Vector3AttribPtr normals, int mappingType)
{
	if (mappingType != 0 && mappingType != 1)
	{
		// unknown mapping type
		return;
	}
	FbxGeometryElementNormal* normalLayer = mesh->CreateElementNormal();
	auto mappingMode = mappingType == 0 ? FbxGeometryElement::eByControlPoint : FbxGeometryElement::eByPolygonVertex;
	normalLayer->SetMappingMode(mappingMode);
	normalLayer->SetReferenceMode(FbxGeometryElement::eDirect);
	auto& fbxNormals = normalLayer->GetDirectArray();
	int normalCount = normals->size();
	for (int i = 0; i < normalCount; ++i)
	{
		const auto& [x, y, z] = normals->at(i);
		fbxNormals.Add(FbxVector4(x, y, z, 0));
	}
}

void FBXWrapper::addUVSet(FbxMesh* mesh, Vector2AttribPtr uvs, const std::string& uvSetName)
{
	FbxGeometryElementUV* uvLayer = mesh->CreateElementUV(uvSetName.c_str());
	uvLayer->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	uvLayer->SetReferenceMode(FbxGeometryElement::eDirect);
	auto& fbxUV = uvLayer->GetDirectArray();
	int uvCount = uvs->size();
	for (int i = 0; i < uvCount; ++i)
	{
		const auto& [u, v] = uvs->at(i);
		fbxUV.Add(FbxVector2(u, v));
	}
}

void FBXWrapper::addColorSet(FbxMesh* mesh, Vector4AttribPtr colors, int mappingType)
{
	if (mappingType != 0 && mappingType != 1)
	{
		// unknown mapping type
		return;
	}
	FbxGeometryElementVertexColor* colorLayer = mesh->CreateElementVertexColor();
	auto mappingMode = mappingType == 0 ? FbxGeometryElement::eByControlPoint : FbxGeometryElement::eByPolygonVertex;
	colorLayer->SetMappingMode(mappingMode);
	colorLayer->SetReferenceMode(FbxGeometryElement::eDirect);
	auto& fbxColors = colorLayer->GetDirectArray();
	int colorCount = colors->size();
	for (int i = 0; i < colorCount; ++i)
	{
		const auto& [r, g, b, a] = colors->at(i);
		fbxColors.Add(FbxVector4(r, g, b, a));
	}
}

bool startsWith(const std::string& str, const std::string& pattern)
{
	if (str.size() > pattern.size())
		return false;
	for (int i = 0; i < pattern.size(); ++i)
	{
		if (str[i] != pattern[i])
			return false;
	}
	return true;
}

FbxMesh* FBXWrapper::createFbxMesh(FbxScene* fbxScene, AttribMeshPtr attribMesh, const std::string& meshName)
{
	FbxMesh* mesh = FbxMesh::Create(fbxScene, meshName.c_str());
	bool hasPoints = attribMesh->containsAttrib(A_POS);
	bool hasPolygons = attribMesh->containsAttrib(A_REF_POINT) && attribMesh->containsAttrib(A_REF_POINT_OFFSET);
	if (hasPoints && hasPolygons)
	{
		setSurface(mesh, attribMesh->getVector3Attrib(A_POS), attribMesh->getIntAttrib(A_REF_POINT_OFFSET), attribMesh->getIntAttrib(A_REF_POINT));
	}
	else if (hasPoints)
	{
		setControlPoints(mesh, attribMesh->getVector3Attrib(A_POS));
	}

	bool hasNormal = attribMesh->containsAttrib(A_NORMAL);
	auto normalDomain = attribMesh->metaOfAttrib[A_NORMAL]->domain;
	int normalMappingType = -1;
	if (normalDomain == D_Point)
		normalMappingType = 0;
	else if (normalDomain == D_Vertex)
		normalMappingType = 1;
	addNormalSet(mesh, attribMesh->getVector3Attrib(A_NORMAL), normalMappingType);

	auto uvSets = std::vector<std::pair<std::string, Vector2AttribPtr> > ();
	for (auto& v2Attr : attribMesh->Vector2Map)
	{
		if (startsWith(v2Attr.first, A_UV))
		{
			uvSets.push_back(v2Attr);
		}
	}
	if (uvSets.size() > 0)
	{
		std::sort(uvSets.begin(), uvSets.end());
		for (int uvSetId = 0; uvSetId < uvSets.size(); ++uvSetId)
		{
			addUVSet(mesh, uvSets[uvSetId].second, uvSets[uvSetId].first);
		}
	}

	auto colorSets = std::vector<std::pair<std::string, Vector4AttribPtr> >();
	for (auto& v4Attr : attribMesh->Vector4Map)
	{
		if (startsWith(v4Attr.first, A_COLOR))
		{
			colorSets.push_back(v4Attr);
		}
	}
	if (colorSets.size() > 0)
	{
		std::sort(colorSets.begin(), colorSets.end());
		for (int colorSetId = 0; colorSetId < colorSets.size(); ++colorSetId)
		{
			auto colorDomain = attribMesh->metaOfAttrib[colorSets[colorSetId].first]->domain;
			int mappingMode = -1;
			if (colorDomain == D_Point)
			{
				mappingMode = 0;
			}
			else if (colorDomain == D_Vertex)
			{
				mappingMode = 1;
			}
			addColorSet(mesh, colorSets[colorSetId].second, mappingMode);
		}
	}
	return mesh;
}

bool FBXWrapper::createMeshesScene(FbxScene* fbxScene, std::vector<AttribMeshPtr>& meshes)
{
	FbxNode* rootNode = fbxScene->GetRootNode();
	int meshCount = meshes.size();
	for (int meshId = 0; meshId < meshCount; ++meshId)
	{
		AttribMeshPtr meshPtr = meshes[meshId];
		std::string meshName;
		bool hasMeshName = false;
		if (meshPtr->containsAttrib(A_NAME))
		{
			hasMeshName = true;
			meshName = meshPtr->getStringAttrib(A_NAME)->at(0);
		}
		if (!hasMeshName)
		{
			meshName = "mesh" + std::to_string(meshId);
		}
		FbxNode* node = FbxNode::Create(fbxScene, meshName.c_str());
		FbxMesh* mesh = createFbxMesh(fbxScene, meshPtr, meshName);
		node->SetNodeAttribute(mesh);
		rootNode->AddChild(node);
	}
	return true;
}


bool FBXWrapper::exportScene(FbxManager* sdkManager, FbxScene* fbxScene, const std::string& filePath, int exportMode)
{
	// Create an exporter.
	FbxExporter* fbxExporter = FbxExporter::Create(sdkManager, "");
	auto pluginRegistry = sdkManager->GetIOPluginRegistry();
	// default format
	int fileFormat = pluginRegistry->GetNativeWriterFormat();
	int formatCount = pluginRegistry->GetWriterFormatCount();
	for (int formatIndex = 0; formatIndex < formatCount; ++formatIndex)
	{
		if (pluginRegistry->WriterIsFBX(formatIndex))
		{
			FbxString description = pluginRegistry->GetWriterFormatDescription(formatIndex);
			if (exportMode == 0 && description.Find("binary") >= 0)
			{
				fileFormat = formatIndex;
				break;
			}
			else if (exportMode == 1 && description.Find("ascii") >= 0)
			{
				fileFormat = formatIndex;
				break;
			}
		}
	}

	auto ioSetting = sdkManager->GetIOSettings();
	ioSetting->SetBoolProp(EXP_FBX_MATERIAL, true);
	ioSetting->SetBoolProp(EXP_FBX_TEXTURE, true);
	ioSetting->SetBoolProp(EXP_FBX_EMBEDDED, false);
	ioSetting->SetBoolProp(EXP_FBX_SHAPE, true);
	ioSetting->SetBoolProp(EXP_FBX_GOBO, true);
	ioSetting->SetBoolProp(EXP_FBX_ANIMATION, true);
	ioSetting->SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	if (fbxExporter->Initialize(filePath.c_str(), fileFormat, ioSetting) == false)
	{
		return false;
	}

	// Export the scene.
	bool status = fbxExporter->Export(fbxScene);

	// Destroy the exporter.
	fbxExporter->Destroy();
	return status;
}

bool FBXWrapper::exportMeshes(const std::string& folder, const std::string& sceneName, std::vector<AttribMeshPtr> meshes)
{
	FbxManager* sdkManager = FbxManager::Create();
	if (sdkManager == nullptr)
	{
		return false;
	}
	FbxIOSettings* ioSetting = FbxIOSettings::Create(sdkManager, IOSROOT);
	sdkManager->SetIOSettings(ioSetting);
	auto fbxScene = FbxScene::Create(sdkManager, sceneName.c_str());
	if (fbxScene == nullptr)
	{
		return false;
	}
	bool result = createMeshesScene(fbxScene, meshes);
	if (result)
	{
		std::filesystem::create_directories(folder);
		if (!std::filesystem::exists(folder))
		{
			return false;
		}
		result = exportScene(sdkManager, fbxScene, folder + "/" + sceneName + ".fbx", 0);
	}
	sdkManager->Destroy();
	return result;
}