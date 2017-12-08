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
  //fprintf(fpout, "%d\n", llen);
  while(llen < OUT_WIDTH) { // Right justify by going through line and adding spaces until it's good
    for(just = n; just < llen && llen < OUT_WIDTH; just++) {
      if(line[just] == ' ')  {
        //fprintf(fpout, "%d ", just);
        for(j = llen; j > just; j--) {
          //fprintf(fpout, "%d ", j);
          line[j] = line[j-1];
        }
        llen++;
        just++; // skip the next space
      }
    }
  }
  line[OUT_WIDTH] = 0;
}

void  generate_formatted_text(char* s){
    int slen = strlen(s);
    int i, j, k, r;

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
            fprintf(fpout, "%s\n", line);
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
          fprintf(fpout, "%s\n", line);  // swapped the order of this
          fflush(fpout); 
        }else{
          //if(line[j-1] == '\n')
            //line[j-1] == 0;
          for(r = 0; r <= j; r++)
            s[r] = line[r];  /* includes backslash 0 */
        }
      }

  }
  //s[0] = 0;
}