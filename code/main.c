extern unsigned char __data_end;
extern unsigned char __heap_base;
// NOTE: stack size is __heap_base - __data_end

unsigned int bump_pointer = (unsigned int)&__heap_base;

#define NULL ((void *)0)

#define Stmnt(S) do{ S }while(0)

#if !defined(AssertBreak)
#define AssertBreak() __builtin_trap() // NOTE: clang thingy
#endif

#if DEBUG
#define Assert(c) Stmnt( if(!(c)){ AssertBreak(); } )
#else
#define Assert(c)
#endif

#define Stringify_(S)   #S
#define Stringify(S)    Stringify_(S)
#define Glue_(A, B)     A##B
#define Glue(A, B)      Glue_(A, B)

#define Min(a,b) (((a)<(b))?(a):(b))
#define Max(a,b) (((a)>(b))?(a):(b))
#define Clamp(x,a,b) (((x)<(a))?(a):((b)<(x))?(b):(x))
#define ClampTop(a,b) Min(a,b)
#define ClampBottom(a,b) Max(a,b)

// types
#include "stdint.h"

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int32_t     b32;
#ifndef char16_t 
typedef uint16_t 	char16_t; // workaround for a conflict with a depndency
#endif
typedef char16_t 	c16;
//typedef wchar_t     w16;		// TODO: fix

typedef float       f32;
typedef double      f64;

typedef uintptr_t   uptr;
//typedef ptrdiff_t   size;		// TODO: fix
//typedef size_t      usize;	// TODO: fix

#ifndef small
typedef char byte; // workaround for a conflict with a depndency
#define small
#endif

static i8  MinI8  = (i8) 0x80;
static i16 MinI16 = (i16)0x8000;
static i32 MinI32 = (i32)0x80000000;
static i64 MinI64 = (i64)0x8000000000000000LL;

static i8  MaxI8  = (i8) 0x7f;
static i16 MaxI16 = (i16)0x7fff;
static i32 MaxI32 = (i32)0x7fffffff;
static i64 MaxI64 = (i64)0x7fffffffffffffffLL;

static u8  MaxU8  = 0xff;
static u16 MaxU16 = 0xffff;
static u32 MaxU32 = 0xffffffff;
static u64 MaxU64 = 0xffffffffffffffffULL;

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define sizeof(x)       (u32)sizeof(x)
#define ArrayCount(a)   (sizeof((a))/sizeof(*(a)))
#define LengthOf(s)     (ArrayCount(s) - 1)

#define IntFromPtr(p) ((u32)((u8*)p - (u8*)0))
#define PtrFromInt(n) ((void*)((u8*)0 + (n)))

#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#include "wasm_simd128.h"

////////////////////////////////////////////////////////////////////////////////
// APPLICATION LOGIC
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// STRINGS
////////////////////////////////////////////////////////////////////////////////
#define NUM_STRINGS	64
#define STRING_LEN	1024
static char strings[NUM_STRINGS][STRING_LEN]; // TODO: play around // TODO: 'or even swap the two dimensions to achieve SOA SIMD

static inline void setString(char *source, i32 index)
{
	Assert(source != NULL);
	char *srcChar = source;
	i32 i = 0;
	while (i < STRING_LEN && *srcChar != '\0')
	{
		strings[index][i++] = *srcChar++;
	}
	strings[index][i] = '\0';
}

static inline i32 getStringLen(i32 stringIndex)
{
	i32 i = 0;
	while (i < STRING_LEN && strings[stringIndex][i++] != '\0');
	
	return i-1;
}

static inline i32 getTextLen(char *text)
{
	i32 i = 0;
	while (i < STRING_LEN && text[i++] != '\0');
	
	return i-1;
}

////////////////////////////////////////////////////////////////////////////////
// DATA
////////////////////////////////////////////////////////////////////////////////

#define MAX_SIZE 1000000 // 65536
//static struct entitiesData
//{
//	i32 x[MAX_SIZE] __attribute__((aligned(16)));
//	i32 y[MAX_SIZE] __attribute__((aligned(16)));
//} entitiesData;
static struct data
{
	f32 value[MAX_SIZE];
} data;

