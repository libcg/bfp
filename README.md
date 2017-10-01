# bfp - Beyond Floating Point

bfp is a C/C++ posit floating point format implementation.

From Dr. John L. Gustafson's intro:

"A new data type called a "posit" is designed for direct drop-in replacement for
IEEE Standard 754 floats. Unlike unum arithmetic, posits do not require
interval-type mathematics or variable size operands, and they round if an answer
is inexact, much the way floats do. However, they provide compelling advantages
over floats, including simpler hardware implementation that scales from as few
as two-bit operands to thousands of bits. For any bit width, they have a larger
dynamic range, higher accuracy, better closure under arithmetic operations, and
simpler exception-handling. For example, posits never overflow to infinity or
underflow to zero, and there is no "Not-a-Number" (NaN) value. Posits should
take up less space to implement in silicon than an IEEE float of the same size.
With fewer gate delays per operation as well as lower silicon footprint, the
posit operations per second (POPS) supported by a chip can be significantly
higher than the FLOPs using similar hardware resources. GPU accelerators, in
particular, could do more arithmetic per watt and per dollar yet deliver
superior answer quality."

### Goals

- Create a human readable posit reference implementation
- Can be used as a library
- Can be used in embedded projects

### References

- [Abstract](http://web.stanford.edu/class/ee380/Abstracts/170201.html)
- [Paper](http://www.johngustafson.net/pdfs/BeatingFloatingPoint.pdf)
- [Video presentation](https://www.youtube.com/watch?v=aP0Y1uAA-2Y)
- [Posit slides](http://supercomputingfrontiers.com/2017/wp-content/uploads/2017/03/2_1100_John-Gustafson.pdf) ([previous slides](http://ee380.stanford.edu/Abstracts/170201-slides.pdf))
- [Unum slides](http://www.johngustafson.net/presentations/Unums2.0.pdf)
