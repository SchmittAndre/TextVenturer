#pragma once

enum GLAttribMask {
    amNone = 0,
    amDepth = GL_DEPTH_BUFFER_BIT,
    amStencil = GL_STENCIL_BUFFER_BIT,
    amColor = GL_COLOR_BUFFER_BIT,
    amDepthStencil = amDepth | amStencil,
    amColorDepth = amColor | amDepth,
    amColorStencil = amColor | amStencil,
    amColorDepthStencil = amColor | amDepth | amStencil
};

enum GLBoolean {
    blFalse = GL_FALSE,
    blTrue
};

enum GLRenderMode {
    rmPoints = GL_POINTS,
    rmLines,
    rmLineLoop,
    rmLineStrip,
    rmTriangles,
    rmTriangleStrip,
    rmTriangleFan,   
    rmLinesAdjacency = GL_LINES_ADJACENCY,
    rmLineStripAdjacency,
    rmTrianglesAdjacency,
    rmTriangleStripAdjacency,
    rmPatches
};

enum GLCompareFunction {
    cfNever = GL_NEVER,
    cfLess,
    cfEqual,
    cfLEqual,
    cfGreater,
    cfNotEqual,
    cfGEqual,
    cfAlways
};

enum GLBlendingFactorSrc {
    bfsZero = GL_ZERO,
    bfsOne,
    bfsSrcColor = GL_SRC_COLOR,
    bfsOneMinusSrcColor,
    bfsSrcAlpha,
    bfsOneMinusSrcAlpha,
    bfsDstAlpha,
    bfsOneMinusDstAlpha,
    bfsDstColor,
    bfsOneMinusDstColor,
    bfsSrcAlphaSaturate,
    bfsConstantColor = GL_CONSTANT_COLOR,
    bfsOneMinusConstantColor,
    bfsConstantAlpha,
    bfsOneMinusConstantAlpha
};

enum GLBlendingFactorDest {
    bfdZero = GL_ZERO,
    bfdOne,
    bfdSrcColor = GL_SRC_COLOR,
    bfdOneMinusSrcColor,
    bfdSrcAlpha,
    bfdOneMinusSrcAlpha,
    bfdDstAlpha,
    bfdOneMinusDstAlpha,
    bfdDstColor,
    bfdOneMinusDstColor,
    bfdConstantColor = GL_CONSTANT_COLOR,
    bfdOneMinusConstantColor,
    bfdConstantAlpha,
    bfdOneMinusConstantAlpha
};

enum GLDrawBufferMode {
    dbmNone = GL_NONE,
    dbmFrontLeft = GL_FRONT_LEFT,
    dbmFrontRight,
    dbmBackLeft,
    dbmBackRight,
    dbmFront,
    dbmBack,
    dbmLeft,
    dbmRight,
    dbmFrontAndBack
};

enum GLErrorCode {
    ecNoError = GL_NO_ERROR,
    ecInvalidEnum = GL_INVALID_ENUM,
    ecInvalidValue,
    ecInvaliedOperation,
    ecOutOfMemory
};

enum GLFrontFaceDirection {
    ffdCW = GL_CW,
    ffdCCW
};

enum GLHintMode {
    hmDontCare = GL_DONT_CARE,
    hmFastest,
    hmNicest
};

enum GLDataType {
    dtByte = GL_BYTE,
    dtUByte,
    dtShort,
    dtUShort,
    dtInt,
    dtUInt,
    dtFloat,
    dtDouble = GL_DOUBLE
};

enum GLLogicOp {
    loClear = GL_CLEAR,  // = 0
    loAND,               // = s and d
    loANDReverse,        // = s and not d
    loCopy,              // = s
    loANDInverted,       // = not s and d
    loNoOp,              // = d
    loXOR,               // = s xor d
    loOR,                // = s or d
    loNOR,               // = not (s or d)
    loEQU,               // = not (s xor d)
    loInvert,            // = not d
    loORReverse,          // = s or not d
    loCopyInverted,      // = not s
    loORInverted,        // = not s or d
    loNAND,              // = not (s and d)
    loSet                // = 1
};

enum GLPixelCopyType {
    pctColor = GL_COLOR,
    pctDepth,
    pctStencil
};

enum GLPixelFormat {
    pfStencilIndex = GL_STENCIL_INDEX,
    pfDepthComponent,
    pfRed,
    pfGreen,
    pfBlue,
    pfALPHA,
    pfRGB,
    pfRGBA
};

enum GLPolygonMode {
    pmPoint = GL_POINT,
    pmLine,
    pmFill
};

enum GLShaderType {
    stFragment = GL_FRAGMENT_SHADER,
    stVertex,
    stGeometry = GL_GEOMETRY_SHADER,
    stCompute = GL_COMPUTE_SHADER
};

enum GLStencilOp {
    soKeep = GL_KEEP,
    soReplace,
    soIncr,
    soDecr
};

enum GLStringName {
    snVendor = GL_VENDOR,
    snRenderer,
    snVersion,
    snExtensions
};

enum GLTextureMagFilter {
    magNearest = GL_NEAREST,
    magLinear
};

enum GLTextureMinFilter {
    minNearest = GL_NEAREST,
    minLinear,
    minNearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    minLinearMipmapNearest,
    minNearestMipmapLinear,
    minLinearMipmapLinear
};

enum GLTextureParameterName {
    tpnTextureMagFilter = GL_TEXTURE_MAG_FILTER,
    tpnTextureMinFilter,
    tpnTextureWrapS,
    tpnTextureWrapT
};

enum GLPixelInternalFormat {
    pifR3G3B2 = GL_R3_G3_B2,
    pifRGB4 = GL_RGB4,
    pifRGB5,
    pifRGB8,
    pifRGB10,
    pifRGB12,
    pifRGB16,
    pifRGBA2,
    pifRGBA4,
    pifRGB5_A1,
    pifRGBA8,
    pifRGB10_A2,
    pifRGBA12,
    pifRGBA16
};

enum GLBufferAccess {
    baReadOnly = GL_READ_ONLY,
    baWriteOnly,
    baReadWrite
};

enum GLBufferTarget {
    btArrayBuffer = GL_ARRAY_BUFFER,
    btElementArrayBuffer
};

enum GLBufferUsage {
    buStreamDraw = GL_STREAM_DRAW,   // Data updates every frame
    buStreamRead,
    buStreamCopy,
    buStaticDraw = GL_STATIC_DRAW,   // Data updates almost never
    buStaticRead,
    buStaticCopy,
    buDynamicDraw = GL_DYNAMIC_DRAW, // Data updates not every frame but very often
    buDynamicRead,
    buDynamicCopy
};

enum GLFBOAttachment {
    fbaColor = GL_COLOR_ATTACHMENT0,
    fbaDepth = GL_DEPTH_ATTACHMENT,
    fbaStencil = GL_STENCIL_ATTACHMENT
};

DWORD getDataSize(GLDataType dataType);