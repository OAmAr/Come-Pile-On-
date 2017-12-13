/* THIS IS THE util.c FILE */

/*
 * Prints a blank line and increments the lines so far
 */
void print_blank_line() {              
  fprintf(fpout, "\n");
  incr_lines_so_far();
  if(check_done_page()){                                //check if we're done with the page 
      print_page_number();                               //Print the page number if so
  }
}
/*
 * Checks to see if a string is completely whitespace
 */
int is_ws(char* s) { 
  int i;
  int slen = strlen(s);
  for(i = 0; i < slen; i++)             // iterate through string and check for ws
    if(s[i] != ' ' && s[i] != '\n' && s[i] != '\t')
      return 0;
  return 1;
}

void  init_lines_so_far(){
  lines_so_far = 0;
}

void  incr_lines_so_far(){
  lines_so_far++;
}
/*
 * Checks to see if the number of lines we've read so far is a full page
 */
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

void set_italics() { // Sets characters which print italics
  line[INDEX] = '\e'; // these characters print italics in the terminal
  line[INDEX] = '[';
  line[INDEX] = '3';
  line[INDEX] = 'm'; 
  if(text_index > 0) { // if there is meaningful text, print a space to separate words
    line[INDEX] = ' '; 
    text_index++;
  }
  spec_chars += 4;
}

void clear_italics() { // Sets characters which clear italics
  line[INDEX] = '\033'; // these characters clear italics in the terminal
  line[INDEX] = '[';
  line[INDEX] = '0';
  line[INDEX] = 'm';
  if(text_index > 0) { // if there is meaningful text, print a space to separate words
    line[INDEX] = ' '; 
    text_index++;
  }
  spec_chars += 4;
}

/*
 * Returns the offset of the first occurence of the string in needle found in the string haystack
 */
int substring(char* haystack, char* needle) { // returns the offset of the first substring of needle in haystack, taken from my 3100 lab :)
  if(strcmp(haystack, needle) == 0)           // code doesn't work if the strings have equal length, so return 0 as there is no offset for equal strings
    return 0;
  int lenh = strlen(haystack);                // get length of strings
  int lenn = strlen(needle);
  int i, j;
  for(i = 0; i < lenh - lenn; i++) {          // iterate until the difference of lengths as past that there can be no substrings
    if(*(haystack+i) == *needle) {            // matched 1st character
      int bad = 0;                            // init flag
      for(j = 1; j < lenn; j++) {             // iterate through needle to check if it is a substring
        if(*(haystack+i+j) != *(needle+j))    // if any chars don't match set the flag to indicate not a substring
          bad = 1;
      }
      if(!bad)                                // it was a substring, i is the offset of the beginning
        return i;
    }
  }
  return -1;                                  // not a substring
}

int next_char(char* s, int offset) { // returns the next non-space character after offset bytes into s, or strlen(s)+1 if none left in s 
  int len = strlen(s);
  int i;
  for(int i = offset; i < len; i++)
    if(s[i] != ' ') return i;
  return i;
}

void right_justify() { // right justifies a single line
  int length = strlen(line); // get total length of whole string, italics characters included
  if(line[length-1] == ' ') // ran into issue with having a space as the last character
    line[(length--)-1] = 0;
  int llen = length - spec_chars; // get length of meaningful text
  int i, j, just, n = 0, found_character = 0;
  if (length < 1)                          // nothing to justify
      return;
  for(i = 0; i < length; i++) {            // iterate through the string to find the first space separating two words
    if(line[i] != ' ' && !found_character) // found a non-space character
      found_character = 1;
    if(line[i] == ' ' && found_character){ // this is the first space after a non-space character
      n = i;
      break;
    }
  }
  while(llen < OUT_WIDTH-ITEM_SPACING) {  // Right justify by going through line and adding spaces until it's filled out enough space
    for(just = n; just < length && llen < OUT_WIDTH-ITEM_SPACING; just++) { // start at the first space, end at the end of the string or at the proper outwidth (less space for itemize items)
      if(line[just] == ' ')  {            // found a space
        for(j = length; j > just; j--) {  // shift everything one to the right in order to "add" a space
          line[j] = line[j-1];
        }
        llen++;
        length++;
        just = next_char(line, just); // find the next non-space character
      }
    }
  }
}

