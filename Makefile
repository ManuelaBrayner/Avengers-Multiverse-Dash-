CC = gcc 
CFLAGS = -Wall -Wextra -O2 -Icli-lib/include 
SRC = jogo.c cli-lib/src/screen.c cli-lib/src/keyboard.c cli-lib/src/timer.c 
avengers_run: $(SRC) tipos.h dados.h    
	$(CC) $(CFLAGS) -o $@ $(SRC) -lm 

asan: $(SRC) tipos.h dados.h
	$(CC) -g -fsanitize=address,undefined -Wall -Wextra -Icli-lib/include -o av_asan $(SRC) -lm 
	
clean:    
	rm -f avengers_run av_asan