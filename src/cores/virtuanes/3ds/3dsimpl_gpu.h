
#include "3dsgpu.h"
#include "3dsmain.h"

#ifndef _3DSIMPL_GPU_H_
#define _3DSIMPL_GPU_H_

struct SVector3i
{
    int16 x, y, z;
};

struct SVector4i
{
    int16 x, y, z, w;
};


struct SVector3f
{
    float x, y, z;
};

struct STexCoord2i
{
    int16 u, v;
};

struct STexCoord2f
{
    float u, v;
};


struct SVertexTexCoord {
    struct SVector4i    Position;
	struct STexCoord2f  TexCoord;
};


struct SVertexColor {
    struct SVector4i    Position;
	u32                 Color;
};


#define SVERTEXTEXCOORD_ATTRIBFORMAT    GPU_ATTRIBFMT (0, 4, GPU_SHORT) | GPU_ATTRIBFMT (1, 2, GPU_FLOAT)
#define SVERTEXCOLOR_ATTRIBFORMAT       GPU_ATTRIBFMT(0, 4, GPU_SHORT) | GPU_ATTRIBFMT(1, 4, GPU_UNSIGNED_BYTE)



typedef struct
{
    SVertexList         quadVertexes;
    SVertexList         tileVertexes;
    SVertexList         rectangleVertexes;

} SGPU3DSExtended;

extern SGPU3DSExtended GPU3DSExt;

#include "stdio.h"


inline void __attribute__((always_inline)) gpu3dsAddQuadVertexes(
    int x0, int y0, int x1, int y1,
    float tx0, float ty0, float tx1, float ty1,
    int data)
{
    SVertexTexCoord *vertices = &((SVertexTexCoord *) GPU3DSExt.quadVertexes.List)[GPU3DSExt.quadVertexes.Count];

	vertices[0].Position = (SVector4i){x0, y0, data, 1};
	vertices[1].Position = (SVector4i){x1, y0, data, 1};
	vertices[2].Position = (SVector4i){x0, y1, data, 1};

	vertices[3].Position = (SVector4i){x1, y1, data, 1};
	vertices[4].Position = (SVector4i){x0, y1, data, 1};
	vertices[5].Position = (SVector4i){x1, y0, data, 1};

	vertices[0].TexCoord = (STexCoord2f){tx0, ty0};
	vertices[1].TexCoord = (STexCoord2f){tx1, ty0};
	vertices[2].TexCoord = (STexCoord2f){tx0, ty1};

	vertices[3].TexCoord = (STexCoord2f){tx1, ty1};
	vertices[4].TexCoord = (STexCoord2f){tx0, ty1};
	vertices[5].TexCoord = (STexCoord2f){tx1, ty0};

    //GPU3DSExt.vertexCount += 6;
    GPU3DSExt.quadVertexes.Count += 6;
}



void gpu3dsDrawRectangle(int x0, int y0, int x1, int y1, int depth, u32 color);
void gpu3dsAddRectangleVertexes(int x0, int y0, int x1, int y1, int depth, u32 color);
void gpu3dsDrawVertexes(bool repeatLastDraw = false, int storeIndex = -1);
void gpu3dsBindTextureMainScreen(SGPUTexture *texture, GPU_TEXUNIT unit);

#endif