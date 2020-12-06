default: sendcount_receive.cpp receive.cpp
	g++ -o receive.out receive.cpp
	g++ -o sendcount_receive.out sendcount_receive.cpp
test: 
	./sendcount_receive.out ./receive.out

clean:
	rm -f *.out 

