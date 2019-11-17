
#include <stdio.h>
#include <stdlib.h>

int damping = 84;

unsigned char _damping(unsigned char c, unsigned char l)
{
   int delta = ((c - l) / 100.0) * (double)damping;

   if (!delta && c < l)
      delta--;
   else if (!delta && c > l)
      delta++;

   return l + delta;
}

unsigned char ddamping(unsigned char c, unsigned char l)
{
   int delta;
   double percent = 100 - (damping);

   delta = ((c - l) / 100.0) * percent;

      if (!delta && c < l)
      delta--;
   else if (!delta && c > l)
      delta++;

   return l + delta;
}
int main(int argc, char** argv)
{
   damping = atoi(argv[2]);
   int c = atoi(argv[1]);
   int o = 100;
   int cnt = 0;

   while (o != c)
   {
      int prev = o;
      o = ddamping(c, o);

      printf("request of %d - set to %d, old was %d \n", c, o, prev);
      cnt++;
   }

   printf("%d steps\n", cnt);

   return 0;
}