//static Character data[MAX_SIZE];
//static i32 freeIndices[MAX_SIZE];
//static i32 numFreeIndices = MAX_SIZE;

//static inline void initData()
//{
//	i32 indexNumber = 0;
//	for (i32 i = MAX_SIZE-1; i >= 0; --i)
//	{
//		freeIndices[i] = indexNumber++;
//	}
//}
//
//static inline i32 insert(Character input)
//{
//	Assert(numFreeIndices > 0); // TODO: Wonder if I even need it XD
//	i32 index = freeIndices[--numFreeIndices];
//	data[index] = input;
//	return index;
//}
//
//static inline void remove(i32 index)
//{
//	data[index].active = 0; // NOTE: see comment on this (bitflags, conditional moves, masked SIMD instructions)
//	freeIndices[numFreeIndices++] = index;
//}

// lookup: 			data[index];
// update existing:	data[index] = input;

////////////////////////////////////////////////////////////////////////////////
// INPUT
////////////////////////////////////////////////////////////////////////////////
typedef struct ButtonState 
{
	byte endedDown;
	byte justPressed;
} ButtonState;

// TODO: IMPORTANT: This is totally ignored in code, I just have a global mouseX, mouseY, wasPressed and that's it...
typedef struct MouseInput
{
	// x and y handled as seperate statics
	union
	{
		ButtonState buttons[3];
		struct
		{
			ButtonState leftMB;
			ButtonState middleMB;
			ButtonState rightMB;
		};
	};
} MouseInput;

typedef struct KeyboardInput
{
	union
	{
		ButtonState buttons[19];
		struct
		{
			ButtonState arrowLeft;
			ButtonState arrowUp;
			ButtonState arrowRight;
			ButtonState arrowDown;
			ButtonState space;
			ButtonState enter;
			ButtonState tab;
			ButtonState escape;
			ButtonState digit1;
			ButtonState digit2;
			ButtonState digit3;
			ButtonState digit4;
			ButtonState digit5;
			ButtonState digit6;
			ButtonState digit7;
			ButtonState digit8;
			ButtonState digit9;
			ButtonState digit0;
			ButtonState shift;
		};
	};
} KeyboardInput;

typedef struct Input
{
	KeyboardInput keyboard;
	MouseInput mouse;
} Input;

static Input frameInput;

////////////////////////////////////////////////////////////////////////////////
// FRAMEBUFFER AND UI
////////////////////////////////////////////////////////////////////////////////
#define MAX_VIEWPORT_WIDTH 3840
#define MAX_VIEWPORT_HEIGHT 2160

static u32 framebuffer[MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT];

static i32 viewportWidth = MAX_VIEWPORT_WIDTH;
static i32 viewportHeight = MAX_VIEWPORT_HEIGHT;
static i32 viewportSize = MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT;

static i32 mouseX;
static i32 mouseY;
static byte mouseLeftClicked;
static byte mouseWheelUp;
static byte mouseWheelDown;

static i32 hotItem;
static i32 activeItem;

static i32 kbdItem;
static i32 switchFocus;
static i32 keyboardSelect;
static i32 keyMod;
static i32 lastItem;

#define GEN_ID (__LINE__)

static b32 isPaused;

void clearFramebuffer(u32 color)
{
	for (int i = 0; i < viewportSize; i++)
	{
		framebuffer[i] = color;
	}
}

void clearFramebufferSIMD(u32 color)
{
    v128_t colorVec = wasm_i32x4_splat(color);

    for (i32 i = 0; i < viewportSize; i += 4)
    {
        wasm_v128_store(&framebuffer[i], colorVec);
    }

    // TODO: Handle the remaining pixels if the viewport size is not a multiple of 4
    //for (; i < viewportSize; i++)
    //{
        //framebuffer[i] = color;
    //}
}

void drawRect(i32 x, i32 y, i32 width, i32 height, u32 color)
{
	for (i32 i = ClampBottom(y, 0); i < ClampTop(y+height, viewportHeight); i++)
	{
		for (i32 j = ClampBottom(x, 0); j < ClampTop(x+width, viewportWidth); j++)
		{
			framebuffer[i*viewportWidth + j] = color;
		}
	}
}

