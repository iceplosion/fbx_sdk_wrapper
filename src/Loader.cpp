#include "fbxsdk.h"
#include "Loader.h"
#include "Constants.h"
#include <queue>
#include <optional>

namespace FBXWrapper
{
	std::vector<std::shared_ptr<AttribMesh> > collectMeshFromScene(FbxScene* scene, FbxGeometryConverter& converter);
	std::shared_ptr<AttribMesh> loadAsAttribMesh(FbxMesh* mesh);

	std::vector<std::shared_ptr<AttribMesh> > loadMeshes(const std::string& filepath)
	{
		std::vector<std::shared_ptr<AttribMesh> > result;
		FbxManager* manager = FbxManager::Create();
		FbxGeometryConverter converter(manager);
		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);
		FbxImporter* importer = FbxImporter::Create(manager, "");
		bool success = true;
		if (!importer->Initialize(filepath.c_str(), -1, manager->GetIOSettings()))
		{
			success = false;
		}
		if (success)
		{
			FbxScene* scene = FbxScene::Create(manager, "main_scene");
			importer->Import(scene);
			importer->Destroy();
			const auto& meshes = collectMeshFromScene(scene, converter);
			result.insert(std::end(result), std::cbegin(meshes), std::cend(meshes));
		}
		manager->Destroy();
		
