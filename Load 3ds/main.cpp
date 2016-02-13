#include "my3dsLoader.h"


//there are so many output that put them in function parameter will be too verbose
//Not to mention recursive function or deep function calling stack
static uint64_t fileSize = 0;
static std::ifstream fileIn;
static uint64_t currentChunkFileEndPos = 0;

//--------Data From File--------
static std::string static_objectName;
static std::vector<Vector3> static_verticesList;
static std::vector<Vector2> static_texcoordList;
static std::vector<UINT> static_indicesList;
static std::vector<N_SubsetInfo> static_subsetList;
static std::vector<N_Material> static_materialList;

int main()
{
	fileIn.open("TexturedBox.3ds", std::ios::binary);

	if (!fileIn.good())
	{
		std::cout << "file cannot open";
		return 0;
	}

	fileIn.seekg(0, std::ios::end);
	currentChunkFileEndPos = 0;
	fileSize = fileIn.tellg();
	fileIn.seekg(0);

	//maybe linearly deal with chunks is also OK....
	//stack will be used when necessary because recursive solution
	//can sometimes be rewritten into non-recursive form using stack.
	while (!fileIn.eof() && fileIn.good())
	{
		ReadChunk();
	}
	fileIn.close();

	system("pause");

}

//when encountering not interested chunk, use absolute end file pos of the chunk to skip
void SkipCurrentChunk()
{
	//sometimes we will encounter error / corrupted data , we must shutdown the reading 
	//of this chunk
	if (currentChunkFileEndPos < fileSize)
	{
		fileIn.seekg(currentChunkFileEndPos);
	}
	else
	{
		fileIn.seekg(0, std::ios::end);
	}
}

//Linearly read chunks
void ReadChunk()
{
	//because 3DS file stores data with a tree topology, so i do have considered 
	//recursive/tree-like reading...but there is some mysterious chunks which might vary
	//from time to time... ehh...linear reading will be clearer???

	const UINT c_chunkHeadByteLength=6;

	//2 numbers at the head of every chunk
	uint16_t chunkID=0;
	uint32_t chunkLength=0;

	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

	//if chunk length corrupted , length data might be invalid
	if (chunkLength > fileSize)
	{
		currentChunkFileEndPos = fileSize - 1;
		fileIn.seekg(0, std::ios::end);
		std::cout << "3DS File Damaged!!!";
		return;
	}

	uint64_t tmpFilePos = fileIn.tellg();
	currentChunkFileEndPos = tmpFilePos + chunkLength - c_chunkHeadByteLength;

	//in CURRENT chunk, there can be several sub-chunks to read
	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_MAIN3D:
		ParseMainChunk();//level1
		break;

	case NOISE_3DS_CHUNKID_3D_EDITOR_CHUNK:
		Parse3DEditorChunk();//level2
		break;

	case NOISE_3DS_CHUNKID_OBJECT_BLOCK:
		ParseObjectBlock();//level3
		break;

	case NOISE_3DS_CHUNKID_TRIANGULAR_MESH:
		ParseTriangularMeshChunk();//level4
		break;

	case NOISE_3DS_CHUNKID_VERTICES_LIST:
		ParseVerticesChunk();//level5
		break;

	case NOISE_3DS_CHUNKID_FACES_DESCRIPTION:
		ParseFaceDescAndIndices();//level5
		break;

	case NOISE_3DS_CHUNKID_FACES_MATERIAL:
		ParseFacesAndMatID();//level 6
		break;

	case NOISE_3DS_CHUNKID_MAPPING_COORDINATES_LIST:
		ParseTextureCoordinate();//level5
		break;

	case NOISE_3DS_CHUNKID_MATERIAL_BLOCK:
		ParseMaterialBlock();//level 3
		break;

	case NOISE_3DS_CHUNKID_MATERIAL_NAME:
		ParseMaterialName();//level 4
		break;

	case NOISE_3DS_CHUNKID_AMBIENT_COLOR:
		ParseAmbientColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_DIFFUSE_COLOR:
		ParseDiffuseColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_SPECULAR_COLOR:
		ParseSpecularColor();//level 4
		break;

	case NOISE_3DS_CHUNKID_TEXTURE_MAP:
		ParseDiffuseMap();//level 4
		break;

	case NOISE_3DS_CHUNKID_BUMP_MAP:
		ParseBumpMap();//level 4
		break;

	case NOISE_3DS_CHUNKID_SPECULAR_MAP:
		ParseSpecularMap();//level 4
		break;

	/*case NOISE_3DS_CHUNKID_REFLECTION_MAP:
		ParseReflectionMap();//4
		break;*/

	default:
		//not interested chunks , skip
		SkipCurrentChunk();
		break;
	}

}

//orderly read a string from file at current position
void ReadStringFromFile(std::string & outString)
{
	do
	{
		char tmpChar = 0;
		BINARY_READ(tmpChar);

		if (tmpChar)
		{
			outString.push_back(tmpChar);
		}
		else
		{
			break;
		}
	} while (true);
}

