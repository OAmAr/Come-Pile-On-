/* THIS IS THE generate.c FILE */
void init_output_page(){
    fprintf(fpout, "\n\n\n\n\n");
    fflush(fpout);
    return;
}

void  generate_sec_header(int i, char* s){
    fprintf(fpout, "\n\n%d %s\n\n", i, s);
    fflush(fpout);

    if (get_gen_toc() == TOC_ON)
        fprintf(fptoc, "\n%d %s ---------- PAGE %d\n", i, s, get_page_no());
}

void  generate_subsec_header(int i,int j, char *s){
    fprintf(fpout, "\n\n%d.%d %s\n\n", i, j, s);
    fflush(fpout);
    if (get_gen_toc() == TOC_ON)
        fprintf(fptoc, "\n%d.%d %s ---------- PAGE %d\n", i, j, s, get_page_no());
}


void print_page_number(){
    while(! check_done_page()){
        fprintf(fpout,"\n");
        incr_lines_so_far();
    }
    fprintf(fpout, "\n\n");
    for(int i=0; i<20; i++) fprintf(fpout, " ");
    char* page = (char*) malloc(sizeof(char) * 100);
    char* p=page;
    int ps = get_page_style();
    int pn = get_page_no();
    switch (ps) {
        case LRoman:
            convertToRoman(pn,page);
            for ( ; *p; ++p) *p = tolower(*p);
            break;
        case CRoman:
            convertToRoman( pn,page);
            for ( ; *p; ++p) *p = toupper(*p);
            break;
        case LAlph:
            sprintf(page, "%c", 'a'+pn-1);
            break;
        case CAlph:
            sprintf(page, "%c", 'A'+pn-1);
            break;
        default:
            sprintf(page, "%d", pn);
            break;
    }
    if ( (ps == LAlph || ps == CAlph) && pn > 26 )
        sprintf(page, "%d", pn);

    fprintf(fpout, "%s\n\n\n", page);
    
    inc_page_no();
    init_lines_so_far();
    return;
}

void vertical_space(char* s) {
  int n = atoi(s);
  int i;
  for(i = 0; i < n; i++)
    fprintf(fpout, "\n");
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
  for(i = 0; i < length; i++) {
    if(line[i] != ' ' && !found_character)
      found_character = 1;
    if(line[i] == ' ' && found_character){
      n = i;
      break;
    }
  }
  while(llen < OUT_WIDTH) { // Right justify by going through line and adding spaces until it's good
    for(just = n; just < length && llen < OUT_WIDTH; just++) {
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
  line[OUT_WIDTH+spec_chars] = 0;
}

void  generate_formatted_text(char* s){
  /*if(it_flag) {
    generate_italics(s);
    return;
  }*/
  line[127] = 0;
  int temp_line = line_spacing;
  if (single_flag)
    line_spacing = 0;
  int slen = strlen(s);
  int i, j, k, r;
  char* spacing[3] = {"\n", "\n\n", "\n\n\n"};

  i = 0;
  while(s[i] == '\n'|| s[i]==' ')
    i+=1;
  for (; i < slen;){
    int flag = 1;
    for (j = INDEX; ((text_index < OUT_WIDTH) && (i < slen)); i++, j++, text_index++) {
      line[j] = s[i];
      if(line[j] == '\n') { // forced new paragraph (\n\n)
        i+=2;
        line[j++] = 0;
        fprintf(fpout, "%s%s", line, spacing[line_spacing]); 
        text_index = 0;
        spec_chars = 0;
        flag = 0;
        break;
      }
    }

    if(flag) {
      if (i < slen){
        if ((line[j-1] != ' ') && (s[i] !=' ')){ // if the last char of the line is not a space and the next character to be read is not a space (a word stretching across the bound)
          for (k = j-1; line[k] != ' '; k--); // Find the last space in the line
          i = i - (j - k - 1); // Reset i back to before the overlapping word (have a feeling this'll break for super long strings with no space)
          j = k; // set k back to the last space so no extra shit gets printed
        }  
        for (;s[i] == ' '; i++); // skip any blank spaces so the next line does not begin with them
      } 

      if (i < slen){
        line[j] = '\0';
        while(line[j-1]=='\n' || line[j-1] == ' ') line[j--] = '\0';
        right_justify();
        text_index = 0;
        spec_chars = 0;
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);  // swapped the order of this
        fflush(fpout); 
      }
    }

    for (int x = 0; x <=line_spacing; x++)
        incr_lines_so_far();
    
    if (check_done_page())
      print_page_number();
  }
    line_spacing = temp_line;
}

