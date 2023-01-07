/*
 * Arfon speach synthesizer driver
 * uses RC2014 sound card i/o ports
 
 * Compiled to .com (.bin) file with z88dk - sdcc
 * PS C:\z88dk\rc2014> zcc +rc2014 -subtype=hbios -v --list -m -SO3 --c-code-in-asm                 -clib=sdcc_iy  --max-allocs-per-node200000 arfon.c  -o arfon  -create-app
 *  
 * D. Richards, May 2020
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <z80.h>

// 8 bit I/O ports
#define IO_SR0	0xd0
#define IO_SR8	0xd8

__sfr __at IO_SR0 s0;
__sfr __at IO_SR8 s8;


char opbuf[8][40];
int opidx;

const char * const spk_dictionary[] = 
{
  "Digitalker",               /* 0 */
  "One",                      /* 1 */
  "Two",                      /* 2 */
  "Three",                    /* 3 */
  "Four",                     /* 4 */
  "Five",                     /* 5 */
  "Six",                      /* 6 */
  "Seven",                    /* 7 */
  "Eight",                    /* 8 */
  "Nine",                     /* 9 */

  "Ten",                      /* 10 */
  "Eleven",                   /* 11 */
  "Twelve",                   /* 12 */
  "Thirteen",                 /* 13 */
  "Fourteen",                 /* 14 */
  "Fifteen",                  /* 15 */
  "Sizteen",                  /* 16 */
  "Seventeen",                /* 17 */
  "Eighteen",                 /* 18 */
  "Ninteen",                  /* 19 */

  "Twenty",                   /* 20 */
  "Thirty",                   /* 21 */
  "Fourty",                   /* 22 */
  "Fifty",                    /* 23 */
  "Sixty",                    /* 24 */
  "Seventy",                  /* 25 */
  "Eighty",                   /* 26 */
  "Ninety",                   /* 27 */
  "Hundred",                  /* 28 */
  "Thousand",                 /* 29 */

  "Million",                  /* 30 */
  "Zero",                     /* 31 */
  "A",                        /* 32 */
  "B",                        /* 33 */
  "C",                        /* 34 */
  "D",                        /* 35 */
  "E",                        /* 36 */
  "F",                        /* 37 */
  "G",                        /* 38 */
  "H",                        /* 39 */

  "I",                        /* 40 */
  "J",                        /* 41 */
  "K",                        /* 42 */
  "L",                        /* 43 */
  "M",                        /* 44 */
  "N",                        /* 45 */
  "O",                        /* 46 */
  "P",                        /* 47 */
  "Q",                        /* 48 */
  "R",                        /* 49 */

  "S",                        /* 50 */
  "T",                        /* 51 */
  "U",                        /* 52 */
  "V",                        /* 53 */
  "W",                        /* 54 */
  "X",                        /* 55 */
  "Y",                        /* 56 */
  "Z",                        /* 57 */
  "Again",                    /* 58 */
  "Ampere",                   /* 59 */

  "And",                      /* 60 */
  "At",                       /* 61 */
  "Cancel",                   /* 62 */
  "Case",                     /* 63 */
  "Cent",                     /* 64 */
  "400Hz",                    /* 65 */
  "80Hz",                     /* 66 */
  " ",                        /* 67 */
  "  ",                       /* 68 */
  "   ",                      /* 69 */

  "    ",                     /* 70 */
  "     ",                    /* 71 */
  "Centi",                    /* 72 */
  "Check",                    /* 73 */
  "Comma",                    /* 74 */
  "Control",                  /* 75 */
  "Danger",                   /* 76 */
  "Degree",                   /* 77 */
  "Dollar",                   /* 78 */
  "Down",                     /* 79 */

  "Equal",                    /* 80 */
  "Error",                    /* 81 */
  "Feet",                     /* 82 */
  "Flow",                     /* 83 */
  "Fuel",                     /* 84 */
  "Gallon",                   /* 85 */
  "Go",                       /* 86 */
  "Gram",                     /* 87 */
  "Great",                    /* 88 */
  "Greater",                  /* 89 */

  "Have",                     /* 90 */
  "High",                     /* 91 */
  "Higher",                   /* 92 */
  "Hour",                     /* 93 */
  "In",                       /* 94 */
  "Inches",                   /* 95 */
  "Is",                       /* 96 */
  "It",                       /* 97 */
  "Kilo",                     /* 98 */
  "Left",                     /* 99 */


  "Less",                     /* 100 */
  "Lesser",                   /* 101 */
  "Limit",                    /* 102 */
  "Low",                      /* 103 */
  "Lower",                    /* 104 */
  "Mark",                     /* 105 */
  "Meter",                    /* 106 */
  "Mile",                     /* 107 */
  "Milli",                    /* 108 */
  "Minus",                    /* 109 */

  "Minute",                   /* 110 */
  "Near",                     /* 111 */
  "Number",                   /* 112 */
  "Of",                       /* 113 */
  "Off",                      /* 114 */
  "On",                       /* 115 */
  "Out",                      /* 116 */
  "Over",                     /* 117 */
  "Parenthesis",              /* 118 */
  "Percent",                  /* 119 */

  "Please",                   /* 120 */
  "Plus",                     /* 121 */
  "Point",                    /* 122 */
  "Pound",                    /* 123 */
  "Pulses",                   /* 124 */
  "Rate",                     /* 125 */
  "Re",                       /* 126 */
  "Ready",                    /* 127 */
  "Right",                    /* 128 */
  "s",                        /* 129 */

  "Second",                   /* 130 */
  "Set",                      /* 131 */
  "Space",                    /* 132 */
  "Speed",                    /* 133 */
  "Star",                     /* 134 */
  "Start",                    /* 135 */
  "Stop",                     /* 136 */
  "Than",                     /* 137 */
  "The",                      /* 138 */
  "Time",                     /* 139 */

  "Try",                      /* 140 */
  "Up",                       /* 141 */
  "Volt",                     /* 142 */
  "Weight"                    /* 143 */

};

