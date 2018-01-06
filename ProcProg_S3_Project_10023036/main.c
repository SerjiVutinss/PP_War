#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "math.h"

#define NUM_ROUNDS 13
#define NUM_SUITS 4
#define CARDS_PER_DECK 52

#define MIN_NUM_PLAYERS 2
#define MAX_NUM_PLAYERS 10

#define GAME_DECK_MAX_DECKS 3
//#define MAX_GAME_DECK_SIZE CARDS_PER_DECK * GAME_DECK_MAX_DECKS
#define MAX_GAME_DECK_SIZE 156

typedef struct Suit {
	int id;
	char name[10];
} Suit; // represents a suit

typedef struct Card {
	int cardId;
	Suit suit;
	int value;
	char cardName[10];
	char fullName[100];
	int isDealtToPlayer;
	int isPlayed;
} Card; // represents a card, which has a suit and a value

// PLAYERS
typedef struct Player {
	//char playerName[100];
	int score;
	Card cards[NUM_ROUNDS];
} Player; // represents a player, with an id, name, score and hand of cards


int numPlayers;
int numDecks;

int numCurrentTurn;

Player gamePlayers[MAX_NUM_PLAYERS];
Card gameDeck[MAX_GAME_DECK_SIZE];
Card roundCards[MAX_NUM_PLAYERS];
Suit gameSuits[4];

//// Prototypes
// Game
void setupGame(); // set up cards and players
void startGame(); // begins the first turn
void addPlayersToGame();

void displayScore();
void printDeck();

// Cards
void buildSuits();
void buildGameDeck();
Card getRandomCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize);
Card getRandomSuitedCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize, int suitId);
void dealAllPlayersCards();


void main() {

	// create a seed for rng
	srand((unsigned)time(NULL));

	int validSelection = 0;
	// load game or start new game
	printf("Welcome\nPlease make a selection: ");

	// main game loop
	do {

		// ask whether the player would like to start a new game, load a game or exit
		printf("\n1. New Game");
		printf("\n2. Load Game");
		printf("\n3. Exit Game");

		int userSelection = 0;
		printf("\nUser selection: ");
		scanf("%d", &userSelection);

		switch (userSelection)
		{
		case 1:
			printf("\n\nNew game selected!");

			// START A NEW GAME - game.c
			setupGame();
			startGame();

			break;
		case 2:
			printf("\nLoad game selected");
			break;
		case 3:
			printf("\nExit game selected");
			printf("\n\nDone!\n\n");
			return;
			//break;
		default:
			printf("\nNot a valid selection!");
			break;
		}
	} while (validSelection == 0);

	printf("\n\nDone!\n\n");
}

// setup a new game
void setupGame() {
	// get the number of players
	/*int validNumPlayers = 0;
	do {
		printf("\nHow many players: ");
		scanf(" %d", &numPlayers);

		if (numPlayers >= MIN_NUM_PLAYERS && numPlayers <= MAX_NUM_PLAYERS) {
			validNumPlayers = 1;
		}
	} while (validNumPlayers == 0);*/


	//printf("\n\n%d Players", numPlayers);

	// add the correct amount of players to the game
	addPlayersToGame();

	// calculate the number of decks to use - must always be at least 4 suits
	printf("\n\n!!!!!!!!!  %d !!!!!!!!!\n", numDecks);
	numDecks = (int)ceil((double)numPlayers / 4);
	printf("\n%d deck(s) will be used", numDecks);
	// shuffle the deck

	// TODO: check if game is loading
	if (1 == 1) {
		// NEW GAME, NOT LOADING

		// populate the gameDeck array with the cards, 
		// using the suits and number of decks - deck.c
		buildGameDeck(gameDeck, gameSuits, numDecks);
		printf("\nGame Deck built!\n");

		// deal the cards
		dealAllPlayersCards();
		printf("\nCards dealt to all players!\n");


	}
	else {
		// LOADING GAME

	}
}

