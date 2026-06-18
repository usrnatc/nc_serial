// nc_serial.h
//
// Simple single-file header library for serialisation and deserialisation
// of common types in embedded contexts
//

#if !defined(__NC_SERIAL_H__)
#define __NC_SERIAL_H__

// @defines____________________________________________________________________
#if !defined(NCSERIAL_DEF)
    #if defined(NCSERIAL_STATIC)
        #define NCSERIAL_DEF static
    #else
        #define NCSERIAL_DEF extern
    #endif
#endif

#if !defined(NCSERIAL_LITTLE_ENDIAN)
    #if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define NCSERIAL_LITTLE_ENDIAN 0
    #else
        #define NCSERIAL_LITTLE_ENDIAN 1
    #endif
#endif

#if !defined(NCSERIAL_HAS_NATIVE_F16)
    #if defined(__STDCPP_FLOAT16_T__)
        #define NCSERIAL_HAS_NATIVE_F16 1
    #elif defined(__FLT16_MANT_DIG__)
        #define NCSERIAL_HAS_NATIVE_F16 1
    #else
        #define NCSERIAL_HAS_NATIVE_F16 0
    #endif
#endif

#if !defined(NCSERIAL_HAS_NATIVE_BF16)
    #if defined(__STDCPP_BFLOAT16_T__)
        #define NCSERIAL_HAS_NATIVE_BF16 1
    #else
        #define NCSERIAL_HAS_NATIVE_BF16 0
    #endif
#endif

#if !defined(NCSERIAL_HAS_NATIVE_F128)
    #if defined(__SIZEOF_FLOAT128__) || defined(__STDCPP_FLOAT128_T__)
        #define NCSERIAL_HAS_NATIVE_F128 1
    #else
        #define NCSERIAL_HAS_NATIVE_F128 0
    #endif
#endif

#if !defined(NCSERIAL_DISABLE_F16)
    #define NCSERIAL_ENABLE_F16 1
#endif
#if !defined(NCSERIAL_DISABLE_BF16)
    #define NCSERIAL_ENABLE_BF16 1
#endif
#if defined(NCSERIAL_ENABLE_F128_OPTIN) || NCSERIAL_HAS_NATIVE_F128
    #define NCSERIAL_ENABLE_F128 1
#endif

#if !defined(NCSERIAL_NO_STDLIB)
    #include <stdint.h>

    typedef uint8_t  u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    typedef int8_t   i8;
    typedef int16_t  i16;
    typedef int32_t  i32;
    typedef int64_t  i64;
    typedef uint32_t b32;
    typedef uint8_t  b8;
    typedef float    f32;
    typedef double   f64;
#else
    typedef unsigned char      u8;
    typedef unsigned short     u16;
    typedef unsigned int       u32;
    typedef unsigned long long u64;
    typedef signed char        i8;
    typedef signed short       i16;
    typedef signed int         i32;
    typedef signed long long   i64;
    typedef u32                b32;
    typedef u8                 b8;
    typedef float              f32;
    typedef double             f64;
#endif

#if NCSERIAL_HAS_NATIVE_F16
    #if defined(__STDCPP_FLOAT16_T__) && !defined(NCSERIAL_NO_STDLIB)
        #include <stdfloat>

        typedef std::float16_t f16;
    #else
        typedef _Float16 f16;
    #endif
#endif

#if NCSERIAL_HAS_NATIVE_F128
    #if defined(__STDCPP_FLOAT128_T__) && !defined(NCSERIAL_NO_STDLIB)
        #include <stdfloat>

        typedef std::float128_t f128;
    #else
        typedef __float128 f128;
    #endif
#endif

#if !defined(FALSE)
    #define FALSE 0
#endif

#if !defined(TRUE)
    #define TRUE 1
#endif

#if !defined(NULL)
    #define NULL 0
#endif

#define IN
#define OUT

