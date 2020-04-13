build:
	g++ -std=c++11 -Wall -g src/main.cpp

run:
	./a.out -f "data.sor" -from 0 -len 100

serialization:
	g++ -std=c++11 -Wall -g src/test_serialization.cpp
	./a.out

clean:
	rm a.out

test:
	g++ -std=c++11 -Wall -g src/test.cpp
	./a.out

valgrind:
	docker run -ti -v `pwd`:/test w2-gtest:0.1 bash -c "cd /test ; g++ -std=c++11 -g -pthread src/main.cpp ; valgrind --leak-check=yes --track-origins=yes --dsymutil=yes ./a.out"

server:
	g++ -std=c++11 -Wall -g src/server.cpp -o server

client:
	g++ -std=c++11 -Wall -g src/client.cpp -o client