		return std::move(result);
	}

	std::vector<std::shared_ptr<AttribMesh> > collectMeshFromScene(FbxScene* scene, FbxGeometryConverter& converter)
	{
		std::vector<std::shared_ptr<AttribMesh> > result;
		FbxNode* root = scene->GetRootNode();
		if (root)
		{
			std::queue<FbxNode*> que;
			que.push(root);
			while (!que.empty())
			{
				FbxNode* currentNode = que.front(); que.pop();
				FbxNodeAttribute* attribute = currentNode->GetNodeAttribute();
				if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					FbxMesh* mesh = (FbxMesh*)attribute;
					std::shared_ptr<AttribMesh> ptr = loadAsAttribMesh(mesh);
					result.push_back(ptr);
				}

				int childCount = currentNode->GetChildCount();
				for (int i = 0; i < childCount; ++i)
				{
					que.push(currentNode->GetChild(i));
				}
			}
		}
		return result;
	}

	std::shared_ptr<AttribMesh> loadAsAttribMesh(FbxMesh* mesh)
	{
		auto result = std::make_shared<AttribMesh>();
		int pointCount = mesh->GetControlPointsCount();
		int polygonCount = mesh->GetPolygonCount();
		FbxVector4* fbxPoints = mesh->GetControlPoints();

		result->tryCreateAttrib(A_POS, T_V3, D_Point);
		auto posAttr = result->getVector3Attrib(A_POS);
		for (int pidx = 0; pidx < pointCount; ++pidx)
		{
			auto P = std::make_tuple(fbxPoints[pidx][0], fbxPoints[pidx][1], fbxPoints[pidx][2]);
			posAttr->push_back(P);

		}

		result->tryCreateAttrib(A_REF_POINT, T_Int, D_Element);
		result->tryCreateAttrib(A_REF_POINT_OFFSET, T_Int, D_Element);
		auto polygonRefPoints = result->getIntAttrib(A_REF_POINT);
		auto polygonOffset = result->getIntAttrib(A_REF_POINT_OFFSET);
		for (int fidx = 0; fidx < polygonCount; ++fidx)
		{
			int polygonVertCount = mesh->GetPolygonSize(fidx);
			polygonOffset->push_back(polygonRefPoints->size());
			for (int localVertIdx = 0; localVertIdx < polygonVertCount; ++localVertIdx)
			{
				int refPointIndex = mesh->GetPolygonVertex(fidx, localVertIdx);
				polygonRefPoints->push_back(refPointIndex);
			}
		}

		int normalSetCount = mesh->GetElementNormalCount();
		if (normalSetCount > 0)
		{
			FbxGeometryElementNormal* fbxNormalLayer = mesh->GetElementNormal(0);
			auto normalMappingMode = fbxNormalLayer->GetMappingMode();
			if (normalMappingMode == FbxGeometryElement::eByControlPoint)
			{
				result->tryCreateAttrib(A_NORMAL, T_V3, D_Point);
				auto normalAttr = result->getVector3Attrib(A_NORMAL);
				for (int pidx = 0; pidx < pointCount; ++pidx)
				{
					auto fbxNormal = fbxNormalLayer->GetDirectArray().GetAt(pidx);
					auto N = std::make_tuple(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
					normalAttr->push_back(N);
				}
			}
			else if (normalMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				result->tryCreateAttrib(A_NORMAL, T_V3, D_Vertex);
				auto normalAttr = result->getVector3Attrib(A_NORMAL);
				auto referenceMode = fbxNormalLayer->GetReferenceMode();
				auto& fbxNormals = fbxNormalLayer->GetDirectArray();
				if (referenceMode == FbxGeometryElement::eDirect)
				{
					for (int vidx = 0; vidx < polygonRefPoints->size(); ++vidx)
					{
						auto fbxNormal = fbxNormals.GetAt(vidx);
						auto N = std::make_tuple(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
						normalAttr->push_back(N);
					}
				}
				else if (referenceMode == FbxGeometryElement::eIndexToDirect)
				{
					auto& indexArray = fbxNormalLayer->GetIndexArray();
					for (int vidx = 0; vidx < polygonRefPoints->size(); ++vidx)
					{
						auto fbxNormalIdx = indexArray.GetAt(vidx);
						auto fbxNormal = fbxNormals.GetAt(fbxNormalIdx);
						auto N = std::make_tuple(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
						normalAttr->push_back(N);
					}
				}
				else
				{
					result->tryRemoveAttrib(A_NORMAL);
				}
			}
		}

		int uvSetCount = mesh->GetElementUVCount();
		for (int uvSetId = 0; uvSetId < uvSetCount; ++uvSetId)
		{
			std::string uvSetName = A_UV;
			if (uvSetId > 0)
			{
				uvSetName += std::to_string(uvSetId + 1);
			}
			FbxGeometryElementUV* uvLayer = mesh->GetElementUV(uvSetId);
			auto uvMappingMode = uvLayer->GetMappingMode();
			if (uvMappingMode == FbxGeometryElement::eByControlPoint)
			{
				result->tryCreateAttrib(uvSetName, T_V2, D_Point);
				auto uvAttr = result->getVector2Attrib(uvSetName);
				auto referenceMode = uvLayer->GetReferenceMode();
				auto& fbxUVs = uvLayer->GetDirectArray();
				if (referenceMode == FbxGeometryElement::eDirect)
				{
					for (int pidx = 0; pidx < pointCount; ++pidx)
					{
						auto fbxUV = fbxUVs.GetAt(pidx);
						auto uv = std::make_tuple(fbxUV[0], fbxUV[1]);
						uvAttr->push_back(uv);
					}
				}
				else if (referenceMode == FbxGeometryElement::eIndexToDirect)
				{
					auto& fbxUVIndexArr = uvLayer->GetIndexArray();
					for (int pidx = 0; pidx < pointCount; ++pidx)
					{
						int uvIndex = fbxUVIndexArr.GetAt(pidx);
						auto fbxUV = fbxUVs.GetAt(uvIndex);
						auto uv = std::make_tuple(fbxUV[0], fbxUV[1]);
						uvAttr->push_back(uv);
					}
				}
				else
				{
					result->tryRemoveAttrib(uvSetName);
				}
			}
			else if (uvMappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				auto referenceMode = uvLayer->GetReferenceMode();
				if (referenceMode == FbxGeometryElement::eDirect ||
					referenceMode == FbxGeometryElement::eIndexToDirect)
				{
					auto& fbxUVs = uvLayer->GetDirectArray();
					result->tryCreateAttrib(uvSetName, T_V2, D_Vertex);
					auto uvAttr = result->getVector2Attrib(uvSetName);
					for (int fidx = 0; fidx < polygonCount; ++fidx)
					{
						for (int inPolyIndex = 0; inPolyIndex < mesh->GetPolygonSize(fidx); ++inPolyIndex)
						{
							int uvIndex = mesh->GetTextureUVIndex(fidx, inPolyIndex);
							auto fbxUV = fbxUVs.GetAt(uvIndex);
							auto uv = std::make_tuple(fbxUV[0], fbxUV[1]);
							uvAttr->push_back(uv);
						}
					}
				}
			}
		}

		return result;
	}
}