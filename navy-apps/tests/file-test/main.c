#include <stdio.h>
#include <assert.h>

int main()
{
  // assert(0);
  printf("here0\n");
  FILE *fp1 = fopen("/bin/hello", "r+");
  FILE *fp = fopen("/share/files/num", "r+");
  printf("here3\n");
  assert(0);
  assert(fp);

  assert(0);
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);
  assert(0);
  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  assert(0);
  for (i = 500; i < 1000; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
  assert(0);
  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i++)
  {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i++)
  {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }
  assert(0);
  fclose(fp);
  assert(0);
  printf("PASS!!!\n");

  return 0;
}
