/* THIS IS THE util.c FILE */

void print_blank_line() {
  fprintf(fpout, "\n");
  incr_lines_so_far();
}

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

int substring(char* haystack, char* needle) {
  if(strcmp(haystack, needle) == 0)
    return 0;
  int lenh = strlen(haystack);
  int lenn = strlen(needle);
  int i, j;
  for(i = 0; i < lenh - lenn; i++) {
    if(*(haystack+i) == *needle) { // matched 1st character
      int bad = 0;
      for(j = 1; j < lenn; j++) {
        if(*(haystack+i+j) != *(needle+j))
          bad = 1;
      }
      if(!bad)
        return i;
    }
  }
  return -1;
}

int less_specchars(int length) { // returns length less characters used for italics and whatnot
  int i = 0;
  while((i = substring(line+i, "\033[0m")) != -1) {
    i++; // increment i to search for next substring
    length -= 4;
  }
  i = 0;
  while((i = substring(line+i, "\e[3m")) != -1) {
    i++;
    length -= 4;
  }
  return length;
}

void right_justify() {
  int length = strlen(line);
  int llen = less_specchars(length);
  int i, j, just, n = 0, found_character = 0;
  if (length < 1)
      return;
  for(i = 0; i < length; i++) {
    if(line[i] != ' ' && !found_character)
      found_character = 1;
    if(line[i] == ' ' && found_character){
      n = i;
      break;
    }
  }
  while(llen < OUT_WIDTH-ITEM_SPACING) { // Right justify by going through line and adding spaces until it's good
    for(just = n; just < length && llen < OUT_WIDTH-ITEM_SPACING; just++) {
      if(line[just] == ' ')  {
        for(j = length; j > just; j--) {
          line[j] = line[j-1];
        }
        llen++;
        length++;
        just++; // skip the next space
      }
    }
  }
}

Table* new_table(char* position) {
  Table* table = (Table*)malloc(sizeof(Table));
  table->entries = (char**)malloc(sizeof(char*)*8);
  if(*position == 'b') table->pos = B_POS;
  else if(*position == 't') table->pos = T_POS;
  else table->pos = H_POS;
  table->id = current_table_id++;
  sprintf(table->id_str, "Table %d. ", table->id);
  table->page = get_page_no();
  table->col_spec = NULL;
  table->label = NULL;
  table->caption = NULL;
  table->capacity = 8;
  table->cols = 0;
  table->rows = 0;
  return table;
}

void free_table(Table* table) {
  if(table->label != NULL) free(table->label);
  if(table->caption != NULL) free(table->caption);
  int i;
  for(i = 0; i < table->rows; i++)
    free(table->entries[i]);
  free(table->entries);
  free(table);
}

void set_cols(Table* table, char* cols) {
  table->cols = strlen(cols);
  table-> col_spec = (int*)malloc(sizeof(int)*table->cols);
  int i;
  for(i = 0; i < table->cols; i++) {
    if(cols[i] == 'r') table->col_spec[i] = R_COL;
    else if(cols[i] == 'c') table->col_spec[i] = C_COL;
    else table->col_spec[i] = L_COL;
  }
}

void set_label(Table* table, char* label) {
  table->label = (char*)malloc(strlen(label));
  strcpy(table->label, label);
}

void set_caption(Table* table, char* caption) {
  table->caption = (char*)malloc(strlen(caption));
  strcpy(table->caption, caption);
}

void check_entry(Table* table, char* entry) {
  int i = 0;
  int count = 0;
  while(1) { // get # of &'s
    int next = substring(entry+i, "&");
    if(next < 0) break;
    i += next + 1;
    count++;
  }
  if(count != table->cols-1) { // check that count matches the number of cols-1
    fprintf(fpout, "\n\n\nError compiling table %d:\nColumn count of entry '%s' does not match table spec (cols = %d)\n\n\n",
      table->id, entry, table->cols);
    exit(1);
  }
}

void add_entry(Table* table, char* entry) {
  check_entry(table, entry);
  if(table->rows == table->capacity) {
    table->capacity *= 2;
    table->entries = (char**)realloc(table->entries, sizeof(char*)*table->capacity);
  }
  table->entries[table->rows] = (char*)malloc(strlen(entry));
  strcpy(table->entries[table->rows], entry);
  table->rows++;
}

