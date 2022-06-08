compile_all:
	@make -s compile_client
	@make -s compile_server

compile_client:
	g++ socket.cpp client.cpp -o client -pthread

compile_server:
	g++ socket.cpp server.cpp -o server -pthread

run_server:
	./server
	@make -s clear_server

deploy_server:
	@make -s compile_server
	@make -s run_server

clear_client:
	rm -f client

clear_server:
	rm -f server