Table* new_table(char* position) {        // creates a new table object
  Table* table = (Table*)malloc(sizeof(Table)); // allocate the table
  table->entries = (char**)malloc(sizeof(char*)*8); // allocate the entries (an entry is the text for a row)
  if(*position == 'b') table->pos = B_POS;// set the position according the the position given, since it is only 1 char we can just use that
  else if(*position == 't') table->pos = T_POS;
  else table->pos = H_POS;
  table->id = current_table_id++;         // store the id of the table and increment the id var for the next table
  sprintf(table->id_str, "Table %d. ", table->id); // store the string representation of the id
  table->page = get_page_no();            // get the page number of the table's definition, though this is not necessary as we don't use it
  table->col_spec = NULL;                 // col spec gives details on the columns of the table, init to null
  table->label = NULL;                    // init label and caption to null
  table->caption = NULL;
  table->capacity = 8;                    // initial capacity is 8
  table->cols = 0;                        // no columns yet
  table->rows = 0;                        // no rows yet
  table->centered = center_flag;          // store whether the table is contained within a center block for printing later
  return table;
}

void free_table(Table* table) {           // free's a table, fairly straightforward
  if(table->label != NULL) free(table->label);
  if(table->caption != NULL) free(table->caption);
  int i;
  for(i = 0; i < table->rows; i++)        // free each entry in the table
    free(table->entries[i]);
  free(table->entries);
  free(table);
}

void set_cols(Table* table, char* cols) { // sets the column spec according to the content of the begin tabular statement (e.x. pass something like "rcl")
  table->cols = strlen(cols);             // this is the # of columns
  table->col_spec = (int*)malloc(sizeof(int)*table->cols); // allocate the col spec
  int i;
  for(i = 0; i < table->cols; i++) {      // for each column, set the col spec according to the character
    if(cols[i] == 'r') table->col_spec[i] = R_COL;
    else if(cols[i] == 'c') table->col_spec[i] = C_COL;
    else table->col_spec[i] = L_COL;
  }
}

void set_label(Table* table, char* label) {    // sets the label of the table (for \ref)
  table->label = (char*)malloc(strlen(label)); // allocate space and copy the parameter into the table
  strcpy(table->label, label);
}

void set_caption(Table* table, char* caption) {    // sets the caption of the table
  table->caption = (char*)malloc(strlen(caption)); // allocate space and copy the parameter into the table
  strcpy(table->caption, caption);
}

void check_entry(Table* table, char* entry) {  // check if a table entry is valid
  int i = 0;
  int count = 0;                          // count the 3 of &'s'
  while(1) {                              // get # of &'s
    int next = substring(entry+i, "&");   // find the next &
    if(next < 0) break;                   // no more & in the entry
    i += next + 1;                        // increment i to get to the next &
    count++;                              // increment the count as we found an &
  }
  if(count != table->cols-1) {            // check that count matches the number of cols-1
    fprintf(fpout, "\n\n\nError compiling table %d:\nColumn count of entry '%s' does not match table spec (cols = %d)\n\n\n",
      table->id, entry, table->cols);
    exit(1);                              // stop compiling as the entry is invalid
  }
}

void add_entry(Table* table, char* entry) { // adds an entry to the table
  check_entry(table, entry);              // check that the entry is valid
  if(table->rows == table->capacity) {    // if there is no more room in the table reallocate it
    table->capacity *= 2;
    table->entries = (char**)realloc(table->entries, sizeof(char*)*table->capacity);
  }
  table->entries[table->rows] = (char*)malloc(strlen(entry)); // allocate space for the entry and copy it in
  strcpy(table->entries[table->rows], entry);
  table->rows++;                         // increment the row count as this entry represents a new row
}

char* table_justify(char* s, int len, int format, int should_space) { // justifies a cell in a table, len is the max length of the cell (justification spaces included)
  char* buf = (char*)malloc(len+1+ITEM_WIDTH); // allocate space for the result
  int slen = strlen(s);                        // get length of string to justify
  int index = 0;
  int i;
  if(slen < len) {                             // only need to justify if there are spaces to add in
    switch(format) {                           // each format is handled differently
      case R_COL:                              // right justify
        for(i = 0; i < len-slen; i++)          // add in spaces at the beginning
          buf[i] = ' ';
        strncpy(buf+i, s, slen);               // copy the entry at the end
        break;
      case C_COL:                              // center
        for(i = 0; i < (len-slen)/2; i++)      // add in half the spaces in the beginning
          buf[index++] = ' ';
        strncpy(buf+index, s, slen);           // copy the entry
        index += slen;                         // increment the index so we know where to start adding the next stuff
        for(i = (len-slen)/2; i < len-slen; i++) { // add the rest of the spaces after the entry
          buf[index] = ' ';
          index++;
        }
        break;
      case L_COL:                             // left justify
        strncpy(buf, s, slen);                // copy the entry into the beginning
        for(i = slen; i < len-slen; i++)  
          line[i] = ' ';
        break;
    }
  } else                                      // the length of the string matches the desired length of the entry so no justification is needed
    strncpy(buf, s, slen);                    // just copy the entry into the buffer

  if(should_space) { // this is necessary as we don't want to add spaces after the last column
    for(i = 0; i < ITEM_WIDTH; i++) // otherwise, just add in spaces to separate the columns
      buf[len+i] = ' ';
    buf[len+ITEM_WIDTH] = 0;
  } else 
    buf[len] = 0;
  return buf;
}
/*
 * Prints a table, {Which is easier said than done}
 */