void drawRectSIMD(i32 x, i32 y, i32 width, i32 height, u32 color)
{
    i32 startX = ClampBottom(x, 0);
    i32 endX = ClampTop(x + width, viewportWidth);
    i32 startY = ClampBottom(y, 0);
    i32 endY = ClampTop(y + height, viewportHeight);

    v128_t colorVec = wasm_u32x4_splat(color);

    for (i32 i = startY; i < endY; i++)
    {
        i32 rowStart = i * viewportWidth + startX;
        i32 rowEnd = i * viewportWidth + endX;

        for (i32 j = rowStart; j < rowEnd; j += 4)
        {
            wasm_v128_store(&framebuffer[j], colorVec);
        }

        // TODO: Handle the remaining pixels if the width is not a multiple of 4
//        for (i32 j = rowEnd - (rowEnd % 4); j < rowEnd; j++)
//        {
//            framebuffer[j] = color;
//        }
    }
}

// NOTE: assuming only horizontal and vertical
void drawLine(i32 x1, i32 y1, i32 x2, i32 y2, u32 color)
{
	if (y1 == y2)
	{
		for (i32 i = ClampBottom(x1, 0); i < ClampTop(x2, viewportWidth); i++)
		{
			framebuffer[y1*viewportWidth + i] = color;
		}
	}
	else if (x1 == x2)
	{
		for (i32 i = ClampBottom(y1, 0); i < ClampTop(y2, viewportWidth); i++)
		{
			framebuffer[i*viewportWidth + x1] = color;
		}
	}
}

void drawLineHoz(i32 x1, i32 y1, i32 x2, i32 y2, u32 color)
{
	y1 = Clamp(y1, 0, viewportHeight);
	for (i32 i = ClampBottom(x1, 0); i < ClampTop(x2, viewportWidth); i++)
	{
		framebuffer[y1*viewportWidth + i] = color;
	}
}

void putPixel(i32 x, i32 y, u32 color)
{
	x = Clamp(x, 0, viewportWidth);
	y = Clamp(y, 0, viewportHeight);
	framebuffer[y*viewportWidth + x] = color;
}

