#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  
  write_string_to_file(".beargit/.prev", "6666666666666666666666666666666666666666");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE* newfindex = fopen(".beargit/.newindex", "w");
  int removed = 0;

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      removed = 1;
      continue;
    }
    fprintf(newfindex, "%s\n", line);    
  }
  fclose(findex);
  fclose(newfindex);
  fs_mv(".beargit/.newindex", ".beargit/.index");
  
  if (removed == 1) {
    return 0;
  }
  fprintf(stderr, "ERROR: File %s not tracked\n", filename);
  return 1;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  if (strstr(msg, go_bears) == NULL)
  {
    /* code */
    return 0;
  }
  return 1;
}

void next_commit_id(char* commit_id) {
  /* COMPLETE THE REST */

  int flag = 0;
  int i = 0;
  do {
    if (commit_id[i] == '6') {
      commit_id[i] = '1';
    } else if(commit_id[i] == '1') {
      commit_id[i] = 'c';
    } else {
      commit_id[i] = '6';
      flag = 1;
    }
    i++;
  } while(i < COMMIT_ID_SIZE && flag == 1);

  return;
}

int beargit_commit(const char* msg) {
  if (is_commit_msg_ok(msg) == 0) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);

  /* COMPLETE THE REST */
  char newidname[FILENAME_SIZE];
  strcpy(newidname, ".beargit/");
  strcat(newidname, commit_id);
  FILE* findex = fopen(".beargit/.index", "r");
  char line[FILENAME_SIZE];

  fs_mkdir(newidname);
  char newindexname[FILENAME_SIZE];
  char newprevname[FILENAME_SIZE];
  strcpy(newindexname, newidname);
  strcpy(newprevname, newidname);
  strcat(newindexname, "/.index");
  strcat(newprevname, "/.prev");
  fs_cp(".beargit/.index", newindexname);
  fs_cp(".beargit/.prev", newprevname);

  while (fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    char trackedfilename[FILENAME_SIZE];
    char newfilename[FILENAME_SIZE];

    strcpy(trackedfilename, line);
    strcpy(newfilename, newidname);
    strcat(newfilename, "/");
    strcat(newfilename, line);
    fs_cp(trackedfilename, newfilename);
  }
  fclose(findex);

  char msgname[FILENAME_SIZE]; 
  strcpy(msgname, newidname);
  strcat(msgname, "/.msg");
  write_string_to_file(msgname, msg);

  write_string_to_file(".beargit/.newprev", commit_id);
  fs_mv(".beargit/.newprev", ".beargit/.prev");

  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  int file_count = 0;
  FILE* findex = fopen(".beargit/.index", "r");

  fprintf(stdout, "Tracked files:\n\n");
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    fprintf(stdout, "  %s\n", line);
    file_count++;
  }
  fprintf(stdout, "\n%d files total\n", file_count);
  fclose(findex);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */
  char prevcommitid[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", prevcommitid, COMMIT_ID_SIZE);
  if (strcmp(prevcommitid, "6666666666666666666666666666666666666666") == 0)
  {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }

  while (strcmp(prevcommitid, "6666666666666666666666666666666666666666") != 0) {
    char prevmsg[MSG_SIZE];
    char prevcommitindex[FILENAME_SIZE];
    char prevmsgname[FILENAME_SIZE];
    strcpy(prevcommitindex, ".beargit/");
    strcat(prevcommitindex, prevcommitid);
    strcpy(prevmsgname, prevcommitindex);
    strcat(prevmsgname, "/.msg");
    read_string_from_file(prevmsgname, prevmsg, MSG_SIZE);

    fprintf(stdout, "\ncommit %s\n    %s\n", prevcommitid, prevmsg);

    char prevprevname[FILENAME_SIZE];
    strcpy(prevprevname, prevcommitindex);
    strcat(prevprevname, "/.prev");
    strcpy(prevcommitid, "");
    read_string_from_file(prevprevname, prevcommitid, COMMIT_ID_SIZE);

  }
  fprintf(stdout, "\n");

  return 0;
}
