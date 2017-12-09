/* THIS IS latexp3code.y */
%{ /* A YACC FOR PART 3 OF THE PROJECT WHERE VERBATIM AND NESTING WORKS */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define  BUF_SIZE       512
#define YYDEBUG 1

#define LAlph  1
#define CAlph  2
#define LRoman 3
#define CRoman 4
#define Arabic 5

int ws_flag = 0;
int noin_flag = 0;
int it_flag = 0;
int line_spacing = 0;
int single_flag = 0;
int text_index = 0;
int tmp_text_index = 0;
int spec_chars = 0;
#define INDEX text_index + spec_chars
int enumerate = 0;
int enumeration = 1;
int itemize = 0;
int item_depth = 0;
int item_width = 4;
#define ITEM_SPACING item_depth*item_width

int center_flag = 0;
int verb_flag = 0;

#include "latexp3c.tab.h"
#include "util.c"
#include "generate.c"
int yyerror(){}
int yylex();
%}

%debug 

%union 
   {
      char   trans[BUF_SIZE+1];
      int    val;
   }

%start latexstatement

%token  BACKSL    LBEGIN    LCURLYB    DOCUMENT    RCURLYB    END
%token  WORD      WS        SPECCHAR   CENTER      VERBATIM   SINGLE  
%token  ITEMIZE   ENUMERATE TABULAR    TABLE       LSQRB      RSQRB
%token  H         T         B          COLS
%token  CAPTION   LABEL     DBLBS      ITEM        SECTION    SUBSEC  
%token  TABOCON   RENEW     BASELINES  PAGENUM     INTEGER    ARABIC1 
%token  LROMAN1   CROMAN1   LALPH1     CALPH1      VSPACE     HSPACE
%token  RM        IT        NOINDENT   REF 
%token  ARABIC2   LROMAN2   CROMAN2    LALPH2      CALPH2

%type <trans> textoption  wsorword WS WORD entry
%type <val> style2 ARABIC2 LROMAN2 CROMAN2 LALPH2 CALPH2 curlyboptions fonts style1

%%
latexstatement   :  startdoc  mainbody  enddoc { fprintf(fplog,"Complete\n");}
                 ;

startdoc         :  LBEGIN  DOCUMENT {fprintf(fplog, "started doc\n");} 
                 ;

enddoc           :  END  DOCUMENT  {fprintf(fplog, "finished doc\n");print_page_number();} 
                 ;

mainbody         :  mainbody  mainoption
                 |  mainoption
                 ;

mainoption       :  textoption
                    {
                      generate_formatted_text($1);
                      tmp_text_index = text_index;
                      text_index = 0;
                    }
                 |  commentoption
                 |  latexoptions
                 ;

textoption       :  textoption  wsorword
                    {
                      strcat($$, " ");
                      strcat($$, $2);
                      
                    }
                 |  wsorword
                    {
                      strcpy($$, $1);
                      
                    }
                 ;

wsorword         :  WS 
                    {
                      strcpy($$, $1);
                    }
                 |  WORD
                    {
                      strcpy($$, $1);
                    }
                 ;

commentoption    :  SPECCHAR  textoption
                 ;

latexoptions     :  backsoptions
                 |  LCURLYB  curlyboptions  RCURLYB
                 {
                   it_flag = $2;
                 }
                    
                 ;

curlyboptions    :  fonts  textoption
                 {    
                    $$ = $1;
                    text_index = tmp_text_index;
                    tmp_text_index = 0;
                    if(text_index > 0) {line[INDEX] = ' '; text_index++;}
                    generate_formatted_text($2);
                 }
                 ;

backsoptions     :  beginendopts {print_line();}
                 |  sectionoptions {print_line();}
                 |  tableofcont {print_line();}
                 |  linespacing {print_line();}
                 |  pagenumbers
                 |  pagenuminit
                 |  spacing {print_line();}
                 |  fonts
                 {
                    text_index = tmp_text_index;
                    tmp_text_index = 0;
                    if(text_index > 0) {line[INDEX] = ' '; text_index++;}
                 }
                 |  specialchar
                 |  nonewpara
                 |  reference {print_line();}
                 ;
     
beginendopts     :  LBEGIN  begcmds  beginblock  endbegin  
                 ;

begcmds          :  CENTER  {center_flag=1;}
                 |  VERBATIM  {ws_flag=1;}
                 |  SINGLE {single_flag = 1;} 
                 |  ITEMIZE 
                 {
                    itemize++;
                    item_depth++;
                 }
                 |  ENUMERATE 
                 {
                    enumerate = 1;
                    item_depth++;
                 }
                 |  TABLE  begtableopts
                 |  TABULAR  begtabularopts
                 ;

endbegin         :  END  endcmds
                 |  endtableopts  TABLE  
                 ;

endcmds          :  CENTER  {center_flag=0;}
                 |  VERBATIM  {ws_flag=0;}
                 |  SINGLE {single_flag = 0;} 
                 |  ITEMIZE 
                 {
                    itemize--;
                    item_depth--;
                 } 
                 |  ENUMERATE
                 {
                    enumerate = 0;
                    enumeration = 0;
                    item_depth--;
                 } 
                 |  TABULAR
                 ;