char* table_justify(char* s, int len, int format, int should_space) {
  char* buf = (char*)malloc(len+1+item_width);
  int slen = strlen(s);
  int index = 0;
  int i;
  if(slen < len) {
    switch(format) {
      case R_COL:
        for(i = 0; i < len-slen; i++)
          buf[i] = ' ';
        strncpy(buf+i, s, slen);
        break;
      case C_COL:
        for(i = 0; i < (len-slen)/2; i++)
          buf[index++] = ' ';
        strncpy(buf+index, s, slen);
        index += slen;
        for(i = (len-slen)/2; i < len-slen; i++) {
          buf[index] = ' ';
          index++;
        }
        break;
      case L_COL:
        strncpy(buf, s, slen);
        for(i = slen; i < len-slen; i++)
          line[i] = ' ';
        break;
    }
  } else 
    strncpy(buf, s, slen);

  if(should_space) {
    for(i = 0; i < item_width; i++)
      buf[len+i] = ' ';
    buf[len+item_width] = 0;
  } else 
    buf[len] = 0;
  return buf;
}

void print_table(Table* table) {
  int tmp_flag = table_flag;
  table_flag = 1;
  int i, j, k, len;
  int cols = table->cols;
  int rows = table->rows;
  char e[rows][cols][32]; // ASSUMPTION: entries in table limited to 32 characters
  int max[cols];
  memset(max, 0, sizeof(int)*cols);

  for(i = 0; i < rows; i++) { // iterate for each line
    int offset = 0;
    for(j = 0; j < cols; j++) {
      int next = substring(table->entries[i]+offset, "&");
      len = (next == -1) ? (strlen(table->entries[i])-offset) : (next);
      strncpy(e[i][j], table->entries[i]+offset, len);
      e[i][j][len] = 0;
      if(len > max[j]) max[j] = len;
      offset += next+1;
    }
  }

  for(i = 0; i < rows; i++) {
    for(j = 0; j < cols; j++) {
      char* buffer = table_justify(e[i][j], max[j], table->col_spec[j], j != cols-1);
      generate_formatted_text(buffer);
      free(buffer);
    }
    print_line();
  }

  fprintf(fpout, "\n"); // may need to change to reflect line spacing, could fill a blank space in line and print line
  char buf[64];
  memset(buf, 0, 64);
  if(table->caption != NULL)
    sprintf(buf, "%s%s", table->id_str, table->caption);
  generate_formatted_text(buf);
  print_line();
  table_flag = tmp_flag;
  free_table(table);
}

int table_lines(Table* table) {
  return table->rows + ((table->caption != NULL) ? (2+(strlen(table->caption)+strlen(table->id_str))/OUT_WIDTH) : (0));
}

Stack* new_stack() {
  Stack* stack = (Stack*)malloc(sizeof(Stack));
  stack->data = (int*)malloc(sizeof(int)*8);
  stack->capacity = 8;
  stack->count = 0;
  return stack;
}

void push(Stack* stack, int n) {
  if(stack->count == stack->capacity) {
    stack->capacity *= 2;
    stack->data = (int*)realloc(stack->data, stack->capacity);
  }
  stack->data[stack->count++] = n;
}

int pop(Stack* stack) {
  if(stack->count > 0) {
    int n = stack->data[stack->count-1];
    stack->count--;
    return n;
  }
  return -1;
}

int top(Stack* stack) {
  return stack->data[stack->count-1];
}

Queue* new_queue() {
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->data = (Table**)malloc(sizeof(Table*)*8);
  queue->capacity = 8;
  queue->count = 0;
  queue->front = 0;
  return queue;
}

void enqueue(Queue* queue, Table* table) {
  if(queue->count == queue->capacity) {
    queue->capacity *= 2;
    queue->data = (Table**)realloc(queue->data, queue->capacity);
  }
  queue->data[(queue->count+queue->front)%queue->capacity] = table;
  queue->count++;
}

Table* dequeue(Queue* queue) {
  if(queue->count == 0) return NULL;
  Table* table = queue->data[queue->front];
  queue->front = (queue->front + 1)%queue->capacity;
  return table;
}

Table* peek(Queue* queue) {
  return queue->data[queue->front];
}