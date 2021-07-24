#include "stdafx.h"

#include<queue>
#include<string>
#include<stack>
#include<unordered_set>
#include<algorithm>
#include<set>
#include<iterator>


using namespace std;


class Solution {
public:
	void solveSudoku(vector<vector<char>>& board) {
		stack<int> vec;
		stack<set<char>> done;
		set<char> avai_set[3][9];
		set<char> ini_set;
		set<char> done_set;
		
		for (int i = 0; i < 9; i++)
		{
			ini_set.insert('1' + i);
		}
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				avai_set[i][j] = ini_set;
			}
		}
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				avai_set[0][i].erase(board[i][j]);
				avai_set[1][j].erase(board[i][j]);
				avai_set[2][i/3*3+j/3].erase(board[i][j]);
			}
		}
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++) {
				if (board[i][j]=='.')
				{
					set<char> temp_set1;
					set<char> temp_set;
					set_intersection(avai_set[0][i].begin(), avai_set[0][i].end(), avai_set[1][j].begin(), avai_set[1][j].end(),inserter(temp_set1, temp_set1.begin()));
					//for each (char var in done_set)
					//{
					//	
					//	temp_set1.erase(var);
					//}
					set<char>::iterator iter = done_set.begin();
					while (iter != done_set.end())
					{
						temp_set1.erase(*iter);
						iter++;
					}
					set_intersection(avai_set[2][i / 3 * 3 + j / 3].begin(), avai_set[2][i / 3 * 3 + j / 3].end(),temp_set1.begin(), temp_set1.end(), inserter(temp_set, temp_set.begin()));
					if (temp_set.empty())
					{


						i= vec.top()/9;
						j= vec.top()%9;
						//board[i][j] = '.';

						recalib(avai_set, i,j, board[i][j], ini_set);
						board[i][j] = '.';

						i = j == 0 ? i - 1 : i;
						j = j > 0 ? j - 1 : 8;

						done_set = done.top();
						vec.pop();
						done.pop();
						
					}
					else
					{

						char a = *temp_set.begin();
						board[i][j] = a;
						done_set.insert(a);
						done.push(done_set);
						done_set.clear();
						vec.push(i*9+j);
						avai_set[0][i].erase(a);
						avai_set[1][j].erase(a);
						avai_set[2][i / 3 * 3 + j / 3].erase(a);
					}
				}
				

			}

		}
	}

	void recalib(set<char> avai_set[3][9], int i, int j, char board, set<char> set) {
		//avai_set[0][i]=set;
		//avai_set[1][j]=set;
		//avai_set[2][i / 3 * 3 + j / 3]=set;

		avai_set[0][i].insert(board);
		avai_set[1][j].insert(board);
		avai_set[2][i / 3 * 3 + j / 3].insert(board);

		/*for (int k = 0; k < 9; k++)
		{
			avai_set[0][i].erase(board[i][k]);
			avai_set[1][j].erase(board[k][j]);
			avai_set[2][i / 3 * 3 + j / 3].erase(board[i / 3 * 3+k/3][j/3*3+k%3]);
		}*/
	}
};



//
//int main()
//{
//	Solution sol;
//	char a[9][9] = { { '5', '3', '.', '.', '7', '.', '.', '.', '.' },{ '6', '.', '.', '1', '9', '5', '.', '.', '.' },{ '.', '9', '8', '.', '.', '.', '.', '6', '.' },{ '8', '.', '.', '.', '6', '.', '.', '.', '3' },{ '4', '.', '.', '8', '.', '3', '.', '.', '1' },{ '7', '.', '.', '.', '2', '.', '.', '.', '6' },{ '.', '6', '.', '.', '.', '.', '2', '8', '.' },{ '.', '.', '.', '4', '1', '9', '.', '.', '5' },{ '.', '.', '.', '.', '8', '.', '.', '7', '9' } };
//	vector<vector<char>> board({ { '5', '3', '.', '.', '7', '.', '.', '.', '.' },{ '6', '.', '.', '1', '9', '5', '.', '.', '.' },{ '.', '9', '8', '.', '.', '.', '.', '6', '.' },{ '8', '.', '.', '.', '6', '.', '.', '.', '3' },{ '4', '.', '.', '8', '.', '3', '.', '.', '1' },{ '7', '.', '.', '.', '2', '.', '.', '.', '6' },{ '.', '6', '.', '.', '.', '.', '2', '8', '.' },{ '.', '.', '.', '4', '1', '9', '.', '.', '5' },{ '.', '.', '.', '.', '8', '.', '.', '7', '9' } });
//	sol.solveSudoku(board);
//
//
//	cout << "Done!!!" << endl;
//	system("pause");
//	return 0;
//}
