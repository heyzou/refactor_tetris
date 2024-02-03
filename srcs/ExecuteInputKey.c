#include "tetris.h"

int MoveDownFast(Struct temp)
{
	temp.row++;  //move down
	if(IsValidPisition(temp))
		current.row++;
	else 
	{
		for(int ri = 0; ri < current.width; ri++)
			for(int cj = 0; cj < current.width; cj++)
				if(current.array[ri][cj] == BLOCK)
					Table[current.row + ri][current.col + cj] = current.array[ri][cj];
		int sum, full_row=0;
		for(int rn=0;rn < R;rn++)
		{
			sum = 0;
			for(int cm = 0;cm < C;cm++) 
				sum+=Table[rn][cm];
			if(sum == C)// full row
			{
				full_row++;
				int rk;
				for(int rk = rn;rk >=1;rk--)
					for(int cl=0;cl<C;cl++)
						Table[rk][cl]=Table[rk-1][cl];
				for(int cl=0;cl<C;cl++)
					Table[rk][cl]=0;
				timer-=decrease--;
			}
		}
		final += 100 * full_row;
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