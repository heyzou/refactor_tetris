#include "tetris.h"

TimerInfo gameTimerConfig = {.initialTimer = 400000, .decreaseRate = 1000};

Struct CopyShape(Struct shape){
	Struct new_shape = shape;
	new_shape.array = malloc(new_shape.width*sizeof(char*));
	if (new_shape.array == NULL)
		exit(1);
	for(int ri = 0; ri < new_shape.width; ri++)
	{
		new_shape.array[ri] = (char*)malloc(new_shape.width*sizeof(char));
		for(int cj = 0; cj < new_shape.width; cj++)
			new_shape.array[ri][cj] = shape.array[ri][cj];
	}
	return new_shape;
}

void DestroyShape(Struct shape){
	for(int i = 0; i < shape.width; i++)
		free(shape.array[i]);
	free(shape.array);
}

int IsValidPisition(Struct shape, char Table[FIELD_ROW][FIELD_COL]){
	char **array = shape.array;
	for(int ri = 0; ri < shape.width; ri++)
	{
		for(int cj = 0; cj < shape.width; cj++)
		{
			//check if the shape is out of the table or if the shape is overlapping with another shape
			if((shape.col + cj < 0 || FIELD_COL <= shape.col + cj || shape.row + ri >= FIELD_ROW))
			{
				if(array[ri][cj])
					return false;
			}
			else if(Table[shape.row + ri][shape.col + cj] && array[ri][cj])
				return false;
		}
	}
	return true;
}

void RotateShape(Struct shape){
	Struct temp = CopyShape(shape);
	int width = shape.width;
	for(int i = 0; i < width ; i++)
		for(int j = 0, k = width - 1; j < width ; j++, k--)
				shape.array[i][j] = temp.array[k][i];
	DestroyShape(temp);
}

void PrintTetris(char Table[FIELD_ROW][FIELD_COL]){
	char Buffer[FIELD_ROW][FIELD_COL] = {0};
	for(int ri = 0; ri < current.width; ri++)
		for(int cj = 0; cj < current.width; cj++)
			if(current.array[ri][cj] == true)
				Buffer[current.row + ri][current.col + cj] = current.array[ri][cj];
	clear();
	for(int cj = 0; cj < FIELD_COL-9; cj++)
		printw(" ");
	printw("42 Tetris\n");
	for(int ri = 0; ri < FIELD_ROW; ri++){
		for(int cj = 0; cj < FIELD_COL; cj++)
			printw("%c ", (Table[ri][cj] + Buffer[ri][cj])? '#': '.');
		printw("\n");
	}
	printw("\nScore: %d\n", final);
}

struct timeval before_now, now;
int isUpdateRequired(){
	suseconds_t current_timestamp = (suseconds_t)(now.tv_sec * 1000000 + now.tv_usec);
	suseconds_t previous_timestamp = (suseconds_t)(before_now.tv_sec * 1000000 + before_now.tv_usec);

	return ((current_timestamp - previous_timestamp) > gameTimerConfig.initialTimer);
}

void set_timeout(int time) {
	time = 1;
	timeout(1);
}

void updateTableWithCurrent(char Table[FIELD_ROW][FIELD_COL]) {
	for(int ri = 0; ri < current.width; ri++)
		for(int cj = 0; cj < current.width; cj++)
			if(current.array[ri][cj] == true)
				Table[current.row + ri][current.col + cj] = current.array[ri][cj];
}

int MoveDownFast(Struct temp, char Table[FIELD_ROW][FIELD_COL])
{
	temp.row++;  //move down
	if(IsValidPisition(temp, Table))
		current.row++;
	else
	{
		updateTableWithCurrent(Table);
		int row_block_num, full_row = 0;
		for(int rn = 0;rn < FIELD_ROW;rn++)
		{
			row_block_num = 0;
			for(int cm = 0; cm < FIELD_COL; cm++)
				row_block_num += (int)Table[rn][cm];
			if(row_block_num == FIELD_COL)// full row
			{

				full_row++;
				int rk;
				for(int rk = rn; rk >= 1; rk--)
					for(int cl = 0; cl < FIELD_COL; cl++)
						Table[rk][cl] = Table[rk - 1][cl];
				for(int cl = 0; cl < FIELD_COL; cl++)
					Table[rk][cl] = 0;
				gameTimerConfig.initialTimer -= gameTimerConfig.decreaseRate;
				gameTimerConfig.decreaseRate--;
			}
		}
		final += 100 * full_row;
		Struct new_shape = CopyShape(StructsArray[rand()%7]);
		new_shape.col = rand()%(FIELD_COL - new_shape.width + 1);
		new_shape.row = 0;
		DestroyShape(current);
		current = new_shape;
		if(!IsValidPisition(current, Table))
			GameOn = false;
		return full_row;
	}
	return 0;
}

void ExecuteInputKey(Struct temp,int input_key, char Table[FIELD_ROW][FIELD_COL]){
	switch(input_key){
		case 's':
			MoveDownFast(temp, Table);
			break;
		case 'd':
			temp.col++;
			if(IsValidPisition(temp, Table))
				current.col++;
			break;
		case 'a':
			temp.col--;
			if(IsValidPisition(temp, Table))
				current.col--;
			break;
		case 'w':
			RotateShape(temp);
			if(IsValidPisition(temp, Table))
				RotateShape(current);
			break;
	}
}

void displayTable(char Table[FIELD_ROW][FIELD_COL]) {
	for(int ri = 0; ri < FIELD_ROW; ri++){
		for(int cj = 0; cj < FIELD_COL; cj++)
			printf("%c ", Table[ri][cj] ? '#': '.');
		printf("\n");
	}
}

void PrintGameOverScreen(char Table[FIELD_ROW][FIELD_COL]) {
	displayTable(Table);
	printf("\nGame over!\n");
	printf("\nScore: %d\n", final);
}

void InitializeGame(Struct new_shape)
{
	final = 0;
	initscr();
	srand(time(0));
	set_timeout(1);
	gettimeofday(&before_now, NULL);
	new_shape.col = rand()%(FIELD_COL-new_shape.width+1);
	new_shape.row = 0;
	DestroyShape(current);
	current = new_shape;
}

int main() {
	int input_key;
	char Table[FIELD_ROW][FIELD_COL] = {0};
	Struct new_shape = CopyShape(StructsArray[rand() % 7]);
	InitializeGame(new_shape);
	if(IsValidPisition(current, Table) == false)
		GameOn = false;
	PrintTetris(Table);
	while(GameOn)
	{
		input_key = getch();
		Struct temp = CopyShape(current);
		if (input_key != ERR)
			ExecuteInputKey(temp,input_key, Table);
		gettimeofday(&now, NULL);
		if (isUpdateRequired())
		{
			MoveDownFast(temp, Table);
			gettimeofday(&before_now, NULL);
		}
		DestroyShape(temp);
		PrintTetris(Table);
	}
	DestroyShape(current);
	endwin();
	PrintGameOverScreen(Table);
	return 0;
}
