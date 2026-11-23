# DCC Commander
----

This is a framework to control model trains over the dcc protocol.
This implementation focusses on simplicity and flexibillity.

## Overview: Layers

```mermaid
classDiagram
    class DccController {
        Tracks train states and makes command decisions based on what trains should do when.
    }

    class DccScheduler {
        Manages packet refresh rate, what packets to send when, and injects idle packets when required.
    }

    class DccSerializer {
        Converts a packet to raw bits and does its checksumming. Primary concern is the framing of the data.
    }

    class BitBanger {
        Queues up bits to write at specific intervals. Concerns itself with timings: what to write when, down to the microsecond.
    }

    DccController ..> DccScheduler : uses
    DccScheduler ..> DccSerializer : uses
    DccSerializer ..> BitBanger : uses
```

## HAL

```mermaid
classDiagram
    direction TB

    class IO~T~ <<concept>> {
        + using PIN : ReadWritable~T~
        + using DDR : ReadWritable~T~
        + using PORT : ReadWritable~T~

        io_write(T value) void$
        io_read() T$
        io_configure(IOMode mode) void$
    }

    class ReadWritable~T~ <<concept>> {
        read() T
        write(T value) void
    }

    class Register~Address,T~ {
        + using ValueType = T
        + ADDRESS : uintptr_t
        + read() T
        + write(T value) void
    }

    class IOPin~IO,Index~ {
        + using ValueType = bool
        &nbsp;
        + using PIN : BitHandle
        + using DDR : BitHandle
         +using PORT : BitHandle
    }

    class BitHandle~Base,Index~ {
        + using BaseType = Base::ValueType
        + MASK : BaseType
        + read() bool
        + write(bool value) void
    }

    class IOPort~PinReg,DDRReg,PortReg~ {
        + using ValueType
        &nbsp;
        + using PIN : Register
        + using DDR : Register
        + using PORT : Register
        &nbsp;
        + Pin~N~ : IOPin
    }

    Register ..|> ReadWritable
    BitHandle ..|> ReadWritable
    IOPort ..|> IO
    IOPin ..|> IO 

    BitHandle ..> ReadWritable : wraps
    IOPort ..> Register : wraps
    IOPort ..> IOPin : creates
    IOPin ..> BitHandle : creates
```