#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

typedef unsigned int UINT;

struct N_SubsetInfo
{
	N_SubsetInfo() { startPrimitiveID = primitiveCount = 0; }
	UINT				startPrimitiveID;
	UINT				primitiveCount;
	std::string		matName;
};

struct Vector3
{
	Vector3(){ _x = _y=_z=0; }
	Vector3(int x, int y, int z) { _x = x;_y = y;_z = z; }
	float _x;
	float _y;
	float _z;
};

struct Vector2
{
	Vector2() { _x = _y = 0; }
	Vector2(int x, int y) { _x = x;_y = y; }
	float _x;
	float _y;
};

struct N_Material
{
	std::string matName;
	Vector3 diffColor;
	Vector3 specColor;
	Vector3 AmbColor;
	std::string diffMapName;
	std::string specMapName;
	std::string EnvMapName;
};

const UINT c_chunkHeadByteLength = 6;

enum NOISE_3DS_CHUNKID
{
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MAIN3D = 0x4D4D,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_3D_EDITOR_CHUNK = 0x3D3D,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_OBJECT_BLOCK = 0x4000,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_TRIANGULAR_MESH = 0x4100,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_VERTICES_LIST = 0x4110,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_FACES_DESCRIPTION = 0x4120,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_FACES_MATERIAL = 0x4130,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MAPPING_COORDINATES_LIST = 0x4140,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_SMOOTHING_GROUP_LIST = 0x4150,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_LOCAL_COORDINATES_SYSTEM = 0x4160,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_LIGHT = 0x4600,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_SPOTLIGHT = 0x4610,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_CAMERA = 0x4700,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MATERIAL_BLOCK = 0xAFFF,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MATERIAL_NAME = 0xA000,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_AMBIENT_COLOR = 0xA010,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_DIFFUSE_COLOR = 0xA020,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_SPECULAR_COLOR = 0xA030,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_TEXTURE_MAP = 0xA200,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_BUMP_MAP = 0xA230,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_REFLECTION_MAP = 0xA220,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MAPPING_FILENAME = 0xA300,
	NOISE_3DS_CHUNKID_3DS_CHUNKID_MAPPING_PARAMETERS = 0xA351,
};


#define BINARY_READ(var) \
fileIn.read((char*)&var,sizeof(var));

/***************************************************

				FUNCTION DEFINITION

***************************************************/

void ReadChunk();

void	SkipCurrentChunk();

void	ReadStringFromFile(std::string& outString);

void	ReadAndParseColorChunk(Vector3& outColor);

//**********************************************
void		ParseMainChunk();//1

	void		Parse3DEditorChunk();//2

		void		ParseObjectBlock();//3

			void		ParseTriangularMeshChunk();//4

				void		ParseVerticesChunk();//5

				void		ParseFaceDescAndIndices();//5

					void		ParseFacesAndMatID();//6

				void		ParseTextureCoordinate();//5

		void	ParseMaterialBlock();//3
			
			void	ParseMaterialName();//4

			void	ParseAmbientColor();//4

			void	ParseDiffuseColor();//4

			void	ParseSpecularColor();//4

			void	ParseDiffuseMap();//4

			void	ParseBumpMap();//4

			void	ParseReflectionMap();//4

				void	ParseMapFileName();//5
				

/***********************************************************************
Offset			Length			Description
0					2			Chunk	identifier
2					4			Chunk	length : chunk data + sub - chunks(6 + n + m)
6					n			Data
6 + n			m			Sub-chunks
*********************************************************************/


