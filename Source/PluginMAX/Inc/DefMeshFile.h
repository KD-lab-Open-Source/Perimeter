#ifndef __DEFMESHFILE_H__
#define __DEFMESHFILE_H__

// ***** id block - 2 byte

#define MF_ID_BLOCK_NONE									0x00
#define MF_ID_BLOCK_SCENE									0x01
#define MF_ID_BLOCK_LOD										0x02
#define MF_ID_BLOCK_CHANNEL									0x03
#define MF_ID_BLOCK_MATERIAL								0x04
#define MF_ID_BLOCK_SUBTEXMAP								0x05
#define MF_ID_BLOCK_OBJECT									0x06
#define MF_ID_BLOCK_ANIMATIONVISIBILITY						0x07
#define MF_ID_BLOCK_ANIMATIONPOSITION						0x08
#define MF_ID_BLOCK_ANIMATIONROTATION						0x09
#define MF_ID_BLOCK_ANIMATIONSCALE							0x0A
#define MF_ID_BLOCK_ANIMATIONMESH							0x0B
#define MF_ID_BLOCK_VERTEXMESH								0x0C
#define MF_ID_BLOCK_FACEMESH								0x0D
#define MF_ID_BLOCK_CHANNELMAPPING							0x0E
#define MF_ID_BLOCK_TEXVERTEXMESH							0x0F
#define MF_ID_BLOCK_TEXFACEMESH								0x10
#define MF_ID_BLOCK_VERTEXNORMALMESH						0x11
#define MF_ID_BLOCK_FACENORMALMESH							0x12
#define MF_ID_BLOCK_HELPEROBJECT							0x13
#define MF_ID_BLOCK_ANIMATIONMATERIAL						0x14
#define MF_ID_BLOCK_LOGIC									0x15
#define MF_ID_BLOCK_ANIMATIONSCALEROTATION					0x16
#define MF_ID_BLOCK_LIGHTOBJECT								0x17
#define MF_ID_BLOCK_ANIMATIONLIGHT							0x18
#define MF_ID_BLOCK_CAMERAOBJECT							0x19
#define MF_ID_BLOCK_ANIMATIONAMBIENT						0x1A
#define MF_ID_BLOCK_ANIMATIONDIFFUSE						0x1B
#define MF_ID_BLOCK_ANIMATIONSPECULAR						0x1C
#define MF_ID_BLOCK_ANIMATION_FLOAT_VAR1					0x1D
#define MF_ID_BLOCK_ANIMATION_FLOAT_VAR2					0x1E
#define MF_ID_BLOCK_ANIMATION_FLOAT_VAR3					0x1F
#define MF_ID_BLOCK_ANIMATION_MATRIX						0x20
#define MF_ID_BLOCK_ANIMATIONEMISSIVE						0x21

// ***** type filed - 2 byte

#define MF_TYPE_BLOCK										0
#define MF_TYPE_COMMENT										1
#define MF_TYPE_ID											2
#define MF_TYPE_NAME										3
#define MF_TYPE_FILENAME									4
#define MF_TYPE_AMBIENTCOLOR								5
#define MF_TYPE_DIFFUSECOLOR								6
#define MF_TYPE_SPECULARCOLOR								7
#define MF_TYPE_FLOAT_VAR1									8
#define MF_TYPE_FLOAT_VAR2									9
#define MF_TYPE_FLOAT_VAR3									10
#define MF_TYPE_NUMBER										11
#define MF_TYPE_KEYVISIBILITY								12
#define MF_TYPE_KEYPOSITION									13
#define MF_TYPE_KEYROTATION									14
#define MF_TYPE_KEYSCALE									15
#define MF_TYPE_PARENT										16
#define MF_TYPE_MATRIX										17
#define MF_TYPE_NUMBERSUBOBJECT								18
#define MF_TYPE_VERTEXMESH									19
#define MF_TYPE_FACEMESH_VER1								20
#define MF_TYPE_FIRSTFRAME									21
#define MF_TYPE_LASTFRAME									22
#define MF_TYPE_FRAMESPEED									23
#define MF_TYPE_TICKSPERFRAME								24
#define MF_TYPE_TIME										25
#define MF_TYPE_LOD											26
#define MF_TYPE_TEXVERTEXMESH_VER1							27
#define MF_TYPE_TEXFACEMESH_VER1							28
#define MF_TYPE_CHANNELNUMBER								29
#define MF_TYPE_HELPERNAME									30
#define MF_TYPE_NUMBERFRAME									31
#define MF_TYPE_FACEMESH									32
#define MF_TYPE_TEXVERTEXMESH								33
#define MF_TYPE_TEXFACEMESH									34
#define MF_TYPE_KEYSCALEROTATION							35
#define MF_TYPE_SHADING										36
#define MF_TYPE_MATRIX1										37
#define MF_TYPE_INT_VAR1									38
#define MF_TYPE_AMBIENTANIMERR								39
#define MF_TYPE_DIFFUSEANIM									40
#define MF_TYPE_SPECULARANIM								41
#define MF_TYPE_MATRIXANIM									42
#define MF_TYPE_FLOAT_VAR1_ANIM								43
#define MF_TYPE_FLOAT_VAR2_ANIM								44
#define MF_TYPE_FLOAT_VAR3_ANIM								45
#define MF_TYPE_AMBIENTANIM									46
#define MF_TYPE_EMISSIVEANIM								47
#define MF_TYPE_INT_VAR2									48
#define MF_TYPE_BLEND										49
#define MF_TYPE_BUMP_SCALE									50

#define MF_TYPE_CHAR										1
#define MF_TYPE_UCHAR										2
#define MF_TYPE_SHORT										3
#define MF_TYPE_USHORT										4
#define MF_TYPE_INT											5
#define MF_TYPE_UINT										6
#define MF_TYPE_FLOAT										7
#define MF_TYPE_DOUBLE										8
#define MF_TYPE_STRING										9
#define MF_TYPE_FLOAT2										10
#define MF_TYPE_FLOAT3										11
#define MF_TYPE_FLOAT4										12
#define MF_TYPE_FLOAT5										13
#define MF_TYPE_FLOAT12										14
#define MF_TYPE_FLOAT13										15
#define MF_TYPE_INT3										16
#define MF_TYPE_INT4										17
#define MF_TYPE_USHORT3										18
#define MF_TYPE_USHORT4										19

struct sTypeField
{
	short type;
    const char *str;
	char var;
};
struct sTypeVariable
{
	short type;
	short size;
};
extern sTypeField GlobalFieldType[];
extern sTypeField GlobalBlockID[];
extern sTypeVariable GlobalTypeVariable[];

#endif // __DEFMESHFILE_H__