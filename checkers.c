#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
 
#include <omp.h>
//#include <mpi.h>

#define BOARD_SIZE 8

void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = ' ';
        }
    }

    // AI 'X'
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if ((row + col) % 2 == 1) {
                board[row][col] = 'X';
            }
        }
    }

    // Player 'O'
    for (int row = 5; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if ((row + col) % 2 == 1) {
                board[row][col] = 'O';
            }
        }
    }
}

// display board
void displayBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("   1 2 3 4 5 6 7 8\n");
    for (int row = 0; row < BOARD_SIZE; row++) {
        printf("%d |", row + 1);
        for (int col = 0; col < BOARD_SIZE; col++) {
            printf("%c|", board[row][col]);
        }
        printf("\n");
    }
}


// Change turn
char change_turn(char turn){
	
	if (turn == 'X'){
	    turn = 'O';
	}else if (turn == 'O'){
	    turn = 'X';
	}
	return turn;
}

// Verfica se é a peça certa
int isYourPiece(char board[BOARD_SIZE][BOARD_SIZE], int row, int col, char player) {

    char piece = board[row][col];

    // Comparar a peça com o jogador especificado
    if ((player == 'X' && (piece == 'X' ||  piece == 'K') ) || (player == 'O' && (piece == 'O' || piece == 'Q'))) {
        return 1; // A peça pertence ao jogador
    }

    return 0; // A peça não pertence ao jogador
}


int isWithinBounds(char board[BOARD_SIZE][BOARD_SIZE], int startX, int startY, int endX, int endY) {
    
    // Verificar se as coordenadas iniciais e finais estão dentro do tabuleiro
    if (startX < 0 || startX >= BOARD_SIZE || startY < 0 || startY >= BOARD_SIZE ||
        endX < 0 || endX >= BOARD_SIZE || endY < 0 || endY >= BOARD_SIZE) {
        return 0; // Movimento fora do tabuleiro
    }

    // Verificar se a casa de destino está vazia
    if (board[endX][endY] != ' ') {
        return 0; // A casa de destino não está vazia
    }
	return 1; // Válido
}

// Função para verificar se um movimento é uma captura diagonal válida
int isValidDiagonalCapture(char board[BOARD_SIZE][BOARD_SIZE], int startX, int startY, int endX, int endY, char player) {

    int deltaX = endX - startX;
    int deltaY = endY - startY;
    int midX = startX + deltaX / 2;
    int midY = startY + deltaY / 2;

    if (!isWithinBounds(board,startX,startY,endX, endY)) {
        return 0;
    }

    // Verificar captura diagonal
    if (abs(deltaX) == 2 && abs(deltaY) == 2) {
        int midX = (startX + endX) / 2;
        int midY = (startY + endY) / 2;

        // Verificar se há uma peça do oponente no meio
        if (board[endX][endY] == ' ' && (board[midX][midY] == 'X' || board[midX][midY] == 'O' || board[midX][midY] == 'Q' ||  board[midX][midY] == 'K') && !isYourPiece(board,midX, midY, player)) {
           	return 1; // Captura válida
        }
    }

    return 0; // Movimento inválido
}

int isValidMove(char board[BOARD_SIZE][BOARD_SIZE], int startX, int startY, int endX, int endY, char player){
	  // Calcular a diferença entre as coordenadas de início e fim
	  int deltaX = endX - startX;
    	  int deltaY = endY - startY;
    	  
    	     // Verificar se o movimento está dentro dos limites do tabuleiro
  	  if (!isWithinBounds(board,startX,startY,endX, endY)) {
       		 return 0; // Inválido
   	 }
    	  
	    // Verificar movimento diagonal simples
   	 if (abs(deltaX) == 1 && abs(deltaY) == 1) {
   	     if (board[endX][endY] == ' ') {
   	      	if ((player == 'O' && deltaX < 0) || (player == 'X' && deltaX > 0)) {
                	return 1; // Válido
            	}
            	if((player == 'X' && board[startX][startY] == 'K') || (player == 'O' &&  board[startX][startY] == 'Q')){
           	return 1; // Captura válida
          	}
            	
   	     }
   	 }
   	 return 0; // Invalido
}

