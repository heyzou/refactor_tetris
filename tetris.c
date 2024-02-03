#include "tetris.h"

Struct CopyShape(Struct shape){
	Struct new_shape = shape;
	char **copyshape = shape.array;
	new_shape.array = (char**)malloc(new_shape.width*sizeof(char*));
    int ri, cj;
    for(ri = 0; ri < new_shape.width; ri++)
	{
		new_shape.array[ri] = (char*)malloc(new_shape.width*sizeof(char));
		for(cj=0; cj < new_shape.width; cj++)
			new_shape.array[ri][cj] = copyshape[ri][cj];
    }
    return new_shape;
}

void DestroyShape(Struct shape){
    int i;
    for(i = 0; i < shape.width; i++)
		free(shape.array[i]);
    free(shape.array);
}

int IsValidPisition(Struct shape){
    char **array = shape.array;
    int ri, cj;
    for(ri = 0; ri < shape.width; ri++)
	{
        for(cj = 0; cj < shape.width; cj++)
		{
			//check if the shape is out of the table or if the shape is overlapping with another shape
            if((shape.col + cj < 0 || C <= shape.col + cj || shape.row + ri >= R))
			{
                if(array[ri][cj])
                    return F;
            }
            else if(Table[shape.row + ri][shape.col + cj] && array[ri][cj])
                return F;
        }
    }
    return T;
}

void RotateShape(Struct shape){
	Struct temp = CopyShape(shape);
	int i, j, k, width;
	width = shape.width;
	for(i = 0; i < width ; i++)
	{
		for(j = 0, k = width-1; j < width ; j++, k--)
				shape.array[i][j] = temp.array[k][i];
	}
	DestroyShape(temp);
}

void PrintTetris(){
    char Buffer[R][C] = {0};
    int ri, cj;
    for(ri = 0; ri < current.width; ri++)
        for(cj = 0; cj < current.width; cj++)
            if(current.array[ri][cj] == BLOCK )
                Buffer[current.row+ri][current.col+cj] = current.array[ri][cj];
    clear();
    for(cj = 0; cj < C-9; cj++)
        printw(" ");
    printw("42 Tetris\n");
    for(ri = 0; ri < R; ri++){
        for(cj = 0; cj < C; cj++)
            printw("%c ", (Table[ri][cj] + Buffer[ri][cj])? '#': '.');
        printw("\n");
    }
    printw("\nScore: %d\n", final);
}

struct timeval before_now, now;
int hasToUpdate(){
	return ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)before_now.tv_sec*1000000 + before_now.tv_usec)) > timer;
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
		int ri, cj;
		for(ri = 0; ri < current.width; ri++)
			for(cj = 0; cj < current.width; cj++)
				if(current.array[ri][cj] == BLOCK)
					Table[current.row + ri][current.col + cj] = current.array[ri][cj];
		int rn, cm, sum, full_row=0;
		for(rn=0;rn<R;rn++)
		{
			sum = 0;
			for(cm=0;cm< C;cm++) 
				sum+=Table[rn][cm];
			if(sum == C)// full row
			{
				full_row++;
				int cl, rk;
				for(rk = rn;rk >=1;rk--)
					for(cl=0;cl<C;cl++)
						Table[rk][cl]=Table[rk-1][cl];
				for(cl=0;cl<C;cl++)
					Table[rk][cl]=0;
				timer-=decrease--;
			}
		}
		final += 100*full_row;
		Struct new_shape = CopyShape(StructsArray[rand()%7]);
		new_shape.col = rand()%(C-new_shape.width+1);
		new_shape.row = 0;
		DestroyShape(current);
		current = new_shape;
		if(!IsValidPisition(current))
			GameOn = F;
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
    int ri, cj;
    for(ri = 0; ri < R; ri++){
        for(cj = 0; cj < C; cj++)
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
    new_shape.col = rand()%(C-new_shape.width+1);
    new_shape.row = 0;
    DestroyShape(current);
	current = new_shape;
	if(!IsValidPisition(current))
		GameOn = F;
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