#define COMPRESSED_FONT_LEN 622
static u8 COMPRESSED_FONT[COMPRESSED_FONT_LEN] = {
	0x00, 0x11, 0x20, 0xa1, 0x41, 0x0c, 0x0e, 0x08, 0x08, 0x40, 0x00, 0x05, 0x38, 0x20, 0x00, 0x01,
	0x20, 0xa1, 0x43, 0xcc, 0x92, 0x08, 0x10, 0x21, 0x50, 0x80, 0x00, 0x02, 0x02, 0x44, 0x60, 0x00,
	0x01, 0x20, 0x03, 0xe5, 0x01, 0x14, 0x00, 0x01, 0x20, 0x10, 0xe0, 0x80, 0x00, 0x02, 0x04, 0x4c,
	0xa0, 0x00, 0x01, 0x20, 0x01, 0x43, 0x82, 0x08, 0x00, 0x01, 0x20, 0x11, 0xf3, 0xe0, 0x0f, 0x80,
	0x08, 0x54, 0x20, 0x00, 0x01, 0x20, 0x03, 0xe1, 0x44, 0x15, 0x00, 0x01, 0x20, 0x10, 0xe0, 0x81,
	0x00, 0x02, 0x10, 0x64, 0x20, 0x00, 0x02, 0x01, 0x47, 0x89, 0x92, 0x00, 0x01, 0x10, 0x21, 0x50,
	0x81, 0x00, 0x02, 0x20, 0x44, 0x20, 0x00, 0x01, 0x20, 0x01, 0x41, 0x01, 0x8d, 0x00, 0x01, 0x08,
	0x40, 0x00, 0x01, 0x02, 0x00, 0x01, 0x04, 0x00, 0x01, 0x38, 0xf8, 0x00, 0x20, 0x38, 0x70, 0x63,
	0xe3, 0x8f, 0x8e, 0x1c, 0x00, 0x04, 0x07, 0x0e, 0x1c, 0x78, 0x70, 0x44, 0x88, 0xa2, 0x04, 0x00,
	0x01, 0x91, 0x22, 0x10, 0x20, 0x20, 0x02, 0x08, 0x91, 0x22, 0x44, 0x88, 0x04, 0x09, 0x22, 0x04,
	0x01, 0x11, 0x22, 0x00, 0x02, 0x43, 0xe1, 0x08, 0x97, 0x22, 0x44, 0x80, 0x08, 0x31, 0xf3, 0xc7,
	0x82, 0x0e, 0x1e, 0x00, 0x02, 0x80, 0x00, 0x01, 0x81, 0x15, 0x3e, 0x78, 0x80, 0x10, 0x08, 0x20,
	0x24, 0x44, 0x11, 0x02, 0x00, 0x01, 0x20, 0x43, 0xe1, 0x02, 0x17, 0x22, 0x44, 0x80, 0x20, 0x88,
	0x20, 0x24, 0x44, 0x11, 0x02, 0x10, 0x20, 0x20, 0x02, 0x00, 0x01, 0x10, 0x22, 0x44, 0x88, 0x7c,
	0x70, 0x23, 0xc3, 0x84, 0x0e, 0x1c, 0x00, 0x01, 0x40, 0x00, 0x02, 0x02, 0x0e, 0x22, 0x78, 0x70,
	0x00, 0x20, 0x78, 0xf9, 0xf1, 0xc4, 0x4f, 0x9f, 0x22, 0x40, 0x89, 0x11, 0xc7, 0x87, 0x1e, 0x1e,
	0x7c, 0x88, 0x44, 0x81, 0x02, 0x24, 0x42, 0x01, 0x22, 0x40, 0xd9, 0x12, 0x24, 0x48, 0x91, 0x20,
	0x10, 0x88, 0x44, 0x81, 0x02, 0x04, 0x42, 0x01, 0x24, 0x40, 0xa9, 0x92, 0x24, 0x48, 0x91, 0x20,
	0x10, 0x88, 0x44, 0xf1, 0xe2, 0x07, 0xc2, 0x01, 0x38, 0x40, 0x89, 0x52, 0x27, 0x88, 0x9e, 0x1c,
	0x10, 0x88, 0x44, 0x81, 0x02, 0x64, 0x42, 0x01, 0x24, 0x40, 0x89, 0x32, 0x24, 0x0a, 0x91, 0x02,
	0x10, 0x88, 0x44, 0x81, 0x02, 0x24, 0x42, 0x11, 0x22, 0x40, 0x89, 0x12, 0x24, 0x09, 0x11, 0x02,
	0x10, 0x88, 0x78, 0xf9, 0x01, 0xc4, 0x4f, 0x8e, 0x22, 0x7c, 0x89, 0x11, 0xc4, 0x06, 0x91, 0x3c,
	0x10, 0x70, 0x00, 0x20, 0x44, 0x89, 0x12, 0x27, 0xc3, 0x00, 0x01, 0x18, 0x10, 0x00, 0x01, 0x80,
	0x04, 0x00, 0x01, 0x01, 0x00, 0x01, 0x18, 0x00, 0x01, 0x44, 0x89, 0x12, 0x20, 0x42, 0x10, 0x08,
	0x28, 0x00, 0x01, 0x40, 0x04, 0x00, 0x01, 0x01, 0x00, 0x01, 0x20, 0x00, 0x01, 0x44, 0x88, 0xa1,
	0x40, 0x82, 0x08, 0x08, 0x00, 0x02, 0x01, 0xc7, 0x87, 0x0f, 0x1c, 0x7c, 0x78, 0x44, 0x88, 0x40,
	0x81, 0x02, 0x04, 0x08, 0x00, 0x03, 0x24, 0x48, 0x91, 0x22, 0x20, 0x88, 0x44, 0xa8, 0xa0, 0x82,
	0x02, 0x02, 0x08, 0x00, 0x02, 0x01, 0xe4, 0x48, 0x11, 0x3e, 0x20, 0x78, 0x28, 0xd9, 0x10, 0x84,
	0x02, 0x01, 0x08, 0x00, 0x02, 0x02, 0x24, 0x48, 0x91, 0x20, 0x20, 0x08, 0x10, 0x89, 0x10, 0x87,
	0xc3, 0x00, 0x01, 0x18, 0x00, 0x01, 0xf8, 0x01, 0xe7, 0x87, 0x0f, 0x1e, 0x20, 0x70, 0x00, 0x20,
	0x40, 0x20, 0x12, 0x04, 0x00, 0x06, 0x02, 0x00, 0x05, 0x40, 0x00, 0x01, 0x02, 0x04, 0x00, 0x06,
	0x02, 0x00, 0x05, 0x78, 0xe0, 0x72, 0x44, 0x0d, 0x1e, 0x1c, 0x78, 0x79, 0x61, 0xe7, 0x88, 0x91,
	0x22, 0x44, 0x88, 0x44, 0x20, 0x13, 0x84, 0x0a, 0x91, 0x22, 0x44, 0x89, 0x92, 0x02, 0x08, 0x91,
	0x22, 0x28, 0x88, 0x44, 0x20, 0x12, 0x44, 0x0a, 0x91, 0x22, 0x78, 0x79, 0x01, 0xc2, 0x08, 0x91,
	0x22, 0x10, 0x78, 0x44, 0x21, 0x12, 0x24, 0x08, 0x91, 0x22, 0x40, 0x09, 0x00, 0x01, 0x22, 0x48,
	0x8a, 0x2a, 0x28, 0x08, 0x44, 0xf8, 0xe2, 0x23, 0x88, 0x91, 0x1c, 0x40, 0x09, 0x03, 0xc1, 0x87,
	0x84, 0x14, 0x44, 0x70, 0x00, 0x21, 0x10, 0x41, 0x00, 0x0e, 0x20, 0x40, 0x80, 0x00, 0x0c, 0x7c,
	0x20, 0x40, 0x82, 0x40, 0x00, 0x0b, 0x08, 0x40, 0x40, 0x45, 0x80, 0x00, 0x0b, 0x10, 0x20, 0x40,
	0x80, 0x00, 0x0c, 0x20, 0x20, 0x40, 0x80, 0x00, 0x0c, 0x7c, 0x10, 0x41, 0x00, 0xbd,
};

