% BUNIFDEF(1) bunifdef user manual | Version 0.1.0
% Alexander Romanov
% July, 2025

Better (not yet) unifdef tool

# SYNOPSIS

```
bunifdef TARGET_FILE [OPTIONS]
```

# DESCRIPTION

Bunifdef is a WIP tool to selectively (not by default) remove C preprocessor
conditionals. This project was born as a result of its author's dissatisfuction
with unifdef tool while having too much free time.

# OPTIONS

**-o [--output-filew] FILE**
Output file name.

**-Dsym=val**
Specify that a macro is defined to a given value.

**-Dsym**
Specify that a macro is defined to the value 1.

**-Usym**
Specify that macro is undefined

Currently this flag maked macro undefined even if it is followed by -Dsym

**-s [ --select ]**
Remove conditional selectively. By default removes all conditionals and
reports errors on undefined macros in **#if** expressions.

# EXAMPLES

1. Run bunifdef on ./file.txt while defining DEF1 and DEF2 to 1 and
   undefining DEF3. Print output to ./processed.txt.

```sh
bunifdef ./file.txt -DDEF1=1 -DDEF2 -UDEF3 -o ./processed.txt
```

1. Run bunifdef on ./file.txt without any macros defined.
   Print output to stdout.

```sh
bunifdef ./file.txt -DDEF1 -UDEF1 -o -
```