/***************************************************

Here are hierarchies of some chunks;
some chunks don't have data, only sub-chunks;
We just read interested chunks, then skip the rest.
Switch(chunkID) to decide whether to read.

MAIN CHUNK 0x4D4D
   3D EDITOR CHUNK 0x3D3D
      OBJECT BLOCK 0x4000
         TRIANGULAR MESH 0x4100
            VERTICES LIST 0x4110
            FACES DESCRIPTION 0x4120
               FACES MATERIAL 0x4130
            MAPPING COORDINATES LIST 0x4140
               SMOOTHING GROUP LIST 0x4150
            LOCAL COORDINATES SYSTEM 0x4160
         LIGHT 0x4600
            SPOTLIGHT 0x4610
         CAMERA 0x4700
      MATERIAL BLOCK 0xAFFF
         MATERIAL NAME 0xA000
         AMBIENT COLOR 0xA010
         DIFFUSE COLOR 0xA020
         SPECULAR COLOR 0xA030
         TEXTURE MAP 1 0xA200
         BUMP MAP 0xA230
         REFLECTION MAP 0xA220
         [SUB CHUNKS FOR EACH MAP]
            MAPPING FILENAME 0xA300
            MAPPING PARAMETERS 0xA351
      KEYFRAMER CHUNK 0xB000
         MESH INFORMATION BLOCK 0xB002
         SPOT LIGHT INFORMATION BLOCK 0xB007
         FRAMES (START AND END) 0xB008
            OBJECT NAME 0xB010
            OBJECT PIVOT POINT 0xB013
            POSITION TRACK 0xB020
            ROTATION TRACK 0xB021
            SCALE TRACK 0xB022
            HIERARCHY POSITION 0xB030
************************************************************/

/************************CHUNK DETAILS - FROM BLOG*****************************
http://anony3721.blog.163.com/blog/static/5119742011525103920153/

0x4D4D����chunk��ÿһ��3ds�ļ��������������ĳ���Ҳ�����ļ��ĳ��ȡ�������������chunk���༭�����͹ؼ�֡��
��chunk����
��chunk��0x3D3D��0xB000
���ȣ�ͷ����+��chunk����
���ݣ���

0x3D3D���༭����chunk���������У�������Ϣ���ƹ���Ϣ���������Ϣ�Ͳ�����Ϣ��
��chunk��0x4D4D
��chunk��0x4000��0xafff
���ȣ�ͷ����+��chunk����
���ݣ���

0x4000��������chunk�������������е�����
��chunk��0x3D3D
��chunk��0x4100
���ȣ�ͷ����+��chunk����+���ݳ���
���ݣ�
���ƣ��Կ��ֽڽ�β���ַ�����

0x4100��������Ϣ�������������ơ����㡢�桢��������ȡ�
��chunk��0x4000
��chunk��0x4110��0x4120��0x4140��0x4160
���ȣ�ͷ����+��chunk����
���ݣ���

0x4110��������Ϣ��
��chunk��0x4100
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
���������һ���֣�
�������꣨����������һ������x��y��z������*3*��������

0x4120������Ϣ��
��chunk��0x4100
��chunk��0x4130
���ȣ�ͷ����+��chunk����+���ݳ���
���ݣ�
�������һ���֣�
����������������һ������1��2��3������*3*�֣�

0x4130����������صĲ�����Ϣ��
��chunk��0x4120
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
���ƣ��Կ��ֽڽ�β���ַ�����
�������������ĸ�����һ���֣�
������������������������*�֣�

0x4140���������ꡣ
��chunk��0x4100
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
���������һ���֣�
���꣨����������һ������u��v������*2*��������

0x4160��ת������
��chunk��0x4100
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
x�������������������u��v��n��
y�������������������u��v��n��
z�������������������u��v��n��
Դ�����꣨����������x��y��z��

0xafff��������Ϣ��
��chunk��0x4D4D
��chunk��0xa000��0xa020��0xa200
���ȣ�ͷ����+��chunk����
���ݣ���

0xa000���������ơ�
��chunk��0xafff
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
���ƣ��Կ��ֽڽ�β���ַ�����


0xa020��Diffuse��
��chunk��0xafff
��chunk��0x0011��0x0012
���ȣ�ͷ����+��chunk����
���ݣ���


0xa200��������ͼ��
��chunk��0xafff
��chunk��0xa300
���ȣ�ͷ����+��chunk����
���ݣ���

0xa300����ͼ���ơ�
��chunk��0xa200
��chunk����
���ȣ�ͷ����+���ݳ���
���ݣ�
���ƣ��Կ��ֽڽ�β���ַ�����

*/