beginblock       :  beginendopts
                 |  textoption /* FOR single or verbatim */
                 {  
                    printf("single or verb\n");
                    
                    if (single_flag){ generate_formatted_text($1); print_line();}

                    if (ws_flag){ fprintf(fpout, "%s", $1); }
                 }
                 |  entrylist  /* FOR center and tabular */
                                    {printf("center or tabular\n");

                                    
                                    }
                 |  listblock  /* FOR item and enumerate */
                                    {printf("item or enumerate\n");}
                 ;

listblock        :  listblock  anitem
                                    {printf("listblockA\n");}
                 |  anitem
                                    {printf("listblockB\n");}
                 ;

anitem           :  ITEM  textoption
                 {
                    generate_item($2);
                 }
                 |  beginendopts
                 ;

entrylist        :  entrylist  anentry
                                    {printf("entrylistA\n");}
                 |  anentry
                                    {printf("entrylistB\n");}
                 ;

anentry          :  entry  DBLBS
                                    {printf("anentryA\n"); if (center_flag) generate_formatted_text($1); print_line();}

                 |  beginendopts
                                    {printf("anentryB\n");}
                 ;

entry            :  entry  SPECCHAR  textoption
                                    {printf("entryA\n");}
                 |  textoption
                                    {printf("entryB\n");strcpy($$,$1);}
                 ;

begtableopts     :  LSQRB  position  RSQRB
                 {
                    //Table* table = new_table($2);
                 }
                 ;
                 
begtabularopts   :  LCURLYB  COLS  RCURLYB
                 ;

position         :  H  
                 |  T  
                 |  B
                 ;

endtableopts     :  END
                 |  CAPTION  LCURLYB  textoption  RCURLYB  captionrest
                 |  labelrest 
                 ;

captionrest      :  END
                 |  labelrest
                 ;

labelrest        :  LABEL  LCURLYB  WORD  RCURLYB  END
                 ;

sectionoptions   :  SECTION  LCURLYB  textoption  RCURLYB
                    {
                      generate_sec_header(get_sec_ctr(), $3);
                      incr_sec_ctr();
                    }
                 |  SUBSEC  LCURLYB  textoption  RCURLYB
                    {
                      generate_subsec_header(get_sec_ctr()-1, get_subsec_ctr(), $3);
                      incr_subsec_ctr();
                    }
                 ;

tableofcont      :  TABOCON
                    {
                      set_gen_toc();
                    }
                 ;

linespacing      :  RENEW  LCURLYB  BASELINES  RCURLYB
                            LCURLYB  WORD  RCURLYB
                    {line_spacing=atoi($6)-1;} 
                 ;

pagenumbers      :  PAGENUM  style2
                    {
                      set_page_style($2);
                    }
                 ;

style2           :  ARABIC2 {$$=Arabic;}
                 |  LROMAN2 {$$=LRoman;}
                 |  CROMAN2 {$$=CRoman;}
                 |  LALPH2  {$$=LAlph;}
                 |  CALPH2  {$$=CAlph;}
                 ;

pagenuminit      :  style1  LCURLYB  WORD  RCURLYB
                    {
                      set_page_no(atoi($3));
                      set_page_style($1);
                    }
                 ;

style1           :  ARABIC1 {$$=Arabic;}
                 |  LROMAN1 {$$=LRoman;}
                 |  CROMAN1 {$$=CRoman;}
                 |  LALPH1  {$$=LAlph;}
                 |  CALPH1  {$$=CAlph;}
                 ;

spacing          :  horvert  LCURLYB  WORD  RCURLYB { vertical_space($3); }
                 ;

horvert          :  VSPACE  
                 |  HSPACE
                 ;

fonts            :  RM

                 {
                  $$=it_flag; 
                  it_flag = 0; 
                  line[tmp_text_index+spec_chars++] = '\033'; 
                  line[tmp_text_index+spec_chars++] = '[';
                  line[tmp_text_index+spec_chars++] = '0';
                  line[tmp_text_index+spec_chars++] = 'm';
                 }
                 |  IT
                 { 
                  $$=it_flag;
                  it_flag = 1;
                  line[tmp_text_index+spec_chars++] = '\e'; 
                  line[tmp_text_index+spec_chars++] = '[';
                  line[tmp_text_index+spec_chars++] = '3';
                  line[tmp_text_index+spec_chars++] = 'm'; 
                 }
                 ;
specialchar      :  SPECCHAR  
                 |  LCURLYB  
                 |  RCURLYB
                 ;

nonewpara        :  NOINDENT
                 {noin_flag = 0;}
                 ;

reference        :  REF  LCURLYB  WORD  RCURLYB
                 ;
%%
#include "lex.yy.c"


int main()
{
    fpout = fopen("latexout","w");
    fptoc = fopen("latextoc","w");
    fplog = fopen("latexlog","w");
    init_lines_so_far();
    init_sec_ctr();
    init_output_page();
    yydebug = 1;
    yyparse();
    return 0;
}
