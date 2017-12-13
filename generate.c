/* THIS IS THE generate.c FILE */

/*
 * Initial page starts with 5 new lines, this simply outputs them
 */
void init_output_page(){
  fprintf(fpout, "\n\n\n\n\n");
  fflush(fpout);
  return;
}
/*
 * This function takes a section number and a section name and generates a header for them
 */
void  generate_sec_header(int i, char* s){
  fprintf(fpout, "\n\n%d %s\n\n", i, s);
  fflush(fpout);

  if (get_gen_toc() == TOC_ON)
    fprintf(fptoc, "\n%d %s ---------- PAGE %d\n", i, s, get_page_no());
}
/*
 * Generates a subsection header from the section text, if needed, prints a toc line to the toc. 
 * ASSUMPTION, TOC should preserve page style
 */
void  generate_subsec_header(int i,int j, char *s){
  fprintf(fpout, "\n\n%d.%d %s\n\n", i, j, s);
  fflush(fpout);
  if (get_gen_toc() == TOC_ON){
      char* page = translate_page_no(get_page_no(), get_page_style());
      fprintf(fptoc, "\n%d.%d %s ---------- PAGE %s\n", i, j, s,page);
      free(page);
  }
}

char* translate_page_no(int n, int style) {                    // this translates page # n into the given style
  char* page = (char*)malloc(sizeof(char)*100);                // ASSUMPTION: a page number will be printable in 100 chars regardless of style
  char* p=page;                                                // used for computation
  switch (style) {
    case LROMAN_STYLE:                                         // lower case roman
      convertToRoman(n, page);                                 // get the page number as a string of roman numerals
      for ( ; *p; ++p) *p = tolower(*p);                       // convert all the chars to lower case
      break;
    case CROMAN_STYLE:
      convertToRoman(n, page);                                 // get the page number as a string of roman numerals
      for ( ; *p; ++p) *p = toupper(*p);                       // convert all the chars to upper case
      break;
    case LALPH_STYLE:                                          // lower case alphabetical
      sprintf(page, "%c", 'a'+n-1);                            // use the ascii value of a as the starting point and add the page number - 1 (page 1 should be a)
      break;
    case CALPH_STYLE:
      sprintf(page, "%c", 'A'+n-1);                            // use the ascii value of A as the starting point and add the page number - 1 (page 1 should be A)
      break;
    default:                                                   // ARABIC_STYLE
      sprintf(page, "%d", n);                                  // just print the number
      break;
  }
  if ((style == LALPH_STYLE || style == CALPH_STYLE) && n > 26 )          // alph only works up to 26 pages, so just print as an arabic number if it's greater
    sprintf(page, "%d", n);
  return page;                                                // Don't forget to free!
}

/*
 * Print page number checks to see if a page is done (if not it prints blank lines until it's done
 * It converts the page number to the correct style and then prints it
 */

void print_page_number(){                                    // fills the rest of the page with blank space, prints the page number, and prints any t tables on the next page
  while(! check_done_page()) print_blank_line();             // print blank lines until the page is full
  fprintf(fpout, "\n");                                      // print out a line to indicate end of page (style decision)
  for(int i=0; i<20; i++) fprintf(fpout, " ");               // Print out 20 spaces in front of the page number
  char* page = translate_page_no(get_page_no(), get_page_style()); // translate the current page number into the appropriate style
  fprintf(fpout, "%s\n\n", page);                            // print out the page number followed by two newlines (style decision)
  free(page);                                                // free allocated memory
  inc_page_no();                                             // increase page number
  init_lines_so_far();                                       // reset lines so far back to 0
  if(peek(t_queue) != NULL) print_table(dequeue(t_queue));   // if there is a t table in the queue, dequeue it and print it out
}

void print_bottom() {                                        // prints a b table
  Table* bottom = dequeue(b_queue);                          // remove the first table from the queue
  while((LINES_PER_PAGE-lines_so_far) > table_lines(bottom)) print_blank_line(); // print blank lines until there is just enough space for the table
  print_table(bottom);                                       // print the table
}

