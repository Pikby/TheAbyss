#ifndef TYPEENUMSLIBRARY
#define TYPEENUMSLIBRARY

enum Faces {FRONTF = 1 << 0, BACKF = 1 << 1,
            TOPF = 1 << 2, BOTTOMF = 1 << 3,
            RIGHTF = 1 << 4, LEFTF = 1 << 5};
enum RenderType{OPAQUE = 0, TRANSLUCENT = 1, TRANSPARENT = 2};
#endif
