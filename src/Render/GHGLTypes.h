// Copyright Golden Hammer Software
#pragma once

// Sometimes we need gl type defines in order to load files outside of opengl projects.
// KTX and glTF are reasons why we need this file.

// todo: macro if !defined define.  is that possible?

#if !defined(GL_BYTE)
#define GL_BYTE									0x1400
#define GL_UNSIGNED_BYTE						0x1401
#define GL_SHORT								0x1402
#define GL_UNSIGNED_SHORT						0x1403
#define GL_INT									0x1404
#define GL_UNSIGNED_INT							0x1405
#define GL_FLOAT								0x1406
#define GL_2_BYTES								0x1407
#define GL_3_BYTES								0x1408
#define GL_4_BYTES								0x1409
#define GL_DOUBLE								0x140A
#endif

#if !defined(GL_RGB)
#define GL_RGB									0x1907
#define GL_RGBA									0x1908
#endif

#if !defined(GL_DEPTH_COMPONENT)
#define GL_DEPTH_COMPONENT						0x1902
#endif
#if !defined(GL_DEPTH_COMPONENT16)
#define GL_DEPTH_COMPONENT16                    0x81A5
#endif

#if !defined(GL_RGB8)
#define GL_PROXY_TEXTURE_1D						0x8063
#define GL_PROXY_TEXTURE_2D						0x8064
#define GL_TEXTURE_PRIORITY						0x8066
#define GL_TEXTURE_RESIDENT						0x8067
#define GL_TEXTURE_BINDING_1D					0x8068
#define GL_TEXTURE_BINDING_2D					0x8069
#define GL_TEXTURE_INTERNAL_FORMAT				0x1003
#define GL_ALPHA4								0x803B
#define GL_ALPHA8								0x803C
#define GL_ALPHA12								0x803D
#define GL_ALPHA16								0x803E
#define GL_LUMINANCE4							0x803F
#define GL_LUMINANCE8							0x8040
#define GL_LUMINANCE12							0x8041
#define GL_LUMINANCE16							0x8042
#define GL_LUMINANCE4_ALPHA4					0x8043
#define GL_LUMINANCE6_ALPHA2					0x8044
#define GL_LUMINANCE8_ALPHA8					0x8045
#define GL_LUMINANCE12_ALPHA4					0x8046
#define GL_LUMINANCE12_ALPHA12					0x8047
#define GL_LUMINANCE16_ALPHA16					0x8048
#define GL_INTENSITY							0x8049
#define GL_INTENSITY4							0x804A
#define GL_INTENSITY8							0x804B
#define GL_INTENSITY12							0x804C
#define GL_INTENSITY16							0x804D
#define GL_R3_G3_B2								0x2A10
#define GL_RGB4									0x804F
#define GL_RGB5									0x8050
#define GL_RGB8									0x8051
#define GL_RGB10								0x8052
#define GL_RGB12								0x8053
#define GL_RGB16								0x8054
#define GL_RGBA2								0x8055
#define GL_RGBA4								0x8056
#define GL_RGB5_A1								0x8057
#define GL_RGBA8								0x8058
#define GL_RGB10_A2								0x8059
#define GL_RGBA12								0x805A
#define GL_RGBA16								0x805B
#endif

// https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc2.txt
#if !defined(COMPRESSED_RGBA_PVRTC_2BPPV2_IMG)
#define COMPRESSED_RGBA_PVRTC_2BPPV2_IMG		0x9137
#endif
#if !defined(COMPRESSED_RGBA_PVRTC_4BPPV2_IMG)
#define COMPRESSED_RGBA_PVRTC_4BPPV2_IMG		0x9138
#endif
// https://www.khronos.org/registry/OpenGL/extensions/IMG/IMG_texture_compression_pvrtc.txt
#if !defined(COMPRESSED_RGB_PVRTC_4BPPV1_IMG)
#define COMPRESSED_RGB_PVRTC_4BPPV1_IMG			0x8C00
#define COMPRESSED_RGB_PVRTC_2BPPV1_IMG			0x8C01
#define COMPRESSED_RGBA_PVRTC_4BPPV1_IMG		0x8C02
#define COMPRESSED_RGBA_PVRTC_2BPPV1_IMG		0x8C03
#endif 

// https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_texture_compression_astc_hdr.txt
// bit lazy, assuming if one doesn't exist then none exist.
#if !defined(COMPRESSED_RGBA_ASTC_4x4_KHR)
#define COMPRESSED_RGBA_ASTC_4x4_KHR            0x93B0
#define COMPRESSED_RGBA_ASTC_5x4_KHR            0x93B1
#define COMPRESSED_RGBA_ASTC_5x5_KHR            0x93B2
#define COMPRESSED_RGBA_ASTC_6x5_KHR            0x93B3
#define COMPRESSED_RGBA_ASTC_6x6_KHR            0x93B4
#define COMPRESSED_RGBA_ASTC_8x5_KHR            0x93B5
#define COMPRESSED_RGBA_ASTC_8x6_KHR            0x93B6
#define COMPRESSED_RGBA_ASTC_8x8_KHR            0x93B7
#define COMPRESSED_RGBA_ASTC_10x5_KHR           0x93B8
#define COMPRESSED_RGBA_ASTC_10x6_KHR           0x93B9
#define COMPRESSED_RGBA_ASTC_10x8_KHR           0x93BA
#define COMPRESSED_RGBA_ASTC_10x10_KHR          0x93BB
#define COMPRESSED_RGBA_ASTC_12x10_KHR          0x93BC
#define COMPRESSED_RGBA_ASTC_12x12_KHR          0x93BD

#define COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR    0x93D0
#define COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR    0x93D1
#define COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR    0x93D2
#define COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR    0x93D3
#define COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR    0x93D4
#define COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR    0x93D5
#define COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR    0x93D6
#define COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR    0x93D7
#define COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR   0x93D8
#define COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR   0x93D9
#define COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR   0x93DA
#define COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR  0x93DB
#define COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR  0x93DC
#define COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR  0x93DD
#endif

// https://www.khronos.org/registry/OpenGL/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt
#if !defined(ETC1_RGB8_OES)
#define ETC1_RGB8_OES							0x8D64
#endif

// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_ES3_compatibility.txt
#if !defined(COMPRESSED_RGB8_ETC2)
#define COMPRESSED_RGB8_ETC2                             0x9274
#define COMPRESSED_SRGB8_ETC2                            0x9275
#define COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2         0x9276
#define COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2        0x9277
#define COMPRESSED_RGBA8_ETC2_EAC                        0x9278
#define COMPRESSED_SRGB8_ALPHA8_ETC2_EAC                 0x9279
#define COMPRESSED_R11_EAC                               0x9270
#define COMPRESSED_SIGNED_R11_EAC                        0x9271
#define COMPRESSED_RG11_EAC                              0x9272
#define COMPRESSED_SIGNED_RG11_EAC                       0x9273
#endif


