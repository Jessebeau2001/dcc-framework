# ATMega 328P Hardware Timers

The chip has 3 timers:

**Timer 0**
- 8 bit counter

**Timer 1**
- 16 bit counter
- has capture compare

**Timer 2**
- 8 bit counter
- can use external clock


Each timer has at least the following registers:

| **Reg**   | Name                           | Purpose                   | Mental model |
| --------- | ------------------------------ | ------------------------- | ------------ |
| **TCCR**  | Timer Counter Control Register | Configure behaviour       | Settings     |
| **TCNT**  | Timer Counter                  | Holds current count       | Stopwatch    |
| **OCR**   | Output Compare Register        | Holds target values Alarm | thresholds   |
| **TIMSK** | Timer Interrupt Mask           | Enables interrupts        | Permission   |
| **TIFR**  | Timer Interrupt Flag Register  | Records events            | Event log    |

### TCCR - The Control Register

This register is 16 bits - or rather its two of them next to eachother.
Lets take TCCR0 for example:

```mermaid
block-beta
    columns 8

    0x7 0x6 0x5 0x4
    0x3 0x2 0x1 0x0

    tccr0a["TCCR0A"]:8
    com0a["COM0A"]:2
    com0b["COM0B"]:2
    res1["Reserved"]:2
    wgm1["WGM01"]:2

    tccr0b["TCCR0B"]:8
    foc0b["FOC 0A"]
    foc0a["FOC 0B"]
    res2["Reserved"]:2
    wgm2["WGM 02"]:1
    cs0["CS0"]:3

    classDef reg fill:#b5c4ff,stroke:#4066ff,color:#292929
    classDef bit fill:#d1fae5,stroke:#059669,color:#292929
    classDef res fill:#ffa8bb,stroke:#ff2656,color:#292929
    classDef hdr fill:#ccfcff,stroke:#7df6ff,color:#292929

    class 0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0 hdr
    class tccr0a,tccr0b reg
    class wgm1,com0b,com0a,cs0,wgm2,foc0b,foc0a bit
    class res1,res2 res
```

| Name | Purpose                        |
| ---- | ------------------------------ |
| TCCR | Timer Counter Control Register |
| WGM  | Waveform Generator Mode        |
| COM  | Compare Output Mode            |
| CS   | Clock Select                   |
| FOC  | Force Output Compare           |

### TCNT - The Counter Register

The Timer Counter counter register is straigtforward. It just holds the current count. For timer 0 this register is 8 bits:

```mermaid
block-beta
    columns 8

    0x7 0x6 0x5 0x4
    0x3 0x2 0x1 0x0

    tcnt0["TCNT0"]:8
    count["Current Count"]:8

    classDef reg fill:#b5c4ff,stroke:#4066ff,color:#292929
    classDef bit fill:#d1fae5,stroke:#059669,color:#292929
    classDef res fill:#ffa8bb,stroke:#ff2656,color:#292929
    classDef hdr fill:#ccfcff,stroke:#7df6ff,color:#292929

    class 0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0 hdr
    class tcnt0 reg
    class count bit
```

### OCR - The Output Compare Register

The OCR holds the value that the TCNT is compared against. For example for timer 0 there are two: OCR0A & OCR0B.
OCR0A can define a `TOP` in certain timer modes. Each OCR can trigger its own inerupt. The TOP is essentially the value that makes a timer reset to 0.

```mermaid
block-beta
    columns 8

    0x7 0x6 0x5 0x4
    0x3 0x2 0x1 0x0

    ocra["OCR0A"]:8
    ta["Compare Target A"]:8
    ocrb["OCR0B"]:8
    tb["Compare Target B"]:8

    classDef reg fill:#b5c4ff,stroke:#4066ff,color:#292929
    classDef bit fill:#d1fae5,stroke:#059669,color:#292929
    classDef res fill:#ffa8bb,stroke:#ff2656,color:#292929
    classDef hdr fill:#ccfcff,stroke:#7df6ff,color:#292929

    class 0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0 hdr
    class ocra,ocrb reg
    class ta,tb bit
```

### TIMSK - The Timer Interupt Mask Register

This is a control register that enables certain interupt actions, see the table below and what bits control them.