int movePiece(char board[BOARD_SIZE][BOARD_SIZE], int startX, int startY, int endX, int endY, char player) {
    if (isValidDiagonalCapture(board, startX, startY, endX, endY,player) && isYourPiece(board,startX, startY, player)) {
        // Realizar a captura
        board[endX][endY] = board[startX][startY]; // Mover a peça para a posição de destino
        board[startX][startY] = ' '; // Limpar a posição de origem
        int midX = (startX + endX) / 2;
        int midY = (startY + endY) / 2;
        board[midX][midY] = ' '; // Remover a peça capturada
        if(endX == 0 && board[endX][endY] == 'O'){
            board[endX][endY] = 'Q';
        }
        if(endX == 7 && board[endX][endY] == 'X'){
            board[endX][endY] = 'K';
        }
        return 1; // Captura bem-sucedida
    } else if (isValidMove(board, startX, startY, endX, endY, player) && isYourPiece(board,startX, startY, player)) {
        // Movimento regular
        board[endX][endY] = board[startX][startY]; // Mover a peça para a posição de destino
        board[startX][startY] = ' '; // Limpar a posição de origem
        return 0; // Movimento regular bem-sucedido
    } else {
        // Movimento inválido
        return -1; // Movimento inválido
    }
}

// PC
void getComputerMove(char board[BOARD_SIZE][BOARD_SIZE], char player) {
    int fromX, fromY, toX, toY;

    // Lista para armazenar os movimentos de captura disponíveis
    int captureMoves[BOARD_SIZE * BOARD_SIZE][4]; // [fromX, fromY, toX, toY]
    int numCaptureMoves = 0;

    for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) {
        for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) {
            if (board[fromRow][fromCol] == 'X' || board[fromRow][fromCol] == 'K') {
                for (int deltaX = -1; deltaX <= 1; deltaX += 2) {
                    for (int deltaY = -1; deltaY <= 1; deltaY += 2) {
                        toX = fromRow + deltaX;
                        toY = fromCol + deltaY;
                        int captureX = fromRow + 2 * deltaX;
                        int captureY = fromCol + 2 * deltaY;

                        if (isWithinBounds(board, fromRow, fromCol, toX, toY) &&
                            isWithinBounds(board, toX, toY, captureX, captureY) &&
                            isValidDiagonalCapture(board, fromRow, fromCol, captureX, captureY, player)) {
                            // Adicione este movimento de captura à lista de captura
                            captureMoves[numCaptureMoves][0] = fromRow;
                            captureMoves[numCaptureMoves][1] = fromCol;
                            captureMoves[numCaptureMoves][2] = captureX;
                            captureMoves[numCaptureMoves][3] = captureY;
                            numCaptureMoves++;
                        }
                    }
                }
            }
        }
    }

    if (numCaptureMoves > 0) {
        // Captura de peça (se disponível)
        int randomCapture = rand() % numCaptureMoves;
        fromX = captureMoves[randomCapture][0];
        fromY = captureMoves[randomCapture][1];
        toX = captureMoves[randomCapture][2];
        toY = captureMoves[randomCapture][3];
    } else {
        //  escolha de um movimento regular válido
        while (1) {
            fromX = rand() % BOARD_SIZE;
            fromY = rand() % BOARD_SIZE;
            toX = rand() % BOARD_SIZE;
            toY = rand() % BOARD_SIZE;

            if (movePiece(board, fromX, fromY, toX, toY, player) != -1) {
                break;
            }
        }
    }

    // Realize o movimento escolhido
    movePiece(board, fromX, fromY, toX, toY, player);
    printf("IA move: %d,%d to %d,%d\n", fromX + 1, fromY + 1, toX + 1, toY + 1);
}


int win(char board[BOARD_SIZE][BOARD_SIZE]){
    int countx, county;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if(board[row][col] == 'X' || board[row][col] == 'K'){
                countx++;
            }else if(board[row][col] == 'O' || board[row][col] == 'Q'){
                county++;
            }
        }
    }

    if(countx == 0){
        return 1; // Player win
    }
    if(county == 0){
        return 2; // PC win
    }

    return 0;
}


int main(){
	char board[BOARD_SIZE][BOARD_SIZE];
	initializeBoard(board);
	displayBoard(board);
	char player = 'O';
	
    	while(1)
        {
            int w = win(board);
            if(w == 1){
                printf("Player WIN");
                break;
            }
            if(w == 2){
                printf("PC WIN");
                break;
            }
            if(player == 'O'){
                 int fromx, fromy, tox, toy;
            int validMove = 0;
            while (!validMove) {
                printf("Move from line: ");
                scanf("%d", &fromx);
                printf("Move from col: ");
                scanf("%d", &fromy);
                printf("Move to line: ");
                scanf("%d", &tox);
                printf("Move to col: ");
                scanf("%d", &toy);

                int moveResult = movePiece(board, fromx - 1, fromy - 1, tox - 1, toy - 1, player);
                if (moveResult == 0) {
                    	    validMove = 1;
                    	    displayBoard(board);
               	} else if (moveResult == -1) {
                	    printf("Invalid move. Try again.\n");
                }
                
            }
                
                player = change_turn(player);
            }
            
            if(player == 'X'){
                getComputerMove(board, player);
                displayBoard(board);
                player = change_turn(player);
            }
            
        }
       
	return 0;
}