#define SPK_DICTIONARY_COUNT ( sizeof( spk_dictionary ) / sizeof( char * )  )

void say(int i); 		// send speach index, then strobe




/* A function that prints given number in words */
void convert_to_words(char *num) 
{
    int len = strlen(num); // Get number of digits in given number 
    opidx=0;
    
    /* Base cases */
    if (len == 0) { 
        fprintf(stderr, "empty string\n"); 
        return; 
    } 
    if (len > 4) { 
        fprintf(stderr, "Length more than 4 is not supported\n"); 
        return; 
    } 
  
    /* The first string is not used, it is to make 
        array indexing simple */
    const char const *single_digits[] = { "zero", "one", "two",  
                               "three", "four","five",  
                               "six", "seven", "eight", "nine"}; 
  
    /* The first string is not used, it is to make  
        array indexing simple */
    const char const *two_digits[] = {"", "ten", "eleven", "twelve",  
                               "thirteen", "fourteen", 
                               "fifteen", "sixteen",  
                               "seventeen", "eighteen", "nineteen"}; 
  
    /* The first two string are not used, they are to make  
        array indexing simple*/
    const char const *tens_multiple[] = {"", "", "twenty", "thirty", "forty", "fifty", 
                            "sixty", "seventy", "eighty", "ninety"}; 
  
    const char const *tens_power[] = {"hundred", "thousand"}; 
    const char const *tens_power_link[] = {"and", ""}; 
  
    /* Used for debugging purpose only */
    printf("\n%s: ", num); 
  
    /* For single digit number */
    if (len == 1) { 
        printf("%s\n", single_digits[*num - '0']); 
        printf(opbuf[opidx++],"%s\n", single_digits[*num - '0']); 
        return; 
    } 
  
    /* Iterate while num is not '\0' */
    while (*num != '\0') { 
  
        /* Code path for first 2 digits */
        if (len >= 3) { 
            if (*num -'0' != 0) { 
                printf("%s ", single_digits[*num - '0']); 
                sprintf(opbuf[opidx++],"%s", single_digits[*num - '0']); 
                printf("%s ", tens_power[len-3]); // here len can be 3 or 4 
                printf("%s ", tens_power_link[len-3]); // here len can be 3 or 4 
                sprintf(opbuf[opidx++],"%s", tens_power[len-3]); // here len can be 3 or 4 
                sprintf(opbuf[opidx++],"%s", tens_power_link[len-3]); // here len can be 3 or 4 
            } 
            --len; 
        } 
  
        /* Code path for last 2 digits */
        else { 
            /* Need to explicitly handle 10-19. Sum of the two digits is 
            used as index of "two_digits" array of strings */
            if (*num == '1') { 
                int sum = *num - '0' + *(num + 1)- '0'; 
                printf("%s\n", two_digits[sum]); 
                sprintf(opbuf[opidx++],"%s", two_digits[sum]); 
                return; 
            } 
  
            /* Need to explicitely handle 20 */
            else if (*num == '2' && *(num + 1) == '0') { 
                printf("twenty\n"); 
                sprintf(opbuf[opidx++],"twenty"); 
                return; 
            } 
  
            /* Rest of the two digit numbers i.e., 21 to 99 */
            else { 
                int i = *num - '0'; 
                printf("%s ", i? tens_multiple[i]: ""); 
                sprintf(opbuf[opidx++],"%s", i? tens_multiple[i]: ""); 
                ++num; 
                if (*num != '0') { 
                    printf("%s ", single_digits[*num - '0']); 
                    sprintf(opbuf[opidx++],"%s", single_digits[*num - '0']); 
		}
            } 
        } 
        ++num; 
    } 
} 

