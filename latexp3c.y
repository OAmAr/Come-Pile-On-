/* THIS IS latexp3code.y */
%{ /* A YACC FOR PART 3 OF THE PROJECT WHERE VERBATIM AND NESTING WORKS */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define  BUF_SIZE       512
#define YYDEBUG 1

#include "latexp3c.tab.h"
#include "project.h"
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

%type <trans> textoption  wsorword WS WORD entry COLS H B T position
%type <val> style2 ARABIC2 LROMAN2 CROMAN2 LALPH2 CALPH2 curlyboptions fonts style1 begcmds endcmds horvert VSPACE HSPACE

%%
latexstatement   :  startdoc  mainbody  enddoc { fprintf(fplog,"Complete\n");}
                 ;

startdoc         :  LBEGIN  DOCUMENT 
                 {
                    fprintf(fplog, "started doc\n"); 
                    block_stack = new_stack();
                    itemize_stack = new_stack();
                    enumeration_stack = new_stack();
                    b_queue = new_queue();
                    t_queue = new_queue();
                 } 
                 ;

enddoc           :  END  DOCUMENT  {fprintf(fplog, "finished doc\n"); end_doc_cleanup();} 
                 ;

mainbody         :  mainbody  mainoption
                 |  mainoption
                 ;

mainoption       :  textoption
                    {
                      generate_formatted_text($1);
                      tmp_text_index = text_index;
                    }
                 |  commentoption
                 |  latexoptions
                 ;

textoption       :  textoption  wsorword
                    {
                      if (!ws_flag) strcat($$, " ");
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
                    if(it_flag) {
                        line[text_index+spec_chars++] = '\e'; 
                        line[text_index+spec_chars++] = '[';
                        line[text_index+spec_chars++] = '3';
                        line[text_index+spec_chars++] = 'm';
                        //line[text_index+spec_chars] = ' ';
                        //text_index++;
                    } else {
                        line[text_index+spec_chars++] = '\033'; 
                        line[text_index+spec_chars++] = '[';
                        line[text_index+spec_chars++] = '0';
                        line[text_index+spec_chars++] = 'm';
                        line[text_index+spec_chars] = ' ';
                        //fprintf(fpout, "[%c]", line[text_index+spec_chars]);
                        //text_index++;
                    }
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

backsoptions     :  beginendopts
                 |  sectionoptions {/*print_line();*/}
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
                 |  nonewpara {print_line();}
                 |  reference {print_line();}
                 ;
     
beginendopts     :  LBEGIN  begcmds  beginblock  endbegin
                 {
                    if(pop(block_stack) != $2) {
                        fprintf(fpout, "\n\n\nMISMATCHED BEGIN/END BLOCKS\n\n\n");
                        exit(1);
                    }
                 } 
                 ;

begcmds          :  CENTER  
                 {
                    print_line();
                    center_flag=1;
                    $$ = CENTER_CMD;
                    fprintf(fplog, "We've got a centea\n");
                    if(table_flag) current_table->centered = center_flag;
                 }
                 |  VERBATIM  
                 {
                    print_line();
                    ws_flag=1;
                    $$ = VERBATIM_CMD;
                 }
                 |  SINGLE 
                 {
                    print_line();
                    single_flag = 1;
                    $$ = SINGLE_CMD;
                 } 
                 |  ITEMIZE 
                 {
                    print_line();
                    push(itemize_stack, ITEMIZE_CMD);
                    $$ = ITEMIZE_CMD;
                 }
                 |  ENUMERATE 
                 {
                    print_line();
                    push(itemize_stack, ENUMERATE_CMD);
                    push(enumeration_stack, 1);
                    $$ = ENUMERATE_CMD;
                 }
                 |  TABLE  begtableopts
                 {
                    print_line();
                    fprintf(fplog, "We've got a table\n");
                    $$ = TABLE_CMD;
                 }
                 |  TABULAR  begtabularopts
                 {
                    print_line();
                    fprintf(fplog, "We've got a tabular\n");
                    $$ =TABULAR_CMD;
                 }
                 ;

endbegin         :  END  endcmds
                 {
                    push(block_stack, $2);
                 }
                 |  endtableopts  TABLE 
                 {
                    push(block_stack, TABLE_CMD);
                    if (current_table->pos==B_POS) enqueue(b_queue, current_table);
                    else if(current_table->pos==T_POS) enqueue(t_queue, current_table);
                    else print_table(current_table);
                    table_flag = 0;
                 } 
                 ;

endcmds          :  CENTER  
                 {
                    center_flag=0;
                    $$ = CENTER_CMD;
                 }
                 |  VERBATIM  
                 { 
                    ws_flag=0;
                    $$ = VERBATIM_CMD;
                 }
                 |  SINGLE 
                 {
                    for(int i = 0; i< line_spacing; i++) print_blank_line();
                    single_flag = 0;
                    $$ = SINGLE_CMD;
                 } 
                 |  ITEMIZE 
                 {
                    pop(itemize_stack);
                    $$ = ITEMIZE_CMD;
                 } 
                 |  ENUMERATE
                 {
                    pop(itemize_stack);
                    pop(enumeration_stack);
                    $$ = ENUMERATE_CMD;
                 } 
                 |  TABULAR
                 {
                    $$ = TABULAR_CMD;
                 }
                 ;
beginblock       :  beginendopts
                 |  textoption /* FOR single or verbatim */
                 {  
                    printf("single or verb\n");
                    
                    if (single_flag) { generate_formatted_text($1); print_line();}

                    if (ws_flag)     { 
                        char* verb = $1;
                        //if(*verb == '\n') fprintf(fpout, "AWODIAOWIDJ");
                        //print_verb_text($1+1);
                        fprintf(fpout, "%s", verb+1); 
                    }
                 }
                 |  entrylist  /* FOR center and tabular */
                                    {printf("center or tabular\n");}
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
                 {
                    printf("anentryA\n");
                    if(table_flag) add_entry(current_table, $1);
                    else if (center_flag) {
                        generate_formatted_text($1);
                        print_line();
                    }
                 }

                 |  beginendopts
                                    {printf("anentryB\n");}
                 ;

entry            :  entry  SPECCHAR  textoption
                 {
                    printf("entryA\n");
                    strcpy($$, $1);
                    strcat($$, "&");
                    strcat($$, $3);
                 }
                 |  textoption
                 {
                    printf("entryB\n");
                    strcpy($$,$1);
                 }
                 ;

begtableopts     :  LSQRB  position  RSQRB
                 {
                    current_table = new_table($2);
                    table_flag = 1;
                 }
                 ;
                 
begtabularopts   :  LCURLYB  COLS  RCURLYB
                 {
                    set_cols(current_table, $2);
                 }
                 ;

position         :  H {strcpy($$, $1);}  
                 |  T {strcpy($$, $1);}
                 |  B {strcpy($$, $1);}
                 ;

endtableopts     :  END
                 |  CAPTION  LCURLYB  textoption  RCURLYB  captionrest
                 {
                    set_caption(current_table, $3);
                 }
                 |  labelrest 
                 ;

captionrest      :  END
                 |  labelrest
                 ;

labelrest        :  LABEL  LCURLYB  WORD  RCURLYB  END
                 {
                    set_label(current_table, $3);
                 }
                 ;

sectionoptions   :  SECTION  LCURLYB  textoption  RCURLYB
                    {
                        //print_line();
                        generate_sec_header(get_sec_ctr(), $3);
                        incr_sec_ctr();
                    }
                 |  SUBSEC  LCURLYB  textoption  RCURLYB
                    {
                        //print_line();
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

spacing          :  horvert  LCURLYB  WORD  RCURLYB 
                 { 
                    if($1 == Vspace)
                        vertical_space($3);
                 }
                 ;

horvert          :  VSPACE
                 {
                    $$ = Vspace;
                 }  
                 |  HSPACE
                 {
                    $$ = Hspace;
                 }
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
                 {noin_flag = 1;}
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
