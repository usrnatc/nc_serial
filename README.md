# nc_serial.h

Single-file bounds-checked binary serialiser for embedded C++.

## About

nc_serial.h is a C++ library for reading and writing binary data to and from a
fixed buffer one field at a time. It never owns the buffer, never allocates, and
never grows: every write is bounds-checked against a caller-provided capacity and
the cursor only advances on success, which makes it suitable for framing packets
straight into a stack buffer, a DMA region, or a flash page on a target with a few
kilobytes of RAM to spare.

The serialiser is a single `nc_serial` struct holding a pointer, a capacity, and a
position. It makes no heap allocations, no system calls, and no I/O of any kind;
moving the finished bytes on or off the wire is the caller's job. Every multi-byte
type has an explicit little-endian and big-endian function, so the format is fixed
by the call you make and is identical on every architecture. Host byte order never
leaks into the output.

Integers are emitted byte at a time with shifts rather than an aliased store, so
there are no unaligned-access faults on strict-alignment cores and no dependence on
the build's endianness. Floating-point support covers `f32` and `f64` always, with
optional `f16`, `bfloat16`, and `f128`. The half formats convert through `f32`
losslessly, using the hardware instruction where the compiler exposes a native type
and a software bit-conversion otherwise, so the same call compiles and produces the
same bytes whether or not the target has half-precision hardware.

The library has no platform dependencies. With `NCSERIAL_NO_STDLIB` it drops every
standard library include and uses internal `MemSet` / `MemCpy`, so it builds in
freestanding.

## Installation

Copy `nc_serial.h` into your project.

In **one** C++ source file, define `NCSERIAL_IMPLEMENTATION` before including the
header:

```c++
#define NCSERIAL_IMPLEMENTATION
#include "nc_serial.h"
```

All other files that need the API include the header without the define.

> [!TIP]
> To confine all symbols to a single translation unit, `NCSERIAL_STATIC` can be
> defined alongside `NCSERIAL_IMPLEMENTATION`.

### Overrides

The following macros can be defined before including the header to replace default
dependencies or tune behaviour:

| Macro | Default | Purpose |
| --- | --- | --- |
| `NCSERIAL_NO_STDLIB` | Undefined | Suppresses `<stdint.h>` / `<string.h>`; caller must provide `u8`, `u16`, `u32`, `u64`, `i8`, `i16`, `i32`, `i64`, `b32`, `b8`, `f32`, `f64` typedefs and gets internal `MemSet` / `MemCpy` |
| `NCSERIAL_STATIC` | Undefined | With `NCSERIAL_IMPLEMENTATION`, gives every symbol internal linkage (`NCSERIAL_DEF` becomes `static`) |
| `NCSERIAL_MEMSET(d, v, n)` | `memset` | Memory fill (zeroes a read destination on overflow) |
| `NCSERIAL_MEMCPY(d, s, n)` | `memcpy` | Memory copy (float bit-conversions and the `f128` path) |
| `NCSERIAL_LITTLE_ENDIAN` | Auto (`__BYTE_ORDER__`) | Host byte order. Only the native `f128` path reads it; every other codec is host-order independent |
| `NCSERIAL_DISABLE_F16` | Undefined | Removes the `f16` functions and their conversion code |
| `NCSERIAL_DISABLE_BF16` | Undefined | Removes the `bfloat16` functions and their conversion code |
| `NCSERIAL_ENABLE_F128_OPTIN` | Undefined | Compiles the `f128` functions; requires a native quad type |
| `NCSERIAL_HAS_NATIVE_F16` | Auto-detected | Override half detection. `1` uses the hardware instruction, `0` forces the software bit-conversion |
| `NCSERIAL_HAS_NATIVE_BF16` | Auto-detected | Override bfloat16 detection |
| `NCSERIAL_HAS_NATIVE_F128` | Auto-detected | Override quad detection. There is no software fallback, so `0` disables `f128` |

## Usage

### Initialisation

Bind the serialiser to a buffer and its capacity. A reader and a writer are the
same struct; reset `Position` to `0` to read back what you wrote, or to overwrite
from the start.

```c++
u8 buffer[256];

nc_serial s = nc_serial_init(buffer, sizeof(buffer));
```

### Writing

Each call appends in the requested byte order and returns the number of bytes it
wrote, or `0` if the write would have run past `Capacity`. `u8` / `i8` have no
endian variant.