#if !defined(NCSERIAL_MEMSET)
    #if !defined(NCSERIAL_NO_STDLIB)
        #include <string.h>

        #define NCSERIAL_MEMSET(X, Y, Z) memset(X, Y, Z)
    #else
        // TODO(nc): at least iterate over more than a byte at a time
        static inline void*
        MemSet(void* _Dst, u8 Value, u32 Count)
        {
            u8* Dst = (u8*) _Dst;
        
            while (Count--)
                *Dst++ = Value;

            return _Dst;
        }

        #define NCSERIAL_MEMSET(X, Y, Z) MemSet(X, Y, Z)
    #endif
#endif

#if !defined(NCSERIAL_MEMCPY)
    #if !defined(NCSERIAL_NO_STDLIB)
        #include <string.h>

        #define NCSERIAL_MEMCPY(X, Y, Z) memcpy(X, Y, Z)
    #else
        static inline void*
        MemCpy(void* _Dst, void const* _Src, u32 Count)
        {
            u8* Dst = (u8*) _Dst;
            u8 const* Src = (u8 const*) _Src;

            while (Count--)
                *Dst++ = *Src++;

            return _Dst;
        }

        #define NCSERIAL_MEMCPY(X, Y, Z) MemCpy(X, Y, Z)
    #endif
#endif

#define nc_serial_write_struct(Serial, Ptr)       nc_serial_write_raw((Serial), (Ptr), sizeof(*(Ptr)))
#define nc_serial_read_struct(Serial, Ptr)        nc_serial_read_raw((Serial), (Ptr), sizeof(*(Ptr)))
#define nc_serial_write_array(Serial, Ptr, Count) nc_serial_write_raw((Serial), (Ptr), (Count) * sizeof(*(Ptr)))
#define nc_serial_read_array(Serial, Ptr, Count)  nc_serial_read_raw((Serial), (Ptr), (Count) * sizeof(*(Ptr)))

// @types______________________________________________________________________
struct nc_serial {
    u8* Ptr;
    u32 Capacity;
    u32 Position;
};

// @runtime____________________________________________________________________


// @functions__________________________________________________________________
NCSERIAL_DEF nc_serial nc_serial_init(void* Buffer, u32 Capacity);
NCSERIAL_DEF u32 nc_serial_write_raw(nc_serial* Serial, void const* Data, u32 DataSize);
NCSERIAL_DEF u32 nc_serial_read_raw(nc_serial* Serial, OUT void* Data, u32 DataSize);
NCSERIAL_DEF u32 nc_serial_write_u8(nc_serial* Serial, u8 Value);
NCSERIAL_DEF u32 nc_serial_write_u16_le(nc_serial* Serial, u16 Value);
NCSERIAL_DEF u32 nc_serial_write_u16_be(nc_serial* Serial, u16 Value);
NCSERIAL_DEF u32 nc_serial_write_u32_le(nc_serial* Serial, u32 Value);
NCSERIAL_DEF u32 nc_serial_write_u32_be(nc_serial* Serial, u32 Value);
NCSERIAL_DEF u32 nc_serial_write_u64_le(nc_serial* Serial, u64 Value);
NCSERIAL_DEF u32 nc_serial_write_u64_be(nc_serial* Serial, u64 Value);
NCSERIAL_DEF u32 nc_serial_write_i8(nc_serial* Serial, i8 Value);
NCSERIAL_DEF u32 nc_serial_write_i16_le(nc_serial* Serial, i16 Value);
NCSERIAL_DEF u32 nc_serial_write_i16_be(nc_serial* Serial, i16 Value);
NCSERIAL_DEF u32 nc_serial_write_i32_le(nc_serial* Serial, i32 Value);
NCSERIAL_DEF u32 nc_serial_write_i32_be(nc_serial* Serial, i32 Value);
NCSERIAL_DEF u32 nc_serial_write_i64_le(nc_serial* Serial, i64 Value);
NCSERIAL_DEF u32 nc_serial_write_i64_be(nc_serial* Serial, i64 Value);
NCSERIAL_DEF u32 nc_serial_write_f32_le(nc_serial* Serial, f32 Value);
NCSERIAL_DEF u32 nc_serial_write_f32_be(nc_serial* Serial, f32 Value);
NCSERIAL_DEF u32 nc_serial_write_f64_le(nc_serial* Serial, f64 Value);
NCSERIAL_DEF u32 nc_serial_write_f64_be(nc_serial* Serial, f64 Value);

