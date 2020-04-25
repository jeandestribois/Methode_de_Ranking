# Execution :
run_v1_petit: compile
	./bin/algo_PR_v1 GraphesWebTest/web3.txt
run_v1_moyen: compile
	./bin/algo_PR_v1 GraphesWebTest/web_10001.txt
run_v1_grand: compile
	./bin/algo_PR_v1 GraphesWebTest/web_Stanford.txt --stanford 

run_v2_petit: compile
	./bin/algo_PR_v2 GraphesWebTest/web3.txt
run_v2_moyen: compile
	./bin/algo_PR_v2 GraphesWebTest/web_10001.txt
run_v2_grand: compile
	./bin/algo_PR_v2 GraphesWebTest/web_Stanford.txt --stanford 

run_v3_petit: compile
	./bin/algo_PR_v3 GraphesWebTest/web3.txt
run_v3_moyen: compile
	./bin/algo_PR_v3 GraphesWebTest/web_10001.txt
run_v3_grand: compile
	./bin/algo_PR_v3 GraphesWebTest/web_Stanford.txt --stanford 

run_v4_petit: compile
	./bin/algo_PR_v4 GraphesWebTest/web3.txt
run_v4_moyen: compile
	./bin/algo_PR_v4 GraphesWebTest/web_10001.txt
run_v4_grand: compile
	./bin/algo_PR_v4 GraphesWebTest/web_Stanford.txt --stanford 

#Compilation :
compile:
	gcc -Wall src/algo_PR_v1.c -o bin/algo_PR_v1
	gcc -Wall src/algo_PR_v2.c -o bin/algo_PR_v2
	gcc -Wall src/algo_PR_v3.c -o bin/algo_PR_v3
	gcc -Wall src/algo_PR_v4.c -o bin/algo_PR_v4

# Clean :
clean:
	rm -f bin/*
	rm -f resultats/*