void test_bottom() {                                         // checks if there is a bottom table and that there is room to print it
  Table* bottom = peek(b_queue);                             // get the next item in the queue
  if((bottom != NULL) && ((LINES_PER_PAGE-lines_so_far) >= table_lines(bottom))) { // if there's room, print the bottom
    print_bottom();
  }
}

void end_doc_cleanup(){                                      // this prints any remaning text, tables, and page numbers
  print_line();                                              // print any remaining text
  test_bottom();                                             // try to print a b table at the bottom of the current page
  while(1) {                                                 // this loop keeps printing out t and b tables until they are all done
    if(peek(b_queue) != NULL || peek(t_queue) != NULL) {     // if there is at least one table to print
      print_page_number();                                   // print the page number as we should be on a new page now, this will print any t pages
      test_bottom();                                         // try to print any b pages
    } else break;
  }
  print_page_number();                                       // print the last page number, no new t pages will print since there are none remaining so this is final
}

/*
 * Correctly spaces/centers a line as necessary
 * Also, checks to see if the page is done and prints the page number
 * Sets the page number/line number accordingly
 */

void print_line() {                                         // prints the line buffer
  if(is_ws(line)) {                                         // We never need to print a line filled with only space in this function the way we have everything, so just reset the line info and exit the function
    text_index = 0;
    spec_chars = 0;
    memset(line, 0, 128);
    return;
  }
  int temp_line = line_spacing;                             // store the line spacing in a temporary var
  if (single_flag)                                          // single overrides the line spacing
    line_spacing = 0;
  char* spacing[3] = {"\n", "\n\n", "\n\n\n"};

  if (center_flag){                                         //If the center flag is set, we find out the length of the line
    int len_of_line = strlen(line);
    int num_of_space_r = (OUT_WIDTH-len_of_line) / 2;       //We figure out how many spaces need to go on the right to to center it
    char tmp_line[128];
    for (int z =0; z < num_of_space_r; tmp_line[z++]=' ');  // We add the spaces to an empty buffer
    memcpy(tmp_line+num_of_space_r, line,len_of_line+1);    //And then we add the contents of the line after the spaces
    memcpy(line, tmp_line, 128);                            //Finally, we move the constructed buffer into the original buffer
  }

  int i;
  for(i = 0; i < ITEM_SPACING; i++)                        // prints out spaces for items
    fprintf(fpout, " ");
  fprintf(fpout, "%s%s", line, spacing[line_spacing]);     // prints the line followed by the amount of newlines dictated by the line spacing
  fflush(fpout);

  text_index = 0;                                          // reset the line
  spec_chars = 0;
  memset(line, 0, 128);

  for (int x = 0; x <=line_spacing; x++){                  // increment the lines so far according to the line and line spacing
    incr_lines_so_far();
    if (check_done_page()){                                //check if we're done with the page 
        print_page_number();                               //Print the page number if so
        break;                                             //stop adding newlines 
    }
  }
  line_spacing = temp_line;                                // reset the line spacing back to the original value
}

/*
 * Prints int s newlines
 */
void vertical_space(char* s) {
  int n = atoi(s);                                        // get int value of input
  int i;
  for(i = 0; i < n; i++){                                  // print out n newlines
      print_blank_line();
      if (check_done_page()){                                //check if we're done with the page 
        print_page_number();                               //Print the page number if so
      }
}
}

