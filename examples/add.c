/* clang -shared -o examples/add.so examples/add.c */

int add3(int x, int y, int z) {
  return x + y + z;
}

int add(int x, int y) {
  return x + y;
}

int add1(int x) {
  return x + 1;
}
