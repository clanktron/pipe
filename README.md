## UID: 005412538

## Pipe Up

The binary pipes together all listed commands, the first one taking input from stdin and the last one printing to stdout.

## Building

Clone the repository and run the following:
```bash
# compile the binary
make
```

## Running

Example usage:
```bash
# one argument (should output the same as ls would by itself)
./pipe ls
# multiple arguments (ls output should be piped to cat, which should ouput to stdout)
./pipe ls cat
# multiple arguments (stdout should be result of less)
./pipe ls cat cat less
```

## Cleaning up

```bash
# remove object and binary files from compilation
make clean
```
