
#include "my3dsLoader.h"

//there are so many output that put them in function parameter will be too verbose
//Not to mention a deep function calling tree 
static std::ifstream fileIn;
static std::string static_objectName;


//The Mission of Parsing Chunks mean to get the head/data of current chunk,
//and call the parser function of subchunk


//*************************************************
//father : none
//child : 3D Editor Chunk
void ParseMainChunk()
{
	fileIn.seekg(0,std::ios::end);
	UINT fileSize = fileIn.tellg();
	if (fileSize < 12)
	{
		std::cout << "File Damaged!!!";
		return;
	}
	fileIn.seekg(0);

	//because 3DS file stores data with a tree topology, so recursive traversal
	//of chunks maybe more natural , just like the traversal of Tree.

	//file pointer pos Offset in this chunk
	UINT	 localFilePointer = 0;

	//2 numbers at the head of every chunk
	uint16_t chunkID=0;
	uint32_t chunkLength=0;

	do
	{
		fileIn.read((char*)&chunkID, 2);
		fileIn.read((char*)&chunkLength, 4);

		switch (chunkID)
		{
		case NOISE_3DS_CHUNKID_MAIN3D:
			Parse3DEditorChunk();
			break;

		default:
			//not interested , skip
			fileIn.seekg(chunkLength, 0);
			break;
		}
		//move file pointer
		localFilePointer += chunkLength;
	} while (!fileIn.eof() && localFilePointer < chunkLength);
}

//*************************************************
//father : Main3D
//child : Object Block ,Material Block
void Parse3DEditorChunk()
{
	UINT	 localFilePointer = 0;
	uint16_t chunkID=0;
	uint32_t chunkLength=0;

	do
	{
		fileIn.read((char*)&chunkID, 2);
		fileIn.read((char*)&chunkLength, 4);

		switch (chunkID)
		{
		case NOISE_3DS_CHUNKID_3D_EDITOR_CHUNK:
			//block will be read in the right order
			ParseObjectBlock();
			ParseMaterialBlock();
			break;

		default:
			//not interested , skip
			fileIn.seekg(chunkLength, 0);
			break;
		}
		//move file pointer
		localFilePointer += chunkLength;
	} while (!fileIn.eof() && localFilePointer < chunkLength);
}

//*************************************************
//father : 3D Editor Chunk
//child : TriangularMesh,  (Light , Camera)
void ParseObjectBlock()
{
	UINT	 localFilePointer = 0;
	uint16_t chunkID=0;
	uint32_t chunkLength=0;

	do
	{
		fileIn.read((char*)&chunkID, 2);
		fileIn.read((char*)&chunkLength, 4);

		switch (chunkID)
		{
		case NOISE_3DS_CHUNKID_OBJECT_BLOCK:
			//object name
			do
			{
				char tmpChar = 0;
				fileIn >> tmpChar;
				if (tmpChar)
				{static_objectName.push_back(tmpChar);}
				else
				{break;}

			} while (true);

			ParseTriangularMeshChunk();
			break;

		default:
			//not interested , skip
			fileIn.seekg(chunkLength, 0);
			break;
		}
		//move file pointer
		localFilePointer += chunkLength;
	}while (!fileIn.eof() && localFilePointer < chunkLength);

}


//*************************************************
//father : Object Block
//child : VerticesChunk, FaceDescription(Indices) , TextureCoordinate
void ParseTriangularMeshChunk()
{
	UINT	 localFilePointer = 0;
	uint16_t chunkID=0;
	uint32_t chunkLength=0;

	do
	 {
		fileIn.read((char*)&chunkID, 2);
		fileIn.read((char*)&chunkLength, 4);

		switch (chunkID)
		{
		case NOISE_3DS_CHUNKID_TRIANGULAR_MESH:
			ParseVerticesChunk();
			ParseFaceDescAndIndices();
			ParseTextureCoordinate();
			break;

		default:
			//not interested , skip
			fileIn.seekg(chunkLength, 0);
			break;
		}
		//move file pointer
		localFilePointer += chunkLength;
	} while (!fileIn.eof() && localFilePointer < chunkLength);
}

void ParseVerticesChunk()
{
}

void ParseFaceDescAndIndices()
{
}

void ParseFacesAndMatID()
{
}

void ParseTextureCoordinate()
{
}

void ParseMaterialBlock()
{
}

void ParseMaterialName()
{
}

void ParseAmbientColor()
{
}

void ParseDiffuseColor()
{
}

void ParseSpecularColor()
{
}

void ParseDiffuseMap()
{
}

void ParseBumpMap()
{
}

void ParseReflectionMap()
{
}

void ParseMapFileName()
{
}








int main()
{
	fileIn.open("box.3ds", std::ios::binary);

	if (!fileIn.good())
	{
		std::cout << "file cannot open";
		return 0;
	}

	ParseMainChunk();
	system("pause");
}