void startGame() {

	Player currentPlayer;
	// each game consists of turns and scoring, followed by a final round(?)
	for (numCurrentTurn = 1; numCurrentTurn <= NUM_ROUNDS; numCurrentTurn++) {
		printf("\n\nTurn %d Starting", numCurrentTurn);
		// loop through players array
		for (int i = 0; i < numPlayers; i++) {
			printf("\nPlayer %d's turn", i + 1);
			currentPlayer = gamePlayers[i];
			//puts(currentPlayer.playerName);

			// print the player's cards and ask for a selection
			for (int j = 0; j < NUM_ROUNDS; j++) {
				if (currentPlayer.cards[j].isPlayed < 1) {
					printf("\n#%d - %s", j + 1, currentPlayer.cards[j].fullName);
				}
			}

			//TODO:  verify card has not already been played
			int selectedCardNumber = 0;

			// select the card by its index + 1
			printf("\nSelect a card(1 -13): ");
			scanf("%d", &selectedCardNumber);

			
			printf("\nPlayer %d played the %s\n", i + 1, currentPlayer.cards[selectedCardNumber - 1].fullName);
			currentPlayer.cards[selectedCardNumber - 1].isPlayed = 1;
			roundCards[i] = currentPlayer.cards[selectedCardNumber - 1];
		}

		printf("\n\nTurn %d Ending", numCurrentTurn);
		// end of player turn

		// calculate the score
		int highestUniqueScore = 0;
		int winnerIndex = -1;
		for (int i = 0; i < numPlayers; i++) {
			Card c = roundCards[i];

			// if it qualifies as the highest score
			if (c.value > highestUniqueScore) {
				highestUniqueScore = c.value;
				winnerIndex = i;
			}
			else if (c.value == highestUniqueScore) {
				highestUniqueScore = 0;
				winnerIndex = -1;
			}
		}

		if (winnerIndex > -1) {
			printf("\nWinner is: Player %d", winnerIndex + 1);
			printf("\n\tHighest Unique score: %d", highestUniqueScore);
			gamePlayers[winnerIndex].score += highestUniqueScore;

		}

		displayScore();
		//printf("\nRound score - ");
	}

}

void displayScore() {



	for (int i = 0; i < numPlayers; i++) {
		printf("\n%s", roundCards[i].fullName);
	}

	printf("\n\nScores");
	for (int i = 0; i < numPlayers; i++) {
		printf("\n\tPlayer %d: %d", i + 1, gamePlayers[i].score);
	}
}

void addPlayersToGame() {

	// sentinel
	int isValidSelection = 0;

	// get the number of players and set to global variable numPlayers
	printf("\nHow many players will play?");
	printf(" (Minimum 2, Maximum 10)");
	do {
		printf("\nEnter number of players: ");
		scanf("%d", &numPlayers);
		// if valid number of players
		if (numPlayers >= 2 && numPlayers <= MAX_NUM_PLAYERS) {
			// end loop
			isValidSelection = 1;
		}
	} while (isValidSelection == 0);

	// print the selection back to the user
	printf("%d players selected!", numPlayers);
	//char newName[100] = "";


	// now we know how many players, add each player
	int i;
	Player newPlayer;

	for (i = 0; i < numPlayers; i++) {
		// set the values
		newPlayer.score = 0;
		// give some feedback
		printf("\nPlayer %d Added!", i + 1);
		// and add the player to the gamePlayers array
		gamePlayers[i] = newPlayer;
	}

	printf("\nAll players added!\n");
}

//// CARD FUNCTIONS
// build an array of the suits
void buildSuits() {
	Suit suitHearts;
	suitHearts.id = 0;
	strcpy(suitHearts.name, "Hearts");
	gameSuits[0] = suitHearts;

	Suit suitClubs;
	suitClubs.id = 1;
	strcpy(suitClubs.name, "Clubs");
	gameSuits[1] = suitClubs;

	Suit suitDiamonds;
	suitDiamonds.id = 2;
	strcpy(suitDiamonds.name, "Diamonds");
	gameSuits[2] = suitDiamonds;

	Suit suitSpades;
	suitSpades.id = 3;
	strcpy(suitSpades.name, "Spades");
	gameSuits[3] = suitSpades;
}

