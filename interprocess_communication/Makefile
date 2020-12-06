ipc1: ipc.cpp rscode.rs w1.cpp
	g++ -o ipc.out ipc.cpp
	g++ -o w1.out w1.cpp
	rustc rscode.rs
ipc2: ipc2.cpp rscode.rs write.cpp
	g++ -o ipc2.out ipc2.cpp
	g++ -o write.out write.cpp
	rustc rscode.rs
ipc3: ipc3.cpp rscode.rs w1.cpp w2.cpp w3.cpp
	g++ -o ipc3.out ipc3.cpp -pthread
	g++ -o w1.out w1.cpp
	g++ -o w2.out w2.cpp
	g++ -o w3.out w3.cpp
	rustc rscode.rs
test1:
	./ipc.out ./w1.out ./rscode
test2:
	./ipc2.out ./write.out 4 ./rscode
test3: 
	./ipc3.out 
testgrad:
	./ipc.out ../grad/dispatcher.out ./rscode
clean:
	rm -f *.out 
	rm -f rscode
