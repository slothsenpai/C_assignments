#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>



int main(){
	
	int dice_fd = open("/dev/dice", O_RDONLY); // /dev/dice is opened as a read-only file
	
	// If open failed, print an error message and exit the program
	if(dice_fd == -1){
		perror("/dev/dice"); 
		exit(1);
	} 
	
	char name[50]; // char array to hold user's name
	char input[50]; // char array to hold user input

	char *dice = malloc(2); // Stack allocated array to hold the resulting dice from the /dev/dice reads
	
	short int die_roll_1; // short int to hold the result of a dice roll
	short int die_roll_2; // ditto ^
	
	int i = 0; // int to be used for for loops
	
	int repeat = 1; // int used to control looping of the game
	
	int played_once = 0; // int to be used as a boolean, will control which statements are printed 

	short int point; // If the player does not win/lose on the first roll, this short int will hold the "point" value
	
	// printfs below greet the player and asks him/her to enter his/her name
	printf("Welcome to the Golden Saucer Casino! Please enter your name\n");
	printf(">> ");
	scanf("%s", name);
	// The entire game loops within a do-while loop and will continue to run until the player answers that he/she would not like to play (anymore)
	do{
		// If the player has already played the game once, the "would you like to play again" message is printed, otherwise the greeting message below is printed
		if(played_once){
			printf("Would you like to play again?\n");
		}
		else{
			printf("%s, Would you like to play a game of Craps? (Yes/No)\n", name);
		}
		printf(">> ");
		scanf("%s", input);
		while(input[i]){
			input[i] = tolower(input[i]);
			i++;
		}
		i = 0;
		
		// The following while loop will continue until the user inputs yes or no, the check ignores any capitalization as the user's input is converted to lowercase
		while(strcmp(input, "yes") != 0 && strcmp(input, "no") != 0){
			if(played_once){
				printf("Would you like to play again?\n");
			}
			else{
				printf("%s, Would you like to play a game of Craps? (Yes/No)\n", name);
			}
			printf(">> ");
			scanf("%s", input);
			while(input[i]){
				input[i] = tolower(input[i]);
				i++;
			}
			i = 0;
		}
		// Player has answered with some variant of "yes", so the game either restarts or begins playing for the first time
		if(strcmp(input, "yes") == 0){
			printf("\nGreat, let's play!\n");
			
			// read is called for /dev/dice which will produce as many bytes as indicated by the last argument of read
			// However, these bytes will correspond to int values from 0 to 5, so 1 is added when the die values are copied from the dice array into die_roll 1 & 2
			// If this were not performed, the dice values would not match an actual die and the game would not work
			read(dice_fd, dice, 2);
			die_roll_1 = dice[0] + 1;
			die_roll_2 = dice[1] + 1;
			
			
			printf("You rolled %d + %d = %d\n", die_roll_1, die_roll_2, die_roll_1+die_roll_2);
			int sum = die_roll_1 + die_roll_2;
			if(sum == 2 || sum == 3 || sum == 12){
				// The player rolled a 2, 3, or 12 on his/her first roll and has lost. The message below is displayed, then the player is asked if he/she would like to play again
				printf("\nYou lose! Better luck next time!\n\n");
			}
			else if(sum == 7 || sum == 11){
				// The user either rolled a 7 or an 11 on his/her first roll and has won. The message below is displayed, then the player is asked if he/she would like to play again
				printf("\nYou won! Congratulations %s!\n\n", name); 
			}
			else{
				// At this point the player has not won or lost, so the point value is set as the sum of the player's initial roll and the sum is reset to 0 to ensure proper looping
				point = sum;
				sum = 0;
				printf("\n***The point is now %d, you must roll %d to win. If you roll 7, you lose\nBegin Auto-rolling...\n", point, point);
				
				// The while loop below will "roll" the dice until the sum of the dice roll matched the point value or is 7
				// If the sum matches the point value, the player wins and is congratulated
				// If the sum matches 7, the players loses and is told so
				// After displaying either of those messages, the program then asks the player if he/she would like to play again and loops acoordingly
				while(sum != 7 && sum != point){
					
					// As above, read is called for /dev/dice and 1 is added to the resulting bytes when they are copied into die_roll 1 & 2
					read(dice_fd, dice, 2);
					
					die_roll_1 = dice[0] + 1;
					die_roll_2 = dice[1] + 1;
					
					sum = die_roll_1 + die_roll_2;
					printf("You rolled %d + %d = %d\n", die_roll_1, die_roll_2, die_roll_1+die_roll_2);
				}
				if(sum == 7){
					printf("\nYou lose! Better luck next time!\n\n");
				}
				else if(sum == point){
					printf("\nYou won! Congratulations %s!\n\n", name);
				}
			}
			repeat = 1;
			played_once = 1;
		}
		// Player has answered with some variant of "no", so the game says goodbye and the program closes
		else{
			printf("\nGoodbye, %s\n", name);
			repeat = 0;
			free(dice);
			close(dice_fd);
		}
	}while(repeat);
	return 0;
}