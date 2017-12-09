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

void print_line() {
  int temp_line = line_spacing;
  if (single_flag)
    line_spacing = 0;
  char* spacing[3] = {"\n", "\n\n", "\n\n\n"};

  int i;
  for(i = 0; i < ITEM_SPACING; i++)
    fprintf(fpout, " ");
  fprintf(fpout, "%s%s", line, spacing[line_spacing]);
  fflush(fpout);

  text_index = 0;
  spec_chars = 0;
  tmp_text_index = 0;
  memset(line, 0, 128);

  for (int x = 0; x <=line_spacing; x++)
    incr_lines_so_far();
  if (check_done_page())
    print_page_number();
  line_spacing = temp_line;
}

void vertical_space(char* s) {
  int n = atoi(s);
  int i;
  for(i = 0; i < n; i++)
    fprintf(fpout, "\n");
}

/*void generate_formatted_text(char* s){
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
        fprintf(fpout, "%s%s", line, spacing[line_spacing]);
        fflush(fpout); 
      }
    }

    for (int x = 0; x <=line_spacing; x++)
        incr_lines_so_far();
    
    if (check_done_page())
      print_page_number();
  }
    line_spacing = temp_line;
}*/

void generate_formatted_text(char* s){
  int slen = strlen(s);
  int i, j, k, r;

  i = 0;
  while(s[i] == '\n'|| s[i]==' ')
    i+=1;
  for (; i < slen;){
    int flag = 1;
    for (j = INDEX; ((text_index < OUT_WIDTH - ITEM_SPACING) && (i < slen)); i++, j++, text_index++) {
      line[j] = s[i];
      if(line[j] == '\n') { // forced new paragraph (\n\n)
        i+=2;
        line[j++] = 0;
        print_line();
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
        print_line(); 
      }
    }
  }
}

void generate_item(char* s) {
  print_line();
  if(enumerate)
    sprintf(line, "%d. ", enumeration++);
  else if(itemize)
    sprintf(line, "- ");
  text_index += strlen(line);
  generate_formatted_text(s);
  print_line();
}