void generate_formatted_text(char* s){                    // generates string s as text
  int slen = strlen(s);
  int i, j, k, r;

  if(last_it_flag != it_flag) {
    if(it_flag) set_italics();
    else clear_italics();
  }

  for (i=0; i < slen;){
    int flag = 1; // a flag to indicate that the line still needs printing
    for (j = INDEX; ((text_index < (OUT_WIDTH - ITEM_SPACING)) && (i < slen)); i++, j++, text_index++) { // Start j at the current index of the line and iterate until the number of characters given by text index fills the line (item spacing for itemization, so need to treat that like a tab of sorts)
      line[j] = s[i];
      if(line[j] == '\n') {                               // forced new paragraph (\n\n) so print the line
        i+=2;                                             // honestly I forgot why this is here but it works
        line[j++] = 0;                                    // Don't actually print the newline, that will be handled by print line
        print_line();                                     // print the line buffer
        flag = 0;                                         // lower the flag so the line isn't printed again
        break;
      }
    }

    if(flag) {                                            // if the line still needs printing
      if (i < slen){                                      // as long as there is still text to print (NOTE: s is not copied to have the last line anymore due to other changes we've made)
        if ((line[j-1] != ' ') && (s[i] !=' ')){          // if the last char of the line is not a space and the next character to be read is not a space (a word stretching across the bound)
          for (k = j-1; line[k] != ' '; k--);             // Find the last space in the line
          i = i - (j - k - 1);                            // Reset i back to before the overlapping word (have a feeling this'll break for super long strings with no space)
          j = k;                                          // set k back to the last space so no extra shit gets printed
        }  
        for (;s[i] == ' '; i++);                          // skip any blank spaces so the next line does not begin with them

        line[j] = '\0';
        while(line[j+spec_chars-1]=='\n' || line[j+spec_chars-1] == ' ') line[(j--)+spec_chars] = '\0'; // I also forgot why this is important :(
        if (!center_flag)                                 // only right justify if the text is not supposed to be centered, verbatim text does not get sent to this function ever so not an issue
            right_justify();
        print_line();                                     // print the line
      }
    }
  }
  last_it_flag = it_flag;
}

/*
 *              This function gets called when we try printing verbatim mode in centered
 * It breaks the text into strings seperated by newlines, and then centers them based on the longest one
 *                                  Along the way, it prints each one
 */


void center_verb_text(char *s){
  int center = find_length_longest_line(s); //We center on the longest line, so we find that length
  int slen = strlen(s);                     //find the total string size
  char tmp_line[slen];
  int cur = 0;

  bzero(tmp_line, slen);                    //init to zero

  for(int i=0; i < slen; i++){              //loop finds seperates into newline strings
    tmp_line[cur++] = s[i];
    if (s[i] == '\n'){                      //When we see a new line, we center the previous line
      char tmp_line2[slen*2];
      bzero(tmp_line2, slen*2);
      tmp_line[cur] = '\0';                 //null terminates that string
      int num_of_space_r = (center-cur) / 2;
      for (int z = 0; z<num_of_space_r; tmp_line2[z++]=' ');
      memcpy(tmp_line2+num_of_space_r, tmp_line, cur+1); //Centers that string
      fprintf(fpout, "%s", tmp_line2);                   //prints that string

      cur = 0;                             //reset the string counter, move on to next new line terminated string
      bzero(tmp_line, slen);
    }
  }
}
/*
 * Simply finds the longest line in a string (newline to new line)
 */
int find_length_longest_line(char *s){ 
  int slen = strlen(s);
  int max = 0;
  int curlen = 0;
  for(int i = 0; i < slen; i++){
    if(s[i]=='\n'){
      if(curlen > max)
        max = curlen;
      curlen = 0;
    }
    else
      curlen++;
  }
  return max;
}

void generate_item(char* s) {                // Generates an item
  char* itemize_strs[3] = {"-", "~", ">"};   // ASSUMPTION: only up to a depth of 3 itemize/enumerate blocks nested together
  int enumeration_strs[3] = {ARABIC_STYLE, CROMAN_STYLE, LROMAN_STYLE}; // ?
  print_line();                              // print out a line if it's already in there, probably a better way to do this but shouldn't do any harm
  if(top(itemize_stack) == ENUMERATE_CMD) {  // Add the enumeration to the beginning of the line
    int top = pop(enumeration_stack);        // get the enumeration value
    char* buf = translate_page_no(top, enumeration_strs[itemize_stack->count-1]); // get string representation of the enum number
    sprintf(line, "%s. ", buf);              // stick the enumeration in the line buffer
    push(enumeration_stack, top+1);          // increment the enumeration and put it back on the stack
    free(buf);
  }
  else if(top(itemize_stack) == ITEMIZE_CMD) // Add a dash to the beginning of the line
    sprintf(line, "%s ", itemize_strs[itemize_stack->count-1]); // add the - to the line buffer
  text_index += strlen(line);                // increment the text index
  generate_formatted_text(s);                // generate the text and print the line
  print_line();
}
