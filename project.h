/* ------------- MACROS ------------- */

#define LAlph  			    1
#define CAlph  			    2
#define LRoman 			    3
#define CRoman 			    4
#define Arabic 			    5

#define Vspace          1
#define Hspace          2

#define CENTER_CMD 		  1
#define VERBATIM_CMD 	  2
#define SINGLE_CMD 		  3
#define ITEMIZE_CMD 	  4
#define ENUMERATE_CMD   5
#define TABULAR_CMD 	  6
#define TABLE_CMD 		  7 

#define OUT_WIDTH       40
#define SPACE_LEFT      5
#define LINES_PER_PAGE  40
#define TOC_ON          1

#define H_POS 			    0
#define	B_POS 			    1
#define	T_POS 			    2
#define R_COL 			    0
#define C_COL 			    1
#define L_COL 			    2

#define INDEX text_index + spec_chars
#define ITEM_DEPTH itemize_stack->count
#define ITEM_SPACING ITEM_DEPTH*item_width

typedef struct latex_table {
  int* col_spec;
  char** entries;
  char* label;
  char* caption;
  char id_str[20]; // ASSUMPTION: only about 10 digits per table id (should be less limiting than the int used to store the table id actually)
  int page;
  int pos;
  int cols;
  int rows;
  int capacity;
  int id;
  int centered;
} Table;

typedef struct table_queue {
  Table** data;
  int capacity;
  int count;
  int front;
} Queue;

typedef struct type_checking_stack {
  int* data;
  int count;
  int capacity;
} Stack;

struct  doc_symtab {
      int     page_no_counter;
      int     page_style;
      int     line_spacing;
      int     current_font;
      int     generate_toc;
      int     section_counter;
      int     subsect_counter;
};

/* ------------- GLOBALS ------------- */

FILE *fpout;
FILE *fptoc;
FILE *fplog;
char  line[128];
int   lines_so_far;
struct  doc_symtab  DST;

Table* current_table = NULL; // used when processing a new table
Queue* b_queue = NULL; // a queue of tables with position = b
Queue* t_queue = NULL; // a queue of tables with position = t
Stack* block_stack = NULL; // a stack for begin/end blocks
Stack* itemize_stack = NULL; // a stack to keep track of itemize/enumeration blocks
Stack* enumeration_stack = NULL; // a stack to keep track of the enumerations for nested enumeration blocks

int ws_flag = 0;
int noin_flag = 0;
int it_flag = 0;
int line_spacing = 0;
int single_flag = 0;
int text_index = 0;
int tmp_text_index = 0;
int spec_chars = 0;
int enumerate = 0;
int itemize = 0;
int item_width = 4; // items have 4 spaces preceeding them
int table_flag = 0;
int center_flag = 0;
int verb_flag = 0;
int current_table_id = 1;


/* ------------- FUNCTIONS ------------- */

// generate.c
void init_output_page();
void generate_sec_header(int i, char* s);
void generate_subsec_header(int i,int j, char *s);
void print_bottom();
void try_bottom();
void print_page_number();
void vertical_space(char* s);
void generate_item(char* s);
void print_line();
void generate_formatted_text(char* s);
void center_verb_text(char* s);
int find_length_longest_line(char *s);
// util.c
void print_blank_line();
int is_ws(char* s);
void  init_lines_so_far();
void  incr_lines_so_far();
int   check_done_page();
void convertToRoman (unsigned int val, char *res);
void  init_sec_ctr();
void  incr_sec_ctr();
void  incr_subsec_ctr();
int  get_sec_ctr();
int  get_subsec_ctr();
int   get_gen_toc();
void  set_gen_toc();
void  set_page_no(int p);
int   get_page_no();
int   inc_page_no();
int get_page_style();
void  set_page_style(int s);
int substring(char* haystack, char* needle);
int less_specchars(int length);
void right_justify();
Table* new_table(char* position);
void free_table(Table* table);
void set_cols(Table* table, char* cols);
void set_label(Table* table, char* label);
void set_caption(Table* table, char* caption);
void check_entry(Table* table, char* entry);
void add_entry(Table* table, char* entry);
char* table_justify(char* s, int len, int format, int should_space);
void print_table(Table* table);
int table_lines(Table* table);
Stack* new_stack();
void push(Stack* stack, int n);
int pop(Stack* stack);
int top(Stack* stack);
Queue* new_queue();
void enqueue(Queue* queue, Table* table);
Table* dequeue(Queue* queue);
Table* peek(Queue* queue);
