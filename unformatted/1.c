#include <stdio.h>
#include <stddef.h>

# define size1 10
# define size2 30
# define size3 16
# define size4 6
/// ISAAC config start
/* a ub4 is an unsigned 4-byte quantity */
typedef  unsigned long int  ub4;

/* external results */
ub4 randrsl[256], randcnt;

/* internal state */
static    ub4 mm[256];
static    ub4 aa=0, bb=0, cc=0;


void isaac()
{
   register ub4 i,x,y;

   cc = cc + 1;    /* cc just gets incremented once per 256 results */
   bb = bb + cc;   /* then combined with bb */

   for (i=0; i<256; ++i)
   {
     x = mm[i];
     switch (i%4)
     {
     case 0: aa = aa^(aa<<13); break;
     case 1: aa = aa^(aa>>6); break;
     case 2: aa = aa^(aa<<2); break;
     case 3: aa = aa^(aa>>16); break;
     }
     aa              = mm[(i+128)%256] + aa;
     mm[i]      = y  = mm[(x>>2)%256] + aa + bb;
     randrsl[i] = bb = mm[(y>>10)%256] + x;

     /* Note that bits 2..9 are chosen from x but 10..17 are chosen
        from y.  The only important thing here is that 2..9 and 10..17
        don't overlap.  2..9 and 10..17 were then chosen for speed in
        the optimized version (rand.c) */
     /* See http://burtleburtle.net/bob/rand/isaac.html
        for further explanations and analysis. */
   }
}


/* if (flag!=0), then use the contents of randrsl[] to initialize mm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

void randinit(int flag)
{
   int i;
   ub4 a,b,c,d,e,f,g,h;
   aa=bb=cc=0;
   a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

   for (i=0; i<4; ++i)          /* scramble it */
   {
     mix(a,b,c,d,e,f,g,h);
   }

   for (i=0; i<256; i+=8)   /* fill in mm[] with messy stuff */
   {
     if (flag)                  /* use all the information in the seed */
     {
       a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
       e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
     }
     mix(a,b,c,d,e,f,g,h);
     mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
     mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
   }

   if (flag)
   {        /* do a second pass to make all of the seed affect all of mm */
     for (i=0; i<256; i+=8)
     {
       a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
       e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
       mix(a,b,c,d,e,f,g,h);
       mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
       mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
     }
   }

   isaac();            /* fill in the first set of results */
   randcnt=256;        /* prepare to use the first set of results */
}
/// ISAAC config end
int abs(int i)
{
    if (i<0) return -i;
    else return i;
}
int usermap(int i)
{
    if (i>=0 && i<=9) return i+48;
    else if (i>=10 && i<=35) return i+55;
    else if (i>=36 && i<=61) return i+61;
    else return -1;
}
int main()
{
    printf("Enter your mainkey.\n");
    char mainkey[size1]={0};
    masterkey_input:
    for (int i=0;i<size1;i++)
    {
        scanf("%c",&mainkey[i]);
        if ((int)mainkey[i]<32 || (int)mainkey[i]>126)
        {
            printf("Error, try again.\n");
            goto masterkey_input;
        }
    }
    int num_mainkey[10]={0};
    for (int i=0;i<size1;i++) num_mainkey[i]=mainkey[i]-32;


    ub4 i,j;
    aa=bb=cc=(ub4)0;


  for (i=0; i<256; ++i) mm[i]=randrsl[i]=(ub4)0;
  randinit(1);
  for (int i=0;i<size1;i++)
  {
      aa+=num_mainkey[i];
      bb+=num_mainkey[(i+1)%size1];
      cc+=num_mainkey[(i+2)%size1];
      isaac();
  }
  printf("Enter the site name or application's package name.\n");
  char sitekey[size2]={0};

      int bug=0;
      sitekey_input:
    for (int i=0;i<size2;i++)
    {
        scanf("%c",&sitekey[i]);
           if ((int)sitekey[i]==10 && i>0)
        {
            sitekey[i]=0;
            break;
        }
        else if ((int)sitekey[i]<32 || (int)sitekey[i]>126)
        {
            if ((int)sitekey[i]!=10) printf("Error, try again.\n");
            goto sitekey_input;
        }
    }
     int num_sitekey[size2]={0};
    for (int i=0;i<size2;i++)
    if ((int)sitekey[i]>0)
    {
        num_sitekey[i]=(int)sitekey[i]-32;
        num_sitekey[i]=num_sitekey[i]^num_mainkey[i];
    }
    isaac();

       for (int i=0;i<size2;i++)
    {
      aa+=num_sitekey[i];
      bb+=num_sitekey[(i+1)%size1];
      cc+=num_sitekey[(i+2)%size1];
      isaac();
    }

    int num_pass[size3];
  printf("Your password is: ");
  for (i=0; i<size3;i++)
  {
    redo:
    num_pass[i]=abs((int)randrsl[i])%95;
    if (num_pass[i]==0 && i==0) num_pass[i]=94;
    if (num_pass[i]==0 && i==size3-1) num_pass[i]=1;
    num_pass[i]+=32;
    printf("%c",(char)num_pass[i]);
  }
  isaac();
  printf("\nYour recommended username is: ");
  int num_user[size4];
  for (i=0; i<size4;i++)
  {
      num_user[i]=usermap(abs((int)randrsl[i])%62);
      printf("%c",(char)num_user[i]);
  }
    return 0;
}