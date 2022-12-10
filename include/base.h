#ifndef _COMMON_BASE_H
#define _COMMON_BASE_H

/*

    TODO: Input (poll several key presses that may happen between two frames)
    TODO: Dump memory function
    TODO: Hot code reloading

*/

#define Unused(x) ((void) x)

#define internal static
#define local_persist static
#define global_variable static

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int32_t bool32;

typedef float f32;
typedef double f64;

void
PlatformPrint(char* message, ...);

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0]))

#define Kilobytes(Value) (Value * 1024)
#define Megabytes(Value) (Kilobytes(Value) * 1024)
#define Gigabytes(Value) (Megabytes(Value) * 1024)
#define Terabytes(Value) (Gigabytes(Value) * 1024)

#define DumpIntExpression(Expression) (PlatformPrint("\tExpression: %s: %d", #Expression, Expression))
#define DumpU16HexExpression(Expression) (PlatformPrint("\tExpression: %s: %04X", #Expression, Expression))
#define DumpU8HexExpression(Expression) (PlatformPrint("\tExpression: %s: %02X", #Expression, Expression))
#define DumpFloatExpression(Expression) (PlatformPrint("\tExpression: %s: %.4f", #Expression, Expression))
#define DumpStringExpression(Expression) (PlatformPrint("\tExpression: %s: '%s'", #Expression, Expression))
#define DumpMemoryByte(Pointer) (PlatformPrint("\tMemory %p: '%02X'", Pointer, *(u8*)Pointer))

#if HOT_CODE
#define ExportApi __declspec(dllexport)
#else
#define ExportApi
#endif

#if CHECKS
#define Assert(Expr) {\
    if (!(Expr)) {\
        PlatformPrint("Assertion failed: '%s' in %s:%d", #Expr, __FILE__, __LINE__);\
        i32* trap = 0;\
        *trap = 1;\
    }\
}\

#define Halt(Message) {\
    PlatformPrint("Program halted: '%s' in %s:%d\n", Message, __FILE__, __LINE__);\
    i32* trap = 0;\
    *trap = 1;\
}\

#else
#define Assert(Expr)
#define Halt(Message)
#endif

#endif