#define FONT_IMAGE_WIDTH 128
#define FONT_IMAGE_HEIGHT 64

#define FONT_IMAGE_COLS 18
#define FONT_IMAGE_ROWS 7

#define FONT_CHAR_WIDTH		FONT_IMAGE_WIDTH / FONT_IMAGE_COLS
#define FONT_CHAR_HEIGHT	FONT_IMAGE_HEIGHT / FONT_IMAGE_ROWS

#define BITS_IN_BYTE 8

static u8 FONT[FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT];

static void decompressFontFromBytes()
{
	i32 n = COMPRESSED_FONT_LEN;
	i32 i = 0;
	i32 pixelsSize = 0;

	while (i < n)
	{
		u8 byte = COMPRESSED_FONT[i];
		if (byte == 0x00)
		{
			i++;
			if (i < n)
			{
				u8 nextByte = COMPRESSED_FONT[i];
				pixelsSize += nextByte * 8;
			}
			else
			{
				break;
			}
			i++;
		}
		else
		{
			for (i32 bitIndex = 0; bitIndex < BITS_IN_BYTE; bitIndex++)
			{
				if (pixelsSize < FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT)
				{
					FONT[pixelsSize] = ((byte >> (BITS_IN_BYTE - bitIndex - 1)) & 1) * 0xFF;
				}
				else
				{
					break;
				}
				pixelsSize++;
			}
			i++;
		}
	}
}

