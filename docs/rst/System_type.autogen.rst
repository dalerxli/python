
===========
System_type
===========

System_type
===========
The parameter is provides the program with a broad
overview of the tyep of system that will be simulated, and is used
by Python to initialize certain variable, and to control what variables
are asked for later.

**Type:** Enum (Int)

**Values:**

0. star - System in which the central object is a star or WD

1. binary - Syatem with a secondary star, which can occult the central object and disk depending on phase

2. agn - AGN or BH binary (curruently)

3. previous - In this case, one is starting from a previous run with python, and one want to either continue the run or change some parameters associated with radiations sources


**Parent(s):** None - This is always read.

**File:** python.c