| **Name** | Function                       |
| -------- | ------------------------------ |
| **OCI**  | Output Compare Interupt Enable |
| **TOI**  | Timer Overflow Interupt Enable |

```mermaid
block-beta
    columns 8

    0x7 0x6 0x5 0x4
    0x3 0x2 0x1 0x0

    timsk["TIMSK0"]:8
    res1["Reserved"]:5
    ocieb["OCIE0B"]
    ociea["OCIE0A"]
    toie["TOIE0"]

    classDef reg fill:#b5c4ff,stroke:#7d99ff,color:#292929
    classDef bit fill:#d1fae5,stroke:#059669,color:#292929
    classDef res fill:#ffa8bb,stroke:#ff2656,color:#292929
    classDef hdr fill:#ccfcff,stroke:#7df6ff,color:#292929

    class 0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0 hdr
    class timsk reg
    class ocieb,ociea,toie bit
    class res1 res
```

For example for timer 0 we have OCI0A and OCI0B which enables the interupts for when the TCNTo reaches the OCR0A or OCR0B of timer 0 respectively.

### TIFR - The Timer Interupt Flag Register

Where TIMSK determines which interrupts are enabled, TIFR records which timer events have occurred.

| **Name** | Function                  |
| -------- | ------------------------- |
| **OCF**  | Output Compare Match Flag |
| **TOV**  | Timer Overflow Flag       |

```mermaid
block-beta
    columns 8

    0x7 0x6 0x5 0x4
    0x3 0x2 0x1 0x0

    tifr["TIFR0"]:8
    res1["Reserved"]:5
    ocfb["OCF0B"]
    ocfa["OCF0A"]
    tov["TOV0"]

    classDef reg fill:#b5c4ff,stroke:#7d99ff,color:#292929
    classDef bit fill:#d1fae5,stroke:#059669,color:#292929
    classDef res fill:#ffa8bb,stroke:#ff2656,color:#292929
    classDef hdr fill:#ccfcff,stroke:#7df6ff,color:#292929

    class 0x7,0x6,0x5,0x4,0x3,0x2,0x1,0x0 hdr
    class tifr reg
    class ocfb,ocfa,tov bit
    class res1 res
```

For example  when TCNT0 matches OCR0A:
- OCF0A is set to 1.
- If OCIE0A is enabled and global interrupts are enabled, the CPU can execute the corresponding ISR.

> Imporant example: OCF0A is set, but interupts are disabled. The moment interupts are enabled the CPU will automatically clear the bit and instantly trigger the attached ISR.

### Summary

```mermaid
---
config:
  flowchart:
    curve: linear
---
flowchart TD
    TCCR["`**TCCR0A/TCCR0B**<br/>Configure how the timer operates`"]
    TCNT["`**TCNT0**<br>Holds the current count`"]
    OCR["`**OCR0A/OCR0B**<br>Provide comparison values`"]
    TIFR["`**TIFR0**<br>Records events`"]
    TIMSK["`**TIMSK0**<br>Enables interupts`"]

    TCCR --> TCNT
    TCNT --> OCR
    OCR --> TIFR
    OCR --> TIMSK

    classDef control fill:#b5c4ff,stroke:#4066ff,color:#292929
    classDef counter fill:#d1fae5,stroke:#059669,color:#292929
    classDef compare fill:#ccfcff,stroke:#0891b2,color:#292929
    classDef flags fill:#ffe5b4,stroke:#d97706,color:#292929
    classDef interrupt fill:#e9d5ff,stroke:#9333ea,color:#292929

    class TCCR control
    class TCNT counter
    class OCR compare
    class TIFR flags
    class TIMSK interrupt
```

## Vector Linking Magic (ISR block)

Below is a little bit of info on how the `ISR(VectorSymbol)` macro from avr-libc works.

```c++
// Attributes give the compiler special instructions on how to compile certain code
__attribute__((used))               // Do not delete this function even though nothing calls it
__attribute__((signal))             // This is an interupt handler. Essentially means wrap in save/restore sreg block
__attribute__((externally_visible)) // Do not optimize - keep a callable global symbol

void __vector__15 (void) __attribute__((used, signal, externally_visible)); // Essentially forward declare and attach attributes
void __vector_15 (void) { /* your code */ } // Then conrete declaration with user code
```