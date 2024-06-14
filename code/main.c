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
typedef struct Character
{
	i32 x;
	i32 y;
	b32 active;
	i32 coolness;
} Character;

#define MAX_SIZE 4096 // 65536 // 32768
//static Character data[MAX_SIZE];
//static i32 freeIndices[MAX_SIZE];
//static i32 numFreeIndices = MAX_SIZE;

// go from AoS to Soa
// AoSoA would be array where one item is 4*entitiesData?
static struct entitiesData
{
	i32 x[MAX_SIZE] __attribute__((aligned(16)));
	i32 y[MAX_SIZE] __attribute__((aligned(16)));
} entitiesData;

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

typedef struct MouseInput
{
	// x and y handled as seperate statics
	union
	{
		ButtonState buttons[3];
		struct
		{
			ButtonState leftMB;
			ButtonState rightMB;
			ButtonState middleMB;
		};
	};
} MouseInput;

typedef struct KeyboardInput
{
	union
	{
		ButtonState buttons[8];
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
		};
	};
	ButtonState shift;
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

    // Load the color into a SIMD register
    v128_t colorVec = wasm_u32x4_splat(color);

    for (i32 i = startY; i < endY; i++)
    {
        i32 rowStart = i * viewportWidth + startX;
        i32 rowEnd = i * viewportWidth + endX;

        // Process 4 pixels at a time
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

static void UI_init()
{
	hotItem = 0;
	switchFocus = frameInput.keyboard.tab.justPressed;
	keyboardSelect = frameInput.keyboard.enter.justPressed;
}

static void UI_cleanup()
{
	if (mouseLeftClicked == 0)
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

u32 getInputAddress()
{
	return (u32)&frameInput;
}

u32 getInputSize()
{
	return sizeof(frameInput);
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

void mouseDown()
{
	mouseLeftClicked = 1;
}

void mouseUp()
{
	mouseLeftClicked = 0;
}

void* myMalloc(i32 n) {
	u32 r = bump_pointer;
	bump_pointer += n;
	return (void *)r;
}

unsigned char* getMemory() {
    return &__heap_base;
}

u32 calculateUsedMemory()
{
	// TODO: implement a macro to automate mem tracking
	// The below is pasted from chat, I feel that I can make it better, by
	// having like a dedicated array or struct or whatever for this, but it's cool
	// #define TRACK_ARRAY(name, size, type) \
    //  static type name[size]; \
	//  static const size_t name##_size = (size * sizeof(type));
	u32 numOfUsedBytes = 0;
	numOfUsedBytes += NUM_STRINGS * STRING_LEN * sizeof(char);
	numOfUsedBytes += MAX_SIZE * sizeof(i32);
	numOfUsedBytes += MAX_SIZE * sizeof(Character);
	numOfUsedBytes += MAX_VIEWPORT_WIDTH * MAX_VIEWPORT_HEIGHT * sizeof(u32);
	numOfUsedBytes += COMPRESSED_FONT_LEN + sizeof(u8);
	numOfUsedBytes += FONT_IMAGE_WIDTH * FONT_IMAGE_HEIGHT * sizeof(u8);

	return numOfUsedBytes;
}

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
		if (activeItem == 0 && mouseLeftClicked)
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

	wasPressed = wasPressed || (mouseLeftClicked == 0 && hotItem == id && activeItem == id);
	return wasPressed;
}

////////////////////
// IMPORTS
////////////////////
extern void callWindowAlert();
extern u32 getMemoryCapacity();
extern b32 getIsApplicationFocused();

// IMPORTANT: tileEdgeInUnits signifies the number of the smallest addressable unit of size in program - you can't go lower than exactly one 'unit'
static i32 tileEdgeInUnits = 16;
static i32 tileWidth = 50;
static i32 tileHeight = 29; // NOTE: width*sqrt(3)/3
// 		tileWidth / tileEdgeInUnits = what is it? how many pixels to express a unit that is horizontal? (vertical for height)
static i32 pixelsPerHorizontalUnit = 3;
static i32 pixelsPerVerticalUnit = 2;
//static i32 tileSide = 115;

static i32 numTilesHorizontally = 100;
static i32 numTilesVertically = 300;

static i32 mapWidth;
static i32 mapHeight;

static i32 cameraX = 0;
static i32 cameraY = 0;
static f32 pixelsPerUnit = 1; // this is a display related value - how many pixels on screen to represent a (well, not sure what it is now, one 'pixel' in game?)

/*
 * NOTE:
 * 
 * entity.x and entity.y - raw coordinates in atomic units (not a tile, not where on tile, but just where on map)
 * tile - coordinates of tile on grid, from 0 to numTiles, easily gettable by x / tileEdgeInUnits
 * do we want a distinct one for where relatively on a tile something is? But that's just easily gettable by x % tileEdgeInUnits...
 * and I guess the last one is where something is on screen
 * 
 */

// THOUGHT: regarding that conversion to isometric - since we're going int only, is the formula for this doable in a simplified way..?
// something like screenX = x*const1 - y*const2?
typedef struct Point
{
	i32 x;
	i32 y;
} Point;

typedef struct Tile
{
	i32 x;
	i32 y;
} Tile;

// Only need one conversion - raw coord to screen (remember that we're doing all the logic on an honest-to-God simple straight grid)
static Point pointToScreenPos(Point point)
{
	Point screenPos = {};
	// TODO: NO IDEA IF THE BELOW WORKS CORRECTLY
	// TODO: Musisz umieć odpowiedzieć na ważne pytanie - jak się ma ten atomic unit do reprezentacji na ekranie (ale na razie mozesz uproscic przypadek)
	//screenPos.x = (point.x / tileEdgeInUnits) * (tileWidth / 2) - (point.y / tileEdgeInUnits) * (tileWidth / 2); // + (numTilesHorizontally * tileWidth / 2); // TODO: numTilesHorizontally or Vertically?
	//screenPos.y = (point.y / tileEdgeInUnits) * (tileHeight / 2) + (point.x / tileEdgeInUnits) * (tileHeight / 2);
	screenPos.x = (point.x - point.y) * pixelsPerHorizontalUnit;
	screenPos.x = (screenPos.x - cameraX); // * pixelsPerUnit;

	screenPos.y = (point.x + point.y)  * pixelsPerVerticalUnit; // SHOULDNT this and the above be divided by tileEdgeInUnits?
	screenPos.y = (screenPos.y - cameraY); // * pixelsPerUnit;

	return screenPos;
}

// TODO: SIMD Version
// TODO: ERROR: should return a new value, you can't just modify the X u donkey!
// TODO: maybe do divide it into two functions just for now for clarity: rawToIsometric -> isometricToScreen
// TODO: SCRATCH THE ABOVE, do the thing from the notebook (no isometric (we may do a perspective twist - vertical is 'slower' than horizontal, coz of the way we're looking at the map))
static v128_t pointToScreenPosXVec(v128_t vecX, v128_t vecY)
{
	v128_t vecCameraX = wasm_i32x4_splat(cameraX);
	//v128_t vecTileEdgeInUnits = wasm_i32x4_splat(tileEdgeInUnits);
	v128_t vecTileWidthHalf = wasm_i32x4_splat(tileWidth / 2);
	v128_t vecHorizontal = wasm_i32x4_splat(pixelsPerHorizontalUnit);
	//v128_t vecTip = wasm_i32x4_splat(numTilesHorizontally * tileWidth / 2);
	//v128_t vecPixelsPerUnit = wasm_i32x4_splat(pixelsPerUnit);
	//v128_t vecTemp = wasm_i32x4_sub(wasm_i32x4_mul(vecX, vecTileWidthHalf), wasm_i32x4_mul(vecY, vecTileWidthHalf));
	v128_t vecTemp = wasm_i32x4_mul(wasm_i32x4_sub(vecX, vecY), vecHorizontal);
	//vecTemp = wasm_i32x4_add(vecTemp, vecTip);
	return wasm_i32x4_sub(vecTemp, vecCameraX);
}

static v128_t pointToScreenPosYVec(v128_t vecX, v128_t vecY)
{
	v128_t vecCameraY = wasm_i32x4_splat(cameraY);
	//v128_t vecTileEdgeInUnits = wasm_i32x4_splat(tileEdgeInUnits);
	v128_t vecTileHeightHalf = wasm_i32x4_splat(tileHeight / 2);
	v128_t vecVertical = wasm_i32x4_splat(pixelsPerVerticalUnit);
	//v128_t vecPixelsPerUnit = wasm_i32x4_splat(pixelsPerUnit);
	//v128_t vecTemp = wasm_i32x4_add(wasm_i32x4_mul(vecY, vecTileHeightHalf), wasm_i32x4_mul(vecX, vecTileHeightHalf));
	v128_t vecTemp = wasm_i32x4_mul(wasm_i32x4_add(vecX, vecY), vecVertical);
	return wasm_i32x4_sub(vecTemp, vecCameraY);
}

static Point screenToMapPosition(Point mousePosition)
{
//	Point screenPos = {};
//
//	screenPos.x = (point.x - point.y) * pixelsPerHorizontalUnit;
//	screenPos.x = (screenPos.x - cameraX); 
//
//	screenPos.y = (point.x + point.y)  * pixelsPerVerticalUnit;
//	screenPos.y = (screenPos.y - cameraY);
//
//	return screenPos;

	//

	Point mapPos = {};
	mapPos.x = ((mousePosition.x + cameraX) / pixelsPerHorizontalUnit + (mousePosition.y + cameraY) / pixelsPerVerticalUnit) / 2;
	mapPos.y = ((mousePosition.y + cameraY) / pixelsPerVerticalUnit - (mousePosition.x + cameraX) / pixelsPerHorizontalUnit) / 2;
	return mapPos;
}

static i32 mapToScreenX(i32 x)
{
	return (x - cameraX) * pixelsPerUnit;
}

static i32 mapToScreenY(i32 y)
{
	return (y - cameraY) * pixelsPerUnit;
}

static v128_t mapToScreenVecX(v128_t vecX)
{
	v128_t vecCameraX = wasm_i32x4_splat(cameraX);
	v128_t vecPixelsPerUnit = wasm_i32x4_splat(pixelsPerUnit);
	return wasm_i32x4_mul(wasm_i32x4_sub(vecX, vecCameraX), vecPixelsPerUnit);
}

static v128_t mapToScreenVecY(v128_t vecY)
{
	v128_t vecCameraY = wasm_i32x4_splat(cameraY);
	v128_t vecPixelsPerUnit = wasm_i32x4_splat(pixelsPerUnit);
	return wasm_i32x4_mul(wasm_i32x4_sub(vecY, vecCameraY), vecPixelsPerUnit);
}

static void drawIsometricTileOutline(i32 x, i32 y)
{
	u32 color = 0xff0000ff;
	f32 topX = (x * tileWidth / 2) - (y * tileWidth / 2) + (numTilesVertically * tileWidth / 2);
	f32 topY = (y * tileHeight / 2) + (x * tileHeight / 2);
	// draw top to right
	x = topX;
	y = topY;
	f32 accumulator = 0;
	f32 ratio = (f32)tileHeight / (f32)tileWidth;
	i32 rectWidth = 4;
	drawRect(mapToScreenX(topX - rectWidth), mapToScreenY(topY - rectWidth), rectWidth, rectWidth, color);
}

static i32 playerId;
static Character player = { .active = 1, .coolness = 9999, .x = 1200, .y = 800 };

void init()
{
	mapWidth = ((f32)(numTilesHorizontally + numTilesVertically) / 2.0f) * tileWidth;
	mapHeight = ((f32)(numTilesHorizontally + numTilesVertically) / 2.0f) * tileHeight;

	decompressFontFromBytes();
	clearFramebuffer(0xff0000ff);
	//initData();
	//playerId = insert(player);
	for (i32 i = 0; i < MAX_SIZE; i++)
	{
		entitiesData.x[i] = (i % numTilesHorizontally) * tileEdgeInUnits;
		entitiesData.y[i] = (i / numTilesHorizontally) * tileEdgeInUnits;
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

i32 absI32(i32 num)
{
	return (num < 0) ? -num : num;
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

	// TODO: Perhaps do like some enum for all the keycodes?
	i32 cameraSpeed = 1;
	i32 playerSpeed = 1;
	i32 enemySpeed = 1;
	if (frameInput.keyboard.arrowLeft.endedDown)
	{
		cameraX -= cameraSpeed * dt;
	}
	if (frameInput.keyboard.arrowUp.endedDown)
	{
		cameraY -= cameraSpeed * dt;
	}
	if (frameInput.keyboard.arrowRight.endedDown)
	{
		cameraX += cameraSpeed * dt;
	}
	if (frameInput.keyboard.arrowDown.endedDown)
	{
		cameraY += cameraSpeed * dt;
	}

	// Mouse control for player movement
	static Point moveTarget = {};
	static b32 shouldMove = false;

	// TODO: check this out, doesn't work
	//if (frameInput.mouse.leftMB.justPressed)
	if (mouseLeftClicked)
	{
		Point mouseScreenPosition = { mouseX, mouseY };
		moveTarget = screenToMapPosition(mouseScreenPosition);
		shouldMove = true;
	}

	if (shouldMove)
	{
		i32 deltaX = (moveTarget.x > player.x) ? playerSpeed * dt : -playerSpeed * dt;
		i32 deltaY = (moveTarget.y > player.y) ? playerSpeed * dt : -playerSpeed * dt;
		player.x += deltaX / 4;
		player.y += deltaY / 4;
		if (absI32(moveTarget.x - player.x) + absI32(moveTarget.y - player.y) < 10) shouldMove = 0;
	}
	
	// UPDATE
	v128_t vecPlayerX = wasm_i32x4_splat(player.x);
    v128_t vecPlayerY = wasm_i32x4_splat(player.y);
	v128_t vecPull = wasm_i32x4_splat(1);
	v128_t squareRadius = wasm_i32x4_splat(1000);
	for (i32 i = 0; i < MAX_SIZE; i += 4)
	{
		v128_t vecX = wasm_v128_load(&entitiesData.x[i]);
		v128_t vecY = wasm_v128_load(&entitiesData.y[i]);

		v128_t distX = wasm_i32x4_sub(vecX, vecPlayerX);
		v128_t distY = wasm_i32x4_sub(vecY, vecPlayerY);

		v128_t withinSquareX = wasm_i32x4_le(wasm_i32x4_abs(distX), squareRadius);
		v128_t withinSquareY = wasm_i32x4_le(wasm_i32x4_abs(distY), squareRadius);

		v128_t withinSquare = wasm_v128_and(withinSquareX, withinSquareY);

		v128_t vecDeltaX = wasm_i32x4_lt(vecX, vecPlayerX);
		v128_t vecDeltaY = wasm_i32x4_lt(vecY, vecPlayerY);
		vecDeltaX = wasm_v128_bitselect(wasm_i32x4_add(vecX, vecPull), wasm_i32x4_sub(vecX, vecPull), vecDeltaX);
		vecDeltaY = wasm_v128_bitselect(wasm_i32x4_add(vecY, vecPull), wasm_i32x4_sub(vecY, vecPull), vecDeltaY);

		v128_t moveX = wasm_v128_bitselect(vecDeltaX, vecX, withinSquare);
		v128_t moveY = wasm_v128_bitselect(vecDeltaY, vecY, withinSquare);

		wasm_v128_store(&entitiesData.x[i], moveX);
		wasm_v128_store(&entitiesData.y[i], moveY);
	}
	
	// imgui setup
	UI_init();

	// clear framebuffer
	clearFramebufferSIMD(0xff8956aa); // AABBBGGRR? is this some endian stuff?	
	drawRectSIMD(mapToScreenX(0), mapToScreenY(0), mapWidth*pixelsPerUnit, mapHeight*pixelsPerUnit, 0xfffffffd);

	// RENDER
	// TODO: Update to the new coord system
	i32 characterWidth = 32;
	i32 screenCharacterWidth = characterWidth * pixelsPerUnit;
	v128_t halfWidth = wasm_i32x4_splat(screenCharacterWidth / 2);
	v128_t vecCameraX = wasm_i32x4_splat(cameraX);
	v128_t vecCameraY = wasm_i32x4_splat(cameraY);
	v128_t vecPixelsPerUnit = wasm_i32x4_splat(pixelsPerUnit);
	u32 color = 0xff0066ff;
	i32 intermediateScreenX[4], intermediateScreenY[4];
	for (i32 i = 0; i < MAX_SIZE; i += 4)
	{
		v128_t vecX = wasm_v128_load(&entitiesData.x[i]);
		v128_t vecY = wasm_v128_load(&entitiesData.y[i]);


		v128_t screenX = pointToScreenPosXVec(vecX, vecY);
		v128_t screenY = pointToScreenPosYVec(vecX, vecY);

		// Commented out for a while, coz pixelsPerUnit is a float! And I AIN'T BOUT DAT LIFE
		//vecX = wasm_i32x4_mul(wasm_i32x4_sub(vecX, vecCameraX), vecPixelsPerUnit);
		//vecY = wasm_i32x4_mul(wasm_i32x4_sub(vecY, vecCameraY), vecPixelsPerUnit);

		//vecX = wasm_i32x4_sub(vecX, vecCameraX);
		//vecY = wasm_i32x4_sub(vecY, vecCameraY);
		//
		//v128_t screenX = wasm_i32x4_sub(vecX, halfWidth);
		//v128_t screenY = wasm_i32x4_sub(vecY, halfWidth);

		wasm_v128_store(&intermediateScreenX[0], screenX);
		wasm_v128_store(&intermediateScreenY[0], screenY);

		// TODO: Or make drawRect SIMD batch version
		drawRectSIMD(
			intermediateScreenX[0],
			intermediateScreenY[0],
			screenCharacterWidth / 2,
			screenCharacterWidth / 2,
			color
		);
		drawRectSIMD(
			intermediateScreenX[1],
			intermediateScreenY[1],
			screenCharacterWidth / 2,
			screenCharacterWidth / 2,
			color
		);
		drawRectSIMD(
			intermediateScreenX[2],
			intermediateScreenY[2],
			screenCharacterWidth / 2,
			screenCharacterWidth / 2,
			color
		);
		drawRectSIMD(
			intermediateScreenX[3],
			intermediateScreenY[3],
			screenCharacterWidth / 2,
			screenCharacterWidth / 2,
			color
		);

		// DEBUG
		i32 deltaX = 2000;
		i32 deltaY = 300;
		u32 debugColor = 0xff00aabb;
		drawRectSIMD(
			deltaX + (wasm_i32x4_extract_lane(vecX, 0) / 4),
			deltaY + (wasm_i32x4_extract_lane(vecY, 0) / 4),
			4,
			4,
			debugColor
		);
		drawRectSIMD(
			deltaX + (wasm_i32x4_extract_lane(vecX, 1) / 4),
			deltaY + (wasm_i32x4_extract_lane(vecY, 1) / 4),
			4,
			4,
			debugColor
		);
		drawRectSIMD(
			deltaX + (wasm_i32x4_extract_lane(vecX, 2) / 4),
			deltaY + (wasm_i32x4_extract_lane(vecY, 2) / 4),
			4,
			4,
			debugColor
		);
		drawRectSIMD(
			deltaX + (wasm_i32x4_extract_lane(vecX, 3) / 4),
			deltaY + (wasm_i32x4_extract_lane(vecY, 3) / 4),
			4,
			4,
			debugColor
		);
	}
	Point playerScreenPos = pointToScreenPos( (Point){ player.x, player.y });
	drawRect(playerScreenPos.x, 
			 playerScreenPos.y, 
			 screenCharacterWidth,
			 screenCharacterWidth,
			 0xff00ff00);
	// DEBUG
	i32 deltaX = 2000;
	i32 deltaY = 300;
	drawRectSIMD(
		deltaX + (player.x / 4),
		deltaY + (player.y / 4),
		8,
		8,
		0xff000000
	);

	// UI
	static u32 test = 0xff0000ff;
	static byte e = 0;
	UI_button(GEN_ID, "Ok", 50, 150);
	if (UI_button(GEN_ID, "Change that rect color, why not!", 150, 150))
	{
		e = e ? 0 : 1;
		test = e ? 0xffffff00 : 0xff0000ff;
	}
	static b32 debugShouldDrawOutlines = 0;
	if (UI_button(GEN_ID, debugShouldDrawOutlines ? "disable outlines" : "enable outlines", 700, 150))
	{
		debugShouldDrawOutlines = !debugShouldDrawOutlines;
	}
	UI_button(GEN_ID, "(btw: Tab and Shift+Tab work)", 50, 300);
	drawRect(595, 295, 74, 58, 0xff000000);
	drawRect(600, 300, 64, 48, test);

	
	if (debugShouldDrawOutlines)
	{
		for (i32 i = 0; i < numTilesHorizontally; i++)
		{
			for (i32 j = 0; j < numTilesVertically; j++)
			{
				drawIsometricTileOutline(i, j);
			}
		}
	}

	// DEBUGGING
	drawRect(0, 0, viewportWidth, 120, 0xff0a3f3f);

	u32 dbgFontColor = 0xffcccccc;

	u32ToStr(viewportWidth, strings[20]);
	renderText("vwprt_width:", 1050, 10, 2, dbgFontColor);
	renderString(20, 1240, 5, 4, dbgFontColor);

	u32ToStr(viewportHeight, strings[21]);
	renderText("vwprt_height:", 1050, 50, 2, dbgFontColor);
	renderString(21, 1240, 45, 4, dbgFontColor);

	u32ToStr(mapWidth, strings[22]);
	renderText("map_width:", 1400, 10, 2, dbgFontColor);
	renderString(22, 1570, 5, 4, dbgFontColor);

	u32ToStr(mapHeight, strings[23]);
	renderText("map_height:", 1400, 50, 2, dbgFontColor);
	renderString(23, 1570, 45, 4, dbgFontColor);

	u32ToStr(player.x, strings[30]);
	u32ToStr(player.y, strings[31]);
	renderText("player.x:", 50, 450, 2, 0xff000000);
	renderString(30, 270, 450, 2, 0xff000000);
	renderText("player.y:", 50, 480, 2, 0xff000000);
	renderString(31, 270, 480, 2, 0xff000000);
	renderText("No. Entities: ", 50, 550, 2, 0xff000000);
	u32ToStr(MAX_SIZE, strings[32]);
	renderString(32, 270, 550, 2, 0xff000000);

	u32ToStr(mouseX, strings[40]);
	u32ToStr(mouseY, strings[41]);
	renderText("mouse.x:", 50, 590, 2, 0xff000000);
	renderString(40, 270, 590, 2, 0xff000000);
	renderText("mouse.y:", 50, 620, 2, 0xff000000);
	renderString(41, 270, 620, 2, 0xff000000);

	Point mouseOnMap = screenToMapPosition((Point){ mouseX, mouseY });
	u32ToStr(mouseOnMap.x, strings[42]);
	u32ToStr(mouseOnMap.y, strings[43]);
	renderText("mouse.x:", 50, 660, 2, 0xff000000);
	renderString(42, 270, 660, 2, 0xff000000);
	renderText("mouse.y:", 50, 690, 2, 0xff000000);
	renderString(43, 270, 690, 2, 0xff000000);
	renderText(shouldMove ? "YES" : "NO:", 50, 730, 2, 0xff000000);

	i32 graphStartX = 10;
	i32 graphBaseline = 65;
	u32 graphRectColor = 0xffff0000;
	drawRect(graphStartX, graphBaseline-60, 1024, 60, 0x88990000);
	for (i32 i = 1024; i > 0; i--)
	{
		FPSBuffer[i] = FPSBuffer[i-1];
		drawRect(graphStartX + i, graphBaseline-FPSBuffer[i], 1, FPSBuffer[i], 0xff00ff00);
	}
	FPSBuffer[0] = Clamp((u32)(1000 / dt), 0, 60);
	drawRect(graphStartX, graphBaseline-FPSBuffer[0], 1, FPSBuffer[0], 0xff00ff00);
	
	drawRect(graphStartX, graphBaseline-30, 1024, 1, graphRectColor);
	dtBuffer[0] = dt;
	for (i32 i = 1024; i > 0; i--)
	{
		dtBuffer[i] = dtBuffer[i-1];
		drawRect(graphStartX + i, graphBaseline-30+(i32)(16.0f-dtBuffer[i]), 1, 2, 0xffff00ff);
	}

	drawRect(graphStartX, graphBaseline-60, 1, 60, graphRectColor);
	drawRect(graphStartX, graphBaseline-60, 1024, 1, graphRectColor);
	drawRect(graphStartX, graphBaseline, 1024, 1, graphRectColor);
	drawRect(graphStartX+1024, graphBaseline-60, 1, 60, graphRectColor);
	
	u32 FPSNumToDisplay = 0;
	for (i32 i = 0; i < 100; i++)
	{
		FPSNumToDisplay += FPSBuffer[i];
	}
	FPSNumToDisplay /= 100;
	u32ToStr(FPSNumToDisplay, strings[22]);
	renderText("FPS:", graphStartX, 70, 2, dbgFontColor);
	renderString(22, graphStartX+60, 70, 4, dbgFontColor);


	u32ToStr(getMemoryCapacity()/1024, strings[23]);
	renderText("Total memory(kb):", 1750, 5, 2, dbgFontColor);
	renderString(23, 2000, 1, 4, dbgFontColor);

	u32ToStr(calculateUsedMemory()/1024, strings[24]);
	renderText("Used memory(kb):", 1750, 45, 2, dbgFontColor);
	renderString(24, 2000, 40, 4, dbgFontColor);

	u32ToStr(100*calculateUsedMemory()/getMemoryCapacity(), strings[25]);
	renderText("Taken(%):", 1750, 80, 2, dbgFontColor);
	renderString(25, 2000, 77, 4, dbgFontColor);

	// imgui clear
	UI_cleanup();
}