// create the game deck which can be made up of 1-3 actual decks of cards
void buildGameDeck() {

	//srand(time(NULL));

	int suitId;
	int cardValue;
	int cardId = 0;
	//char cardFullName[100];

	int numCurrentDeck;

	buildSuits();

	// define a new card
	Card c;

	// game deck is built with multiple decks
	for (numCurrentDeck = 1; numCurrentDeck < numDecks + 1; numCurrentDeck++) {
		// loop through each suit in the suit list
		for (suitId = 0; suitId < 4; suitId++) {
			// add a new card, 1 - 13 to the gameDeck array
			for (cardValue = 2; cardValue <= 14; cardValue++) {
				// give the card its suit
				c.suit = gameSuits[suitId];
				// and give the card its value
				c.value = cardValue;

				// give the card a name based on its value
				switch (cardValue) {
				case 11:
					strcpy(c.cardName, "Jack");
					break;
				case 12:
					strcpy(c.cardName, "Queen");
					break;
				case 13:
					strcpy(c.cardName, "King");
					break;
				case 14:
					strcpy(c.cardName, "Ace");
					break;
				default:
					sprintf(c.cardName, "%d", c.value);
					//strcpy(c.cardName, c.value);
				}
				// set the cardId - also its location in the gameDeck
				c.cardId = cardId;

				// create the full name string
				strcpy(c.fullName, c.cardName);
				strcat(c.fullName, " of ");
				strcat(c.fullName, c.suit.name);

				// set a default value of 0
				c.isDealtToPlayer = 0;

				// insert the card into the deck at the correct location
				gameDeck[cardId] = c;
				cardId++;
			}
		}
	}
}

Card getRandomCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize) {

	int numIsValid = 0;
	Card c;
	// random number
	int r;
	do {
		r = rand() % deckSize;
		if (gameDeck[r].isDealtToPlayer == 0) {
			numIsValid = 1;
			gameDeck[r].isDealtToPlayer = 1;
			c = gameDeck[r];
		}

	} while (numIsValid == 0);

	return c;
}

Card getRandomSuitedCardFromDeck(Card gameDeck[MAX_GAME_DECK_SIZE], int deckSize, int suitId) {

	int suitIsEqual = 0;
	Card c;
	// random number
	int r;
	do {
		r = rand() % deckSize;
		if (gameDeck[r].isDealtToPlayer == 0 && gameDeck[r].suit.id == suitId) {
			suitIsEqual = 1;
			gameDeck[r].isDealtToPlayer = 1;
			c = gameDeck[r];
		}

	} while (suitIsEqual == 0);

	return c;
}

// performs the intial deal
void dealAllPlayersCards() {

	// player will be dealt four cards of the first suit they are dealt
	int suitId = 0; // may randomise this in future
	int numOfFirstSuit = 4;  // number of cards of the first dealt suit to be dealt

	for (int i = 0; i < numPlayers; i++) {
		for (int j = 0; j < NUM_ROUNDS; j++) {
			// first 4 cards will match suit
			if (j < numOfFirstSuit) {
				gamePlayers[i].cards[j] = getRandomSuitedCardFromDeck(gameDeck, CARDS_PER_DECK * numDecks, suitId);
			}
			// get completely random last 7 cards
			else {
				gamePlayers[i].cards[j] = getRandomCardFromDeck(gameDeck, CARDS_PER_DECK * numDecks);
			}
		}

		// increment the suitId - don't go past 
		if (suitId < 3) {
			suitId++;
		}
		else {
			suitId = 0;
		}
	}
	printf("\n\n\nCard dealt OK!!!!!!");
}

void printDeck() {
	// and test that the deck has built correctly
	for (int i = 0; i < numDecks * 52; i++) {
		printf("\n%s of %s: %d\n (isDealt = %d)", gameDeck[i].cardName, gameDeck[i].suit.name, gameDeck[i].value, gameDeck[i].isDealtToPlayer);
	}
}