#if NCSERIAL_ENABLE_F16
    NCSERIAL_DEF u32 nc_serial_write_f16_le(nc_serial* Serial, f32 Value);
    NCSERIAL_DEF u32 nc_serial_write_f16_be(nc_serial* Serial, f32 Value);
#endif

#if NCSERIAL_ENABLE_BF16
    NCSERIAL_DEF u32 nc_serial_write_bf16_le(nc_serial* Serial, f32 Value);
    NCSERIAL_DEF u32 nc_serial_write_bf16_be(nc_serial* Serial, f32 Value);
#endif

#if NCSERIAL_ENABLE_F128
    NCSERIAL_DEF u32 nc_serial_write_f128_le(nc_serial* Serial, f128 Value);
    NCSERIAL_DEF u32 nc_serial_write_f128_be(nc_serial* Serial, f128 Value);
#endif

NCSERIAL_DEF u32 nc_serial_read_u8(nc_serial* Serial, OUT u8* Value);
NCSERIAL_DEF u32 nc_serial_read_u16_le(nc_serial* Serial, OUT u16* Value);
NCSERIAL_DEF u32 nc_serial_read_u16_be(nc_serial* Serial, OUT u16* Value);
NCSERIAL_DEF u32 nc_serial_read_u32_le(nc_serial* Serial, OUT u32* Value);
NCSERIAL_DEF u32 nc_serial_read_u32_be(nc_serial* Serial, OUT u32* Value);
NCSERIAL_DEF u32 nc_serial_read_u64_le(nc_serial* Serial, OUT u64* Value);
NCSERIAL_DEF u32 nc_serial_read_u64_be(nc_serial* Serial, OUT u64* Value);
NCSERIAL_DEF u32 nc_serial_read_i8(nc_serial* Serial, OUT i8* Value);
NCSERIAL_DEF u32 nc_serial_read_i16_le(nc_serial* Serial, OUT i16* Value);
NCSERIAL_DEF u32 nc_serial_read_i16_be(nc_serial* Serial, OUT i16* Value);
NCSERIAL_DEF u32 nc_serial_read_i32_le(nc_serial* Serial, OUT i32* Value);
NCSERIAL_DEF u32 nc_serial_read_i32_be(nc_serial* Serial, OUT i32* Value);
NCSERIAL_DEF u32 nc_serial_read_i64_le(nc_serial* Serial, OUT i64* Value);
NCSERIAL_DEF u32 nc_serial_read_i64_be(nc_serial* Serial, OUT i64* Value);
NCSERIAL_DEF u32 nc_serial_read_f32_le(nc_serial* Serial, OUT f32* Value);
NCSERIAL_DEF u32 nc_serial_read_f32_be(nc_serial* Serial, OUT f32* Value);
NCSERIAL_DEF u32 nc_serial_read_f64_le(nc_serial* Serial, OUT f64* Value);
NCSERIAL_DEF u32 nc_serial_read_f64_be(nc_serial* Serial, OUT f64* Value);

#if NCSERIAL_ENABLE_F16
    NCSERIAL_DEF u32 nc_serial_read_f16_le(nc_serial* Serial, OUT f32* Value);
    NCSERIAL_DEF u32 nc_serial_read_f16_be(nc_serial* Serial, OUT f32* Value);
#endif

#if NCSERIAL_ENABLE_BF16
    NCSERIAL_DEF u32 nc_serial_read_bf16_le(nc_serial* Serial, OUT f32* Value);
    NCSERIAL_DEF u32 nc_serial_read_bf16_be(nc_serial* Serial, OUT f32* Value);
#endif

#if NCSERIAL_ENABLE_F128
    NCSERIAL_DEF u32 nc_serial_read_f128_le(nc_serial* Serial, OUT f128* Value);
    NCSERIAL_DEF u32 nc_serial_read_f128_be(nc_serial* Serial, OUT f128* Value);
#endif

#endif // __NC_SERIAL_H__

