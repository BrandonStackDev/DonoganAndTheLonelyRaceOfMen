#ifndef TEXTURE_H
#define TEXTURE_H

#include "raylib.h"

Texture2D LoadMyTexture(const char* filename)
{
    Texture2D tex = LoadTexture(filename);

    if (tex.id != 0)
    {
        GenTextureMipmaps(&tex);
        SetTextureFilter(tex, TEXTURE_FILTER_TRILINEAR);
    }

    return tex;
}
#endif //TEXTURE_H