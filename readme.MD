# BeanScript
___
BeanScript (.bs) is a minimal and efficient scripting language designed for writing intuitive scripts that automate a 
series of keystrokes. By design, BeanScript has a limited set of operations, enabling its interpreter to be simple, 
efficient, and robust.

### Requirements
___ 
This project utilizes the [interception driver](https://github.com/oblitum/Interception) to send keyboard inputs. The linked repository provides a library for 
interacting with the driver. As the target platform for this project is Windows, the library must be built using MinGW. 
I have provided the DLL, compiled from the library using MinGW, under /lib. However, if you wish to build it yourself, 
a makefile is included for use with mingw32-make. Please ensure to use the [x64-variant for MinGW](https://github.com/niXman/mingw-builds-binaries/releases) to avoid compatibility 
issues when linking with this project.

# Using BeanScript
___
## Installation
___
Before using BeanScript, ensure the following prerequisites are met:
1. Install the Interception driver.
2. Build or obtain a built x64 version of the Interception library.
3. Fork/Clone and build this project.

### Usage
___ 
After installing BeanScript, you can start writing scripts. The compiled executable serves both as the loader and the 
interpreter for the script. The loader recognizes any `.bs` file in the same directory. Simply select the script you 
wish to run, and the loader will execute it.

# Writing Scripts
BeanScript employs a consistent instruction syntax. Each script line is parsed as a single instruction, following the 
format below. Instruction IDs can be any string of characters, including spaces, and parameters are comma-separated.

`<instruction type> <instruction id> with <parameter 1>, <parameter 2>, ...`

### Instruction Types
The following is a list of all instructions types. 

| Instruction | Description                                                                                                                                                                                                                                                                                                                           | Used Parameters                                              |
|-------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------|
| key         | Defines a keystroke.                                                                                                                                                                                                                                                                                                                  | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| press       | Presses a key or an in place defined key.                                                                                                                                                                                                                                                                                             | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| hold        | Holds a key or an in place defined key.                                                                                                                                                                                                                                                                                               | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| release     | Releases a key or an in place defined key.                                                                                                                                                                                                                                                                                            | `button`,`before`,`after`                                    |
| start       | Starts a defined `script`, `group`, `waitlist`, or `routine`.                                                                                                                                                                                                                                                                         | `button`,`duration`,`before`,`after`                         |
| stop        | Stops a previously started `script`, `group`, `waitlist`, or `routine`.                                                                                                                                                                                                                                                               | `button`,`duration`,`before`,`after`                         |
| script      | Defines the name (scope) of the current script.                                                                                                                                                                                                                                                                                       | `button`,`duration`,`before`,`after`, `repeat`               |
| window      | Defines the name of the application to focus when the script starts.                                                                                                                                                                                                                                                                  | `before`,`after`                                             |
| waitlist    | Defines a waitlist. A waitlist is a collection of instructions executed in ascending order of availability. Availability is defined by `cooldown`. Initially every element in the waitlist has availability 0. Once executed, availability is calculated using `cooldown` and the timestamp of when the instruction completed.        | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| routine     | Defines a routine. A routine is a collection of instructions executed in the order each instruction was inserted in. Each instruction must wait `cooldown` time before they can be executed again. If the current instruction cannot be executed, the routine is blocked until the current instruction is available and has executed. | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| random      | Defines a random. A random is a collection of instructions executed in random order based on availability. The randomly selected instruction is chosen from the set of available instructions. Availability is defined by `cooldown`. If no instructions can be ran, the random is blocked until an instruction is available.         | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |
| group       | Defines a group. A group is a sequence of defined instructions.                                                                                                                                                                                                                                                                       | `button`,`duration`,`before`,`after`, `repeat`,`instruction` |


## Instruction Parameters
The following list is a list of all parameters any instruction can have. Each parameter can have 0, 1, or 2 values. 

| Parameter  | Value `A`   | Value `B`          | Description                                                                                                                                                                                                          |
|------------|-------------|--------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `button`   | `button id` | None               | Button to press during execution.                                                                                                                                                                                    |
| `duration` | Time (ms)   | Optional time (ms) | Presses for `A` ms; if `B` is given, it's random between `A` & `B`.                                                                                                                                                  |
| `before`   | Time (ms)   | Optional time (ms) | Waits `A` ms before press; if `B` given, it's random between `A` & `B`.                                                                                                                                              |
| `after`    | Time (ms)   | Optional time (ms) | Waits `A` ms after press; if `B` given, it's random between `A` & `B`.                                                                                                                                               |
| `repeat`   | Count       | Optional count     | Repeats `A` times; if `B` given, it's random between `A` & `B`.                                                                                                                                                      |
| `<id>`     | None        | None               | Uses a predefined instruction's settings.                                                                                                                                                                            |
| `cooldown` | Time (ms)   | Optional time (ms) | Amount of time in ms before the instruction can execute again. If `epoch` is the time in ms when an instruction completed, then `epoch+cooldown` defines the time since epoch when the instruction can be ran again. |


# Development Overview
The implementation aims for simplicity and intuitiveness. In brief, a script is loaded by the interpreter, tokenized, 
and parsed into `Instructions`. Tokenization uses a bucketing structure and shares memory with the buffer containing the 
instruction string from the input file, minimizing memory usage. Once tokenized, the parser constructs an `Instruction`
object, stored in a map for quick lookup. After populating the `Instruction Map` with each script instruction, the 
interpreter discards the input file and begins executing the script's instructions.


## Walkthrough Example
Suppose for example we had the following script.
```javascript
1.    window sample window2
2.    script sample script2 with button alt, before 1000, after 1000
3.  
4.    key k0 with button a, duration 1 2, after 1, repeat 1
5.    key k1 with button b, duration 1 2, after 1, repeat 1
6.  
7.    routine r1 with k0, k1
8.  
9.    group g1 with after 1, repeat -1
10.      press k1 with repeat 2
11.  
12.   start r1
```

The instruction lines are passed into the `lexer` to be tokenized using a bucketing structure using the mentioned 
structure.
```javascript
<instruction type> <instruction id> with <parameter 1>, <parameter 2>, ..., <parameter n>
```

The `instruction type` is expected to be the first token in the instruction line and has a bucket length of exactly 1. 
The `instruction id` is a union of all tokens between the `instruction type` and the `with` keyword. The `with` keyword
signifies the end of the `instruction id` and the beginning of the `instruction parameters`. The
`instruction parameters` are comma separated and can have any positive number of tokens. Following this format, the
`lexer` will tokenize the instruction line into the following bucket.


Let us consider lines 5, 9, and 10. The lexer will produce the following buckets from lines 5, 9, and 10.
```javascript
Bucket (size 6) {
    Bucket 00: [key]
    Bucket 01: [k1]
    Bucket 02: [button, b]
    Bucket 03: [duration, 1, 2]
    Bucket 04: [after, 1]
    Bucket 05: [repeat, 1]
}

Bucket (size 4) {
    Bucket 00: [group]
    Bucket 01: [g1]
    Bucket 02: [after, 1]
    Bucket 03: [repeat, -1]
}

Bucket (size 3) {
    Bucket 00: [press]
    Bucket 01: [k1]
    Bucket 02: [repeat, 2]
}
```

Using the buckets created, the parser will construct an `Instruction` object for each instruction line. Following the 
example, the parser will construct the following `Instruction` objects from the buckets created from lines 5, 9, and 10.

```javascript
Instruction (line 5) {
    type: key
    id: k1
    indent_count: 0
    button: b
    duration: Random(min=1, max=2)
    before: Random(min=0, max=0)
    after: Random(min=1, max=1)
    repeat: Random(min=1, max=1)
    cooldown: Random(min=0, max=0)
    sub_instructions: []
}

Instruction (line 9) {
        type: group
        id: g1
        indent_count: 0
        button: none
        duration: Random(min=50, max=70)
        before: Random(min=0, max=0)
        after: Random(min=1, max=1)
        repeat: Random(min=-1, max=-1)
        cooldown: Random(min=0, max=0)
        sub_instructions: [Alias_00(k1)]
}

Instruction (line 10) {
        type: press
        id: Alias_00(k1)
        indent_count: 4
        button: none
        duration: Random(min=50, max=70)
        before: Random(min=0, max=0)
        after: Random(min=30, max=50)
        repeat: Random(min=2, max=2)
        cooldown: Random(min=0, max=0)
        sub_instructions: [k1]
}
````