#if defined(NCSERIAL_IMPLEMENTATION)
    NCSERIAL_DEF nc_serial 
    nc_serial_init(void* Buffer, u32 Capacity)
    {
        nc_serial Result = {};

        Result.Ptr = (u8*) Buffer;
        Result.Capacity = Capacity;
        Result.Position = 0;

        return Result;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_raw(nc_serial* Serial, void const* Data, u32 DataSize)
    {
        u32 Result = 0;

        if (DataSize > Serial->Capacity - Serial->Position) {
            return Result;
        }

        u8 const* Src = (u8 const*) Data;

        for (u32 Index = 0; Index < DataSize; ++Index)
            Serial->Ptr[Serial->Position++] = Src[Index];

        return DataSize;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_raw(nc_serial* Serial, OUT void* Data, u32 DataSize)
    {
        if (DataSize > Serial->Capacity - Serial->Position) {
            NCSERIAL_MEMSET(Data, 0, DataSize);

            return 0;
        }

        u8* Dst = (u8*) Data;

        for (u32 Index = 0; Index < DataSize; ++Index)
            Dst[Index] = Serial->Ptr[Serial->Position++];

        return DataSize;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u8(nc_serial* Serial, u8 Value)
    {
        if (Serial->Position + 1 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = Value;

        return 1;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u16_le(nc_serial* Serial, u16 Value)
    {
        if (Serial->Position + 2 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >> 0);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 8);

        return 2;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u16_be(nc_serial* Serial, u16 Value)
    {
        if (Serial->Position + 2 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >> 8);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 0);

        return 2;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u32_le(nc_serial* Serial, u32 Value)
    {
        if (Serial->Position + 4 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >>  0);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  8);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 16);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 24);

        return 4;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u32_be(nc_serial* Serial, u32 Value)
    {
        if (Serial->Position + 4 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >> 24);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 16);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  8);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  0);

        return 4;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u64_le(nc_serial* Serial, u64 Value)
    {
        if (Serial->Position + 8 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >>  0);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  8);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 16);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 24);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 32);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 40);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 48);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 56);

        return 8;
    }

    NCSERIAL_DEF u32 
    nc_serial_write_u64_be(nc_serial* Serial, u64 Value)
    {
        if (Serial->Position + 8 > Serial->Capacity) {
            return 0;
        }

        Serial->Ptr[Serial->Position++] = (u8) (Value >> 56);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 48);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 40);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 32);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 24);
        Serial->Ptr[Serial->Position++] = (u8) (Value >> 16);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  8);
        Serial->Ptr[Serial->Position++] = (u8) (Value >>  0);

        return 8;
    }

    NCSERIAL_DEF u32
    nc_serial_write_f32_le(nc_serial* Serial, f32 Value)
    {
        u32 Bits = 0;

        NCSERIAL_MEMCPY(&Bits, &Value, sizeof(Bits));

        return nc_serial_write_u32_le(Serial, Bits);
    }

    NCSERIAL_DEF u32
    nc_serial_write_f32_be(nc_serial* Serial, f32 Value)
    {
        u32 Bits = 0;

        NCSERIAL_MEMCPY(&Bits, &Value, sizeof(Bits));

        return nc_serial_write_u32_be(Serial, Bits);
    }

    NCSERIAL_DEF u32
    nc_serial_write_f64_le(nc_serial* Serial, f64 Value)
    {
        u64 Bits = 0;

        NCSERIAL_MEMCPY(&Bits, &Value, sizeof(Bits));

        return nc_serial_write_u64_le(Serial, Bits);
    }

    NCSERIAL_DEF u32
    nc_serial_write_f64_be(nc_serial* Serial, f64 Value)
    {
        u64 Bits = 0;

        NCSERIAL_MEMCPY(&Bits, &Value, sizeof(Bits));

        return nc_serial_write_u64_be(Serial, Bits);
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i8(nc_serial* Serial, i8  Value) 
    {
        return nc_serial_write_u8(Serial, (u8)  Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i16_le(nc_serial* Serial, i16 Value) 
    { 
        return nc_serial_write_u16_le(Serial, (u16) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i16_be(nc_serial* Serial, i16 Value) 
    { 
        return nc_serial_write_u16_be(Serial, (u16) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i32_le(nc_serial* Serial, i32 Value) 
    { 
        return nc_serial_write_u32_le(Serial, (u32) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i32_be(nc_serial* Serial, i32 Value) 
    { 
        return nc_serial_write_u32_be(Serial, (u32) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i64_le(nc_serial* Serial, i64 Value) 
    { 
        return nc_serial_write_u64_le(Serial, (u64) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_write_i64_be(nc_serial* Serial, i64 Value) 
    { 
        return nc_serial_write_u64_be(Serial, (u64) Value); 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u8(nc_serial* Serial, OUT u8* Value)
    {
        if (Serial->Position + 1 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = Serial->Ptr[Serial->Position++];

        return 1;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u16_le(nc_serial* Serial, OUT u16* Value)
    {
        if (Serial->Position + 2 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u16) (
            (u16) Serial->Ptr[Serial->Position] |
            ((u16) Serial->Ptr[Serial->Position + 1] << 8)
        );
        Serial->Position += 2;

        return 2;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u16_be(nc_serial* Serial, OUT u16* Value)
    {
        if (Serial->Position + 2 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u16) (
            ((u16) Serial->Ptr[Serial->Position] << 8) |
            (u16) Serial->Ptr[Serial->Position + 1]
        );
        Serial->Position += 2;

        return 2;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u32_le(nc_serial* Serial, OUT u32* Value)
    {
        if (Serial->Position + 4 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u32) (
            (u32) Serial->Ptr[Serial->Position] |
            ((u32) Serial->Ptr[Serial->Position + 1] << 8) |
            ((u32) Serial->Ptr[Serial->Position + 2] << 16) |
            ((u32) Serial->Ptr[Serial->Position + 3] << 24)
        );
        Serial->Position += 4;

        return 4;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u32_be(nc_serial* Serial, OUT u32* Value)
    {
        if (Serial->Position + 4 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u32) (
            ((u32) Serial->Ptr[Serial->Position] << 24) |
            ((u32) Serial->Ptr[Serial->Position + 1] << 16) |
            ((u32) Serial->Ptr[Serial->Position + 2] << 8) |
            (u32) Serial->Ptr[Serial->Position + 3]
        );
        Serial->Position += 4;

        return 4;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u64_le(nc_serial* Serial, OUT u64* Value)
    {
        if (Serial->Position + 8 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u64) (
            (u64) Serial->Ptr[Serial->Position] |
            ((u64) Serial->Ptr[Serial->Position + 1] << 8) |
            ((u64) Serial->Ptr[Serial->Position + 2] << 16) |
            ((u64) Serial->Ptr[Serial->Position + 3] << 24) |
            ((u64) Serial->Ptr[Serial->Position + 4] << 32) |
            ((u64) Serial->Ptr[Serial->Position + 5] << 40) |
            ((u64) Serial->Ptr[Serial->Position + 6] << 48) |
            ((u64) Serial->Ptr[Serial->Position + 7] << 56)
        );
        Serial->Position += 8;

        return 8;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_u64_be(nc_serial* Serial, OUT u64* Value)
    {
        if (Serial->Position + 8 > Serial->Capacity) {
            *Value = 0;

            return 0;
        }

        *Value = (u64) (
            ((u64) Serial->Ptr[Serial->Position] << 56) |
            ((u64) Serial->Ptr[Serial->Position + 1] << 48) |
            ((u64) Serial->Ptr[Serial->Position + 2] << 40) |
            ((u64) Serial->Ptr[Serial->Position + 3] << 32) |
            ((u64) Serial->Ptr[Serial->Position + 4] << 24) |
            ((u64) Serial->Ptr[Serial->Position + 5] << 16) |
            ((u64) Serial->Ptr[Serial->Position + 6] << 8) |
            (u64) Serial->Ptr[Serial->Position + 7]
        );
        Serial->Position += 8;

        return 8;
    }


    NCSERIAL_DEF u32
    nc_serial_read_f32_le(nc_serial* Serial, OUT f32* Value)
    {
        u32 Bits = 0;
        u32 Read = nc_serial_read_u32_le(Serial, &Bits);

        NCSERIAL_MEMCPY(Value, &Bits, sizeof(*Value));

        return Read;
    }

    NCSERIAL_DEF u32
    nc_serial_read_f32_be(nc_serial* Serial, OUT f32* Value)
    {
        u32 Bits = 0;
        u32 Read = nc_serial_read_u32_be(Serial, &Bits);

        NCSERIAL_MEMCPY(Value, &Bits, sizeof(*Value));

        return Read;
    }

    NCSERIAL_DEF u32
    nc_serial_read_f64_le(nc_serial* Serial, OUT f64* Value)
    {
        u64 Bits = 0;
        u32 Read = nc_serial_read_u64_le(Serial, &Bits);

        NCSERIAL_MEMCPY(Value, &Bits, sizeof(*Value));

        return Read;
    }

    NCSERIAL_DEF u32
    nc_serial_read_f64_be(nc_serial* Serial, OUT f64* Value)
    {
        u64 Bits = 0;
        u32 Read = nc_serial_read_u64_be(Serial, &Bits);

        NCSERIAL_MEMCPY(Value, &Bits, sizeof(*Value));

        return Read;
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i8(nc_serial* Serial, OUT i8*  Value) 
    { 
        u8 Unsigned = 0; 
        u32 Result = nc_serial_read_u8(Serial, &Unsigned); 

        *Value = (i8) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i16_le(nc_serial* Serial, OUT i16* Value) 
    { 
        u16 Unsigned = 0; 
        u32 Result = nc_serial_read_u16_le(Serial, &Unsigned); 

        *Value = (i16) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i16_be(nc_serial* Serial, OUT i16* Value) 
    { 
        u16 Unsigned = 0; 
        u32 Result = nc_serial_read_u16_be(Serial, &Unsigned); 

        *Value = (i16) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i32_le(nc_serial* Serial, OUT i32* Value) 
    { 
        u32 Unsigned = 0; 
        u32 Result = nc_serial_read_u32_le(Serial, &Unsigned); 

        *Value = (i32) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i32_be(nc_serial* Serial, OUT i32* Value) 
    { 
        u32 Unsigned = 0; 
        u32 Result = nc_serial_read_u32_be(Serial, &Unsigned); 

        *Value = (i32) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i64_le(nc_serial* Serial, OUT i64* Value) 
    { 
        u64 Unsigned = 0; 
        u32 Result = nc_serial_read_u64_le(Serial, &Unsigned); 

        *Value = (i64) Unsigned; 

        return Result; 
    }

    NCSERIAL_DEF u32 
    nc_serial_read_i64_be(nc_serial* Serial, OUT i64* Value) 
    { 
        u64 Unsigned = 0; 
        u32 Result = nc_serial_read_u64_be(Serial, &Unsigned); 

        *Value = (i64) Unsigned; 

        return Result; 
    }

    #if NCSERIAL_ENABLE_F128
        NCSERIAL_DEF u32
        nc_serial_write_f128_le(nc_serial* Serial, f128 Value)
        {
            if (Serial->Position + 16 > Serial->Capacity) 
                return 0;

            u8 const* Src = (u8 const*) &Value;

        #if NCSERIAL_LITTLE_ENDIAN
            for (u32 Index = 0; Index < 16; ++Index) 
                Serial->Ptr[Serial->Position++] = Src[Index];
        #else
            for (u32 Index = 0; Index < 16; ++Index) 
                Serial->Ptr[Serial->Position++] = Src[15 - Index];
        #endif
            return 16;
        }

        NCSERIAL_DEF u32
        nc_serial_write_f128_be(nc_serial* Serial, f128 Value)
        {
            if (Serial->Position + 16 > Serial->Capacity) 
                return 0;

            u8 const* Src = (u8 const*) &Value;
        #if NCSERIAL_LITTLE_ENDIAN
            for (u32 Index = 0; Index < 16; ++Index) 
                Serial->Ptr[Serial->Position++] = Src[15 - Index];
        #else
            for (u32 Index = 0; Index < 16; ++Index)
                Serial->Ptr[Serial->Position++] = Src[Index];
        #endif

            return 16;
        }

        NCSERIAL_DEF u32
        nc_serial_read_f128_le(nc_serial* Serial, OUT f128* Value)
        {
            if (Serial->Position + 16 > Serial->Capacity) {
                NCSERIAL_MEMSET(Value, 0, 16);

                return 0;
            }

            u8* Dst = (u8*) Value;

        #if NCSERIAL_LITTLE_ENDIAN
            for (u32 Index = 0; Index < 16; ++Index) 
                Dst[Index] = Serial->Ptr[Serial->Position++];
        #else
            for (u32 Index = 0; Index < 16; ++Index) 
                Dst[15 - Index] = Serial->Ptr[Serial->Position++];
        #endif

            return 16;
        }

        NCSERIAL_DEF u32
        nc_serial_read_f128_be(nc_serial* Serial, OUT f128* Value)
        {
            if (Serial->Position + 16 > Serial->Capacity) {
                NCSERIAL_MEMSET(Value, 0, 16);

                return 0;
            }

            u8* Dst = (u8*) Value;

        #if NCSERIAL_LITTLE_ENDIAN
            for (u32 Index = 0; Index < 16; ++Index) 
                Dst[15 - Index] = Serial->Ptr[Serial->Position++];
        #else
            for (u32 Index = 0; Index < 16; ++Index) 
                Dst[Index] = Serial->Ptr[Serial->Position++];
        #endif

            return 16;
        }
    #endif // NCSERIAL_ENABLE_F128

    // TODO(nc): these need to be tested more harshly
    #if NCSERIAL_ENABLE_F16 && !NCSERIAL_HAS_NATIVE_F16
        static inline u32
        __nc_serial_half_to_f32_bits(u16 Value)
        {
            u32 Sign = (u32) (Value & 0x8000U) << 16;
            u32 Exp = (Value >> 10) & 0x1FU;
            u32 Mantissa =  Value & 0x3FFU;

            if (!Exp) {
                if (Mantissa == 0) 
                    return Sign;

                i32 ExpI = 1;

                while (!(Mantissa & 0x400U)) { 
                    Mantissa <<= 1;
                    --ExpI;
                }

                Mantissa &= 0x3FFU;

                return Sign | ((u32) (ExpI - 15 + 127) << 23) | (Mantissa << 13);
            }

            if (Exp == 0x1F) 
                return Sign | 0x7F800000U | (Mantissa << 13);

            return Sign | ((Exp - 15 + 127) << 23) | (Mantissa << 13);
        }

        static inline u16
        __nc_serial_f32_bits_to_half(u32 Value)
        {
            u32 Sign = (Value >> 16) & 0x8000U;
            i32 Exp  = (i32) ((Value >> 23) & 0xFFU) - 127 + 15;
            u32 Mantissa  = Value & 0x7FFFFFU;

            if (((Value >> 23) & 0xFFu) == 0xFFU)
                return (u16) (Sign | 0x7C00U | (Mantissa ? 0x0200U : 0U));

            if (Exp >= 0x1F) 
                return (u16) (Sign | 0x7C00U);

            if (Exp <= 0) {
                if (Exp < -10) 
                    return (u16) Sign;

                Mantissa |= 0x800000U;

                u32 Shift = (u32)(14 - Exp);
                u32 R = Mantissa >> Shift;
                u32 Rem = Mantissa & ((1U << Shift) - 1U);
                u32 Half = 1U << (Shift - 1);

                if (Rem > Half || (Rem == Half && (R & 1U))) 
                    ++R;

                return (u16) (Sign | R);
            }

            u16 Result = (u16) (Sign | ((u32) Exp << 10) | (Mantissa >> 13));
            u32 Rem = Mantissa & 0x1FFFU;

            if (Rem > 0x1000U || (Rem == 0x1000U && (Result & 1U))) 
                ++Result;

            return Result;
        }
    #endif // NCSERIAL_ENABLE_F16 && !NCSERIAL_HAS_NATIVE_F16

    #if NCSERIAL_ENABLE_F16
        static inline u16
        __nc_serial_f32_to_f16_bits(f32 Value)
        {
        #if NCSERIAL_HAS_NATIVE_F16
            f16 Half = (f16) Value;
            u16 Result; 

            NCSERIAL_MEMCPY(&Result, &Half, sizeof(Result)); 

            return Result;
        #else
            union { 
                f32 F;
                u32 U;
            } C; 

            C.F = Value;

            return __nc_serial_f32_bits_to_half(C.U);
        #endif
        }

        static inline f32
        __nc_serial_f16_bits_to_f32(u16 Value)
        {
        #if NCSERIAL_HAS_NATIVE_F16
            f16 Result; 

            NCSERIAL_MEMCPY(&Result, &Value, sizeof(Value)); 

            return (f32) Result;
        #else
            union { 
                u32 U;
                f32 F;
            } C;

            C.U = __nc_serial_half_to_f32_bits(Value);

            return C.F;
        #endif
        }
    #endif // NCSERIAL_ENABLE_F16

    #if NCSERIAL_ENABLE_BF16
        static inline u16
        __nc_serial_f32_to_bf16_bits(f32 Value)
        {
            union { 
                f32 F; 
                u32 U; 
            } C; 

            C.F = Value;

            u32 Result = C.U;

            if (((Result >> 23) & 0xFFU) == 0xFFU && (Result & 0x7FFFFFU))
                return (u16) ((Result >> 16) | 0x0040U);

            return (u16) ((Result + 0x7FFFU + ((Result >> 16) & 1U)) >> 16);
        }

        static inline f32
        __nc_serial_bf16_bits_to_f32(u16 Value)
        {
            union { 
                u32 U; 
                f32 F; 
            } C; 

            C.U = (u32) Value << 16; 

            return C.F;
        }
    #endif // NCSERIAL_ENABLE_BF16

    #if NCSERIAL_ENABLE_F16
        NCSERIAL_DEF u32 
        nc_serial_write_f16_le(nc_serial* Serial, f32 Value)
        { 
            return nc_serial_write_u16_le(
                Serial, 
                __nc_serial_f32_to_f16_bits(Value)
            ); 
        }

        NCSERIAL_DEF u32 
        nc_serial_write_f16_be(nc_serial* Serial, f32 Value)
        { 
            return nc_serial_write_u16_be(
                Serial, 
                __nc_serial_f32_to_f16_bits(Value)
            ); 
        }

        NCSERIAL_DEF u32 
        nc_serial_read_f16_le(nc_serial* Serial, OUT f32* Value)
        { 
            u16 Bits = 0; 
            u32 Result = nc_serial_read_u16_le(Serial,&Bits); 

            *Value = __nc_serial_f16_bits_to_f32(Bits); 

            return Result; 
        }

        NCSERIAL_DEF u32 
        nc_serial_read_f16_be(nc_serial* Serial, OUT f32* Value)
        { 
            u16 Bits = 0; 
            u32 Result = nc_serial_read_u16_be(Serial,&Bits); 

            *Value = __nc_serial_f16_bits_to_f32(Bits); 

            return Result; 
        }
    #endif // NCSERIAL_ENABLE_F16

    #if NCSERIAL_ENABLE_BF16
        NCSERIAL_DEF u32 
        nc_serial_write_bf16_le(nc_serial* Serial, f32 Value)
        { 
            return nc_serial_write_u16_le(
                Serial, 
                __nc_serial_f32_to_bf16_bits(Value)
            ); 
        }

        NCSERIAL_DEF u32 
        nc_serial_write_bf16_be(nc_serial* Serial, f32 Value)
        { 
            return nc_serial_write_u16_be(
                Serial, 
                __nc_serial_f32_to_bf16_bits(Value)
            ); 
        }

        NCSERIAL_DEF u32 
        nc_serial_read_bf16_le(nc_serial* Serial, OUT f32* Value)
        { 
            u16 Bits = 0; 
            u32 Result = nc_serial_read_u16_le(Serial, &Bits); 

            *Value = __nc_serial_bf16_bits_to_f32(Bits); 

            return Result; 
        }

        NCSERIAL_DEF u32 
        nc_serial_read_bf16_be(nc_serial* Serial, OUT f32* Value)
        { 
            u16 Bits = 0; 
            u32 Result = nc_serial_read_u16_be(Serial,&Bits); 

            *Value = __nc_serial_bf16_bits_to_f32(Bits); 

            return Result; 
        }
    #endif //NCSERIAL_ENABLE_BF16
#endif // NCSERIAL_IMPLEMENTATION
