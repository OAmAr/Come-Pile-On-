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
latexstatement   :  startdoc  mainbody  enddoc 
                 { 
                    fprintf(fplog,"Complete\n");
                 }
                 ;

startdoc         :  LBEGIN  DOCUMENT 
                 {
                    fprintf(fplog, "started doc\n"); 
                    block_stack = new_stack(); // init data structs
                    itemize_stack = new_stack();
                    enumeration_stack = new_stack();
                    b_queue = new_queue();
                    t_queue = new_queue();
                 } 
                 ;

enddoc           :  END  DOCUMENT  
                 {
                    fprintf(fplog, "finished doc\n"); 
                    end_doc_cleanup();
                 } 
                 ;

mainbody         :  mainbody  mainoption
                 |  mainoption
                 ;

mainoption       :  textoption
                 {
                    generate_formatted_text($1);
                    tmp_text_index = text_index; // used in case a \it or \rm command interrupts a line
                 }
                 |  commentoption
                 |  latexoptions
                 ;

textoption       :  textoption  wsorword
                 {
                    if (!ws_flag) strcat($$, " "); // add a single space only if not verbatim
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
                        line[text_index+spec_chars++] = '\e'; // this inserts characters which generate italics
                        line[text_index+spec_chars++] = '[';
                        line[text_index+spec_chars++] = '3';
                        line[text_index+spec_chars++] = 'm';
                        line[text_index+spec_chars] = ' ';
                        text_index++;
                    } else {
                        line[text_index+spec_chars++] = '\033'; // this inserts characters which clear italics
                        line[text_index+spec_chars++] = '[';
                        line[text_index+spec_chars++] = '0';
                        line[text_index+spec_chars++] = 'm';
                        line[text_index+spec_chars] = ' ';
                        text_index++;
                    }
                 }
                    
                 ;

curlyboptions    :  fonts  textoption
                 {    
                    $$ = $1;
                    text_index = tmp_text_index; // store a tmp text index to deal with interrupted textoptions
                    tmp_text_index = 0;
                    if(text_index > 0) { // Add a space to separate italics/roman text unless at the front of a line
                        line[INDEX] = ' '; 
                        text_index++;
                    }
                    generate_formatted_text($2);
                 }
                 ;

backsoptions     :  beginendopts
                 |  sectionoptions
                 |  tableofcont {print_line();}
                 |  linespacing {print_line();}
                 |  pagenumbers
                 |  pagenuminit
                 |  spacing {print_line();}
                 |  fonts
                 {
                    text_index = tmp_text_index; // same reasoning for this as the block in curlyboptions
                    tmp_text_index = 0;
                    if(text_index > 0) {
                        line[INDEX] = ' '; 
                        text_index++;
                    }
                 }
                 |  specialchar
                 |  nonewpara {print_line();}
                 |  reference {print_line();}
                 ;
     
beginendopts     :  LBEGIN  begcmds  beginblock  endbegin
                 {
                    if(pop(block_stack) != $2) { // the top of the stack (the end) should match the value of the begin command, otherwise throw error and exit
                        fprintf(fpout, "\n\n\nMISMATCHED BEGIN/END BLOCKS\n\n\n");
                        exit(1);
                    }
                 } 
                 ;

begcmds          :  CENTER  
                 {
                    print_line(); // want to print line before all these begin commands to clear the line buffer
                    center_flag=1; // raise the appropriate flag for each command
                    $$ = CENTER_CMD; // set return value of each command
                    fprintf(fplog, "We've got a center\n");
                    if(table_flag) current_table->centered = center_flag; // set the center flag of the table if we are working on a table
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
                    push(itemize_stack, ITEMIZE_CMD); // push an itemize onto the itemize stack to increase the depth
                    $$ = ITEMIZE_CMD;
                 }
                 |  ENUMERATE 
                 {
                    print_line();
                    push(itemize_stack, ENUMERATE_CMD); // push an enumerate onto the itemize stack
                    push(enumeration_stack, 1); // push a 1 onto the enumeration stack to start counting from 1 at this depth
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
                    push(block_stack, $2); // push the value of the command onto the block stack
                 }
                 |  endtableopts  TABLE 
                 {
                    push(block_stack, TABLE_CMD); // push the value onto the command stack
                    if (current_table->pos==B_POS) enqueue(b_queue, current_table); // if it's a b table add it to the b queue
                    else if(current_table->pos==T_POS) enqueue(t_queue, current_table); // do the same for t tables
                    else print_table(current_table); // print in place if it's an h table
                    table_flag = 0; // lower the table flag as the table has finished setting up
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
                    for(int i = 0; i< line_spacing; i++) print_blank_line(); // print blank lines at the end of the block to realign with document spacing
                    single_flag = 0;
                    $$ = SINGLE_CMD;
                 } 
                 |  ITEMIZE 
                 {
                    pop(itemize_stack); // pop the itemize stack to reduce the depth
                    $$ = ITEMIZE_CMD;
                 } 
                 |  ENUMERATE
                 {
                    pop(itemize_stack); // pop the itemize stack to reduce the depth
                    pop(enumeration_stack); // pop the enumeration stack to discard the count for that depth
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
                    if (ws_flag)     { 
                        char* verb = $1+1; // the first line of verbatim is always a \n so skip it
                        if (center_flag) // centered verbatim text is special
                            center_verb_text(verb);
                        else // otherwise just dump out the text as is
                            fprintf(fpout, "%s", verb); 
                    }
                    else if (single_flag) { 
                        generate_formatted_text($1); 
                        print_line();
                    }

                 }
                 |  entrylist  /* FOR center and tabular */
                 {
                    printf("center or tabular\n");
                 }
                 |  listblock  /* FOR item and enumerate */
                 {
                    printf("item or enumerate\n");
                 }
                 ;