//father: xxx color chunk
//child:none
void ReadAndParseColorChunk(Vector3 & outColor)
{
	//please refer to project "Assimp" for more chunk information

	uint16_t chunkID = 0;
	uint32_t chunkLength = 0;
	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_RGBF://float
	{
		BINARY_READ(outColor._x);
		BINARY_READ(outColor._y);
		BINARY_READ(outColor._z);
	}
		break;

	case NOISE_3DS_CHUNKID_RGBB://byte
	{
		uint8_t colorByte = 0;
		BINARY_READ(colorByte);
		outColor._x = float(colorByte) / 255.0f;
		BINARY_READ(colorByte);
		outColor._y = float(colorByte) / 255.0f;
		BINARY_READ(colorByte);
		outColor._z = float(colorByte) / 255.0f;
	}
		break;

	case NOISE_3DS_CHUNKID_PERCENTF://grey scale??
	{
		float colorPercentF = 0.0f;
		BINARY_READ(colorPercentF);
		outColor._x = outColor._y = outColor._z = colorPercentF;
	}
		break;

	case NOISE_3DS_CHUNKID_PERCENTW: //grey scale??
	{
		uint8_t colorByte = 0;
		BINARY_READ(colorByte);
		outColor._x = outColor._y = outColor._z = float(colorByte) / 255.0f;
	}
		break;

	default:
		//skip this color chunk
		fileIn.seekg(chunkLength - c_chunkHeadByteLength, std::ios::cur);
		return;
	}
}

//father: xxx map chunk
//child:none
void ReadAndParseMapChunk(std::string & outMapFileName)
{
	//please refer to project "Assimp" for more chunk information

	uint16_t chunkID = 0;
	uint32_t chunkLength = 0;
	BINARY_READ(chunkID);
	BINARY_READ(chunkLength);

		/*case Discreet3DS::CHUNK_PERCENTF:
		case Discreet3DS::CHUNK_PERCENTW:
		case Discreet3DS::CHUNK_MAT_MAP_USCALE:
		case Discreet3DS::CHUNK_MAT_MAP_VSCALE:
		case Discreet3DS::CHUNK_MAT_MAP_UOFFSET:
		case Discreet3DS::CHUNK_MAT_MAP_VOFFSET:
		case Discreet3DS::CHUNK_MAT_MAP_ANG:
		case Discreet3DS::CHUNK_MAT_MAP_TILING:*/

	switch (chunkID)
	{
	case NOISE_3DS_CHUNKID_MAPPING_FILENAME:
		ReadStringFromFile(outMapFileName);
		break;

	default:
		//skip this color chunk
		fileIn.seekg(chunkLength - c_chunkHeadByteLength, std::ios::cur);
		break;
	}

}

//*****************************************************************

//*************************************************
//father : none
//child : 3D Editor Chunk
void ParseMainChunk()
{
	//no data in this chunk ,only need to go into sub-chunks
};

//*************************************************
//father : Main3D
//child :	3D EDITOR CHUNK 0x3D3D
//			MATERIAL BLOCK 0xAFFF
void Parse3DEditorChunk()
{
	//no data in this chunk ,only need to go into sub-chunks
};

//*************************************************
//father : 3D Editor Chunk
//child : TriangularMesh,  (Light , Camera)
void ParseObjectBlock()
{
	//data: object name
	ReadStringFromFile(static_objectName);
};

//*************************************************
//father : Object Block
//child : VerticesChunk, FaceDescription(Indices) , TextureCoordinate
void ParseTriangularMeshChunk()
{
	//no data in this chunk ,only need to go into sub-chunks
}

//*************************************************
//father::Triangular Mesh Chunk
//child: none
void ParseVerticesChunk()
{
	//data: Vertices List:
	uint16_t verticesCount = 0;//unsigned short
	BINARY_READ(verticesCount);

	static_verticesList.reserve(verticesCount);
	for (UINT i = 0;i < verticesCount;i++)
	{
		Vector3 tmpVertex(0,0,0);
		BINARY_READ(tmpVertex._x);
		BINARY_READ(tmpVertex._y);
		BINARY_READ(tmpVertex._z);
		static_verticesList.push_back(tmpVertex);
	}

}

//*************************************************
//father::Triangular Mesh Chunk
//child: FACES MATERIAL 0x4130
void ParseFaceDescAndIndices()
{
	//data:faces count + n * (3*indices+faceDesc)
	uint16_t faceCount = 0;
	BINARY_READ(faceCount);

	static_indicesList.reserve(faceCount);

	//I want to combine various object into one 
	uint16_t indexOffset = uint16_t(static_indicesList.size());
	for (UINT i = 0;i < faceCount;i++)
	{
		//1 face for 3 indices
		uint16_t tmpIndex=0;

		for (UINT i = 0;i < 3;i++)
		{
			BINARY_READ(tmpIndex);
			static_indicesList.push_back(indexOffset + tmpIndex);
		}

		uint16_t faceFlag = 0;
		//face option,side visibility , etc.
		BINARY_READ(faceFlag);
	}

}

