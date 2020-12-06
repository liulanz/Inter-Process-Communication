default: write.cpp receive.cpp pipe_sigstop.cpp
	g++ -o write.out write.cpp
	g++ -o receive.out receive.cpp
	g++ -o pipe_sigstop.out pipe_sigstop.cpp -pthread
test: 
	./pipe_sigstop.out ./write.out ./receive.out

clean:
	rm -f *.out 