void print_table(Table* table) {       
  int tmp_flag = table_flag;                 // store the table_flag global into a tmp var
  int tmp_center = center_flag;              // store the center_flag into a tmp var
  center_flag = table->centered;             // set the center flag according to the value in the table
  table_flag = 1;                            // set the table flag to 1 (important as it might not be a 1 if not was not printed in place)
  int i, j, k, len;                          // control variables
  int cols = table->cols;
  int rows = table->rows;
  char e[rows][cols][32];                    // ASSUMPTION: entries in table limited to 32 characters, don't really want to deal with it otherwise :/
  int max[cols];                             // an array that contains the max length for each column, used for justification
  memset(max, -1, sizeof(int)*cols);

  for(i = 0; i < rows; i++) {                // iterate through each row
    int offset = 0;
    for(j = 0; j < cols; j++) {              // extract each column
      int next = substring(table->entries[i]+offset, "&"); // find the next & (col separator)
      len = (next == -1) ? (strlen(table->entries[i])-offset) : (next); // get the length of the string based on the substring offset, if next is -1 then get the length based off the length of the string cause it's the last column
      strncpy(e[i][j], table->entries[i]+offset, len); // copy the extracted entry into the entries matrix
      e[i][j][len] = 0;                      // terminate the entry in the matrix
      if(len > max[j]) max[j] = len;         // update the max array if appropriate
      offset += next+1;                      // set the offset to after the & so we can find the next one
    }
  }

  for(i = 0; i < rows; i++) {                // iterate through the matrix and print each entry
    for(j = 0; j < cols; j++) {
      char* buffer = table_justify(e[i][j], max[j], table->col_spec[j], j != cols-1); // justify the entry
      generate_formatted_text(buffer);       // generate the entry as text
      free(buffer);                          // free the buffer
    }
    print_line();                            // print the row
  }

  print_blank_line();                      // may need to change to reflect line spacing, could fill a blank space in line and print line
  char buf[64];                              // ASSUMPTION: table # + caption can't be more than 64 chars
  memset(buf, 0, 64);
  if(table->caption != NULL)                 // print out the table#+caption if a caption exists
    sprintf(buf, "%s%s", table->id_str, table->caption);
  generate_formatted_text(buf);              // generate the text for the caption
  print_line();                              // print the caption
  if(table->caption != NULL) { // print a blank line to make space after the caption
    print_blank_line();
  }
  table_flag = tmp_flag;                     // reset table_flag and center_flag
  center_flag = tmp_center;
  free_table(table);                         // table no longer needed so free it
}

int table_lines(Table* table) {              // returns the # of lines a table will take up, assumed that a row will be a single line cause of time constraints but in theory it could be changed
  return table->rows + ((table->caption != NULL) ? (3+(strlen(table->caption)+strlen(table->id_str))/OUT_WIDTH) : (0));
}

Stack* new_stack() {                         // creates a new int stack, standard
  Stack* stack = (Stack*)malloc(sizeof(Stack));
  stack->data = (int*)malloc(sizeof(int)*8);
  stack->capacity = 8;
  stack->count = 0;
  return stack;
}

void push(Stack* stack, int n) {            // pushes an int onto the stack
  if(stack->count == stack->capacity) {
    stack->capacity *= 2;
    stack->data = (int*)realloc(stack->data, stack->capacity);
  }
  stack->data[stack->count++] = n;
}

int pop(Stack* stack) {                    // removes the top item from the stack
  if(stack->count > 0) {
    int n = stack->data[stack->count-1];
    stack->count--;
    return n;
  }
  return -1;
}

int top(Stack* stack) {                    // looks at the top element of the stack
  return stack->data[stack->count-1];
}

Queue* new_queue() {                       // creates a new Table* queue, fairly standard
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->data = (Table**)malloc(sizeof(Table*)*8);
  queue->capacity = 8;
  queue->count = 0;
  queue->front = 0;
  return queue;
}

void enqueue(Queue* queue, Table* table) { // enqueue's a table
  if(queue->count == queue->capacity) {
    queue->capacity *= 2;
    queue->data = (Table**)realloc(queue->data, queue->capacity);
  }
  queue->data[(queue->count+queue->front)%queue->capacity] = table;
  queue->count++;
}

Table* dequeue(Queue* queue) { // dequeue's a table
  if(queue->count == 0) return NULL;
  Table* table = queue->data[queue->front];
  queue->front = (queue->front + 1)%queue->capacity;
  return table;
}

Table* peek(Queue* queue) { // looks at the front of the queue
  return queue->data[queue->front];
}
