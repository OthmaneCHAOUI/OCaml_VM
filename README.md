# OCaml Virtual Machine

A C implementation of a virtual machine that executes OCaml bytecode in the SOBF format.

## Overview

The OCaml programming language compiles source code to bytecode, a low-level representation that is then interpreted by a virtual machine. This project implements a subset of that virtual machine in C, allowing the execution of compiled OCaml programs on any platform with a C11-compatible compiler.

The VM reads `.sobf` binary files containing a code table (instructions and data) and a table of global values, then interprets the instructions sequentially. It manages an accumulator, a dynamically-sized value stack, and a table of global values. The implementation covers arithmetic instructions, stack manipulation, branching, memory block allocation, and basic I/O primitives.

The project was developed as part of the **PRIM (Programmation impérative)** module at ENSIIE.

## Features

- Reading and validation of `.sobf` binary files
- Sequential file parsing with on-the-fly structure validation
- Implementation of a subset of OCaml bytecode instructions:
  - Arithmetic operations: addition, subtraction, multiplication, division, modulo, negation
  - Stack manipulation: push, pop, seek at arbitrary depth
  - Branching instructions
  - Memory block allocation
  - Basic I/O primitives: read/write characters, flush, access to `stdin`/`stdout`/`stderr`
- Dynamic value stack with automatic resizing
- Proper memory management for VM structures
- Command-line interface with optional debug mode

## Tech Stack

| Component | Technology |
|---|---|
| Language | C (C11) |
| Compiler | GCC |
| Build | Make |
| Version control | Git |
| Environment | Debian (WSL, VirtualBox) |

## Architecture

The VM operates on two data types:

- **Codes**: 32-bit instructions (`int` in C)
- **Values**: 64-bit data, potentially memory addresses (`long int` in C)

Integers are encoded as odd numbers: `n` is represented by the value `2n + 1`. Booleans `false` and `true` are encoded as `1` and `3` respectively.

The VM is composed of:

- A **code table** containing instructions and data
- An **index** pointing to the currently-executed instruction (initially 0)
- An **accumulator** holding a value (initially 1)
- A **stack** of values (initially empty, top element at depth 0)
- A **global values table**

Execution proceeds by reading the code at the current index, performing the associated action (which may modify the index, accumulator, stack, or globals), then moving to the next instruction unless a branch occurs.

The codebase is split into four modules:

| Module | Responsibility |
|---|---|
| `file_reader` | Reading and validating SOBF files |
| `stack` | Value stack implementation |
| `virtual_machine` | VM core and instruction execution |
| `main` | User interface and entry point |

## Requirements

- GCC (with C11 support) or any compatible C compiler
- GNU Make
- Linux environment (tested on Debian)

## Installation

Clone the repository:

```bash
git clone git@github.com:OthmaneCHAOUI/OCaml_VM.git
cd ocaml-vm
```

Build the project:

```bash
make
```

This produces the executable `bin/ocaml_vm`.

## Usage

Run a bytecode file:

```bash
./bin/ocaml_vm path/to/program.sobf
```

Enable the debug flag to print the final state of the machine:

```bash
./bin/ocaml_vm path/to/program.sobf --print-end-machine
```

Example:

```bash
$ ./bin/ocaml_vm test_units/unit0/fact.sobf
```

## Testing

Test files are located in the `test_units/` directory:

- `unit0/` — test cases provided by the instructor
- `unit3/` — additional tests created during development

Each `.sobf` file has a corresponding `.txt` file containing the expected output. To run a test, simply execute it through the VM:

```bash
./bin/ocaml_vm test_units/unit0/base.sobf
```

Tests cover arithmetic operations, stack manipulation, branching, memory blocks, and I/O primitives.

## Project Structure

```text
.
├── bin/                  # Compiled executable
├── docs/                 # Documentation, report, and reference material
├── obj/                  # Intermediate object files
├── src/
│   ├── header/           # Header files (.h)
│   ├── implementation/   # Source files (.c)
│   └── main/             # Entry point (main.c)
├── test_units/
│   ├── unit0/            # Provided tests
│   └── unit3/            # Additional tests
├── Makefile
└── README.md
```

## Design Decisions

### Sequential file reading

Two approaches were considered for reading `.sobf` files:

1. Load the entire file into a string and validate by index access.
2. Read sequentially and validate on the fly.

The second approach was chosen because it avoids intermediate string conversions, reduces memory usage, and detects format errors early.

### Unified data structure

Rather than reading each field (codes, globals, sizes) through separate functions that reopen the file, a single `file_data` structure is filled by one function in a single pass. This is more efficient and simplifies memory cleanup.

### Binary mode

Files are opened with `"rb"` instead of `"r"` to prevent text-mode end-of-line conversions that would corrupt binary data.

### Stack implementation

The stack is implemented as a dynamically-resizable array. It starts with a small capacity and grows automatically when needed, supporting programs with arbitrarily deep stacks.

### Instruction dispatch

Instructions are dispatched through a single `switch` statement on the opcode. Variables local to `case` blocks are wrapped in braces to create a proper scope — otherwise the compiler rejects jumps that skip initializations.

### Arithmetic optimization

Integer encoding is respected using optimized formulas. For example, `ADDINT` computes `2(n + m) + 1` directly from the encoded operands `2n + 1` and `2m + 1`, avoiding decode/re-encode overhead.

## Limitations

- **No garbage collector**: memory blocks allocated by the VM are never freed until program termination.
- **Partial instruction set**: only a subset of OCaml bytecode instructions is implemented (those defined in the project specification).
- **No exception handling**: OCaml exceptions are not supported.

## Roadmap

- [x] SOBF file parsing and validation
- [x] Core instructions (arithmetic, stack, branches, blocks)
- [x] Basic I/O primitives
- [x] Dynamic stack
- [ ] Simple garbage collector
- [ ] Additional instructions (exception handling, closures)
- [ ] Expanded primitive set

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.


## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## Contact

For questions or feedback, please open an issue on GitHub.