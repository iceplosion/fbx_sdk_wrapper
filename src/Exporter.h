#ifndef _EXPORTER_H
#define _EXPORTER_H
#include "fbxsdk.h"
#include "AttribMesh.h"

namespace FBXWrapper
{
	void setControlPoints(FbxMesh* mesh, Vector3AttribPtr points);
	/// <summary>
	/// Set polygons to mesh
	/// </summary>
	/// <param name="polyOffset">array of beginning index in vertexIndices array for each polygon</param>
	/// <param name="verticesIndices">array of vertex indices</param>
	void setPolygons(FbxMesh* mesh, IntAttribPtr polyOffset, IntAttribPtr vertexIndices);
	void setSurface(FbxMesh* mesh, Vector3AttribPtr points, IntAttribPtr polyOffset, IntAttribPtr vertexIndices);
	/// <summary>
	/// add normal set to mesh
	/// type: 0 -> per control point normal; 1 -> per polygon per vertex normal
	/// </summary>
	void addNormalSet(FbxMesh* mesh, Vector3AttribPtr normals, int mappingType);
	void addUVSet(FbxMesh* mesh, Vector2AttribPtr uvs, const std::string& uvSetName);
	/// <summary>
	/// add vertex color set to mesh
	/// type: 0 -> per control point; 1 -> per polygon per vertex
	/// </summary>
	void addColorSet(FbxMesh* mesh, Vector4AttribPtr colors, int mappingType);
	FbxMesh* createFbxMesh(FbxScene* fbxScene, AttribMeshPtr attribMesh, const std::string& meshName);
	bool createMeshesScene(FbxScene* fbxScene, std::vector<AttribMeshPtr>& meshes);
	/// <summary>
	/// export fbx scene
	/// exportMode:
	/// 0 -> binary
	/// 1 -> ascii
	/// </summary>
	bool exportScene(FbxManager* sdkManager, FbxScene* fbxScene, const std::string& filePath, int exportMode);
	bool exportMeshes(const std::string& folder, const std::string& sceneName, std::vector<AttribMeshPtr> meshes);
}

#endif