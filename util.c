/* THIS IS THE util.c FILE */

FILE *fpout;
FILE *fptoc;
FILE *fplog;

#define  OUT_WIDTH       40
#define  SPACE_LEFT       5
#define  LINES_PER_PAGE  40
#define  TOC_ON           1

char  line[128];
int   lines_so_far;

void  init_lines_so_far(){
  lines_so_far = 0;
}

void  incr_lines_so_far(){
  lines_so_far++;
}

int   check_done_page(){
  if (lines_so_far < LINES_PER_PAGE) 
    return 0;
  else
    return 1;
}
/* Took this function from 
 * https://stackoverflow.com/questions/4986521/how-to-convert-integer-value-to-roman-numeral-string 
 * TA said that would be okay, hope so
 * */
void convertToRoman (unsigned int val, char *res) {
    char *huns[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    char *tens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    char *ones[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    int   size[] = { 0,   1,    2,     3,    2,   1,    2,     3,      4,    2};

    //  Add 'M' until we drop below 1001.
    while (val >= 1000) {
        *res++ = 'M';
        val -= 1000;
    }
    // Add each of the correct elements, adjusting as we go.

     strcpy (res, huns[val/100]); res += size[val/100]; val = val % 100;
     strcpy (res, tens[val/10]);  res += size[val/10];  val = val % 10;
     strcpy (res, ones[val]);     res += size[val];

     //Finish string off.
   
     *res = '\0';
}


struct  doc_symtab {
      int     page_no_counter;
      int     page_style;
      int     line_spacing;
      int     current_font;
      int     generate_toc;
      int     section_counter;
      int     subsect_counter;
};

struct  doc_symtab  DST;

void  init_sec_ctr(){
  DST.section_counter = 1;
  DST.subsect_counter = 1;
}

void  incr_sec_ctr(){
  DST.section_counter++;
  DST.subsect_counter = 1;
}
void  incr_subsec_ctr(){
  DST.subsect_counter++;
}

int  get_sec_ctr(){
  return DST.section_counter;
}

int  get_subsec_ctr(){
  return DST.subsect_counter;
}

int   get_gen_toc(){
  return  DST.generate_toc;
}

void  set_gen_toc(){
  DST.generate_toc = 1;
}

void  set_page_no(int p){
  DST.page_no_counter = p ;
}

int   get_page_no(){
  return DST.page_no_counter;
}

int   inc_page_no(){
  DST.page_no_counter++;
  return (DST.page_no_counter - 1);
}
int get_page_style(){
  return DST.page_style;
}

void  set_page_style(int s){
  DST.page_style = s;
}
