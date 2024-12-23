# Binary Size Optimization

Here's some notes on keeping binary size low that I've picked up over time.

First of all, I apply many of the tips from the [Rust binary size optimization guide](https://github.com/johnthagen/min-sized-rust). The most effective things have been:

- Build with size-optimized LLVM profile
- Abort on panic
- Remove panic location info, formatting strings, and formatting code
  - Removing panic strings and formatting code is behind a nightly-only flag. I try to avoid nightly features, but this one is pretty justified
- Avoid core::fmt (although it's less safe, I wrap `mkb::sprintf`)

I also recently discovered that avoiding 64-bit integer types saves around ~7kb! I guess for this 32-bit architecture it causes many software math functions to be pulled in.

## Size Increases from Innocuous Changes

I've found that certain innocuous-looking changes can unexpectly increase the binary size significantly. I guess this is some sort of compiler issue?

For example: the `cbrt_approx()` function in math.rs increase binary size by 6KB if it is instead placed in menu_impl.rs, at least as of the time this note was committed.