```c++
nc_serial_write_u8 (&s, value);
nc_serial_write_i8 (&s, value);
nc_serial_write_u16_le(&s, value);
nc_serial_write_u16_be(&s, value);
nc_serial_write_i16_le(&s, value);
nc_serial_write_i16_be(&s, value);
nc_serial_write_u32_le(&s, value);
nc_serial_write_u32_be(&s, value);
nc_serial_write_i32_le(&s, value);
nc_serial_write_i32_be(&s, value);
nc_serial_write_u64_le(&s, value);
nc_serial_write_u64_be(&s, value);
nc_serial_write_i64_le(&s, value);
nc_serial_write_i64_be(&s, value);
nc_serial_write_f32_le(&s, value);
nc_serial_write_f32_be(&s, value);
nc_serial_write_f64_le(&s, value);
nc_serial_write_f64_be(&s, value);
```

### Reading

Each read takes an `OUT` pointer, advances the cursor, and returns the bytes
consumed. On overflow it returns `0`, leaves `Position` untouched, and zeroes the
destination so a short read never hands back uninitialised memory.

```c++
nc_serial_read_u8 (&s, &value);
nc_serial_read_u16_le(&s, &value);
nc_serial_read_u16_be(&s, &value);
nc_serial_read_u32_le(&s, &value);
nc_serial_read_u32_be(&s, &value);
nc_serial_read_u64_le(&s, &value);
nc_serial_read_u64_be(&s, &value);
nc_serial_read_f32_le(&s, &value);
nc_serial_read_f32_be(&s, &value);
nc_serial_read_f64_le(&s, &value);
nc_serial_read_f64_be(&s, &value);
// ...and the matching i8 / i16 / i32 / i64 readers
```

### Raw bytes and structs

`nc_serial_write_raw` / `nc_serial_read_raw` move an arbitrary span. The struct and
array helpers are thin `sizeof` wrappers over them.

```c++
nc_serial_write_raw(&s, ptr, size);
nc_serial_read_raw (&s, ptr, size);

nc_serial_write_struct(&s, &obj);          // sizeof(*ptr) raw bytes
nc_serial_write_array (&s, arr, count);    // count * sizeof(*ptr) raw bytes
nc_serial_read_struct (&s, &obj);
nc_serial_read_array  (&s, arr, count);
```

The struct and array macros copy the in-memory representation verbatim, so they
carry struct padding and host endianness. They round-trip within one build but are
not a portable format; see Limitations.

### Endianness

For integers and for `f32` / `f64` / `f16` / `bf16`, the wire bytes are fixed
entirely by the function name. A `_le` call emits the same bytes on a little-endian
and a big-endian host, because the value is assembled with shifts rather than copied
out of a register. The only codec that consults `NCSERIAL_LITTLE_ENDIAN` is native
`f128`, which has no shift-based path and reorders its 16 in-memory bytes to match
the requested order.

### Error handling

There is no error flag. A call reports how many bytes it moved; `0` means it refused
to overrun the buffer. Check the return where it matters, or gate a batch on the
remaining capacity before committing to it:

```c++
u32 needed = 2 + 4 + 4;   // u16 + u32 + f32

if (s.Capacity - s.Position >= needed) {
    nc_serial_write_u16_le(&s, header);
    nc_serial_write_u32_le(&s, id);
    nc_serial_write_f32_le(&s, reading);
}
```

`s.Position` after a write run is the exact number of bytes to transmit or flush.

### Half and extended floats

`f16` and `bf16` take and return `f32`. `f32` represents every half value exactly,
so the carrier is lossless and callers never need a native half type for the API to
compile. Drop either format entirely with `NCSERIAL_DISABLE_F16` /
`NCSERIAL_DISABLE_BF16`.

```c++
nc_serial_write_f16_le(&s, value);   
nc_serial_write_f16_be(&s, value);
nc_serial_write_bf16_le(&s, value);   
nc_serial_write_bf16_be(&s, value);
nc_serial_read_f16_le(&s, &value);  
nc_serial_read_f16_be(&s, &value);
nc_serial_read_bf16_le(&s, &value);  
nc_serial_read_bf16_be(&s, &value);
```

`f128` is compiled only when the compiler exposes a native quad type (or
`NCSERIAL_ENABLE_F128_OPTIN` is set) and takes that native type directly, since
`f64` cannot carry it.

```c++
nc_serial_write_f128_le(&s, value);   
nc_serial_write_f128_be(&s, value);
nc_serial_read_f128_le(&s, &value);  
nc_serial_read_f128_be(&s, &value);
```

## Examples

### Struct round-trip

Pack a record field by field, then read it straight back out of the same buffer by
rewinding `Position`. The packed stream has no padding in it, which is the point of
serialising field by field rather than copying the struct.

