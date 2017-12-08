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

void right_justify(int n) {
  int llen = strlen(line);
  int l, just, j;
  while(llen < OUT_WIDTH) { // Right justify by going through line and adding spaces until it's good
    for(just = n; just < llen && llen < OUT_WIDTH; just++) {
      if(line[just] == ' ')  {
        for(j = llen; j > just; j--) {
          line[j] = line[j-1];
        }
        llen++;
        just++; // skip the next space
      }
    }
  }
  line[OUT_WIDTH] = 0;
}
void print_page_number(){
    while(! check_done_page()){
        fprintf(fpout,"\n");
        incr_lines_so_far();
    }
    for(int i=0; i<20; i++)
        fprintf(fpout, " ");
    fprintf(fpout, "%d\n", get_page_no());
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

void right_justify_italics(int n) {
  int llen = strlen(line);
  int l, just, j;
  while(llen < 2*OUT_WIDTH) { // Right justify by going through line and adding spaces until it's good
    for(just = n; just < llen - 1 && llen < 2*OUT_WIDTH; just+=1) {
      if(line[just] == ' ')  {
        for(j = llen+1; j > just; j-=1) {
          line[j] = line[j-2];
        }
        llen+=2;
        just+=2; // skip the next space
      }
    }
  }
  line[2*OUT_WIDTH] = 0;
}

void generate_italics(char* s) {
  int slen = strlen(s);
  int i, j, k, r;
  char* spacing[3] = {"\n", "\n\n", "\n\n\n"};

  i = 0;
  while(s[i] == '\n'|| s[i]==' ')
    i+=1;
  for (; i <= slen;){
    int flag = 1;
    int space = 0;
    int found_character = 0;
    for (j = 0; ((j < OUT_WIDTH*2) && (i <= slen)); i++, j+=2) {
      line[j] = s[i];
      line[j+1] = 7;
      if(line[j] != ' ' && line[j] != 7)
        found_character = 1;
      if(line[j] == ' ' && !space && found_character)
        space = j;
      if(line[j] == '\n') {
        i+=2;
        line[j++] = 0;
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);  // swapped the order of this
        flag = 0;
        break;
      }
    }
    if(flag) {
      if (i <= slen){
        if ((line[j-1] != ' ') && (s[i] !=' ')){ // if the last char of the line is not a space and the next character to be read is not a space (a word stretching across the bound)
          for (k = j-1; line[k] != ' '; k--); // Find the last space in the line
          i = i - (j - k - 1); // Reset i back to before the overlapping word (have a feeling this'll break for super long strings with no space)
          j = k; // set k back to the last space so no extra shit gets printed
        }
        for (;s[i] == ' '; i++); // skip any blank spaces so the next line does not begin with them
      }

      line[j] = '\0';
      while(line[j-1]=='\n' || line[j-1] == ' ') line[j--] = '\0'; 

      if (i <= slen){
        right_justify_italics(space);
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);  // swapped the order of this
        fflush(fpout); 
      }else{
        for(r = 0; r <= j; r++)
          s[r] = line[r];  /* includes backslash 0 */
      }
    }

  }
}

void  generate_formatted_text(char* s){
  if(it_flag) {
    generate_italics(s);
    return;
  }
  int slen = strlen(s);
  int i, j, k, r;
  char* spacing[3] = {"\n", "\n\n", "\n\n\n"};

  i = 0;
  while(s[i] == '\n'|| s[i]==' ')
    i+=1;
  for (; i <= slen;){
    int flag = 1;
    int space = 0;
    int found_character = 0;
    for (j = 0; ((j < OUT_WIDTH) && (i <= slen)); i++, j++) {
      line[j] = s[i];
      if(line[j] != ' ')
        found_character = 1;
      if(line[j] == ' ' && !space && found_character)
        space = j;
      if(line[j] == '\n') {
        i+=2;
        line[j++] = 0;
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);  // swapped the order of this
        flag = 0;
        break;
      }
    }
    if(flag) {
      if (i <= slen){
        if ((line[j-1] != ' ') && (s[i] !=' ')){ // if the last char of the line is not a space and the next character to be read is not a space (a word stretching across the bound)
          for (k = j-1; line[k] != ' '; k--); // Find the last space in the line
          i = i - (j - k - 1); // Reset i back to before the overlapping word (have a feeling this'll break for super long strings with no space)
          j = k; // set k back to the last space so no extra shit gets printed
        }
        for (;s[i] == ' '; i++); // skip any blank spaces so the next line does not begin with them
      }

      line[j] = '\0';
      while(line[j-1]=='\n' || line[j-1] == ' ') line[j--] = '\0'; 

      if (i <= slen){
        right_justify(space);
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);  // swapped the order of this
        fflush(fpout); 
      }else{
        for(r = 0; r <= j; r++)
          s[r] = line[r];  /* includes backslash 0 */
      }
    }

    incr_lines_so_far();
    if (check_done_page())
      print_page_number();
  }
}