CC = g++ -std=c++11 -Wall -Wextra -g

client: client.o function.o buffer.o
	$(CC) $^ -o $@

client.o: client.cpp
	$(CC) $(CFLAGS) $< -c

function.o: function.cpp
	$(CC) $(CFLAGS) $< -c

buffer.o: buffer.cpp
	$(CC) $(CFLAGS) $< -c

run: client
	./client

clean:
	rm -f *.o client