//*************************************************
//father::FACES DESCRIPTION 0x4120
//child: none
void ParseFacesAndMatID()
{
	//this chunk belongs to one indices block ,i guess one indices(faces) block can
	//possess several this kind of FaceMaterial Block ...

	//data : 
	//1. material name (end with '/0')
	//2. faces count that linked to current material (ushort) 
	//3. FACE indices

	std::string matName("");

	std::vector<N_SubsetInfo> currMatSubsetList;//faces can be 
	std::vector<uint16_t> static_indicesList;

	//material name
	ReadStringFromFile(matName);
	
	//faces that link to this material
	uint16_t faceCount = 0;
	BINARY_READ(faceCount);

	//read FACE indices block
	static_indicesList.reserve(faceCount);
	for (UINT i = 0;i < faceCount;i++)
	{
		uint16_t tmpFaceIndex = 0;
		BINARY_READ(tmpFaceIndex);
		static_indicesList.push_back(tmpFaceIndex);
	}

	if (static_indicesList.size() == 0)
	{
		std::cout << "face Material Info : no faces attached to this material!!";
		return;
	}

	//generate subsets list (for current Material)
	N_SubsetInfo newSubset;
	newSubset.matName = matName;
	newSubset.primitiveCount = 0;
	newSubset.startPrimitiveID = static_indicesList.at(0);
	currMatSubsetList.push_back(newSubset);
	for (UINT i =0;i < static_indicesList.size();i++)
	{
		UINT startTriangleID = currMatSubsetList.back().startPrimitiveID;
		UINT endTriangleID = startTriangleID + currMatSubsetList.back().primitiveCount-1;
		//region growing (extend subset region)
		//if next index is adjacent to current region ,then grow
		if (endTriangleID + 1 == static_indicesList[i])
		{
			currMatSubsetList.back().primitiveCount += 1;
		}
		else
		{
			//this index isn't adjacent to the growing region
			newSubset.matName = matName;
			newSubset.primitiveCount = 1;
			newSubset.startPrimitiveID = static_indicesList.at(i);
			currMatSubsetList.push_back(newSubset);
		}
	}

	//add to global subset list
	for (auto subset : currMatSubsetList)
	{
		static_subsetList.push_back(subset);
	}

}

//*************************************************
//father::Triangular Mesh
//child: smoothing group
void ParseTextureCoordinate()
{
	//data: 
	//1.texcoord count (2byte)
	//2.  n * (u + v) ,2n float
	uint16_t texcoordCount = 0;
	BINARY_READ(texcoordCount);

	for (UINT i = 0;i < texcoordCount;i++)
	{
		Vector2 tmpTexCoord(0, 0);
		BINARY_READ(tmpTexCoord._x);
		BINARY_READ(tmpTexCoord._y);
		static_texcoordList.push_back(tmpTexCoord);
	}

}

//*************************************************
//father:3D Editor Chunk 
//child: MATERIAL NAME 0xA000
//         AMBIENT COLOR 0xA010
//			DIFFUSE COLOR 0xA020
//			SPECULAR COLOR 0xA030
//			TEXTURE MAP 1 0xA200
//			BUMP MAP 0xA230
//			REFLECTION MAP 0xA220
void ParseMaterialBlock()
{
	//Data:None

	//but this identifier signify creation a new material
	N_Material tmpMat;
	static_materialList.push_back(tmpMat);
}

//*************************************************
//father:Material block
//child:  none
void ParseMaterialName()
{
	//data: material name
	ReadStringFromFile(static_materialList.back().matName);
}

//*************************************************
//father:Material Block
//child	: sub color chunk
void ParseAmbientColor()
{
	//data:sub chunks

	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(static_materialList.back().AmbColor);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}

//father:Material Block
//child	: sub color chunk
void ParseDiffuseColor()
{
	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(static_materialList.back().diffColor);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}

//father:Material Block
//child	: sub color chunk
void ParseSpecularColor()
{
	uint64_t filePos = 0;
	do
	{
		ReadAndParseColorChunk(static_materialList.back().specColor);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void ParseDiffuseMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		ReadAndParseMapChunk(static_materialList.back().diffMapName);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void ParseBumpMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		ReadAndParseMapChunk(static_materialList.back().normalMapName);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}

//father:Material Block
//child	: Mapping File Path  (Map option) 
void ParseSpecularMap()
{
	//data:sub chunks
	uint64_t filePos = 0;
	do
	{
		ReadAndParseMapChunk(static_materialList.back().specMapName);
		filePos = fileIn.tellg();
	} while (filePos<currentChunkFileEndPos);
}