// TODO: make faster pweamz :) ^_^
static void renderAscii(u8 code, i32 x, i32 y, i32 scale, u32 color)
{
	if (code >= 32 && code <= 126)
	{
		i32 charCol = (code - 32) % FONT_IMAGE_COLS;
		i32 charRow = (code - 32) / FONT_IMAGE_COLS;

		for (i32 charY = 0; charY < FONT_CHAR_HEIGHT; charY++)
		{
			for (i32 charX = 0; charX < FONT_CHAR_WIDTH; charX++)
			{
				for (i32 scaleX = 0; scaleX < scale; scaleX++)
				{
					for (i32 scaleY = 0; scaleY < scale; scaleY++)
					{
						i32 fontX = charCol * FONT_CHAR_WIDTH + charX;
						i32 fontY = charRow * FONT_CHAR_HEIGHT + charY;
						i32 viewportX = x + charX * scale + scaleX;
						i32 viewportY = y + charY * scale + scaleY;

						u8 alpha = FONT[fontY * FONT_IMAGE_WIDTH + fontX];
						if (alpha == 0xFF)
						{
							putPixel(viewportX, viewportY, color);
						}
					}
				}
			}
		}
	}
	else
	{
		renderAscii((u8)'?', x, y, scale, color);
	}
}

static void renderBytes(u8 *bytes, i32 len, i32 x, i32 y, i32 scale, u32 color)
{
	for (i32 i = 0; i < len; i++)
	{
		renderAscii( * (bytes + i), (x + i * FONT_CHAR_WIDTH * scale), y, scale, color);
	}
}

static void renderString(i32 index, i32 x, i32 y, i32 scale, u32 color)
{
	renderBytes((u8 *)strings[index], getStringLen(index), x, y, scale, color);
}

static i32 getTextSize(char *text, i32 scale)
{
	return getTextLen(text) * scale * 7;
}

static void renderText(char *text, i32 x, i32 y, i32 scale, u32 color)
{
	renderBytes((u8 *)text, getTextLen(text), x, y, scale, color);
}

static void u32ToStr(u32 number, char *string) {
    int i = 0;

    if (number == 0) {
        string[i++] = '0';
        string[i] = '\0';
        return;
    }

    while (number != 0) {
        string[i++] = (number % 10) + '0';
        number /= 10;
    }

    string[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = string[j];
        string[j] = string[k];
        string[k] = temp;
    }
}

static void i32ToStr(i32 number, char *string) {
    int i = 0;
    bool isNegative = false;

    if (number == 0) {
        string[i++] = '0';
        string[i] = '\0';
        return;
    }

    if (number < 0) {
        isNegative = true;
        number = -number;
        string[i++] = '-';
    }

    while (number != 0) {
        string[i++] = (number % 10) + '0';
        number /= 10;
    }

    if (isNegative) {
        for (int j = i - 1, k = i; j >= 1; j--, k--) {
            string[k] = string[j];
        }
        string[0] = '-';
    } else {
        for (int j = 0, k = i - 1; j < k; j++, k--) {
            char temp = string[j];
            string[j] = string[k];
            string[k] = temp;
        }
    }

    string[i] = '\0';
}

static void UI_init()
{
	hotItem = 0;
	switchFocus = frameInput.keyboard.tab.justPressed;
	keyboardSelect = frameInput.keyboard.enter.justPressed;
}

static void UI_cleanup()
{
	if (frameInput.mouse.leftMB.endedDown == 0)
	{
		activeItem = 0;
	}
	else
	{
		activeItem = activeItem == 0 ? -1 : activeItem;
	}
	if (switchFocus)
		kbdItem = 0;
	switchFocus = 0;
	// NOTE: I don't think we have to zero keyMod, cause it only appears with keyEntered

	// TODO: I don't like this solution at all!
	mouseWheelDown = 0;
	mouseWheelUp = 0;
}

static byte mouseInRect(i32 x, i32 y, i32 w, i32 h)
{
	byte hit = 1;
	hit = (mouseX < x) ? 0 : hit;
	hit = (mouseY < y) ? 0 : hit;
	hit = (mouseX >= x + w) ? 0 : hit;
	hit = (mouseY >= y + h) ? 0 : hit;
	
	return hit;
}

//////////////////////////////////////////////////////////////////
// EXPORTS
//////////////////////////////////////////////////////////////////
u32 getFramebufferAddr()
{
	return (u32)&framebuffer;
}

void setViewportDimensions(i32 newWidth, i32 newHeight, i32 newSize)
{
	viewportWidth = newWidth;
	viewportHeight = newHeight;
	viewportSize = newSize;
}

