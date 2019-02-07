#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <sys/wait.h> /* waitpid */
#include <unistd.h> /* fork */

void process(int number, int time) {
  printf("Prosess %d kjører\n", number);
  sleep(time);
  printf("Prosess %d kjørte i %d sekunder\n", number, time);
}

int main() {
  int p[6]; //Prossesser

  // START P0
  // Fork en ny proc
  p[0] = fork(); 
  // Hvis barn:
  if(p[0] == 0) {
    // Kjør proc func
    process(0, 1);
    // Avslutt child
    exit(0); 
  }

  // START P2
  // Fork en ny proc
  p[2] = fork(); 
  // Hvis barn:
  if(p[2] == 0) {
    // Kjør proc func
    process(2, 3);
    // Avslutt child
    exit(0); 
  }

  // VENT P0
  waitpid(p[0], NULL, 0);

  // START P1
  p[1] = fork();
  if(p[1]==0){
	  process(1,2);
	  exit(0);
  }
  // START P4
  p[4] = fork();
  if(p[4]==0){
	  process(4,3);
	  exit(0);
  }
  // VENT P1
  waitpid(p[1], NULL, 0);
  // VENT P2
  waitpid(p[2], NULL, 0);
  // START P3
  p[3] = fork();
  if(p[3]==0){
	  process(3,2);
	  exit(0);
  }
  // VENT P4
  waitpid(p[4], NULL, 0);
  // START P5
  p[5] = fork();
  if(p[5]==0){
	  process(5,3);
	  exit(0);
  }
  // VENT P3
  waitpid(p[3], NULL, 0);
  // VENT P5
  waitpid(p[5], NULL, 0);

  return 0;
}