listblock        :  listblock  anitem
                 {
                    printf("listblockA\n");
                 }
                 |  anitem
                 {
                    printf("listblockB\n");
                 }
                 ;

anitem           :  ITEM  textoption
                 {
                    generate_item($2);
                 }
                 |  beginendopts
                 ;

entrylist        :  entrylist  anentry
                 {
                    printf("entrylistA\n");
                 }
                 |  anentry
                 {
                    printf("entrylistB\n");
                 }
                 ;

anentry          :  entry  DBLBS
                 {
                    printf("anentryA\n");
                    if(table_flag) add_entry(current_table, $1); // take priority over tables
                    else if (center_flag) {
                        generate_formatted_text($1);
                        print_line();
                    }
                 }

                 |  beginendopts
                 {
                    printf("anentryB\n");
                 }
                 ;

entry            :  entry  SPECCHAR  textoption
                 {
                    printf("entryA\n");
                    strcpy($$, $1); // this is a table entry so just concatenate everything together to separate by &
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
                    current_table = new_table($2); // start of a new table
                    table_flag = 1; // raise the table flag
                 }
                 ;
                 
begtabularopts   :  LCURLYB  COLS  RCURLYB
                 {
                    set_cols(current_table, $2); // set the column spec of the table according to COLS
                 }
                 ;

position         :  H 
                 {
                    strcpy($$, $1);
                 }  
                 |  T 
                 {
                    strcpy($$, $1);
                 }
                 |  B 
                 {
                    strcpy($$, $1);
                 }
                 ;

endtableopts     :  END
                 |  CAPTION  LCURLYB  textoption  RCURLYB  captionrest
                 {
                    set_caption(current_table, $3); // set the caption of the table
                 }
                 |  labelrest 
                 ;

captionrest      :  END
                 |  labelrest
                 ;

labelrest        :  LABEL  LCURLYB  WORD  RCURLYB  END
                 {
                    set_label(current_table, $3); // set the label of the table for \ref
                 }
                 ;

sectionoptions   :  SECTION  LCURLYB  textoption  RCURLYB
                 {
                    print_line(); // need to clear the line buffer
                    generate_sec_header(get_sec_ctr(), $3);
                    incr_sec_ctr();
                 }
                 |  SUBSEC  LCURLYB  textoption  RCURLYB
                 {
                    print_line();
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
                 {
                    line_spacing=atoi($6)-1;
                 } 
                 ;

pagenumbers      :  PAGENUM  style2
                 {
                    set_page_style($2);
                 }
                 ;

style2           :  ARABIC2 
                 {
                    $$=Arabic;
                 }
                 |  LROMAN2 
                 {
                    $$=LRoman;
                 }
                 |  CROMAN2 
                 {
                    $$=CRoman;
                 }
                 |  LALPH2  
                 {
                    $$=LAlph;
                 }
                 |  CALPH2  
                 {
                    $$=CAlph;
                 }
                 ;

pagenuminit      :  style1  LCURLYB  WORD  RCURLYB
                 {
                    set_page_no(atoi($3));
                    set_page_style($1);
                 }
                 ;

style1           :  ARABIC1 
                 {
                    $$=Arabic;
                 }
                 |  LROMAN1 
                 { 
                    $$=LRoman;
                 }
                 |  CROMAN1 
                 {  
                    $$=CRoman;
                 }
                 |  LALPH1  
                 {
                    $$=LAlph;
                 }
                 |  CALPH1  
                 {
                    $$=CAlph;
                 }
                 ;

spacing          :  horvert  LCURLYB  WORD  RCURLYB 
                 { 
                    if($1 == Vspace) // hspace not required so we didn't add anything for it
                        vertical_space($3);
                 }
                 ;

horvert          :  VSPACE
                 {
                    print_line(); // need to clear the line buffer
                    $$ = Vspace;
                 }  
                 |  HSPACE
                 {
                    print_line();
                    $$ = Hspace;
                 }
                 ;

fonts            :  RM
                 {
                    $$=it_flag; 
                    it_flag = 0; 
                    line[tmp_text_index+spec_chars++] = '\033'; // same as the curlyb options case -- characters set\remove italics
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
                 {
                    noin_flag = 1;
                 }
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