void pauseGame() 
{
	isPaused = 1;
}

u32 getDataAddress()
{
	return (u32)&data;
}

u32 getDataSize()
{
	return sizeof(data);
}

u32 getDataCount()
{
	return MAX_SIZE;
}

u32 getInputAddress()
{
	return (u32)&frameInput;
}

u32 getInputSize()
{
	return sizeof(frameInput);
}

u32 getKeyboardInputSize()
{
	return sizeof(KeyboardInput);
}

u32 getButtonStateSize()
{
	return sizeof(ButtonState);
}

void mouseMove(i32 x, i32 y)
{
	mouseX = x;
	mouseY = y;
}

void mouseScrollWheelUp()
{
	mouseWheelUp = 1;
}

void mouseScrollWheelDown()
{
	mouseWheelDown = 1;
}

//void* myMalloc(i32 n) {
//	u32 r = bump_pointer;
//	bump_pointer += n;
//	return (void *)r;
//}

//unsigned char* getMemory() {
//    return &__heap_base;
//}

static byte UI_button(i32 id, char *label, i32 x, i32 y)
{
	i16 labelWidth = getTextSize(label, 2);
	i16 buttonWidth = Max(labelWidth+18, 48);
	i16 buttonHeight = 48;
	byte wasPressed = 0;

	if (kbdItem == 0)
	{
		kbdItem = id;
	}
	if (mouseInRect(x, y, buttonWidth, buttonHeight))
	{
		hotItem = id;
		if (activeItem == 0 && frameInput.mouse.leftMB.justPressed) // mouseLeftClicked)
			activeItem = id;
	}

	u32 buttonColor = 0xffffff;
	if (hotItem == id && activeItem == id)
	{
		buttonColor = 0xffaaaaaa;
	}
	if (hotItem == id && activeItem != id)
	{
		buttonColor = 0xffcccccc;
	}
	drawRect(x, y, buttonWidth, buttonHeight, buttonColor);
	
	i8 outlineWidth = (kbdItem == id) ? 4 : 2;
	drawRect(x-outlineWidth/2, y-outlineWidth/2, outlineWidth, buttonHeight+outlineWidth/2, 0xff000000);
	drawRect(x-outlineWidth/2, y-outlineWidth/2, buttonWidth+outlineWidth/2, outlineWidth, 0xff000000);
	drawRect(x+buttonWidth-outlineWidth/2, y-outlineWidth/2, outlineWidth, buttonHeight+outlineWidth, 0xff000000);
	drawRect(x-outlineWidth/2, y+buttonHeight-outlineWidth/2, buttonWidth+outlineWidth/2, outlineWidth, 0xff000000);
	
	if (label)
	{
		renderText(label, x + (buttonWidth - labelWidth)/2, y+15, 2, 0xff000000);
	}

	if (kbdItem == id)
	{
		if (switchFocus)
		{
			kbdItem = frameInput.keyboard.shift.endedDown ? lastItem : 0;
			switchFocus = 0;
		}
		if (keyboardSelect)
		{
			wasPressed = 1;
		}
	}
	lastItem = id;

	wasPressed = wasPressed || (frameInput.mouse.leftMB.endedDown == 0 && hotItem == id && activeItem == id);
	return wasPressed;
}

////////////////////
// IMPORTS
////////////////////
extern void callWindowAlert();
extern void logU32(u32 number);
extern u32 getMemoryCapacity();
extern b32 getIsApplicationFocused();
extern f32 js_sqrtf(f32 number);



static i32 cameraX = 0;
static i32 cameraY = 0;


static inline i32 absI32(i32 num) {
    i32 mask = num >> 31;
    return (num ^ mask) - mask;
}

void init()
{
	decompressFontFromBytes();
	clearFramebuffer(0xff0000ff);

	for (i32 i = 0; i < MAX_SIZE; i++)
	{
	}
}

static u32 FPSBuffer[1024];
static f32 dtBuffer[1024];

u32 getMaxViewportWidth()
{
	return MAX_VIEWPORT_WIDTH;
}

