#include "mymalloc.h"
#include "mymalloc.c"

int main(){
	char *test = my_bestfit_malloc(sizeof(char));
	char *test2 = my_bestfit_malloc(sizeof(char));
	int *test3 = my_bestfit_malloc(sizeof(int));
	int *test4 = my_bestfit_malloc(sizeof(int));
	int *test5 = my_bestfit_malloc(0);
	printlist();
	my_free(test2);
	printlist();
	my_free(test3);
	printlist();
	char *fit = my_bestfit_malloc(sizeof(char));
	printlist();
	int *fit2 = my_bestfit_malloc(sizeof(int));
	printlist();
	my_free(fit2);
	printlist();
	my_free(test4);
	printlist();
	char *fit3 = my_bestfit_malloc(sizeof(char));
	printlist();
	my_free(fit3);
	printlist();
	my_free(test);
	printlist();
	double *test6 = my_bestfit_malloc(sizeof(double));
	printlist();
	double *test7 = my_bestfit_malloc(sizeof(double));
	printlist();
	int *test8 = my_bestfit_malloc(sizeof(int));
	printlist();
	my_free(test7);
	printlist();
	my_free(test6);
	printlist();
	my_free(test8);
	printlist();
	char *test10 = my_bestfit_malloc(sizeof(char));
	printlist();
	my_free(test10);
	printlist();
	return 0;
}