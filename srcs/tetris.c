#include "tetris.h"

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

int IsValidPisition(Struct shape){
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

void PrintTetris(){
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
int hasToUpdate(){
	suseconds_t current_timestamp = (suseconds_t)(now.tv_sec * 1000000 + now.tv_usec);
	suseconds_t previous_timestamp = (suseconds_t)(before_now.tv_sec * 1000000 + before_now.tv_usec);

	return ((current_timestamp - previous_timestamp) > timer);
}

void set_timeout(int time) {
	time = 1;
	timeout(1);
}

int MoveDownFast(Struct temp)
{
	temp.row++;  //move down
	if(IsValidPisition(temp))
		current.row++;
	else
	{
		for(int ri = 0; ri < current.width; ri++)
			for(int cj = 0; cj < current.width; cj++)
				if(current.array[ri][cj] == true)
					Table[current.row + ri][current.col + cj] = current.array[ri][cj];
		int sum, full_row=0;
		for(int rn=0;rn < FIELD_ROW;rn++)
		{
			sum = 0;
			for(int cm = 0;cm < FIELD_COL;cm++)
				sum+=Table[rn][cm];
			if(sum == FIELD_COL)// full row
			{
				full_row++;
				int rk;
				for(int rk = rn;rk >=1;rk--)
					for(int cl=0;cl<FIELD_COL;cl++)
						Table[rk][cl]=Table[rk-1][cl];
				for(int cl=0;cl<FIELD_COL;cl++)
					Table[rk][cl]=0;
				timer-=decrease--;
			}
		}
		final += 100*full_row;
		Struct new_shape = CopyShape(StructsArray[rand()%7]);
		new_shape.col = rand()%(FIELD_COL-new_shape.width+1);
		new_shape.row = 0;
		DestroyShape(current);
		current = new_shape;
		if(!IsValidPisition(current))
			GameOn = false;
		return full_row;
	}
	return 0;
}

void ExecuteInputKey(Struct temp,int input_key){
	switch(input_key){
		case 's':
			MoveDownFast(temp);
			break;
		case 'd':
			temp.col++;
			if(IsValidPisition(temp))
				current.col++;
			break;
		case 'a':
			temp.col--;
			if(IsValidPisition(temp))
				current.col--;
			break;
		case 'w':
			RotateShape(temp);
			if(IsValidPisition(temp))
				RotateShape(current);
			break;
	}
}

void PrintGameOverScreen() {
	for(int ri = 0; ri < FIELD_ROW; ri++){
		for(int cj = 0; cj < FIELD_COL; cj++)
			printf("%c ", Table[ri][cj] ? '#': '.');
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", final);
}

int main() {
	srand(time(0));
	final = 0;
	int input_key;
	initscr();
	gettimeofday(&before_now, NULL);
	set_timeout(1);
	Struct new_shape = CopyShape(StructsArray[rand()%7]);
	new_shape.col = rand()%(FIELD_COL-new_shape.width+1);
	new_shape.row = 0;
	DestroyShape(current);
	current = new_shape;
	if(!IsValidPisition(current))
		GameOn = false;
	PrintTetris();
	while(GameOn)
	{
		input_key = getch();
		Struct temp = CopyShape(current);
		if (input_key != ERR)
			ExecuteInputKey(temp,input_key);
		gettimeofday(&now, NULL);
		if (hasToUpdate())
		{
			MoveDownFast(temp);
			gettimeofday(&before_now, NULL);
		}
		DestroyShape(temp);
		PrintTetris();
	}
	DestroyShape(current);
	endwin();
	PrintGameOverScreen();
	return 0;
}
