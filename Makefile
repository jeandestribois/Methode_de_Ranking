# Execution :
run_v1_petit: compile
	./bin/algo_PR_v1 GraphesWebTest/web3.txt
run_v1_moyen: compile
	./bin/algo_PR_v1 GraphesWebTest/web_10001.txt
run_v1_grand: compile
	./bin/algo_PR_v1 GraphesWebTest/web_Stanford.txt --stanford 

#Compilation :
compile:
	gcc -Wall src/algo_PR_v1.c -o bin/algo_PR_v1

# Clean :
clean:
	rm -f bin/*