```c++
struct foo {
    u8  U8;  
    i8  I8;
    u16 U16; 
    i16 I16;
    u32 U32; 
    i32 I32;
    u64 U64; 
    i64 I64;
    f32 F32; 
    f64 F64;
};

foo In = { 1, 2, 3, 4, 5, 6, 7, 8, 9.0f, 10.0 };
u8  Buffer[1024];

nc_serial s = nc_serial_init(Buffer, sizeof(Buffer));

nc_serial_write_u8 (&s, In.U8);   
nc_serial_write_i8 (&s, In.I8);
nc_serial_write_u16_le(&s, In.U16); 
nc_serial_write_i16_le(&s, In.I16);
nc_serial_write_u32_le(&s, In.U32); 
nc_serial_write_i32_le(&s, In.I32);
nc_serial_write_u64_le(&s, In.U64); 
nc_serial_write_i64_le(&s, In.I64);
nc_serial_write_f32_le(&s, In.F32); 
nc_serial_write_f64_le(&s, In.F64);

// inspect the packed bytes (printf %s would stop at the first 0x00)
for (u32 i = 0; i < s.Position; ++i)
    printf("%02X ", (u8) Buffer[i]);

printf("\n");

foo Out = {};

s.Position = 0;
nc_serial_read_u8 (&s, &Out.U8);
nc_serial_read_i8 (&s, &Out.I8);
nc_serial_read_u16_le(&s, &Out.U16);
nc_serial_read_i16_le(&s, &Out.I16);
nc_serial_read_u32_le(&s, &Out.U32);
nc_serial_read_i32_le(&s, &Out.I32);
nc_serial_read_u64_le(&s, &Out.U64);
nc_serial_read_i64_le(&s, &Out.I64);
nc_serial_read_f32_le(&s, &Out.F32);
nc_serial_read_f64_le(&s, &Out.F64);
```

```txt
01 02 03 00 04 00 05 00 00 00 06 00 00 00 07 00 00 00 00 00 00 00 08 00 00 00 00 00 00 00 00 00 10 41 00 00 00 00 00 00 24 40
```

42 bytes: `09.0f` lands as `00 00 10 41` (IEEE-754 `0x41100000` byte-reversed) and
`10.0` as `00 00 00 00 00 00 24 40` (`0x4024000000000000`), both least-significant
byte first because of the `_le` writers. `Out` reads back equal to `In`.

### Telemetry frame

Mixed byte order in one buffer: a big-endian header in network order, a
little-endian half-precision payload. The frame is exactly `s.Position` bytes wide
with no padding, ready to hand to a radio or socket.

```c++
u8 frame[16];

nc_serial w = nc_serial_init(frame, sizeof(frame));

nc_serial_write_u16_be(&w, 0xCAFE);        // magic, network byte order
nc_serial_write_u8(&w, 1);                 // version
nc_serial_write_u32_be(&w, sequence);      // header counter
nc_serial_write_f16_le(&w, temperature);   // payload, half precision
nc_serial_write_f16_le(&w, humidity);

u32 frame_len = w.Position;                // 11 bytes to send

// ... on the receiver ...

nc_serial r = nc_serial_init(frame, frame_len);

u16 magic; u8 version; u32 seq; f32 temp, hum;

nc_serial_read_u16_be(&r, &magic);
nc_serial_read_u8(&r, &version);
nc_serial_read_u32_be(&r, &seq);
nc_serial_read_f16_le(&r, &temp);          // delivered back as f32
nc_serial_read_f16_le(&r, &hum);
```

## Limitations

| Constraint | Value | Defined by |
| --- | --- | --- |
| Buffer size | 4 GiB | `u32` `Capacity` / `Position` |
| Widest single field | 16 bytes | `f128` |

The struct and array macros are not a portable format. They copy the in-memory
layout, so struct padding and host endianness travel with the bytes; use them only
to round-trip within a single build, and the field-by-field functions for anything
that crosses a device boundary or is persisted.

The software `f16` path (used only when the compiler has no native half type) handles
subnormals, infinities, NaNs, and round-to-nearest-even, but those edges are where
half conversion is easiest to get subtly wrong. It is worth round-tripping against a
hardware `_Float16` build or a known reference before trusting it on a target.

`f128` assumes a native IEEE-754 binary128 whose in-memory byte order matches the
host's integer byte order, which holds on every target that actually has the type.
There is no software fallback; the format is unavailable where the compiler has no
quad type.

There are no length-prefixed blobs, varints, zigzag signed encodings, or schema or
versioning support. The order of your calls is the format; build any framing you need
on top of `nc_serial_write_raw` and the integer functions.