void init(void)
{
  s8=0x07;		 	// a & b outputs
  s0=0xc0;
  s8=0x0f;			// clear b
  s0=0x00;
  s8=0xe0;
  s0=0x00;
}

void wait(void) 		// wait for Int after speach is finished
{
  volatile int ia;
  int wd=0;

  s8=0x07;			// select ddr register
  s0=0x80;			// set a inputs, b outputs

  for(wd=0; wd<32000; wd++)
  {
    s8=0x0e;			// select a
    ia=s8;			// read a
    if(ia&0x80)                 // check for int high
    {
      break;
    }     
  }  
  s8=0x07;			// restor a & b as outputs
  s0=0xc0;
  return;
}

void say(int i) 		// send speach index, then strobe
{
  s8=0x0e;			// select a
  s0=0x00;			// set low

  s8=0x0f;			// select b
  s0=i;				// set key

  s8=0x0e;			// select a
  s0=0x01;			// set strobe

  wait();
}

int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

int find(char * inword, int *key) // lookup index for word to say
{
  int i;
  int value;
  for(i=0; i<SPK_DICTIONARY_COUNT; i++)
  {

    // try to convert text to non zero integer
    value = (int) strtol(inword, (char **)NULL, 10);      
    if(value != 0)
    {
        *key = value;
        return 1;
    }

    // try to find word in dictionary  
    if(strcicmp(spk_dictionary[i], inword)==0)
    {
      *key = i;
//      printf("key %d, Word %s\n",i,spk_dictionary[i]);
      return 1;
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  char buffer[80];
  char * pch;
  int key=-1;
  int i;

  init();               

  if(argc>1)
  {
        
    for(i=1; i<argc; i++)
    {
      printf("argc %d, argv=[%s]\n", i, argv[i]);
      convert_to_words(argv[i]);
      if(opidx>0)
      {
        for(i=0; i<opidx; i++)
        {
  //	      printf("%s ",opbuf[i]);
          if(find(opbuf[i], &key) == 1)
          {
            say(key);
          }
          else
            printf("?[%s] ",opbuf[i]);
        } 
      }      
    }

#if 0  
    for(i=1; i<argc; i++)
    {
      if(find(argv[i], &key))
      {
        say(key);
//         printf("key %d = %s\n", key, spk_dictionary[key]);
      }
        
    }
#endif

  } 
  else 
  {
  
    while (key!=0) 
    {
      printf("\nWords? ");
      scanf ("%s",buffer);

      pch = strtok (buffer,", .-");
      while (pch != NULL)
      {  
        if(find(pch, &key) == 1)
        {
          say(key);
        }
        pch = strtok (NULL, ", .-");
      }
    }
  }
  return 0;
}