u32 getMaxViewportHeight()
{
	return MAX_VIEWPORT_HEIGHT;
}

void doFrame(f32 dt)
{
	if (frameInput.keyboard.space.justPressed) isPaused = !isPaused; 
	if (isPaused) 
	{
		drawRect(330, 530, getTextSize("GAME PAUSED. PRESS SPACE TO RESUME", 8)+240, 240, 0xff000000);
		drawRect(350, 550, getTextSize("GAME PAUSED. PRESS SPACE TO RESUME", 8)+200, 200, 0xff);
		renderText("GAME PAUSED. PRESS SPACE TO RESUME", 400, 600, 8, 0xff000000);
		return;
	}

	if (frameInput.keyboard.digit1.endedDown)
	{
	}
	if (frameInput.keyboard.digit2.endedDown)
	{
	}
	if (frameInput.keyboard.digit3.endedDown)
	{
	}
	if (frameInput.keyboard.digit4.endedDown)
	{
	}
	if (frameInput.keyboard.digit5.endedDown)
	{
	}

	if (frameInput.keyboard.arrowLeft.endedDown)
	{
	}
	if (frameInput.keyboard.arrowUp.endedDown)
	{
	}
	if (frameInput.keyboard.arrowRight.endedDown)
	{
	}
	if (frameInput.keyboard.arrowDown.endedDown)
	{
	}

	if (mouseWheelUp)
	{
	}
	if (mouseWheelDown)
	{
	}

	if (frameInput.mouse.leftMB.justPressed)
	{
	}
	if (frameInput.mouse.rightMB.justPressed)
	{
	}
	
	// UPDATE
	for (i32 i = 0; i < MAX_SIZE; i++)
	{
	}
	
	// imgui setup
	UI_init();	// TODO: inline?

	// clear framebuffer
	clearFramebufferSIMD(0xff8956aa); // AABBBGGRR? is this some endian stuff?	

	// RENDER
	for (i32 i = 0; i < MAX_SIZE; i++)
	{
		i32 screenX = (i / (f32)MAX_SIZE) * viewportWidth;
		i32 screenY = (1.0f - (data.value[i] / (f32)MAX_SIZE)) * viewportHeight;

		putPixel(screenX, screenY, 0xff0000ff);
		//drawRect(screenX, screenY, 4, 4, 0xff0000ff);
	}

	// UI
	static u32 test = 0xff0000ff;
	static byte e = 0;
	UI_button(GEN_ID, "Ok", 50, 150);
	if (UI_button(GEN_ID, "Change that rect color, why not!", 150, 150))
	{
		e = e ? 0 : 1;
		test = e ? 0xffffff00 : 0xff0000ff;
	}
	UI_button(GEN_ID, "(btw: Tab and Shift+Tab work)", 50, 300);

	// DEBUGGING
	u32 dbgFontColor = 0xff000000;
	u32ToStr(viewportWidth, strings[20]);
	renderText("vwprt_width:", 1050, 10, 2, dbgFontColor);
	renderString(20, 1240, 5, 4, dbgFontColor);

	u32ToStr(viewportHeight, strings[21]);
	renderText("vwprt_height:", 1050, 50, 2, dbgFontColor);
	renderString(21, 1240, 45, 4, dbgFontColor);

	renderText("No. Entities: ", 50, 550, 2, 0xff000000);
	u32ToStr(MAX_SIZE, strings[32]);
	renderString(32, 270, 550, 2, 0xff000000);

	i32ToStr(mouseX, strings[40]);
	i32ToStr(mouseY, strings[41]);
	renderText("mouse.x:", 50, 590, 2, 0xff000000);
	renderString(40, 270, 590, 2, 0xff000000);
	renderText("mouse.y:", 50, 620, 2, 0xff000000);
	renderString(41, 270, 620, 2, 0xff000000);


	u32 FPSNumToDisplay = 0;
	for (i32 i = 0; i < 100; i++)
	{
		FPSNumToDisplay += FPSBuffer[i];
	}
	FPSNumToDisplay /= 100;
	u32ToStr(FPSNumToDisplay, strings[22]);

	// imgui clear
	UI_cleanup();
}

